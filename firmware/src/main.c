#include <msp430.h>
#include <stdio.h>
#include <string.h>

#include "gitrev.h"

#include "cc430uart.h"
#include "flashctl.h"
#include "i2c.h"
#include "lsm.h"
#include "pins.h"
#include "proto.h"
#include "timer_a.h"
#include "ucs.h"

#undef BREAK_FLASH    // < disable flash operations (simulate flash failure)
#define PLANNER_DEBUG // < extra messages from the planner
#undef SR_DEBUG       // < extra messages from the state recorder

enum {
    ST_EXIT,
    RADIO_INIT,
    RADIO_TX_B0,
    RADIO_TX_B1,
    SENSOR_INIT,
    SENSOR_READ,
    ACTUATE,
    NUM_STATES,
    ERASED = 0xff
};

#define STATEREC_LEN 128
const volatile uint8_t __attribute__ ((section (".infoB"))) staterec_arr[STATEREC_LEN];

const char* VERSION_STR = "Spinor DEBUG (" GIT_REV ")\r\n";

uint16_t data_gyro_fifo[96];

static void init_core() {
    // Disable WDT
    WDTCTL = WDTPW | WDTHOLD;

    // Reference FLL to REFO and set MCLK, SMCLK -> 1 MHz
    UCS_initClockSignal(UCS_FLLREF, UCS_REFOCLK_SELECT, UCS_CLOCK_DIVIDER_1);
    UCS_initFLLSettle(1000, 1000000/UCS_REFOCLK_FREQUENCY);

    // VLO -> ACLK (note that Zac uses REFO -> ACLK instead)
    UCS_initClockSignal(UCS_ACLK, UCS_VLOCLK_SELECT, UCS_CLOCK_DIVIDER_1);

    // POWER: Turn ADC and reference voltage off to conserve power
    ADC12CTL0 &= ~ADC12ENC;
    ADC12CTL0 &= ~ADC12ON;
    ADC12CTL0 &= ~ADC12REFON;
    REFCTL0 &= ~REFON;
    REFCTL0 |= REFTCOFF; // Temp sensor disabled

    // Set the High-Power Mode Request Enable bit so LPM3 can be entered
    // with active radio enabled
    PMMCTL0_H = 0xA5;
    PMMCTL0_L |= PMMHPMRE_L;
    PMMCTL0_H = 0x00;

    setup_pins();
    __enable_interrupt();
}

void sleep(uint16_t ms, unsigned short mode) {
    struct Timer_A_initUpModeParam params = {
        TIMER_A_CLOCKSOURCE_ACLK,
        TIMER_A_CLOCKSOURCE_DIVIDER_10,
        ms,
        TIMER_A_TAIE_INTERRUPT_DISABLE,
        TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE,
        TIMER_A_DO_CLEAR,
        true};
    Timer_A_initUpMode(TIMER_A0_BASE, &params);
    __bis_SR_register(mode);
}

void delay(uint16_t ms) {
    sleep(ms, LPM0_bits);
}

void deep_sleep(uint16_t ms) {
    sleep(ms, LPM3_bits);
}

static void blink(const uint16_t ms) {
    P3OUT |= BIT7;
    deep_sleep(50);
    P3OUT &= ~BIT7;
    deep_sleep(ms - 50);
}

static void blink_main() {
    while (1) blink(1000);
}

static void check_power() {
    P1IE = 0;
    if (P1IN & BIT1) {
        // PGOOD high, set interrupt to negative edge
        P1IES = BIT1;
        P1IE = BIT1;
    } else {
        // PGOOD low, set interrupt to positive edge and sleep now
        P1IES = 0;
        P1IE = BIT1;
        LPM4;
    }
}

static void state_main(const uint8_t curr_state) {
    switch (curr_state) {
    case SENSOR_INIT:
        if (lsm_setup())
            uart_write("true\n\r", 6);
        else
            uart_write("false\n\r", 7);
        break;
    case SENSOR_READ:
    {
        uint16_t data_mag[3];
        char str[32];
        uint16_t data_gyro[3];
        readGyro(data_gyro);
        snprintf(str, sizeof(str), "%u, %u, %u\r\n",
                data_gyro[0], data_gyro[1], data_gyro[2]);
        uart_write(str, strlen(str));

        readMag(data_mag);
        snprintf(str, sizeof(str), "%u, %u, %u\r\n",
                data_mag[0], data_mag[1], data_mag[2]);
        uart_write(str, strlen(str));
    }
        break;
    }
}

static uint8_t state_next(const uint8_t curr_state) {
    if (curr_state == SENSOR_READ) {
        return SENSOR_INIT;
    }
    else if (curr_state + 1 != NUM_STATES) {
        return curr_state + 1;
    } else {
        return 0;
    }
}

static void init_debug() {
    blink(200);
    uart_begin(9600, SERIAL_8N1);
    uart_write(VERSION_STR, strlen(VERSION_STR));
}

uint8_t staterec_next; // < index of next free position in staterec_arr or -1
uint8_t staterec_prev; // < previous state
bool staterec_exited;  // < true if previous state was exited cleanly

/**
 * Read state records from flash.
 */
void staterec_read() {
    // Find index of next free byte and store as staterec_next.
    staterec_next = STATEREC_LEN;
    for (uint8_t i = 0; i < STATEREC_LEN; i++) {
        if (staterec_arr[i] == ERASED) {
            staterec_next = i;
            break;
        }
    }

#ifdef SR_DEBUG
    char buf[20];
    snprintf(buf, sizeof(buf), "next free %u\r\n", staterec_next);
    uart_write(buf, sizeof(buf));
#endif

    // If freshly erased, assume last exit was successful.
    if (staterec_next > 0)
        staterec_exited = (staterec_arr[staterec_next - 1] == ST_EXIT);
    else
        staterec_exited = true;

    // If freshly erased, can't tell what last state was.
    if (staterec_exited) {
        staterec_prev =
            (staterec_next - 2 >= 0) ? staterec_arr[staterec_next - 2] : ERASED;
    } else {
        staterec_prev =
            (staterec_next - 1 >= 0) ? staterec_arr[staterec_next - 1] : ERASED;
    }

    // Check if last recorded state is valid. Will occur after programming.
    if (staterec_prev == 0) {
        staterec_prev = ERASED;
    }
}

void staterec_write(uint8_t state) {
    // Check if erase is required.
    if (staterec_next == STATEREC_LEN) {
        check_power();
#ifndef BREAK_FLASH
        FlashCtl_eraseSegment((unsigned char*)staterec_arr);
#endif
        staterec_next = 0;
    }

    uint8_t* dst = (uint8_t*)&staterec_arr[staterec_next];
    check_power();
#ifndef BREAK_FLASH
    FlashCtl_write8(&state, dst, 1);
#endif
    if (*dst != state) {
        // TODO: flash readback failure
        char buf[30];
        snprintf(buf, sizeof(buf), "!! flash write %u failed !!\r\n", *dst);
        uart_write(buf, sizeof(buf));
    } else {
#ifdef SR_DEBUG
        char buf[20];
        snprintf(buf, sizeof(buf), "wrote %u to ind %u\r\n", state,
                 staterec_next);
        uart_write(buf, sizeof(buf));
#endif
        staterec_next += 1;
    }
}

/**
 * Record state entry in flash.
 */
void staterec_enter(uint8_t state) {
    staterec_write(state);
    staterec_prev = state;
    staterec_exited = false;
}

/**
 * Record exit from previously-entered state.
 */
void staterec_finish() {
    staterec_write(ST_EXIT);
    staterec_exited = true;
}

uint8_t planner_next_state() {
#ifdef PLANNER_DEBUG
    char buf[20];
    snprintf(buf, sizeof(buf), "last state %u, %u\r\n", staterec_prev,
             staterec_exited);
    uart_write(buf, strlen(buf));
#endif
    if (staterec_prev == ERASED) {
        return RADIO_INIT;
    } else {
        return state_next(staterec_prev);
    }
}

void planner_main() {
    while (true) {
        uint8_t state = planner_next_state();
        staterec_enter(state);
        check_power();
#ifdef PLANNER_DEBUG
        char buf[20];
        snprintf(buf, sizeof(buf), "running %d\r\n", state);
        uart_write(buf, strlen(buf));
#endif
        state_main(state);
        // TODO: checkpoint data
        staterec_finish();
    }
}

int main() {
    init_core();
#ifdef BLINK
    deep_sleep(600);
    blink_main();
#else
    init_debug();
    staterec_read();
    planner_main();
#endif
}

void __interrupt_vec(TIMER0_A0_VECTOR) isr_timer_a0() {
    LPM3_EXIT;
}

void __interrupt_vec(PORT1_VECTOR) isr_p1() {
    switch (__even_in_range(P1IV, P1IV_P1IFG7)) {
    case P1IV_P1IFG1:
        check_power();
        LPM4_EXIT;
        break;
    case P1IV_P1IFG7:
        readGyroFifo(data_gyro_fifo);
        break;
    default:
        break;
    }
}

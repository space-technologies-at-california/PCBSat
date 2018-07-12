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

struct missioninfo {
    uint8_t state;
    uint8_t reset_count;
};

const volatile struct missioninfo __attribute__ ((section (".infoB"))) info = {};

const char* VERSION_STR = "Spinor DEBUG (" GIT_REV ")\r\n";

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

static void print_state() {
    char buf[20];
    snprintf(buf, sizeof(buf), "state %x\r\n", info.state);
    uart_write(buf, strlen(buf));
    snprintf(buf, sizeof(buf), "counter %x\r\n", info.reset_count);
    uart_write(buf, strlen(buf));
}

static void flash_missioninfo(struct missioninfo* new) {
    FlashCtl_eraseSegment(&info);
    FlashCtl_write32(new, &info, sizeof(struct missioninfo)/4 + 1);
    if (memcmp(&info, new, sizeof(struct missioninfo)) != 0) {
        uart_write("flash MISMATCH\r\n", 16);
    } else {
        uart_write("flash OK\r\n", 10);
    }
}

int main() {
    init_core();
    blink(200);
    uart_begin(9600, SERIAL_8N1);
    uart_write(VERSION_STR, strlen(VERSION_STR));
    print_state();
    struct missioninfo info_next = {info.state, info.reset_count + 1};
    flash_missioninfo(&info_next);
    blink(200);
    check_power();
    blink(200);
    bool lsm_init = lsm_setup();
    blink(200);

    deep_sleep(600);
    uint16_t data_mag[3];
    char str[8];
    uint16_t data_gyro[3];
    delay(1000);
#ifdef BLINK
    blink_main();
#else
//    radio_main();
    if (lsm_init) 
        uart_write("true\n\r", 6);
    else 
        uart_write("false\n\r", 7);

while (1) {
    readGyro(data_gyro);
    snprintf(str, 8, "%l\n\r", data_gyro[0]);
    uart_write(str, strlen(str));
    snprintf(str, 8, "%l\n\r", data_gyro[1]);
    uart_write(str, strlen(str));
    snprintf(str, 8, "%l\n\r", data_gyro[2]);
    uart_write(str, strlen(str));
      
//    readMag(data_mag);

    }
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
    default:
        break;
    }
}

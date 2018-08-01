#include <msp430.h>
#include <stdio.h>
#include <string.h>

#include "battery_mon.h"
#include "cc430uart.h"
#include "drvr.h"
#include "fault.h"
#include "flashctl.h"
#include "gitrev.h"
#include "i2c.h"
#include "lsm.h"
#include "pins.h"
#include "proto.h"
#include "timer_a.h"
#include "ucs.h"

uint8_t faults = FAULT_RECENT_POR;

#define DEC_OR_ZERO(x) x = (x > 0) ? x - 1 : 0;

#ifdef DEBUG
unsigned char counter_debug = 0;
#endif
unsigned char counter_por = 15;
unsigned char counter_tx = 0;
unsigned char counter_lsm = 0;

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

/**
 * Call this once after init_core(). After that, only from the PGOOD ISR.
 */
static void check_power() {
    P1IE &= ~BIT1;
    if (P1IN & BIT1) {
        // PGOOD high, set interrupt to negative edge
        faults &= ~FAULT_POWER;
        P1IES |= BIT1;
        P1IE |= BIT1;
    } else {
        // PGOOD low, set interrupt to positive edge
        faults |= FAULT_POWER;
        P1IES &= ~BIT1;
        P1IE |= BIT1;
    }
}

static void init_debug() {
    blink(200);
    uart_begin(9600, SERIAL_8N1);
    uart_write(VERSION_STR, strlen(VERSION_STR));
}

void tick() {
#ifdef DEBUG
    DEC_OR_ZERO(counter_debug);
#endif
    DEC_OR_ZERO(counter_por);
    DEC_OR_ZERO(counter_tx);
    DEC_OR_ZERO(counter_lsm);

    if (counter_por == 0)
        faults &= ~FAULT_RECENT_POR;
}

bool radio_precond() {
    return !(faults & FAULT_POWER);
}

bool lsm_precond() {
    return !(faults & FAULT_POWER);
}

bool actuate_precond() {
    return lsm_precond() && !(faults & FAULT_RECENT_POR);
}

int main() {
    init_core();
    init_debug();
    check_power();

    while (true) {
        if (counter_tx == 0 && radio_precond()) {
            run_radio();
            counter_tx = 10; // TODO: random delay goes here
        } else if (counter_lsm == 0 && lsm_precond()) {
            run_lsm();
            counter_lsm = 5;
#ifdef DEBUG
        } else if (counter_debug == 0) {
            char buf[32];
            snprintf(buf, sizeof(buf), "faults: 0x%x\r\n", faults);
            uart_write(buf, strlen(buf));
            counter_debug = 2;
#endif
        } else if (actuate_precond()) {
            run_actuation();
        }
        // FIXME: get a real way of timekeeping
        tick();
        deep_sleep(1000);
    }
}

void __interrupt_vec(TIMER0_A0_VECTOR) isr_timer_a0() {
    LPM3_EXIT;
}

void __interrupt_vec(PORT1_VECTOR) isr_p1() {
    switch (__even_in_range(P1IV, P1IV_P1IFG7)) {
    case P1IV_P1IFG1:
        check_power();
        LPM3_EXIT; // exit early from pending delays
        break;
    default:
        break;
    }
}

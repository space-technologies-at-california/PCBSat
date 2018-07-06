#include <msp430.h>

#include "pins.h"
#include "proto.h"
#include "timer_a.h"
#include "ucs.h"

static void init_core() {
    // Disable WDT
    WDTCTL = WDTPW | WDTHOLD;

    // Reference FLL to REFO and set MCLK, SMCLK -> 1 MHz
    UCS_initClockSignal(UCS_FLLREF, UCS_REFOCLK_SELECT, UCS_CLOCK_DIVIDER_1);
    UCS_initFLL(1000, 1000000/UCS_REFOCLK_FREQUENCY);

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
    __bis_SR_register(mode + GIE);
}

void delay(uint16_t ms) {
    sleep(ms, LPM0_bits);
}

void deep_sleep(uint16_t ms) {
    sleep(ms, LPM4_bits);
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

#define BLINK

int main() {
    init_core();
    blink(200);
    blink(200);
    deep_sleep(600);
#ifdef BLINK
    blink_main();
#else
    radio_main();
#endif
}

void __interrupt_vec(TIMER0_A0_VECTOR) isr_timer_a0() {
    LPM4_EXIT;
}

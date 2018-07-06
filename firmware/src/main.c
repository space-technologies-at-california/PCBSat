#include <msp430.h>

#include "hal_pmm.h"
#include "pins.h"
#include "sprite_radio.h"
#include "timer_a.h"
#include "ucs.h"

#define _SET_VCORE_1MHZ(vCore) setVcoreMCLK(vCore, DCORSEL_0, 0x01F)
#define _SET_VCORE_8MHZ(vCore) setVcoreMCLK(vCore, DCORSEL_5, 0x0F4)

/**
 * setMCLK
 *
 * Config MCLK registers
 *
 * @param dcorsel CPU DCORSEL value
 * @param flln MCLK multiplier bits
 */
void __inline__ setMCLK(uint16_t dcorsel, uint16_t flln) {
    /**
     * Configure CPU clock for 8MHz
     */
    _BIS_SR(SCG0);           // Disable the FLL control loop
    UCSCTL0 = 0x0000;        // Set lowest possible DCOx, MODx
    UCSCTL1 = dcorsel;       // Select suitable range
    UCSCTL2 = FLLD_1 + flln; // Set DCO Multiplier
    _BIC_SR(SCG0);           // Enable the FLL control loop

    // Worst-case settling time for the DCO when the DCO range bits have been
    // changed is n x 32 x 32 x f_MCLK / f_FLL_reference. See UCS chapter in 5xx
    // UG for optimization.
    // 32 x 32 x 8 MHz / 32,768 Hz = 250000 = MCLK cycles for DCO to settle
    //__delay_cycles(250000);
    __delay_cycles(0xFFFF);
}

/**
 * setVcoreMCLK
 *
 * Config VCORE and MCLK registers
 *
 * @param vCore VCORE level
 * @param dcorsel CPU DCORSEL value
 * @param flln MCLK multiplier bits
 */
void __inline__ setVcoreMCLK(uint8_t vCore, uint16_t dcorsel, uint16_t flln) {
    // Configure PMM
    SetVCore(vCore);

    // Set MCLK
    setMCLK(dcorsel, flln);
}

void core_radio_init() {
    // Increase PMMCOREV level to 2 for proper radio operation
    _SET_VCORE_8MHZ(2);

    ResetRadioCore();
    radio_init();

    /*
     * Select Interrupt edge for PA_PD and SYNC signal:
     * Interrupt Edge select register: 1 == Interrupt on High to Low transition.
     */
    RF1AIES = BIT0 | BIT9;

    radio_sleep();
    radio_wait_for_sleeping();
}

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

    // Config pins as outputs by default except P2, wich contains the ADC inputs
    P1DIR = 0xFF;
    P3DIR = 0xFF;
    PJDIR = 0xFF;
    INIT_ONBOARD_LED();
}

void radio_main() {
    while (1) {
        __delay_cycles(24000000);
        // Blink LED while transmitter is on
        P3OUT |= BIT7;
        // Serial.println("TX");
        radio_transmit("Hello Earthlings\n", 17);
        P3OUT &= ~BIT7;
    }
}

void delayMicroseconds(int us) {
    for (int i = 0; i < us; i++) {
        __delay_cycles(1);
    }
}

void deep_sleep(uint16_t ms) {
    struct Timer_A_initUpModeParam params = {
        TIMER_A_CLOCKSOURCE_ACLK,
        TIMER_A_CLOCKSOURCE_DIVIDER_10,
        ms,
        TIMER_A_TAIE_INTERRUPT_DISABLE,
        TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE,
        TIMER_A_DO_CLEAR,
        true};
    Timer_A_initUpMode(TIMER_A0_BASE, &params);
    __bis_SR_register(LPM4_bits + GIE);
}

static void blink(const uint16_t ms) {
    P3OUT |= BIT7;
    deep_sleep(50);
    P3OUT &= ~BIT7;
    deep_sleep(ms - 50);
}

void blink_main() {
    while (1) blink(1000);
}

int main() {
    init_core();
    blink_main();

    /*
    radio_main();
    */
}

void __interrupt_vec(TIMER0_A0_VECTOR) isr_timer_a0() {
    LPM4_EXIT;
}

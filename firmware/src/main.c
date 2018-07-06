#include <msp430.h>

#include "hal_pmm.h"
#include "pins.h"
#include "sprite_radio.h"

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

/**
 * init
 *
 * Initialize CC430 core
 * VCORE = 1 and SCLK = 8 MHz when no argument is passed
 *
 * @param vCore VCORE level
 * @param dcorsel CPU DCORSEL value
 * @param flln MCLK multiplier bits
 */
void core_init() {
    // Increase PMMCOREV level to 2 for proper radio operation
    _SET_VCORE_8MHZ(2);

    ResetRadioCore();
    radio_init();

    /*
     * Select internal RC oscillator as FLL reference
     */
    UCSCTL3 |= SELREF_2; // Set DCO FLL reference = REFO
    UCSCTL4 |= SELA_2;   // Set ACLK = REFO

    /*
     * Select Interrupt edge for PA_PD and SYNC signal:
     * Interrupt Edge select register: 1 == Interrupt on High to Low transition.
     */
    RF1AIES = BIT0 | BIT9;

    // POWER: Turn ADC and reference voltage off to conserve power
    ADC12CTL0 &= ~ADC12ENC;
    ADC12CTL0 &= ~ADC12ON;
    ADC12CTL0 &= ~ADC12REFON;
    REFCTL0 &= ~REFON;
    REFCTL0 |= REFTCOFF; // Temp sensor disabled

    radio_sleep();
    radio_wait_for_sleeping();

    // Config pins as outputs by default except P2, wich contains the ADC inputs
    P1DIR = 0xFF;
    P3DIR = 0xFF;
    PJDIR = 0xFF;
}

void blink_main() {
    unsigned long i = 0;
    P3DIR = LED;
    while (1) {
        P3OUT = LED;
        __delay_cycles(100000);
        P3OUT = 0;
        __delay_cycles(100000);
    }
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

static void blink() {
    P3OUT |= BIT7;
    __delay_cycles(1000000);
    P3OUT &= ~BIT7;
    __delay_cycles(1000000);
}

int main() {
    WDTCTL = WDTPW | WDTHOLD;
    INIT_ONBOARD_LED();
    blink();

    core_init();
    __eint();
    blink();

    radio_main();
    // blink_main();
}

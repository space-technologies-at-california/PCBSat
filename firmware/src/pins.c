#include <msp430.h>

#include "pins.h"

void setup_pins() {
    // Config pins as outputs by default except P2, wich contains the ADC inputs
    P1DIR = ~BIT1; // P1.1 is PGOOD
    P1REN = ~BIT1; // No PU/PD on PGOOD
    P3DIR = 0xFF;
    PJDIR = 0xFF;
    P3OUT &= ~BIT7; // Init LED
   
    // Allow port remapping
    PMAPKEYID = PMAPKEY;
    PMAPCTL |= PMAPRECFG;
   
    // Setup PWM stuff
    P2MAP1 = PM_TA1CCR2A; // pinPWMmap
    P1MAP4 = PM_UCB0SCL; P1MAP3 = PM_UCB0SDA; // pinI2Cmap

    // Disallow Port mapping
    PMAPKEYID = 0x0;

    // Setup up pins
    P1SEL |= BIT3 | BIT4; // pinI2cConfig: Set P1.4 and P1.3 to USCI function
#ifdef DEBUG
    P1SEL |= BIT6 | BIT5; // pinUARTconfig: Set P1.6 and P1.5 to USCI Mode
#endif
    P2SEL |= BIT1 | BIT2; // pinPWMconfig
}

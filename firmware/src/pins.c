#include <msp430.h>
#include "pins.h"


/**********************************************************
*                   Setup Pins here
**********************************************************/

void setup_pins() {
    // Config pins as outputs by default except P2, wich contains the ADC inputs
    P1DIR &= ~BIT1 & ~BIT2 & ~BIT7; // P1.1 is PGOOD, P1.2 is DRDY, P1.7 is INT1
    P1REN &= ~BIT1; // No PU/PD on PGOOD
    P1REN &= ~BIT2; // No PU/PD on DRDY
    P1REN |= BIT7;  // PU on INT1
    P1OUT |= BIT7; 
    P3DIR &= 0xFF;
    PJDIR &= 0xFF;
    INIT_ONBOARD_LED();
   
    // Allow port remapping
    PMAPKEYID = PMAPKEY;
    PMAPCTL |= PMAPRECFG;
   
    // Setup PWM stuff
    pinPWMmap();
    pinI2Cmap();

    // Disallow Port mapping
    PMAPKEYID = 0x0;

    // Setup up pins
    pinI2Cconfig();
    pinPWMconfig();
    
}

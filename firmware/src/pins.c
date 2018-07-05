#include <msp430.h>
#include "pins.h"


/**********************************************************
*                   Setup Pins here
**********************************************************/
void setup_pins() {
   
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
    

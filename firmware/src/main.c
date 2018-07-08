#include <msp430.h>
#include "pins.h"
#include "i2c.h"
#include "lsm.h"

int main() {
    WDTCTL = WDTPW | WDTHOLD;
    unsigned long i = 0;
    INIT_ONBOARD_LED();
    setup_pins();
    if (!lsm_setup()) {
        Serial.print("false")
    }
    while (1) {
        P3OUT = LED;
        __delay_cycles(100000);
    
        P3OUT = 0;
        __delay_cycles(100000);
    }
}

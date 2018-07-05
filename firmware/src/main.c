#include <msp430.h>
#include "pins.h"

#define LED BIT7

int main() {
    WDTCTL = WDTPW | WDTHOLD;
    unsigned long i = 0;
    P3DIR = LED;

    while (1) {
        P3OUT = LED;
        __delay_cycles(100000);
        P3OUT = 0;
        __delay_cycles(100000);
    }
}

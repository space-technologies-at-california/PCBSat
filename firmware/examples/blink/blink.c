#include <msp430.h>

#define LED BIT6

int main() {
    WDTCTL = WDTPW | WDTHOLD;
    unsigned long i = 0;
    P1DIR = LED;

    while (1) {
        P1OUT = LED;
        __delay_cycles(100000);
        P1OUT = 0;
        __delay_cycles(100000);
    }
}

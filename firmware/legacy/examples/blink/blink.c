#include <msp430.h>

#define LED BIT7
#define REGD0 BIT0
#define REGD1 BIT6
 
int main() {
    WDTCTL = WDTPW | WDTHOLD;
    unsigned long i = 0;
    P3DIR = LED;
    P3DIR |= REGD1; 
    P1DIR = REGD0;

    while (1) {
        P3OUT ^= LED;
        __delay_cycles(10000000);
        P3OUT ^= LED;
        __delay_cycles(10000000);
    }
}

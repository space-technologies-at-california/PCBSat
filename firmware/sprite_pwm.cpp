#include <msp430.h>

#define PWM_PIN1    0
#define PWM_PIN2    1

int main(void) {

    WDTCTL = WDTPW | WDTHOLD;
    P1DIR |= 0x01;

    for (;;) {
        volatile unsigned int i;

        P1OUT ^= 0x01;

        i = 10000;
        do i--;
        while (i != 0);

    }

    return 0;

}

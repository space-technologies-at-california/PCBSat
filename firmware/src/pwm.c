#include <msp430.h>
#include "pwm.h"

#define PWM_PIN1    0
#define PWM_PIN2    1

void setup_pwm() {

    // Configure PWM pins to be outputs
    P2DIR |= PWM_PIN1 | PWM_PIN2; 

    //Configure TimerA1
    //Use ACLK, Input Divider=1, Mode=Up (count to TA1CCR0)
    TA1CTL |= TASSEL_1 | MC_0;

}

void write_drvr(int drvr, int pwm) {

    // First enable the specific Driver
    switch (pwm) {
        case 1 : 
            P3DIR |= 1;
            break;
    }
}

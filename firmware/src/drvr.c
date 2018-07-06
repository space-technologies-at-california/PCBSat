#include <msp430.h>
#include "drvr.h"
#include "pins.h"

void setup_drvr() {

    // Configure DRVR pins to be outputs
    P2DIR |= BIT1 | BIT2 | BIT3;
    P5DIR |= BIT0 | BIT1;

    //Configure TimerA1
    //Use ACLK, Input Divider=1, Mode=Up (count to TA1CCR0)
    TA1CTL |= TASSEL_1 | MC_0;

}

void drvr_on(int drvr, int pwm) {

    // First enable the specific Driver
    switch (drvr) {
        case XAXIS : 
            P2OUT |= BIT3;
            break;
        case YAXIS : 
            P5OUT |= BIT0;
            break;
        case ZAXIS : 
            P5OUT |= BIT1;
    }

    // Enable the timer
    if (pwm < 0) {
        TA1CCTL1 |= OUTMOD_4;
        TA1CCTL2 |= OUTMOD_7;
        pwm = ~pwm + 1; 
    } else {
        TA1CCTL1 |= OUTMOD_7;
        TA1CCTL2 |= OUTMOD_4;
    }

    TA1CCR0 = 100;
    TA1CCR1 = pwm;
    TA1CCR2 = pwm; 
    TA1CTL |= MC_1 | TACLR;

}

void drvr_off() {

    TA1CTL = TASSEL_1 | MC_0;
    P2OUT &= BIT3;
    P5OUT &= BIT0 | BIT1;

}
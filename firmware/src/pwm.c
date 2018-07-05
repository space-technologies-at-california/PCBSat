#include <msp430.h>
#include "pwm.h"

#define PWM_PIN1    0
#define PWM_PIN2    1

void setup_pwm() {

    P1DIR |= 0xFF;

}

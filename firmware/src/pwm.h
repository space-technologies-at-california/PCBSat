#ifndef __PWM_H
#define __PWM_H

void setup_pwm(void);

void drvr_on(int drvr, int pwm);

void drvr_off(void);

enum Driver {
    XAXIS = 0;
    YAXIS = 1;
    ZAXIS = 2;
}
#endif

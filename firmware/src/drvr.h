#pragma once

enum Driver {
    XAXIS = 0,
    YAXIS = 1,
    ZAXIS = 2,
};

void drvr_off(void);
void drvr_on(int drvr, int pwm);
void run_actuation(void);
void setup_pwm(void);

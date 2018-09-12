#pragma once

enum Driver {
    XAXIS = 0, // EN_3, Spiral inductor
    YAXIS = 1, // EN_2, Back top
    ZAXIS = 2, // EN_1, Back left
};

void run_actuation(void);
void setup_pwm(void);

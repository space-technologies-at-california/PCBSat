#pragma once

enum Driver {
    XAXIS = 0, // EN_3, Spiral inductor
    YAXIS = 1, // EN_2, Back top
    ZAXIS = 2, // EN_1, Back left
};

void run_actuation(uint8_t axis, int8_t power);
void setup_pwm(void);

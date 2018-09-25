#include <stdio.h>

#include "calc.h"

int main() {
    struct vec3_s m_data = {10, 10, 10}; // put fake data here
    struct vec3_s g_data = {10, 10, 10}; // and here
    uint8_t axis;
    int8_t power;
    magnetorquer_out(m_data, g_data, &axis, &power);
    printf("axis: %u, power: %d\n", axis, power);
}

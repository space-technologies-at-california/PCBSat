#pragma once
#include <stdint.h>

#define MOVING_AVG_FILTER_LEN          5

struct vec3_s {
    int16_t x, y, z;
};

/* calc.c */
int16_t moving_average(int16_t sample, int16_t arr[5]);
void setup_filters(int16_t filter[5]);
void vec_cross(struct vec3_s a, struct vec3_s b, struct vec3_s res[static 1]);
int32_t vec_dot(struct vec3_s a, struct vec3_s b);
void magnetorquer_out(struct vec3_s m_data, struct vec3_s g_data, uint8_t *axis, int8_t *power);

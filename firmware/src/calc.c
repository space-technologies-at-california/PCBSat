#include <stdint.h>

#include "calc.h"
#include "drvr.h"
#include "stdio.h"

#define CONTROLLER_GAIN                1000// TODO DOUBLE CHECK VALUE

// MOVING_AVG_FILTER_LEN Point Moving average function
// arr[0] is always most recent sample
int16_t moving_average(int16_t sample, int16_t arr[MOVING_AVG_FILTER_LEN]) {
    // Shift, must happen in reverse or will overwrite each other
    int16_t sum = 0;
    for (unsigned int i = MOVING_AVG_FILTER_LEN - 1; i > 0; i--) {
        arr[i] = arr[i-1];
        sum += arr[i];
    }
    arr[0] = sample;
    sum += sample;
    return sum / MOVING_AVG_FILTER_LEN;
}

void setup_filters(int16_t filter[MOVING_AVG_FILTER_LEN]) {
    uint8_t i;
    for (i = 0; i < MOVING_AVG_FILTER_LEN; i++) {
        filter[i] = 0;
    }
}

uint32_t norm(struct vec3_s *x) {
    return (x->x * x->x) + (x->y * x->y) + (x->z * x-> z);
}

void vec_cross(struct vec3_s a, struct vec3_s b, struct vec3_s res[static 1]) {
    res->x = a.y * b.z - a.z * b.y;
    res->y = a.z * b.x - a.x * b.z;
    res->z = a.x * b.y - a.y * b.x;
}

int32_t vec_dot(struct vec3_s a, struct vec3_s b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

void magnetorquer_out(struct vec3_s m_data, struct vec3_s g_data, uint8_t *axis, int8_t *power) {
    // everything is a integer? but assuming that int32 will be scaler linearly to floats


    int32_t magnetorquer_properties[3] = {100, 100, 200}; // TODO DOUBLE CHECK VALUES using python file

    struct vec3_s unit_dir[3];
    for (int i = 0; i < 3; i++) {
        unit_dir[i].x = 0;
        unit_dir[i].y = 0;
        unit_dir[i].z = 0;
    }
    unit_dir[0].x = 1;
    unit_dir[1].y = 1;
    unit_dir[2].z = 1;

    int32_t similarity[3];
    for (int i = 0; i < 3; i++) {
        struct vec3_s angular_acceleration;
        vec_cross(unit_dir[i], m_data, &angular_acceleration);
        similarity[i] = vec_dot(angular_acceleration, g_data) * magnetorquer_properties[i];
    }

    int32_t sx = similarity[0] * similarity[0];
    int32_t sy = similarity[1] * similarity[1];
    int32_t sz = similarity[2] * similarity[2];

    if (sx > sy && sx > sz) {
        *axis = XAXIS;
        *power = similarity[0] / CONTROLLER_GAIN;
    }
    if (sy > sx && sy > sz) {
        *axis = YAXIS;
        *power = similarity[1] / CONTROLLER_GAIN;
    }
    if (sz > sx && sz > sy) {
        *axis = ZAXIS;
        *power = similarity[2] / CONTROLLER_GAIN;
    }
}

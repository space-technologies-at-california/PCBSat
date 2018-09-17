#include <msp430.h>
#include <stdio.h>
#include <string.h>

#include "cc430uart.h"
#include "fault.h"
#include "i2c.h"
#include "lsm.h"
#include "drvr.h"
#include "proto.h"

static int16_t mag_x_arr[MOVING_AVG_FILTER_LEN];
static int16_t mag_y_arr[MOVING_AVG_FILTER_LEN];
static int16_t mag_z_arr[MOVING_AVG_FILTER_LEN];
static int16_t gyro_x_arr[MOVING_AVG_FILTER_LEN];
static int16_t gyro_y_arr[MOVING_AVG_FILTER_LEN];
static int16_t gyro_z_arr[MOVING_AVG_FILTER_LEN];

// MOVING_AVG_FILTER_LEN Point Moving average function
// arr[0] is always most recent sample
static int16_t moving_average(int16_t sample, int16_t arr[MOVING_AVG_FILTER_LEN]) {
    uint8_t i;
    int32_t sum = 0;
    for (i = 1; i < MOVING_AVG_FILTER_LEN; i++) {
        arr[i] = arr[i-1];
        sum += arr[i];
    }
    arr[0] = sample;
    sum += sample;
    return (int16_t) (sum / MOVING_AVG_FILTER_LEN);
}

static void setup_filters(int16_t filter[MOVING_AVG_FILTER_LEN]) {
    uint8_t i;
    for (i = 0; i < MOVING_AVG_FILTER_LEN; i++) {
        filter[i] = 0;
    }
}

bool lsm_setup() {
    setup_filters(mag_x_arr);
    setup_filters(mag_y_arr);
    setup_filters(mag_z_arr);
    setup_filters(gyro_x_arr);
    setup_filters(gyro_y_arr);
    setup_filters(gyro_z_arr);

    uint8_t id;
    i2c_read_buff(SADDR_G, LSM_REG_WHO_AM_I_XG, 1, &id);
    if (id != LSM_XG_ID)
        return false;

    i2c_read_buff(SADDR_M, LSM_REG_WHO_AM_I_M, 1, &id);
    if (id != LSM_MAG_ID)
        return false;

    // enable gyro continuous
    i2c_write8(SADDR_G, LSM_REG_CTRL_REG1_G, 0x40); // on XYZ

    // Setup Mag
    uint8_t reg;
    i2c_read_buff(SADDR_M, LSM_REG_CTRL_REG2_M, 1, &reg);

    reg &= ~(0b01100000);
    i2c_write8(SADDR_M, LSM_REG_CTRL_REG2_M, reg);
    i2c_write8(SADDR_M, LSM_REG_CTRL_REG1_M, 0x94);     // Temp Comp, ODR = 20Hz
    i2c_write8(SADDR_M, LSM_REG_CTRL_REG5_M, 0x40);     // Block data update (BDU)
    i2c_write8(SADDR_M, LSM_REG_CTRL_REG3_M, 0x00);

    // Setup Gyro
    i2c_read_buff(SADDR_G, LSM_REG_CTRL_REG1_G, 1, &reg);

    reg &= ~(0b00011000);
    reg |= LSM_GYRO_SCALE;
    i2c_write8(SADDR_G, LSM_REG_CTRL_REG1_G, reg);
    i2c_write8(SADDR_G, LSM_REG_CTRL_REG2_G, 0x02);
    i2c_write8(SADDR_G, LSM_REG_CTRL_REG3_G, 0x40);

    return true;
}

void readGyro(struct vec3_s data[static 1]) {
    uint8_t buffer[6];
    i2c_read_buff(SADDR_G, 0x80 | LSM_REG_OUT_X_L_G, 6, buffer);
        
    data->x = moving_average((int16_t)((buffer[1] << 8) | buffer[0]), gyro_x_arr);
    data->y = moving_average((int16_t)((buffer[3] << 8) | buffer[2]), gyro_y_arr);
    data->z = moving_average((int16_t)((buffer[5] << 8) | buffer[4]), gyro_z_arr);
}

void readMag(struct vec3_s data[static 1]) {
    uint8_t buffer[6];
    i2c_read_buff(SADDR_M, 0x80 | LSM_REG_OUT_X_L_M, 6, buffer);

    data->x = moving_average((int16_t)((buffer[1] << 8) | buffer[0]), mag_x_arr);
    data->y = moving_average((int16_t)((buffer[3] << 8) | buffer[2]), mag_y_arr);
    data->z = moving_average((int16_t)((buffer[5] << 8) | buffer[4]), mag_z_arr);
}

uint8_t pick_torquer() {
    struct vec3_s data;
    readMag(&data);
    uint32_t magx = data.y*data.y + data.z*data.z;
    uint16_t magy = data.x*data.x + data.z*data.z;
    uint16_t magz = data.x*data.x + data.y*data.y;
    magz = magz / (10000);
    if (magx > magy && magx > magz) return XAXIS;
    if (magy > magx && magy > magz) return YAXIS;
    if (magz > magx && magz > magy) return ZAXIS;
    return XAXIS;
}


void vec_cross(struct vec3_s a, struct vec3_s b, struct vec3_s res[static 1]) {
    res->x = a.y * b.z - a.z * b.y;
    res->y = a.z * b.x - a.x * b.z;
    res->z = a.x * b.y - a.y * b.x;
}

int32_t vec_dot(struct vec3_s a, struct vec3_s b) {
    return a.x * b.x + a.y + b.y + a.z * b.z;
}


void magnetorquer_out(uint8_t *axis, int8_t *power) {
    // everything is a integer? but assuming that int32 will be scaler linearly to floats

    struct vec3_s m_data;
    readMag(&m_data);
    struct vec3_s g_data;
    readGyro(&g_data);

    uint32_t magnetorquer_properties = {100, 100, 200};

    struct vec3_s unit_dir[3];
    for (int i = 0; i < 3, i++) {
        unit_dir[i].x = 0;
        unit_dir[i].y = 0;
        unit_dir[i].z = 0;
    }
    unit_dir[0].x = 1;
    unit_dir[1].y = 1;
    unit_dir[2].z = 1;

    uint32_t similarity[3];
    for (int i = 0; i < 3, i++) {
        struct vec3_s angular_acceleration;
        vec_cross(unit_dir[0], b, &angular_acceleration);
        similarity[i] = vec_dot(angular_acceleration, g_data) * magnetorquer_properties[i];
    }

    uint32_t sx = similarity[0] * similarity[0];
    uint32_t sy = similarity[1] * similarity[1];
    uint32_t sz = similarity[2] * similarity[2];

    if (sx > sy && sx > sz) {
        &axis = XAXIS;
        &power = similarity[0] * CONTROLLER_GAIN;
    }
    if (sy > sx && sy > sz) {
        &axis = YAXIS;
        &power = similarity[1] * CONTROLLER_GAIN;
    }
    if (sz > sx && sz > sy) {
        &axis = ZAXIS;
        &power = similarity[2] * CONTROLLER_GAIN;
    }
}

void run_lsm(struct vec3_s *data) {
    static bool has_lsm_setup = false;
    if (!has_lsm_setup) {
        if (!lsm_setup()) {
#ifdef DEBUG
            uart_write("lsm setup failed\n\r", 18);
#endif
            faults |= FAULT_LSM_SETUP;
            return;
        }
        has_lsm_setup = true;
    }

    char str[30];
    struct vec3_s data_gyro_init;
    readGyro(&data_gyro_init);
    uint8_t i;
    for (i = 0; i < 100; i++) {
        readGyro(&global_omega);
        sleep(50, LPM3_bits);
    }
    struct vec3_s data_gyro_final;
    readGyro(&data_gyro_final);
    data->x = data_gyro_final.x - data_gyro_init.x;
    data->y = data_gyro_final.y - data_gyro_init.y;
    data->z = data_gyro_final.z - data_gyro_init.z;
#ifdef DEBUG
    snprintf(str, sizeof(str), "%d, %d, %d\r\n", data->x, data->y, data->z);
    uart_write(str, strlen(str));
#endif
}

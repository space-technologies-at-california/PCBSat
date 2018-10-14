#include <msp430.h>
#include <stdio.h>
#include <string.h>

#include "config.h"

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
    char str[30];
    snprintf(str, sizeof(str), "delta gyro %d, %d, %d\r\n", data->x, data->y, data->z);
    uart_write(str, strlen(str));
#endif
}

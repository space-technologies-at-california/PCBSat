#include <msp430.h>
#include <stdio.h>
#include <string.h>

#include "cc430uart.h"
#include "fault.h"
#include "i2c.h"
#include "lsm.h"

bool lsm_setup() {
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
    reg |= LSM_MAG_GAIN; //| (1 << 3);                     // Apparently reboot is
                                                        // required, fuck ST
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
    char str[6];
    i2c_read_buff(SADDR_G, 0x80 | LSM_REG_OUT_X_L_G, 6, buffer);
        
    data->x = (buffer[1] << 8) | buffer[0];
    data->y = (buffer[3] << 8) | buffer[2];
    data->z = (buffer[5] << 8) | buffer[4];
}

void readMag(struct vec3_s data[static 1]) {
    uint8_t buffer[6];
    i2c_read_buff(SADDR_M, 0x80 | LSM_REG_OUT_X_L_M, 6, buffer);

    data->x = (buffer[1] << 8) | buffer[0];
    data->y = (buffer[3] << 8) | buffer[2];
    data->z = (buffer[5] << 8) | buffer[4];
}

void run_lsm() {
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
    struct vec3_s data_gyro;
    readGyro(&data_gyro);
    snprintf(str, sizeof(str), "%d, %d, %d\r\n",
            (int16_t) data_gyro.x, (int16_t) data_gyro.y, (int16_t) data_gyro.z);
    uart_write(str, strlen(str));

    struct vec3_s data_mag;
    readMag(&data_mag);
    snprintf(str, sizeof(str), "%d, %d, %d\r\n",
            (int16_t) data_mag.x, (int16_t) data_mag.y, (int16_t) data_mag.z);
    uart_write(str, strlen(str));
}

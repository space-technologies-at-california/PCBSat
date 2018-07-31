#include <msp430.h>
#include <stdio.h>
#include <string.h>

#include "cc430uart.h"
#include "fault.h"
#include "i2c.h"
#include "lsm.h"

uint16_t data_gyro_fifo[96];
uint16_t data_mag[3];

bool lsm_setup() {
    P3OUT ^= BIT7;
    // soft reset & reboot accel/gyro
    i2c_write8(SADDR_G, LSM_REG_CTRL_REG8, 0x05);
    P3OUT ^= BIT7;
    // soft reset & reboot magnetometer
    i2c_write8(SADDR_M, LSM_REG_CTRL_REG2_M, 0x0C);    

    uint8_t id; 
    i2c_read_buff(SADDR_G, LSM_REG_WHO_AM_I_XG, 1, &id);

    
    if (id != LSM_XG_ID)
        return false;

    i2c_read_buff(SADDR_M, LSM_REG_WHO_AM_I_M, 1, &id);

    if (id != LSM_MAG_ID)
        return false;

    // enable gyro continuous
    i2c_write8(SADDR_G, LSM_REG_CTRL_REG1_G, 0x40);     // ODR = 59.5Hz

    // Setup Mag
    uint8_t reg;
    i2c_read_buff(SADDR_M, LSM_REG_CTRL_REG2_M, 1, &reg);

    reg &= ~(0b01100000);
    reg |= LSM_MAG_GAIN;
    i2c_write8(SADDR_M, LSM_REG_CTRL_REG2_M, reg);

    i2c_write8(SADDR_M, LSM_REG_CTRL_REG1_M, 0x94);     // Temp Comp, ODR = 20Hz
    i2c_write8(SADDR_M, LSM_REG_CTRL_REG3_M, 0x1);      // ADC single conversion
    i2c_write8(SADDR_M, LSM_REG_CTRL_REG5_M, 0x40);     // Block data update (BDU)

    // Setup Gyro
    i2c_read_buff(SADDR_G, LSM_REG_CTRL_REG1_G, 1, &reg);

    reg &= ~(0b00011000);
    reg |= LSM_GYRO_SCALE;
    i2c_write8(SADDR_G, LSM_REG_CTRL_REG1_G, reg);

    i2c_write8(SADDR_G, LSM_REG_CTRL_REG3_G, 0x80);
    i2c_write8(SADDR_G, LSM_REG_CTRL_REG8, 0x74);
    i2c_write8(SADDR_G, LSM_REG_CTRL_REG9, 0x0A);

    i2c_write8(SADDR_G, LSM_FIFO_CTRL, 0xC0);

    // Enable interrupts
    P1IE |= BIT2 | BIT7;
    return true;
}

void readGyro(uint16_t* data) {

    // Read gyro
    uint8_t buffer[6];
    char str[6];
    i2c_read_buff(SADDR_G, 0x80 | LSM_REG_OUT_X_L_G, 6, buffer);
        
    uint16_t x = (buffer[1] << 8) | buffer[0];
    uint16_t y = (buffer[3] << 8) | buffer[2];
    uint16_t z = (buffer[5] << 8) | buffer[4];

    data[0] = x;
    data[1] = y;
    data[2] = z;

}

void readMag(uint16_t* data) {

    // Read the magnetometer
    uint8_t buffer[6];
    i2c_read_buff(SADDR_M, 0x80 | LSM_REG_OUT_X_L_M, 6, buffer);

    uint16_t x = (buffer[1] << 8) | buffer[0];
    uint16_t y = (buffer[3] << 8) | buffer[2];
    uint16_t z = (buffer[5] << 8) | buffer[4];


    data[0] = x;
    data[1] = y;
    data[2] = z;

}

void run_lsm() {
#ifdef DEBUG
    uart_write("read lsm\n\r", 10);
#endif

    if (!lsm_setup()) {
#ifdef DEBUG
        uart_write("lsm setup failed\n\r", 18);
#endif
        faults |= FAULT_LSM_SETUP;
        return;
    }

    char str[32];
    if (data_gyro_fifo[0] && data_gyro_fifo[1] && data_gyro_fifo[2]) {
        snprintf(str, sizeof(str), "%u, %u, %u\r\n",
                data_gyro_fifo[0], data_gyro_fifo[1], data_gyro_fifo[2]);
        uart_write(str, strlen(str));
        memset(data_gyro_fifo, 0, sizeof(data_gyro_fifo));
    }

    if (data_mag[0] && data_mag[1] && data_mag[2]) {
        snprintf(str, sizeof(str), "%u, %u, %u\r\n",
                data_mag[0], data_mag[1], data_mag[2]);
        uart_write(str, strlen(str));
        memset(data_mag, 0, sizeof(data_mag));
    }
}

void readGyroFifo(uint16_t* data) {
    int i;
    for (i = 0; i < 32; i++) {
        readGyro(data + 3*i);
    }
}

void lsm_off(void) {
    i2c_write8(SADDR_G, LSM_REG_CTRL_REG1_G, 0x00);     // Turn off XL and G
    i2c_write8(SADDR_G, LSM_REG_CTRL_REG3_M, 0x03);     // Turn off M
}

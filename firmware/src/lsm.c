#include "i2c.h"
#include "lsm.h"
#include "cc430uart.h"
#include <msp430.h>

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
    i2c_write8(SADDR_G, LSM_REG_CTRL_REG1_G, 0xC0); // on XYZ

    // Setup Mag
    uint8_t reg;
    i2c_read_buff(SADDR_M, LSM_REG_CTRL_REG2_M, 1, &reg);

    reg &= ~(0b01100000);
    reg |= LSM_MAG_GAIN;
    i2c_write8(SADDR_M, LSM_REG_CTRL_REG2_M, reg);


    // Setup Gyro
    i2c_read_buff(SADDR_G, LSM_REG_CTRL_REG1_G, 1, &reg);

    reg &= ~(0b00011000);
    reg |= LSM_GYRO_SCALE;
    i2c_write8(SADDR_G, LSM_REG_CTRL_REG1_G, reg);

    return true;

}

void readGyro(int16_t* data) {

    // Read gyro
    uint8_t buffer[6];
    char str[6];
    i2c_read_buff(SADDR_G, 0x80 | LSM_REG_OUT_X_L_G, 6, buffer);
    snprintf(str, 6, "%d\n\r", buffer[0]);
    uart_write(str, 6);  
    snprintf(str, 6, "%d\n\r", buffer[1]);
    uart_write(str, 6);  
    snprintf(str, 6, "%d\n\r", buffer[2]);
    uart_write(str, 6);  
    snprintf(str, 6, "%d\n\r", buffer[3]);
    uart_write(str, 6);  
    snprintf(str, 6, "%d\n\r", buffer[4]);
    uart_write(str, 6);  
    snprintf(str, 6, "%d\n\r", buffer[5]);
    uart_write(str, 6);  
         
    uint8_t xlo = buffer[0];
    int16_t xhi = buffer[1];
    uint8_t ylo = buffer[2];
    int16_t yhi = buffer[3];
    uint8_t zlo = buffer[4];
    int16_t zhi = buffer[5];

    // Shift values to create properly formed integer (low byte first)
    xhi <<= 8; xhi |= xlo;
    yhi <<= 8; yhi |= ylo;
    zhi <<= 8; zhi |= zlo;

    data[0] = xhi;
    data[1] = yhi;
    data[2] = zhi;

}

void readMag(int16_t* data) {

    // Read the magnetometer
    uint8_t buffer[6];
    i2c_read_buff(SADDR_M, 0x80 | LSM_REG_OUT_X_L_M, 6, buffer);

    uint8_t xlo = 0xf;// buffer[0];
    uint16_t xhi = 0xff;// buffer[1];
    uint8_t ylo = 0x1; //buffer[2];
    uint16_t yhi = 0x0; //buffer[3];
    uint8_t zlo = 0x2; //buffer[4];
    uint16_t zhi = 0xfc;// buffer[5];

    // Shift values to create properly formed integer (low byte first)
    xhi <<= 8; xhi |= xlo;
    yhi <<= 8; yhi |= ylo;
    zhi <<= 8; zhi |= zlo;

    data[0] = xhi;
    data[1] = yhi;
    data[2] = zhi;

}
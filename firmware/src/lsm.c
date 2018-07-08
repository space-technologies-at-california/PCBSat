#include "i2c.h"
#include "lsm.h"

bool lsm_setup() {
    // soft reset & reboot accel/gyro
    write8(SADDR_G, LSM_REG_CTRL_REG8, 0x05);
    // soft reset & reboot magnetometer
    write8(SADDR_M, LSM_REG_CTRL_REG2_M, 0x0C);    
    uint8_t id; 
    readBuff(SADDR_G, LSM_REG_WHO_AM_I_XG, 1, &id);
    
    if (id != LSM_XG_ID)
        return false;

    readBuff(SADDR_M, LSM_REG_WHO_AM_I_M, 1, &id);
    if (id != LSM_MAG_ID)
        return false;

    // enable gyro continuous
    write8(SADDR_G, LSM_REG_CTRL_REG1_G, 0xC0); // on XYZ

    // Setup Mag
    uint8_t reg;
    readBuff(SADDR_M, LSM_REG_CTRL_REG2_M, 1, &reg);
    reg &= ~(0b01100000);
    reg |= LSM_MAG_GAIN;
    write8(SADDR_M, LSM_REG_CTRL_REG2_M, reg);

    // Setup Gyro
    readBuff(SADDR_G, LSM_REG_CTRL_REG1_G, 1, &reg);
    reg &= ~(0b00011000);
    reg |= LSM_GYRO_SCALE;
    write8(SADDR_G, LSM_REG_CTRL_REG1_G, reg);

    return true;

}

void readGyro(int16_t* data) {

    // Read gyro
    uint8_t buffer[6];
    readBuff(SADDR_G, 0x80 | LSM_REG_OUT_X_L_G, 6, buffer);
  
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
    readBuff(SADDR_M, 0x80 | LSM_REG_OUT_X_L_M, 6, buffer);

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

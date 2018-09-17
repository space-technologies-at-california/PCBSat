#pragma once

#include <stdbool.h>
#include <stdint.h>

#define LSM_XG_ID                      (0b01101000)
#define LSM_MAG_ID                     (0b00111101)
#define LSM_MAG_GAIN                   (0b00 << 5)  // +/- 4 gauss
#define LSM_GYRO_SCALE                 (0b00 << 3)  // +/- 245 degrees per second rotation

#define MOVING_AVG_FILTER_LEN          5
#define CONTROLLER_GAIN                10000
typedef enum
{
  LSM_REG_WHO_AM_I_XG         = 0x0F,
  LSM_REG_CTRL_REG1_G         = 0x10,
  LSM_REG_CTRL_REG2_G         = 0x11,
  LSM_REG_CTRL_REG3_G         = 0x12,
  LSM_REG_TEMP_OUT_L          = 0x15,
  LSM_REG_TEMP_OUT_H          = 0x16,
  LSM_REG_STATUS_REG          = 0x17,
  LSM_REG_OUT_X_L_G           = 0x18,
  LSM_REG_OUT_X_H_G           = 0x19,
  LSM_REG_OUT_Y_L_G           = 0x1A,
  LSM_REG_OUT_Y_H_G           = 0x1B,
  LSM_REG_OUT_Z_L_G           = 0x1C,
  LSM_REG_OUT_Z_H_G           = 0x1D,
  LSM_REG_CTRL_REG4           = 0x1E,
  LSM_REG_CTRL_REG5_XL        = 0x1F,
  LSM_REG_CTRL_REG6_XL        = 0x20,
  LSM_REG_CTRL_REG7_XL        = 0x21,
  LSM_REG_CTRL_REG8           = 0x22,
  LSM_REG_CTRL_REG9           = 0x23,
  LSM_REG_CTRL_REG10          = 0x24,

  LSM_REG_OUT_X_L_XL          = 0x28,
  LSM_REG_OUT_X_H_XL          = 0x29,
  LSM_REG_OUT_Y_L_XL          = 0x2A,
  LSM_REG_OUT_Y_H_XL          = 0x2B,
  LSM_REG_OUT_Z_L_XL          = 0x2C,
  LSM_REG_OUT_Z_H_XL          = 0x2D,

} LSMAccGyroRegisters_t;

typedef enum
{

  LSM_REG_WHO_AM_I_M         = 0x0F,
  LSM_REG_CTRL_REG1_M        = 0x20,
  LSM_REG_CTRL_REG2_M        = 0x21,
  LSM_REG_CTRL_REG3_M        = 0x22,
  LSM_REG_CTRL_REG4_M        = 0x23,
  LSM_REG_CTRL_REG5_M        = 0x24,
  LSM_REG_STATUS_REG_M       = 0x27,
  LSM_REG_OUT_X_L_M          = 0x28,
  LSM_REG_OUT_X_H_M          = 0x29,
  LSM_REG_OUT_Y_L_M          = 0x2A,
  LSM_REG_OUT_Y_H_M          = 0x2B,
  LSM_REG_OUT_Z_L_M          = 0x2C,
  LSM_REG_OUT_Z_H_M          = 0x2D,
  LSM_REG_CFG_M              = 0x30,
  LSM_REG_INT_SRC_M          = 0x31,
} LSMMagRegisters_t;

typedef enum
{
  LSM_MAGDATARATE_3_125HZ          = (0b000 << 2),
  LSM_MAGDATARATE_6_25HZ           = (0b001 << 2),
  LSM_MAGDATARATE_12_5HZ           = (0b010 << 2),
  LSM_MAGDATARATE_25HZ             = (0b011 << 2),
  LSM_MAGDATARATE_50HZ             = (0b100 << 2),
  LSM_MAGDATARATE_100HZ            = (0b101 << 2)
} LSMMagDataRate_t;

struct vec3_s {
    int16_t x, y, z;
};

bool lsm_setup(void);

void readGyro(struct vec3_s[static 1]);

void readMag(struct vec3_s[static 1]);

uint8_t pick_torquer(void);

void magnetorquer_out(uint8_t *axis, int8_t *power);

void run_lsm(struct vec3_s *data);

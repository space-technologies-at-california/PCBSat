/**************************
 *      I2C include
 */

#ifndef _I2C_H
#define _I2C_H

#include <stdbool.h>
#include <stdint.h>

#define I2C_TIMEOUT     0xFFF
#define SADDR_G         0x6B
#define SADDR_M         0x1E
#define BUFF_LEN        32

static uint8_t tx_buff[BUFF_LEN];
static uint8_t tx_buff_len;

static uint8_t rx_buff[BUFF_LEN];
static uint8_t rx_buff_len;

/**
 * beginTransmission
 * 
 * Start I2C transaction
 *
 * @param slaAddr I2C slave address
 */
void beginTransmission(uint16_t slaveAddr);

/**
 * write
 * 
 * Send data to I2C slave
 *
 * @param buf Buffer to be sent
 * @param len Amount of bytes to be transferred
 * @param stop If true, transmit stop condition
 *
 * @return Amount of bytes transmitted
 */
uint16_t write(const uint8_t *buf, uint16_t len, uint16_t slaveAddr, bool stop);

void write8(uint8_t saddr, uint8_t reg, uint8_t val);

/**
 * read
 * 
 * Receive data from I2C slave
 *
 * @param buf Buffer received
 * @param len Amount of bytes to be read
 * @param stop If true, transmit stop condition
 *
 * @return Amount of bytes received
 */
uint16_t read(uint8_t *buf, uint16_t len, uint16_t slaveAddr, bool stop);

uint8_t readBuff(uint8_t saddr, uint8_t reg, uint8_t len, uint8_t *buffer);

uint8_t requestFrom(uint8_t saddr, uint16_t quantity, bool sendStop);
#endif


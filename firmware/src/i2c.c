/***********************************
 *       I2C code for PCBSat
 */
#include <msp430.h>

#include "i2c.h"

static uint8_t tx_buff[BUFF_LEN];
static uint8_t tx_buff_len;

static uint8_t rx_buff[BUFF_LEN];
static uint8_t rx_buff_len;

// Start I2C transaction/
void i2c_begin_transmission(uint16_t slaveAddr) {
  UCB0CTL1 |= UCSWRST;                    // Enable SW reset
  UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;   // I2C Master, synchronous mode
  UCB0CTL1 = UCSSEL_2 + UCSWRST;          // Use SMCLK
  UCB0BR0 = 120;                          // fSCL = SMCLK/120 = ~100kHz
  UCB0BR1 = 0;
  UCB0I2CSA = slaveAddr;                    // Slave Address
  UCB0CTL1 &= ~UCSWRST;                   // Clear SW reset, resume operation
  UCB0IFG = 0;                            // Reset interrupt flags
}

// I2C write
uint16_t i2c_write(const uint8_t *buf, uint16_t len, uint16_t slaveAddr, bool stop) {
  uint16_t i, res = len;
  uint32_t timeout;

  UCB0I2CSA = slaveAddr;
  do {
    UCB0IFG = 0;                         // Reset interrupt flags
    UCB0CTL1 |= UCTR + UCTXSTT;          // Send start condition and slave address
  
    while (!(UCB0IFG & UCTXIFG));       // Wait until start condition is sent

  } while (UCB0IFG & UCNACKIFG);   // If slave does not ack, try again 
     
  // Send bytes
  for(i=0 ; i<len ; i++) {
    timeout = I2C_TIMEOUT;
    UCB0TXBUF = buf[i];
    // Wait until each data byte is sent
    while ((!(UCB0IFG & UCTXIFG)) && --timeout);
    if (!timeout)
      res = 0;
  }
  
  if (stop) {
    UCB0CTL1 |= UCTXSTP;               // I2C stop condition
    while ((UCB0CTL1 & UCTXSTP));     // Ensure stop condition got sent
  }

  return res;
}

// I2C Read
uint16_t i2c_read(uint8_t *buf, uint16_t len, uint16_t slaveAddr, bool stop) 
{
  uint16_t i, res = len;
  uint32_t timeout;

  buf[0] = UCB0RXBUF;                   // Clear UCRXIFG
  UCB0I2CSA = slaveAddr;
  UCB0IFG = 0;                          // Reset interrupt flags
  UCB0CTL1 &= ~UCTR;                    // Receiver mode
  UCB0CTL1 |= UCTXSTT;                  // Send start condition

  while ((UCB0IFG & UCTXSTT));         // Wait until start condition is sent

  // For every byte
  for(i=0 ; i<len ; i++) {
    timeout = I2C_TIMEOUT;
    // Poll for UCRXIFG event to occur    
    while ((!(UCB0IFG & UCRXIFG)) && --timeout);
    buf[i] = UCB0RXBUF;                 // Read byte
  }

  if (stop) {
    UCB0CTL1 |= UCTXSTP;                // I2C stop condition
    while ((UCB0CTL1 & UCTXSTP));      // Ensure stop condition got sent
  }

  return res;
}

void i2c_write8(uint8_t saddr, uint8_t reg, uint8_t val) {
    i2c_begin_transmission(saddr);
    tx_buff[0] = reg;
    tx_buff[1] = val;
    uint8_t res = i2c_write(tx_buff, 2, saddr, true);
    tx_buff_len = 0;
}

uint8_t i2c_read_buff(uint8_t saddr, uint8_t reg, uint8_t len, uint8_t *buffer) {
    i2c_begin_transmission(saddr);
    tx_buff[0] = reg;
    i2c_write(tx_buff, 1, saddr, true);

    if (i2c_request_from(saddr, len, true) != len) {
        return 0;
    }
    if (len > BUFF_LEN)
        len = BUFF_LEN;
    int i;
    for (i=0; i < len; i++) {
        buffer[i] = rx_buff[i];
    }
    return len;
} 

uint8_t i2c_request_from(uint8_t address, uint16_t quantity, bool sendStop)
{
  // clamp to buffer length
  if(quantity > BUFF_LEN)
    quantity = BUFF_LEN;

  // perform blocking read into buffer
  uint16_t res = i2c_read(rx_buff, quantity, address, sendStop);
  // set rx buffer iterator vars
  rx_buff_len = res;

  return res;
}

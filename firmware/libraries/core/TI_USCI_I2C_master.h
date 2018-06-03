#ifndef USCI_LIB
#define USCI_LIB

#define I2C_PRESCALE 12

#include "cc430f5137.h"
#include "signal.h"  //needed for interrupt syntax with mspgcc4

#ifdef __cplusplus
extern "C" {
#endif

void TI_USCI_I2C_receiveinit(unsigned char slave_address, unsigned char prescale);
void TI_USCI_I2C_transmitinit(unsigned char slave_address, unsigned char prescale);
void TI_USCI_I2C_receive(unsigned char *field, unsigned char byte_count);
void TI_USCI_I2C_transmit(unsigned char *field, unsigned char byte_count);

#ifdef __cplusplus
}
#endif

#endif

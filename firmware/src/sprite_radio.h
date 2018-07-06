/*
  SpriteRadio.h - An Energia library for transmitting data using the CC430
  series of devices

  by Zac Manchester

*/

#ifndef SpriteRadio_h
#define SpriteRadio_h

#define SR_DEBUG_MODE 1

#define PRN_LENGTH_BYTES 64

#include "RF1A.h"

/* cc430radio.c */
void writeTXBuffer(unsigned char *data, unsigned char length);
void writeTXBufferZeros(unsigned char length);
void readRXBuffer(unsigned char *data, unsigned char length);
/* sprite_radio.c */
void radio_set_power(int tx_power_dbm);
char fecEncode(char data);
void radio_transmit(char bytes[], unsigned int length);
void transmitByte(char byte);
void rawTransmit(unsigned char bytes[], unsigned int length);
void beginRawTransmit(unsigned char bytes[], unsigned int length);
void continueRawTransmit(unsigned char bytes[], unsigned int length);
void endRawTransmit(void);
void radio_sleep(void);
void radio_init(void);
void radio_wait_for_sleeping(void);

#endif // SpriteRadio_h

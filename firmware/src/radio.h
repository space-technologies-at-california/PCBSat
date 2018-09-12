/*
  SpriteRadio.h - An Energia library for transmitting data using the CC430
  series of devices

  by Zac Manchester

*/

#pragma once

#define SR_DEBUG_MODE 1
#define PRN_LENGTH_BYTES 64

#include "RF1A.h"

/* cc430radio.c */
void writeTXBuffer(unsigned char *data, unsigned char length);
void writeTXBufferZeros(unsigned char length);
void readRXBuffer(unsigned char *data, unsigned char length);

/* radio.c */
static void radio_set_power(int tx_power_dbm);
static char fecEncode(char data);
static void radio_transmit(char bytes[], unsigned int length);
static void transmitByte(char byte);
static void beginRawTransmit(unsigned char bytes[], unsigned int length);
static void continueRawTransmit(unsigned char bytes[], unsigned int length);
static void endRawTransmit(void);
static void wait_for_idle(void);
static void init_radio(void);

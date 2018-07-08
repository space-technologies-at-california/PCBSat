/*
  cc430radio.h - A low-level interface library for the CC1101 radio core in the
  CC430 series of devices.

  Adapted from the CC430 RF Examples from TI:
  http://www.ti.com/lit/an/slaa465b/slaa465b.pdf

  by Zac Manchester

*/

#include <msp430.h>

// Write data to the transmit FIFO buffer. Max length is 64 bytes.
void writeTXBuffer(unsigned char* data, unsigned char length) {

    // Write Burst works wordwise not bytewise - known errata
    unsigned char i;

    while (!(RF1AIFCTL1 & RFINSTRIFG))
        ; // Wait for the Radio to be ready for next instruction
    RF1AINSTRW =
        ((RF_TXFIFOWR | RF_REGWR) << 8) + data[0]; // Send address + instruction

    for (i = 1; i < length; i++) {
        RF1ADINB = data[i]; // Send data
        while (!(RFDINIFG & RF1AIFCTL1))
            ; // Wait for TX to finish
    }
    i = RF1ADOUTB; // Reset RFDOUTIFG flag which contains status byte
}

// Write zeros to the transmit FIFO buffer. Max length is 64 bytes.
void writeTXBufferZeros(unsigned char length) {

    // Write Burst works wordwise not bytewise - known errata
    unsigned char i;

    while (!(RF1AIFCTL1 & RFINSTRIFG))
        ; // Wait for the Radio to be ready for next instruction
    RF1AINSTRW =
        ((RF_TXFIFOWR | RF_REGWR) << 8) + 0; // Send address + instruction

    for (i = 1; i < length; i++) {
        RF1ADINB = 0; // Send data
        while (!(RFDINIFG & RF1AIFCTL1))
            ; // Wait for TX to finish
    }
    i = RF1ADOUTB; // Reset RFDOUTIFG flag which contains status byte
}

// Read data from the receive FIFO buffer. Max length is 64 bytes.
void readRXBuffer(unsigned char* data, unsigned char length) {

    unsigned int i;

    while (!(RF1AIFCTL1 & RFINSTRIFG))
        ; // Wait for INSTRIFG
    RF1AINSTR1B =
        (RF_RXFIFORD | RF_REGRD); // Send addr of first conf. reg. to be read
                                  // ... and the burst-register read instruction
    for (i = 0; i < (length - 1); i++) {
        while (!(RFDOUTIFG & RF1AIFCTL1))
            ; // Wait for the Radio Core to update the RF1ADOUTB reg
        data[i] = RF1ADOUT1B; // Read DOUT from Radio Core + clears RFDOUTIFG
                              // Also initiates auo-read for next DOUT byte
    }
    data[length - 1] = RF1ADOUT0B; // Store the last DOUT from Radio Core
}

#include <msp430.h>

#include "sprite_radio.h"

extern RF_SETTINGS rfSettings;

/*
  This example code will configure the CC1101 radio core in the CC430 to
  repeatedly transmit a text message. The output signal will be MSK modulated
  at 64 kbps on a 437.24 MHz carrier and bits are encoded by alternating between
  two different 511 bit Gold codes.
*/

// Chipcon
// Product = CC430Fx13x
// Chip version = C   (PG 0.7)
// Crystal accuracy = 10 ppm
// X-tal frequency = 26 MHz
// RF output power = 0 dBm
// RX filterbandwidth = 101.562500 kHz
// Deviation = 19 kHz
// Datarate = 38.383484 kBaud
// Modulation = (1) GFSK
// Manchester enable = (0) Manchester disabled
// RF Frequency = 914.999969 MHz
// Channel spacing = 199.951172 kHz
// Channel number = 0
// Optimization = -
// Sync mode = (3) 30/32 sync word bits detected
// Format of RX/TX data = (0) Normal mode, use FIFOs for RX and TX
// CRC operation = (1) CRC calculation in TX and CRC check in RX enabled
// Forward Error Correction =
// Length configuration = (0) Fixed packet length, packet length configured by
// PKTLEN Packetlength = 61 Preamble count = (2)  4 bytes Append status = 1
// Address check = (0) No address check
// FIFO autoflush = 0
// Device address = 0
// GDO0 signal selection = ( 6) Asserts when sync word has been sent / received,
// and de-asserts at the end of the packet GDO2 signal selection = (41) RF_RDY
RF_SETTINGS rfSettings = {
    0x0E, // FSCTRL1
    0x00, // FSCTRL0
    0x10, // FREQ2
    0xD1, // FREQ1
    0x21, // FREQ0
    0x0B, // MDMCFG4
    0x43, // MDMCFG3
    0x70, // MDMCFG2
    0x02, // MDMCFG1
    0xF8, // MDMCFG0
    0x00, // CHANNR
    0x07, // DEVIATN
    0xB6, // FREND1
    0x10, // FREND0
    0x18, // MCSM0
    0x1D, // FOCCFG
    0x1C, // BSCFG
    0xC7, // AGCCTRL2
    0x00, // AGCCTRL1
    0xB0, // AGCCTRL0
    0xEA, // FSCAL3
    0x2A, // FSCAL2
    0x00, // FSCAL1
    0x1F, // FSCAL0
    0x59, // FSTEST
    0x88, // TEST2
    0x31, // TEST1
    0x09, // TEST0
    0x07, // FIFOTHR
    0x29, // IOCFG2
    0x06, // IOCFG0
    0x00, // PKTCTRL1  Packet Automation (0x04 = append status bytes)
    0x02, // PKTCTRL0  0x02 = infinite packet length, 0x00 = Fixed Packet Size,
          // 0x40 = whitening, 0x20 = PN9
    0x00, // ADDR      Device address.
    0xFF  // PKTLEN    Packet Length (Bytes)
};

unsigned char m_prn0[64] = {
    0b00000001, 0b01011110, 0b11010100, 0b01100001, 0b00001011, 0b11110011,
    0b00110001, 0b01011100, 0b01100110, 0b10010010, 0b01011011, 0b00101010,
    0b11100000, 0b10100011, 0b00000000, 0b11100001, 0b10111011, 0b10011111,
    0b00110001, 0b11001111, 0b11110111, 0b11000000, 0b10110010, 0b01110101,
    0b10101010, 0b10100111, 0b10100101, 0b00010010, 0b00001111, 0b01011011,
    0b00000010, 0b00111101, 0b01001110, 0b01100000, 0b10001110, 0b00010111,
    0b00110100, 0b10000101, 0b01100001, 0b01000101, 0b00000110, 0b10100010,
    0b00110110, 0b00101111, 0b10101001, 0b00011111, 0b11010111, 0b11111101,
    0b10011101, 0b01001000, 0b00011001, 0b00011000, 0b10101111, 0b00110110,
    0b10010011, 0b00000000, 0b00010000, 0b10000101, 0b00101000, 0b00011101,
    0b01011100, 0b10101111, 0b01100100, 0b11011010};

unsigned char m_prn1[64] = {
    0b11111101, 0b00111110, 0b01110111, 0b11010101, 0b00100101, 0b11101111,
    0b00101100, 0b01101001, 0b00101010, 0b11101001, 0b00111100, 0b11000100,
    0b00000111, 0b10010011, 0b11000101, 0b00000111, 0b00110111, 0b00011111,
    0b01111011, 0b11010001, 0b10111010, 0b00000111, 0b10010000, 0b00110111,
    0b11011111, 0b01011010, 0b11101101, 0b11001000, 0b10001100, 0b01101001,
    0b10010111, 0b00101001, 0b10101100, 0b11011001, 0b11010110, 0b00011010,
    0b11010110, 0b10101000, 0b00000101, 0b11010011, 0b01101010, 0b11001011,
    0b11010110, 0b01010010, 0b00111111, 0b11100111, 0b10000010, 0b10000110,
    0b01101110, 0b10011010, 0b01100101, 0b10100110, 0b00101110, 0b01010100,
    0b11110100, 0b01111010, 0b11001011, 0b00101110, 0b01100011, 0b10111111,
    0b01010100, 0b11000100, 0b11010100, 0b01010100};

int m_power;

// Set the output power of the transmitter.
void radio_set_power(int tx_power_dbm) {

    // These values are from TI Design Note DN013 and are calibrated for
    // operation at 434 MHz.
    switch (tx_power_dbm) {
    case 10:
        m_power = 0xC0;
        break;
    case 9:
        m_power = 0xC3;
        break;
    case 8:
        m_power = 0xC6;
        break;
    case 7:
        m_power = 0xC9;
        break;
    case 6:
        m_power = 0x82;
        break;
    case 5:
        m_power = 0x84;
        break;
    case 4:
        m_power = 0x87;
        break;
    case 3:
        m_power = 0x8A;
        break;
    case 2:
        m_power = 0x8C;
        break;
    case 1:
        m_power = 0x50;
        break;
    case 0:
        m_power = 0x60;
        break;
    case -1:
        m_power = 0x52;
        break;
    case -2:
        m_power = 0x63;
        break;
    case -3:
        m_power = 0x65;
        break;
    case -4:
        m_power = 0x57;
        break;
    case -5:
        m_power = 0x69;
        break;
    case -6:
        m_power = 0x6A;
        break;
    case -7:
        m_power = 0x6C;
        break;
    case -8:
        m_power = 0x6D;
        break;
    case -9:
        m_power = 0x6E;
        break;
    case -10:
        m_power = 0x34;
        break;
    case -11:
        m_power = 0x25;
        break;
    case -12:
        m_power = 0x26;
        break;
    case -13:
    case -14:
    case -15:
        m_power = 0x1D;
        break;
    case -16:
    case -17:
    case -18:
        m_power = 0x1A;
        break;
    case -19:
    case -20:
        m_power = 0x0E;
        break;
    case -21:
    case -22:
    case -23:
        m_power = 0x0A;
        break;
    case -24:
    case -25:
    case -26:
        m_power = 0x07;
        break;
    case -27:
    case -28:
    case -29:
    case -30:
        m_power = 0x03;
        break;
    default:
        m_power = 0xC3; // 10 dBm
    }
}

char fecEncode(char data) {
    // Calculate parity bits using a (16,8,5) block code
    // given by the following generator matrix:
    /*unsigned char G[8][16] = {
            {1, 0, 0, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0},
            {0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0},
            {1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0},
            {0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0},
            {0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0},
            {1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0},
            {0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
            {1, 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1}};*/

    char p = 0;
    p |= (((data & BIT7) >> 7) ^ ((data & BIT5) >> 5) ^ ((data & BIT2) >> 2) ^
          (data & BIT0))
         << 7;
    p |= (((data & BIT6) >> 6) ^ ((data & BIT5) >> 5) ^ ((data & BIT4) >> 4) ^
          ((data & BIT2) >> 2) ^ ((data & BIT1) >> 1) ^ (data & BIT0))
         << 6;
    p |= (((data & BIT4) >> 4) ^ ((data & BIT3) >> 3) ^ ((data & BIT2) >> 2) ^
          ((data & BIT1) >> 1))
         << 5;
    p |= (((data & BIT7) >> 7) ^ ((data & BIT3) >> 3) ^ ((data & BIT2) >> 2) ^
          ((data & BIT1) >> 1) ^ (data & BIT0))
         << 4;
    p |= (((data & BIT7) >> 7) ^ ((data & BIT6) >> 6) ^ ((data & BIT5) >> 5) ^
          ((data & BIT1) >> 1))
         << 3;
    p |= (((data & BIT7) >> 7) ^ ((data & BIT6) >> 6) ^ ((data & BIT5) >> 5) ^
          ((data & BIT4) >> 4) ^ (data & BIT0))
         << 2;
    p |= (((data & BIT7) >> 7) ^ ((data & BIT6) >> 6) ^ ((data & BIT4) >> 4) ^
          ((data & BIT3) >> 3) ^ ((data & BIT2) >> 2) ^ (data & BIT0))
         << 1;
    p |= (((data & BIT5) >> 5) ^ ((data & BIT4) >> 4) ^ ((data & BIT3) >> 3) ^
          (data & BIT0));

    return p;
}

void radio_transmit(char bytes[], unsigned int length) {
#ifdef SR_DEBUG_MODE

    for (unsigned int k = 0; k < length; ++k) {
        transmitByte(bytes[k]);
        // delay(1000);
    }

#else

    // delay(random(0, 2000));

    for (unsigned int k = 0; k < length; ++k) {
        transmitByte(bytes[k]);

        // delay(random(8000, 12000));
    }

#endif
}

void transmitByte(char byte) {
    char parity = fecEncode(byte);

    // Transmit preamble (1110010)
    beginRawTransmit(m_prn1, PRN_LENGTH_BYTES);
    continueRawTransmit(m_prn1, PRN_LENGTH_BYTES);
    continueRawTransmit(m_prn1, PRN_LENGTH_BYTES);
    continueRawTransmit(m_prn0, PRN_LENGTH_BYTES);
    continueRawTransmit(m_prn0, PRN_LENGTH_BYTES);
    continueRawTransmit(m_prn1, PRN_LENGTH_BYTES);
    continueRawTransmit(m_prn0, PRN_LENGTH_BYTES);

    // Transmit parity byte
    parity& BIT7 ? continueRawTransmit(m_prn1, PRN_LENGTH_BYTES)
                 : continueRawTransmit(m_prn0, PRN_LENGTH_BYTES);
    parity& BIT6 ? continueRawTransmit(m_prn1, PRN_LENGTH_BYTES)
                 : continueRawTransmit(m_prn0, PRN_LENGTH_BYTES);
    parity& BIT5 ? continueRawTransmit(m_prn1, PRN_LENGTH_BYTES)
                 : continueRawTransmit(m_prn0, PRN_LENGTH_BYTES);
    parity& BIT4 ? continueRawTransmit(m_prn1, PRN_LENGTH_BYTES)
                 : continueRawTransmit(m_prn0, PRN_LENGTH_BYTES);
    parity& BIT3 ? continueRawTransmit(m_prn1, PRN_LENGTH_BYTES)
                 : continueRawTransmit(m_prn0, PRN_LENGTH_BYTES);
    parity& BIT2 ? continueRawTransmit(m_prn1, PRN_LENGTH_BYTES)
                 : continueRawTransmit(m_prn0, PRN_LENGTH_BYTES);
    parity& BIT1 ? continueRawTransmit(m_prn1, PRN_LENGTH_BYTES)
                 : continueRawTransmit(m_prn0, PRN_LENGTH_BYTES);
    parity& BIT0 ? continueRawTransmit(m_prn1, PRN_LENGTH_BYTES)
                 : continueRawTransmit(m_prn0, PRN_LENGTH_BYTES);

    // Transmit data byte
    byte& BIT7 ? continueRawTransmit(m_prn1, PRN_LENGTH_BYTES)
               : continueRawTransmit(m_prn0, PRN_LENGTH_BYTES);
    byte& BIT6 ? continueRawTransmit(m_prn1, PRN_LENGTH_BYTES)
               : continueRawTransmit(m_prn0, PRN_LENGTH_BYTES);
    byte& BIT5 ? continueRawTransmit(m_prn1, PRN_LENGTH_BYTES)
               : continueRawTransmit(m_prn0, PRN_LENGTH_BYTES);
    byte& BIT4 ? continueRawTransmit(m_prn1, PRN_LENGTH_BYTES)
               : continueRawTransmit(m_prn0, PRN_LENGTH_BYTES);
    byte& BIT3 ? continueRawTransmit(m_prn1, PRN_LENGTH_BYTES)
               : continueRawTransmit(m_prn0, PRN_LENGTH_BYTES);
    byte& BIT2 ? continueRawTransmit(m_prn1, PRN_LENGTH_BYTES)
               : continueRawTransmit(m_prn0, PRN_LENGTH_BYTES);
    byte& BIT1 ? continueRawTransmit(m_prn1, PRN_LENGTH_BYTES)
               : continueRawTransmit(m_prn0, PRN_LENGTH_BYTES);
    byte& BIT0 ? continueRawTransmit(m_prn1, PRN_LENGTH_BYTES)
               : continueRawTransmit(m_prn0, PRN_LENGTH_BYTES);

    // Transmit postamble (1011000)
    continueRawTransmit(m_prn1, PRN_LENGTH_BYTES);
    continueRawTransmit(m_prn0, PRN_LENGTH_BYTES);
    continueRawTransmit(m_prn1, PRN_LENGTH_BYTES);
    continueRawTransmit(m_prn1, PRN_LENGTH_BYTES);
    continueRawTransmit(m_prn0, PRN_LENGTH_BYTES);
    continueRawTransmit(m_prn0, PRN_LENGTH_BYTES);
    continueRawTransmit(m_prn0, PRN_LENGTH_BYTES);

    endRawTransmit();
}

void rawTransmit(unsigned char bytes[], unsigned int length) {

    beginRawTransmit(bytes, length);
    endRawTransmit();
}

void beginRawTransmit(unsigned char bytes[], unsigned int length) {
    char status;

    // Wait for radio to be in idle state
    status = Strobe(RF_SIDLE);
    while (status & 0xF0) {
        status = Strobe(RF_SNOP);
    }

    // Clear TX FIFO
    status = Strobe(RF_SFTX);

    if (length <= 64) {
        writeTXBuffer(bytes, length); // Write bytes to transmit buffer
        status = Strobe(RF_STX);      // Turn on transmitter
    } else {
        unsigned char bytes_free, bytes_to_write;
        unsigned int bytes_to_go, counter;

        writeTXBuffer(bytes, 64); // Write first 64 bytes to transmit buffer
        bytes_to_go = length - 64;
        counter = 64;

        status = Strobe(RF_STX); // Turn on transmitter

        // Wait for oscillator to stabilize
        while (status & 0xC0) {
            status = Strobe(RF_SNOP);
        }

        while (bytes_to_go) {
            // Wait for some bytes to be transmitted
            sleep(1, LPM0_bits);

            bytes_free = Strobe(RF_SNOP) & 0x0F;
            bytes_to_write =
                bytes_free < bytes_to_go ? bytes_free : bytes_to_go;

            writeTXBuffer(bytes + counter, bytes_to_write);
            bytes_to_go -= bytes_to_write;
            counter += bytes_to_write;
        }
    }
}

void continueRawTransmit(unsigned char bytes[], unsigned int length) {

    unsigned char bytes_free, bytes_to_write;
    unsigned int bytes_to_go, counter;

    bytes_to_go = length;
    counter = 0;

    if (bytes) {
        while (bytes_to_go) {
            // Wait for some bytes to be transmitted
            sleep(1, LPM0_bits);

            bytes_free = Strobe(RF_SNOP) & 0x0F;
            bytes_to_write =
                bytes_free < bytes_to_go ? bytes_free : bytes_to_go;

            writeTXBuffer(bytes + counter, bytes_to_write);
            bytes_to_go -= bytes_to_write;
            counter += bytes_to_write;
        }
    } else {
        while (bytes_to_go) {
            // Wait for some bytes to be transmitted
            sleep(1, LPM0_bits);

            bytes_free = Strobe(RF_SNOP) & 0x0F;
            bytes_to_write =
                bytes_free < bytes_to_go ? bytes_free : bytes_to_go;

            writeTXBufferZeros(bytes_to_write);
            bytes_to_go -= bytes_to_write;
            counter += bytes_to_write;
        }
    }

    return;
}

void endRawTransmit() {

    char status = Strobe(RF_SNOP);

    // Wait for transmission to finish
    while (status != 0x7F) {
        status = Strobe(RF_SNOP);
    }
    Strobe(RF_SIDLE); // Put radio back in idle mode
    return;
}

void radio_sleep() {

    Strobe(RF_SIDLE); // Put radio back in idle mode
}

void radio_init(void) {
    // Initialize random number generator
    /*
    randomSeed(((int)m_prn0[0]) + ((int)m_prn1[0]) + ((int)m_prn0[1]) +
               ((int)m_prn1[1]));
   */

    // Set the High-Power Mode Request Enable bit so LPM3 can be entered
    // with active radio enabled
    PMMCTL0_H = 0xA5;
    PMMCTL0_L |= PMMHPMRE_L;
    PMMCTL0_H = 0x00;

    WriteRfSettings(&rfSettings);

    WriteSinglePATable(0xC3);
}

void radio_wait_for_sleeping() {
    char status;
    while (status & 0xF0) {
        status = Strobe(RF_SNOP);
    }
}

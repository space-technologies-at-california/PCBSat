#include <msp430.h>
#include <setjmp.h>
#include <stdbool.h>
#include <stdlib.h>

#include "config.h"

#include "cc430uart.h"
#include "fault.h"
#include "hal_pmm.h"
#include "proto.h"
#include "radio.h"

/*
  This example code will configure the CC1101 radio core in the CC430 to
  repeatedly transmit a text message. The output signal will be MSK modulated
  at 64 kbps on a 437.24 MHz carrier and bits are encoded by alternating between
  two different 511 bit Gold codes.
*/

static jmp_buf fault_buf;
static unsigned char fault_count = 0;

// Product = CC430Fx13x
// X-tal frequency = 26 MHz
// RF output power = 0 dBm
// Datarate = 64.0717 kBaud
// Modulation = MSK
// Manchester enable = (0) Manchester disabled
// RF Frequency = 437.240 MHz
// Channel spacing = 49.99 kHz
// Channel number = 0
static RF_SETTINGS rfSettings = {
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

static int m_power;

extern unsigned char m_prn0[64], m_prn1[64]; // defined in prn.c

// Set the output power of the transmitter.
static void radio_set_power(int tx_power_dbm) {

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

static char fecEncode(char data) {
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

static void radio_transmit(char bytes[], unsigned int length) {
    for (unsigned int k = 0; k < length; ++k) {
        if (!radio_precond()) {
#ifdef DEBUG
            uart_write("TX early exit\r\n", 15);
#endif
            break;
        }
        transmitByte(bytes[k]);
#ifdef DEBUG
            uart_write(".", 1);
#endif
        sleep(1000, LPM0_bits);
    }
#ifdef DEBUG
            uart_write("\r\n", 2);
#endif
}

/**
 * Transmit a byte. Without pauses, each byte takes (30*64*8)/64000 = 0.24 s.
 */
static void transmitByte(char byte) {
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

static void wait_for_status(unsigned char s) {
    char status = Strobe(RF_SNOP);
    for (int i = 0; i < 100; i++) {
        if (status & s) {
            status = Strobe(RF_SNOP);
        } else {
            return;
        }
    }
    longjmp(fault_buf, 1);
}

static void wait_for_idle() {
    Strobe(RF_SIDLE);
    wait_for_status(0xF0);
}

static void beginRawTransmit(unsigned char bytes[], unsigned int length) {
    char status;
    wait_for_idle();

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
        wait_for_status(0xC0);

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

static void continueRawTransmit(unsigned char bytes[], unsigned int length) {

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

static void endRawTransmit() {

    char status = Strobe(RF_SNOP);

    // Wait for transmission to finish
    while (status != 0x7F) {
        status = Strobe(RF_SNOP);
    }
    Strobe(RF_SIDLE); // Put radio back in idle mode
    return;
}

static void init_radio() {
    // Increase PMMCOREV level to 2 for proper radio operation
    SetVCore(2);

    ResetRadioCore();
    WriteRfSettings(&rfSettings);
    WriteSinglePATable(0xC3);

    /*
     * Select Interrupt edge for PA_PD and SYNC signal:
     * Interrupt Edge select register: 1 == Interrupt on High to Low transition.
     */
    RF1AIES = BIT0 | BIT9;

    wait_for_idle();
}

void run_radio() {
    static bool has_rng_init = false;
    static bool has_radio_init = false;
    fault_count = 0;

    if (!has_rng_init) {
        // Initialize random number generator
        srand(((int)m_prn0[0]) + ((int)m_prn1[0]) + ((int)m_prn0[1]) +
              ((int)m_prn1[1]));
        has_rng_init = true;
    }

    if (setjmp(fault_buf)) {
        // We are handling a radio error. Try resetting radio.
        faults |= FAULT_RADIO;
        has_radio_init = false;
        fault_count += 1;
#ifdef DEBUG
        uart_write("radio fault\r\n", 13);
#endif
    }

    if (fault_count >= 3) {
        // Give up after a number of tries.
        return;
    }

    if (!has_radio_init) {
        init_radio();
        has_radio_init = true;
    }

#ifdef DEBUG
    // Blink LED while transmitter is on
    P3OUT |= BIT7;
    uart_write("TX\r\n", 4);
#endif
    radio_transmit(tx_msg, sizeof(tx_msg)/sizeof(char));
#ifdef DEBUG
    P3OUT &= ~BIT7;
#endif
}

/**
 * Copyright (c) 2018 STAC <stac.berkeley.edu>
 * 
 * Author: Aviral Pandey
 * Creation date: 07/01/2018
 */

#ifndef _PINS_H
#define _PINS_H

#include <stdint.h>

#define __SPRITE_VERSION_2_0__  1

// Enable pseudo-pin interrupts on P3
#define __ENABLE_P3_PIN_INTERRUPTS__  1

// UART
#define pinUARTmap()      P1MAP6 = PM_UCA0TXD; P1MAP5 = PM_UCA0RXD
#define pinUARTconfig()   P1SEL |= BIT6 | BIT5      // Set P1.6 and P1.5 to USCI Mode
// I2C
#define pinI2Cmap()       P1MAP4 = PM_UCB0SCL; P1MAP3 = PM_UCB0SDA
#define pinI2Cconfig()    P1SEL |= BIT3 | BIT4      // Set P1.4 and P1.3 to USCI function
// SPI
#define pinSPImap()       P1MAP0 = PM_UCB0SIMO; P1MAP1 = PM_UCB0SOMI; P1MAP2 = PM_UCB0CLK
#define pinSPIconfig()    P1SEL |= BIT0 | BIT1 | BIT2  // Set P1.0, P1.1 and P1.2 to USCI function

#define NOT_A_PORT 0
#define NOT_A_PIN 0
#define NOT_ON_TIMER 0
#define NOT_ON_ADC 0xFF

// Ports
enum{
  P1 = 1,
  P2,
  P3,
  PJ
};

// Timers
enum{
  T0A0,
  T0A1,
  T0A2,
  T0A3,
  T0A4,
  T1A0,
  T1A1,
  T1A2
};

// ADC channels
// [Avi] Updated on 7/1, seemed incorrect before
static const uint8_t A0 = 0;
static const uint8_t A1 = 1;
static const uint8_t A2 = 2;
static const uint8_t A3 = 3;
// static const uint8_t A4 = 4;
// static const uint8_t A5 = 5;        // Connected to NTC
// static const uint8_t A10 = 128 + 10; // special. This is the internal temp sensor
static const uint8_t Atemp = 0b1010; // special. This is Vcc/2

// On-board LED
// Da Fuq are these? 
#define ONBOARD_LED 23
#define LED      ONBOARD_LED
#define INIT_ONBOARD_LED()    P3DIR |= BIT7; P3OUT &= ~BIT7
// ADC resolution
#define ADC_RESOLUTION  0xFFF

// LD-BOARD definitions
#define HGM     20
#define LNA_EN  19
#define PA_EN   18

const uint16_t digital_pin_to_pmap[] = {
	(uint16_t) &P1MAP0, 
	(uint16_t) &P1MAP1,
  (uint16_t) &P1MAP2,
  (uint16_t) &P1MAP3,
  (uint16_t) &P1MAP4,
  (uint16_t) &P1MAP5,
  (uint16_t) &P1MAP6,
  (uint16_t) &P1MAP7,
	(uint16_t) &P2MAP0,
  (uint16_t) &P2MAP1,
  (uint16_t) &P2MAP2,
  (uint16_t) &P2MAP3,
  (uint16_t) &P2MAP4,
  (uint16_t) &P2MAP5,
  (uint16_t) &P2MAP6,
  (uint16_t) &P2MAP7,
	(uint16_t) &P3MAP0,
  (uint16_t) &P3MAP1,
  (uint16_t) &P3MAP2,
  (uint16_t) &P3MAP3,
  (uint16_t) &P3MAP4,
  (uint16_t) &P3MAP5,
  (uint16_t) &P3MAP6,
  (uint16_t) &P3MAP7  // On-board LED
};

const uint16_t port_to_input[] = {
	NOT_A_PORT,
	(uint16_t) &P1IN,
	(uint16_t) &P2IN,
	(uint16_t) &P3IN,
	(uint16_t) &PJIN
};

const uint16_t port_to_output[] = {
	NOT_A_PORT,
	(uint16_t) &P1OUT,
	(uint16_t) &P2OUT,
	(uint16_t) &P3OUT,
	(uint16_t) &PJOUT
};

const uint16_t port_to_dir[] = {
	NOT_A_PORT,
	(uint16_t) &P1DIR,
	(uint16_t) &P2DIR,
	(uint16_t) &P3DIR,
	(uint16_t) &PJDIR
};

const uint16_t port_to_ren[] = {
	NOT_A_PORT,
	(uint16_t) &P1REN,
	(uint16_t) &P2REN,
	(uint16_t) &P3REN,
	(uint16_t) NOT_A_PORT  //No PJREN
};

const uint16_t port_to_sel[] = {
	NOT_A_PORT,
	(uint16_t) &P1SEL,
	(uint16_t) &P2SEL,
	(uint16_t) &P3SEL,
	(uint16_t) NOT_A_PORT  // No PJSEL,
};

const uint16_t port_to_pmap[] = {
	NOT_A_PORT,	/* PMAP starts at port P1 */
	(uint16_t) &P1MAP0,
	(uint16_t) &P2MAP0,
	(uint16_t) &P3MAP0,
	NOT_A_PORT  // No PJMAP0
};

const uint8_t digital_pin_to_timer[] = {
	T0A0,         /* P1.0 - note: A0 output cannot be used with analogWrite */
	T0A1,         /* P1.1 */
	T0A2,         /* P1.2 */
	T0A3,         /* P1.3 */
	T0A4,         /* P1.4 */
	NOT_ON_TIMER, /* P1.5 */
	NOT_ON_TIMER, /* P1.6 */
	NOT_ON_TIMER, /* P1.7 */
	NOT_ON_TIMER, /* P2.0 */
	NOT_ON_TIMER, /* P2.1 */
	NOT_ON_TIMER, /* P2.2 */
	NOT_ON_TIMER, /* P2.3 */
	NOT_ON_TIMER, /* P2.4 */
	NOT_ON_TIMER, /* P2.5 */
	NOT_ON_TIMER, /* P2.6 */
	NOT_ON_TIMER, /* P2.7 */
  NOT_ON_TIMER, /* P3.0 */
	T1A0,         /* P3.1 - note: A0 output cannot be used with analogWrite */
	T1A1,         /* P3.2 */
	T1A2,         /* P3.3 */
  NOT_ON_TIMER, /* P3.4 */
  NOT_ON_TIMER, /* P3.5 */
  NOT_ON_TIMER, /* P3.6 */
	NOT_ON_TIMER  /* P3.7 -> ONBOARD_LED */
};

const uint8_t digital_pin_to_port[] = {
	P1,           /* P1.0 */
	P1,           /* P1.1 */
	P1,           /* P1.2 */
	P1,           /* P1.3 */
	P1,           /* P1.4 */
	P1,           /* P1.5 */
	P1,           /* P1.6 */
	P1,           /* P1.7 */
	P2,           /* P2.0 */
	P2,           /* P2.1 */
	P2,           /* P2.2 */
	P2,           /* P2.3 */
	P2,           /* P2.4 */
	P2,           /* P2.5 */
	P2,           /* P2.6 */
	P2,           /* P2.7 */
	P3,           /* P3.0 */
	P3,           /* P3.1 */
	P3,           /* P3.2 */
  P3,           /* P3.3 */
  P3,           /* P3.4 */
  P3,           /* P3.5 */
  P3,           /* P3.6 */  
	P3            /* P3.7 -> ONBOARD_LED */
};

const uint8_t digital_pin_to_bit_mask[] = {
	BIT0,        /* P1.0 */
	BIT1,        /* P1.1 */
	BIT2,        /* P1.2 */
	BIT3,        /* P1.3 */
	BIT4,        /* P1.4 */
	BIT5,        /* P1.5 */
	BIT6,        /* P1.6 */
	BIT7,        /* P1.7 */
	BIT0,        /* P2.0 */
	BIT1,        /* P2.1 */
	BIT2,        /* P2.2 */
	BIT3,        /* P2.3 */
	BIT4,        /* P2.4 */
	BIT5,        /* P2.5 */
	BIT6,        /* P2.6 */
	BIT7,        /* P2.7 */
	BIT0,        /* P3.0 */
	BIT1,        /* P3.1 */
	BIT2,        /* P3.2 */
  BIT3,        /* P3.3 */
  BIT4,        /* P3.4 */
  BIT5,        /* P3.5 */
  BIT6,        /* P3.6 */
	BIT7         /* P3.7 -> ONBOARD_LED */
};

const uint16_t digital_pin_to_analog_in[] = {
	NOT_ON_ADC,   /* P1.0 */
	NOT_ON_ADC,   /* P1.1 */
	NOT_ON_ADC,   /* P1.2 */
	NOT_ON_ADC,   /* P1.3 */
	NOT_ON_ADC,   /* P1.4 */
	NOT_ON_ADC,   /* P1.5 */
	NOT_ON_ADC,   /* P1.6 */
	NOT_ON_ADC,   /* P1.7 */
	0,            /* P2.0 */
	1,            /* P2.1 */
	2,            /* P2.2 */
	3,            /* P2.3 */
	4,            /* P2.4 */
	5,            /* P2.5 */
	NOT_ON_ADC,   /* P2.6 */
	NOT_ON_ADC,   /* P2.7 */
	NOT_ON_ADC,   /* P3.0 */
	NOT_ON_ADC,   /* P3.1 */
	NOT_ON_ADC,   /* P3.2 */
	NOT_ON_ADC,   /* P3.3 */
  NOT_ON_ADC,   /* P3.4 */
  NOT_ON_ADC,   /* P3.5 */
	NOT_ON_ADC,   /* P3.6 */
	NOT_ON_ADC    /* P3.7 -> ONBOARD_LED */
};

#endif

/*
  ************************************************************************
  *	pins_energia.h
  *
  *	Energia core files for MSP430
  *		Copyright (c) 2012 Robert Wessels. All right reserved.
  *
  *     Contribution: Zac Manchester
  *
  ***********************************************************************
  Derived from:
  pins_arduino.h - Pin definition functions for Arduino
  Part of Arduino - http://www.arduino.cc/

  Copyright (c) 2007 David A. Mellis

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General
  Public License along with this library; if not, write to the
  Free Software Foundation, Inc., 59 Temple Place, Suite 330,
  Boston, MA  02111-1307  USA
*/

#ifndef Pins_Arduino_h
#define Pins_Arduino_h
#ifndef BV
#define BV(x) (1 << (x))
#endif

#if defined(__MSP430_HAS_USCI_A0__)
static const uint8_t SS      = 9;  /* P1.4 */
static const uint8_t SCK     = 4;  /* P1.7 */
static const uint8_t MOSI    = 5;  /* P1.6 */
static const uint8_t MISO    = 6;  /* P1.5 */
static const uint8_t TWISDA  = 10; /* P1.3 */
static const uint8_t TWISCL  = 11; /* P1.2 */
static const uint8_t UARTRXD = 5;  /* Receive  Data (RXD) at P1.6 */
static const uint8_t UARTTXD = 6;  /* Transmit Data (TXD) at P1.5 */
#define TWISDA_SET_MODE  (PORT_SELECTION0 | PORT_SELECTION1 | INPUT_PULLUP)
#define TWISCL_SET_MODE  (PORT_SELECTION0 | PORT_SELECTION1 | INPUT_PULLUP)
#define UARTRXD_SET_MODE (PORT_SELECTION0 | PORT_SELECTION1 | INPUT)
#define UARTTXD_SET_MODE (PORT_SELECTION0 | PORT_SELECTION1 | OUTPUT)
#endif

static const uint8_t A0  = 0;
static const uint8_t A1  = 1;
static const uint8_t A2  = 2;
static const uint8_t A3  = 3;
static const uint8_t A4  = 4;
static const uint8_t A5  = 5;
static const uint8_t A6  = 6;
static const uint8_t A7  = 7;
static const uint8_t A10 = 10; // Special - This is the internal temp sensor
static const uint8_t A11 = 11; // Special - This is the 1/2 the supply voltage
static const uint8_t A12 = 12;
static const uint8_t A13 = 13;
static const uint8_t A14 = 14;
static const uint8_t A15 = 15;

//                      +---\/---+
//         (A2)  P2.2  1|        |48  P2.3  (A3)
//         (A1)  P2.1  2|        |47  P2.4  (A4)
//         (A0)  P2.0  3|        |46  P2.5  (A5)
//               P1.7  4|        |45  AVCC
//               P1.6  5|        |44  P5.0/XIN
//               P1.5  6|        |43  P5.1/XOUT
//               VCORE 7|        |42  AVSS
//               DVCC  8|        |41  DVCC
//               P1.4  9|        |40  RST
//               P1.3 10|        |39  TEST
//               P1.2 11|        |38  PJ.3
//               P1.1 12|        |37  PJ.2
//               P1.0 13|        |36  PJ.1
//               P3.7 14|        |35  PJ.0
//               P3.6 15|        |34  GUARD
//               P3.5 16|        |33  RBIAS
//               P3.4 17|        |32  AVCCRF
//               P3.3 18|        |31  AVCCRF
//               P3.2 19|        |30  RFN
//               P3.1 20|        |29  RFP
//               P3.0 21|        |28  AVCCRF
//               DVCC 22|        |27  AVCCRF
//               P2.7 23|        |26  RFXOUT
//               P2.6 24|        |25  RFXIN
//                      +--------+
//

// Pin names based on the silkscreen
//
static const uint8_t P1_0 = 13;
static const uint8_t P1_1 = 12;
static const uint8_t P1_2 = 11;
static const uint8_t P1_3 = 10;
static const uint8_t P1_4 = 9;
static const uint8_t P1_5 = 6;
static const uint8_t P1_6 = 5;
static const uint8_t P1_7 = 4;

static const uint8_t P2_0 = 3;
static const uint8_t P2_1 = 2;
static const uint8_t P2_2 = 1;
static const uint8_t P2_3 = 48;
static const uint8_t P2_4 = 47;
static const uint8_t P2_5 = 46;
static const uint8_t P2_6 = 24;
static const uint8_t P2_7 = 23;

static const uint8_t P3_0 = 21;
static const uint8_t P3_1 = 20;
static const uint8_t P3_2 = 19;
static const uint8_t P3_3 = 18;
static const uint8_t P3_4 = 17;
static const uint8_t P3_5 = 16;
static const uint8_t P3_6 = 15;
static const uint8_t P3_7 = 14;

static const uint8_t P5_0 = 44;
static const uint8_t P5_1 = 43;

static const uint8_t PJ_0 = 35;
static const uint8_t PJ_1 = 34;
static const uint8_t PJ_2 = 33;
static const uint8_t PJ_3 = 32;

static const uint8_t RED_LED = 15;
static const uint8_t GREEN_LED = 13;
static const uint8_t PUSH2 = 4;
static const uint8_t TEMPSENSOR = 10;


#ifdef ARDUINO_MAIN

const uint16_t port_to_input[] = {
	NOT_A_PORT,
	(uint16_t) &P1IN,
	(uint16_t) &P2IN,
#ifdef __MSP430_HAS_PORT3_R__
	(uint16_t) &P3IN,
#endif
#ifdef __MSP430_HAS_PORT4_R__
	(uint16_t) &P4IN,
#endif
#ifdef __MSP430_HAS_PORT5_R__
	(uint16_t) &P5IN,
#endif
#ifdef __MSP430_HAS_PORTJ_R__
	(uint16_t) &PJIN,
#endif
};

const uint16_t port_to_output[] = {
	NOT_A_PORT,
	(uint16_t) &P1OUT,
	(uint16_t) &P2OUT,
#ifdef __MSP430_HAS_PORT3_R__
	(uint16_t) &P3OUT,
#endif
#ifdef __MSP430_HAS_PORT4_R__
	(uint16_t) &P4OUT,
#endif
#ifdef __MSP430_HAS_PORT5_R__
	(uint16_t) &P5OUT,
#endif
#ifdef __MSP430_HAS_PORTJ_R__
	(uint16_t) &PJOUT,
#endif
};

const uint16_t port_to_dir[] = {
	NOT_A_PORT,
	(uint16_t) &P1DIR,
	(uint16_t) &P2DIR,
#ifdef __MSP430_HAS_PORT3_R__
	(uint16_t) &P3DIR,
#endif
#ifdef __MSP430_HAS_PORT4_R__
	(uint16_t) &P4DIR,
#endif
#ifdef __MSP430_HAS_PORT5_R__
	(uint16_t) &P5DIR,
#endif
#ifdef __MSP430_HAS_PORTJ_R__
	(uint16_t) &PJDIR,
#endif
};

const uint16_t port_to_ren[] = {
	NOT_A_PORT,
	(uint16_t) &P1REN,
	(uint16_t) &P2REN,
#ifdef __MSP430_HAS_PORT3_R__
	(uint16_t) &P3REN,
#endif
#ifdef __MSP430_HAS_PORT4_R__
	(uint16_t) &P4REN,
#endif
#ifdef __MSP430_HAS_PORT5_R__
	(uint16_t) &P5REN,
#endif
#ifdef __MSP430_HAS_PORTJ_R__
	(uint16_t) NOT_A_PORT, //&PJREN
#endif
};

const uint16_t port_to_sel0[] = {
	NOT_A_PORT,
	(uint16_t) &P1SEL,
	(uint16_t) &P2SEL,
#ifdef __MSP430_HAS_PORT3_R__
	(uint16_t) &P3SEL,
#endif
#ifdef __MSP430_HAS_PORT4_R__
	(uint16_t) &P4SEL,
#endif
#ifdef __MSP430_HAS_PORT5_R__
	(uint16_t) &P5SEL,
#endif
#ifdef __MSP430_HAS_PORTJ_R__
	(uint16_t) NOT_A_PORT, //&PJSEL,
#endif
};

/* 
 * Defines for devices with 2x TA3 timers (e.g. MSP430g2553). On the 20pin devices, upto 3 analog outputs are available
 * T0A1, T1A1 and T1A2 
 */
const uint8_t digital_pin_to_timer[] = {
	NOT_ON_TIMER, /*  dummy */
	T0A1,         /*  1 - P2.2 */
	T0A0,         /*  2 - P2.1 */
	NOT_ON_TIMER, /*  3 - P2.0 */
	NOT_ON_TIMER, /*  4 - P1.7 */
	NOT_ON_TIMER, /*  5 - P1.6 */
	NOT_ON_TIMER, /*  6 - P1.5 */
	NOT_ON_TIMER, /*  7 - VCORE */
	NOT_ON_TIMER, /*  8 - DVCC */
	NOT_ON_TIMER, /*  9 - P1.4 */
	NOT_ON_TIMER, /* 10 - P1.3 */
	NOT_ON_TIMER, /* 11 - P1.2 */
	NOT_ON_TIMER, /* 12 - P1.1 */
	NOT_ON_TIMER, /* 13 - P1.0 */
	NOT_ON_TIMER, /* 14 - P3.7 */
	NOT_ON_TIMER, /* 15 - P3.6 */
	T1A4,         /* 16 - P3.5 */  
	T1A3,         /* 17 - P3.4 */  
	T1A2,         /* 18 - P3.3 */
	T1A1,         /* 19 - P3.2 */
	T1A0,         /* 20 - P3.1 */
	NOT_ON_TIMER, /* 21 - P3.0 */
	NOT_ON_TIMER, /* 22 - DVCC */
	NOT_ON_TIMER, /* 23 - P2.7 */
	NOT_ON_TIMER, /* 24 - P2.6 */
	NOT_ON_TIMER, /* 25 - RF_XIN */
	NOT_ON_TIMER, /* 26 - RF_XOUT */
	NOT_ON_TIMER, /* 27 - AVCC_RF */
	NOT_ON_TIMER, /* 28 - AVCC_RF */
	NOT_ON_TIMER, /* 29 - RF_P */
	NOT_ON_TIMER, /* 30 - RF_N */
	NOT_ON_TIMER, /* 31 - AVCC_RF */
	NOT_ON_TIMER, /* 32 - AVCC_RF */
	NOT_ON_TIMER, /* 33 - R_BIAS */
	NOT_ON_TIMER, /* 34 - GUARD */
	NOT_ON_TIMER, /* 35 - PJ.0 */
	NOT_ON_TIMER, /* 36 - PJ.1 */
	NOT_ON_TIMER, /* 37 - PJ.2 */
	NOT_ON_TIMER, /* 38 - PJ.3 */
	NOT_ON_TIMER, /* 39 - TEST */
	NOT_ON_TIMER, /* 40 - RST */
	NOT_ON_TIMER, /* 41 - DVCC */
	NOT_ON_TIMER, /* 42 - AVSS */
	NOT_ON_TIMER, /* 43 - P5.1 */
	NOT_ON_TIMER, /* 44 - P5.0 */
	NOT_ON_TIMER, /* 45 - AVCC */
	NOT_ON_TIMER, /* 46 - P2.5 */
	NOT_ON_TIMER, /* 47 - P2.4 */
	T0A2,         /* 48 - P2.3 */
};

const uint8_t digital_pin_to_port[] = {
	NOT_A_PIN,	 /*  dummy */
	P2,          /*  1 - P2.2  */
	P2,          /*  2 - P2.1 */
	P2,          /*  3 - P2.0 */
	P1,		     /*  4 - P1.7 */
	P1,          /*  5 - P1.6 */
	P1,          /*  6 - P1.5 */
	NOT_A_PIN,   /*  7 - VCORE */
	NOT_A_PIN,   /*  8 - DVCC */
	P1,          /*  9 - P1.4 */
	P1,          /* 10 - P1.3 */
	P1,          /* 11 - P1.2 */
	P1,          /* 12 - P1.1 */
	P1,          /* 13 - P1.0 */
	P3,          /* 14 - P3.7 */
	P3,          /* 15 - P3.6 */
	P3,          /* 16 - P3.5 */  
	P3,          /* 17 - P3.4 */  
	P3,          /* 18 - P3.3 */
	P3,          /* 19 - P3.2 */
	P3,          /* 20 - P3.1 */
	P3,          /* 21 - P3.0 */
	NOT_A_PIN,   /* 22 - DVCC */
	P2,          /* 23 - P2.7 */
	P2,          /* 24 - P2.6 */
	NOT_A_PIN,   /* 25 - RF_XIN */
	NOT_A_PIN,   /* 26 - RF_XOUT */
	NOT_A_PIN,   /* 27 - AVCC_RF */
	NOT_A_PIN,   /* 28 - AVCC_RF */
	NOT_A_PIN,   /* 29 - RF_P */
	NOT_A_PIN,   /* 30 - RF_N */
	NOT_A_PIN,   /* 31 - AVCC_RF */
	NOT_A_PIN,   /* 32 - AVCC_RF */
	NOT_A_PIN,   /* 33 - R_BIAS */
	NOT_A_PIN,   /* 34 - GUARD */
	PJ,          /* 35 - PJ.0 */
	PJ,          /* 36 - PJ.1 */
	PJ,          /* 37 - PJ.2 */
	PJ,          /* 38 - PJ.3 */
	NOT_A_PIN,   /* 39 - TEST */
	NOT_A_PIN,   /* 40 - RST */
	NOT_A_PIN,   /* 41 - DVCC */
	NOT_A_PIN,   /* 42 - AVSS */
	P5,          /* 43 - P5.1 */
	P5,          /* 44 - P5.0 */
	NOT_A_PIN,   /* 45 - AVCC */
	P2,          /* 46 - P2.5 */
	P2,          /* 47 - P2.4 */
	P2,          /* 48 - P2.3 */
};

const uint8_t digital_pin_to_bit_mask[] = {
	NOT_A_PIN,	 /*  dummy */
	BV(2),       /*  1 - P2.2  */
	BV(1),       /*  2 - P2.1 */
	BV(0),       /*  3 - P2.0 */
	BV(7),	     /*  4 - P1.7 */
	BV(6),       /*  5 - P1.6 */
	BV(5),       /*  6 - P1.5 */
	NOT_A_PIN,   /*  7 - VCORE */
	NOT_A_PIN,   /*  8 - DVCC */
	BV(4),       /*  9 - P1.4 */
	BV(3),       /* 10 - P1.3 */
	BV(2),       /* 11 - P1.2 */
	BV(1),       /* 12 - P1.1 */
	BV(0),       /* 13 - P1.0 */
	BV(7),       /* 14 - P3.7 */
	BV(6),       /* 15 - P3.6 */
	BV(5),       /* 16 - P3.5 */  
	BV(4),       /* 17 - P3.4 */  
	BV(3),       /* 18 - P3.3 */
	BV(2),       /* 19 - P3.2 */
	BV(1),       /* 20 - P3.1 */
	BV(0),       /* 21 - P3.0 */
	NOT_A_PIN,   /* 22 - DVCC */
	BV(7),       /* 23 - P2.7 */
	BV(6),       /* 24 - P2.6 */
	NOT_A_PIN,   /* 25 - RF_XIN */
	NOT_A_PIN,   /* 26 - RF_XOUT */
	NOT_A_PIN,   /* 27 - AVCC_RF */
	NOT_A_PIN,   /* 28 - AVCC_RF */
	NOT_A_PIN,   /* 29 - RF_P */
	NOT_A_PIN,   /* 30 - RF_N */
	NOT_A_PIN,   /* 31 - AVCC_RF */
	NOT_A_PIN,   /* 32 - AVCC_RF */
	NOT_A_PIN,   /* 33 - R_BIAS */
	NOT_A_PIN,   /* 34 - GUARD */
	BV(0),       /* 35 - PJ.0 */
	BV(1),       /* 36 - PJ.1 */
	BV(2),       /* 37 - PJ.2 */
	BV(3),       /* 38 - PJ.3 */
	NOT_A_PIN,   /* 39 - TEST */
	NOT_A_PIN,   /* 40 - RST */
	NOT_A_PIN,   /* 41 - DVCC */
	NOT_A_PIN,   /* 42 - AVSS */
	BV(1),       /* 43 - P5.1 */
	BV(0),       /* 44 - P5.0 */
	NOT_A_PIN,   /* 45 - AVCC */
	BV(5),       /* 46 - P2.5 */
	BV(4),       /* 47 - P2.4 */
	BV(3),       /* 48 - P2.3 */
};
#endif
#endif

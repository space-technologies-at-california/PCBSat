/*
  ************************************************************************
  *	WInterrupts.c
  *
  *	Arduino core files for MSP430
  *		Copyright (c) 2012 Robert Wessels. All right reserved.
  *
  *
  ***********************************************************************
  Derived from:

  WInterrupts.c Part of the Wiring project - http://wiring.uniandes.edu.co

  Copyright (c) 2004-05 Hernando Barragan

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

  Modified 24 November 2006 by David A. Mellis
  Modified 1 August 2010 by Mark Sproul
*/

#include <inttypes.h>
#include <stdio.h>

#include "wiring_private.h"

#ifndef BV
#define BV(x) (1 << (x))
#endif

#define bit_pos(A) ((A) == 1u << 0 ? 0 \
: (A) == 1u << 1 ? 1 \
: (A) == 1u << 2 ? 2 \
: (A) == 1u << 3 ? 3 \
: (A) == 1u << 4 ? 4 \
: (A) == 1u << 5 ? 5 \
: (A) == 1u << 6 ? 6 \
: (A) == 1u << 7 ? 7 \
: 0)

#define NUM_INTS_PER_PORT 8
static volatile voidFuncPtr intFuncP1[NUM_INTS_PER_PORT];
static volatile uint8_t intChangeVectP1 = 0x00;
#if defined(__MSP430_HAS_PORT2_R__)
static volatile voidFuncPtr intFuncP2[NUM_INTS_PER_PORT];
static volatile uint8_t intChangeVectP2 = 0x00;
#endif
#if defined(__MSP430_HAS_PORT3_R__) && defined (__ENABLE_P3_PIN_INTERRUPTS__)
static volatile voidFuncPtr intFuncP3;
static uint16_t captureMode;
#endif

void attachInterrupt(uint8_t interruptNum, void (*userFunc)(void), int mode) {
	uint8_t bit = digitalPinToBitMask(interruptNum);
	uint8_t port = digitalPinToPort(interruptNum);
  volatile uint8_t *pmap = digitalPinToPortMap(interruptNum);
	if ((port == NOT_A_PIN) || !((mode == FALLING) || (mode == RISING)
		|| (mode == CHANGE))) return;

	__dint();
	switch(port) {
	case P1:
		if(mode != CHANGE) {
			P1IES = mode ? P1IES | bit : P1IES & ~bit;
		} else {
			intChangeVectP1 |= bit;
			P1IES = (P1IN & bit) ? (P1IES | bit) : (P1IES & ~bit);
		}
		P1IFG &= ~bit;
		intFuncP1[bit_pos(bit)] = userFunc;
		P1IE |= bit;
		break;
	#if defined(__MSP430_HAS_PORT2_R__)
	case P2:
		if(mode != CHANGE) {
			P2IES = mode ? P2IES | bit : P2IES & ~bit;
		} else {
			intChangeVectP2 |= bit;
			P2IES = (P2IN & bit) ? (P2IES | bit) : (P2IES & ~bit);
		}
		P2IFG &= ~bit;
		intFuncP2[bit_pos(bit)] = userFunc;
		P2IE |= bit;
		break;
	#endif
  // Port P3 is not interruptible but we can do a workaround by using
  // a timer capture interrupt
	#if defined(__MSP430_HAS_PORT3_R__) && defined(__ENABLE_P3_PIN_INTERRUPTS__)
	case P3:
    intFuncP3 = userFunc;
    // Use Port Mapping Controller to map TA0.2 to P3 pin
    PMAPPWD = PMAPKEY;       // Get write-access to port mapping regs
    *pmap = PM_TA0CCR2A;     // Map TA0.2 input to P3 pin
    PMAPPWD = 0;             // Lock port mapping registers
    P3SEL = bit;
       
    if (mode == RISING)
      captureMode = CM_1;
    else
      captureMode = CM_2;
      
    // setup TA0.2 as compare input on falling edge
    TA0CCTL2 = captureMode + CCIS_0 + SCS + CAP + CCIE;
    TA0CTL = TASSEL__SMCLK + MC__UP + TACLR;
    break;
  #endif
	default:
		break;
	}

	__eint();
}

void detachInterrupt(uint8_t interruptNum) {
	uint8_t bit = digitalPinToBitMask(interruptNum);
	uint8_t port = digitalPinToPort(interruptNum);

	if (port == NOT_A_PIN) return;

	switch(port) {
	case P1:
		P1IE &= ~bit;
		intFuncP1[bit_pos(bit)] = 0;
		intChangeVectP1 &= ~(bit);
		break;
	#if defined(__MSP430_HAS_PORT2_R__)
	case P2:
		P2IE &= ~bit;
		intFuncP2[bit_pos(bit)] = 0;
		intChangeVectP2 &= ~(bit);
		break;
	#endif
	default:
		break;
	}
}

__attribute__((interrupt(PORT1_VECTOR)))
void Port_1(void)
{
	uint8_t i;
	for(i = 0; i < 8; i++) {
		if((P1IFG & BV(i)) && intFuncP1[i]) {
			intFuncP1[i]();
			if(intChangeVectP1 & BV(i)) {
				P1IES ^= BV(i);
				P1IFG = ((P1IN & BV(i)) == (P1IES & BV(i))) ? \
					(P1IFG & ~BV(i)) : P1IFG;
			} else {
				P1IFG &= ~BV(i);
			}
		}
	}
  __bic_SR_register_on_exit(LPM4_bits);  // clears the bits corresponding to LPM4 and exits the low power mode
}

#if defined(__MSP430_HAS_PORT2_R__)
__attribute__((interrupt(PORT2_VECTOR)))
void Port_2(void)
{
	uint8_t i;
	for(i = 0; i < 8; i++) {
		if((P2IFG & BV(i)) && intFuncP2[i]) {
			intFuncP2[i]();
			if(intChangeVectP2 & BV(i)){
				P2IES ^= BV(i);
				P2IFG = ((P2IN & BV(i)) == (P2IES & BV(i))) ? \
					(P2IFG & ~BV(i)) : P2IFG;
			} else {
				P2IFG &= ~BV(i);
			}
		}
	}
  __bic_SR_register_on_exit(LPM4_bits);  // clears the bits corresponding to LPM4 and exits the low power mode
}
#endif

#if defined(__MSP430_HAS_PORT3_R__) && defined(__ENABLE_P3_PIN_INTERRUPTS__)
#pragma vector=TIMER0_A1_VECTOR
__interrupt void Timer_A0_CCRx_Isr(void)
{
  if (intFuncP3)
  {
    intFuncP3();
    // setup TA0.2 as compare input
    TA0CCTL2 = captureMode + CCIS_0 + SCS + CAP + CCIE;
  }
  __bic_SR_register_on_exit(LPM4_bits);  // clears the bits corresponding to LPM4 and exits the low power mode
}
#endif

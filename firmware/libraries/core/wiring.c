/*
  ************************************************************************
  *	wiring.c
  *
  *	Arduino core files for MSP430
  *		Copyright (c) 2012 Robert Wessels. All right reserved.
  *
  *
  ***********************************************************************
  Derived from:
  wiring.c - Partial implementation of the Wiring API for the ATmega8.
  Part of Arduino - http://www.arduino.cc/

  Copyright (c) 2005-2006 David A. Mellis

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
#include "Energia.h"

void initClocks(void);
void enableWatchDogIntervalMode(void);
void disableWatchDog(void);

void init()
{
        disableWatchDog();
	initClocks();
        enableWatchDogIntervalMode();
        __eint();

     /* Port Mapping for I2C - Sprite/CC430 Specific */
  	UCB0CTL1 |= UCSWRST;
  	PMAPPWD = 0x02D52;	// Get write-access to port mapping regs
  	PMAPCTL = PMAPRECFG;  // Allow future runtime port mapping
  	P2MAP0 = PM_UCB0SCL;	// Map SCL input to P2.0 
  	P2MAP1 = PM_UCB0SDA;	// Map SDA output to P2.1 
  	PMAPPWD = 0;		// Lock port mapping registers
  	P2SEL |= BIT0 | BIT1;	//Set P2.0 and P2.1 to USCI Mode
}

void disableWatchDog()
{
        /* Diable watchdog timer */
	WDTCTL = WDTPW | WDTHOLD;
}

void enableWatchDog()
{
	enableWatchDogIntervalMode();
}

/* WDT_TICKS_PER_MILISECOND = (F_CPU / WDT_DIVIDER) / 1000
 * WDT_TICKS_PER_MILISECONDS = 1.953125 = 2 */
#define SMCLK_FREQUENCY F_CPU
#define WDT_TICKS_PER_MILISECOND (2*SMCLK_FREQUENCY/1000000)
#define WDT_DIV_BITS WDT_MDLY_0_5

void enableWatchDogIntervalMode(void)
{
	/* WDT Password + WDT interval mode + Watchdog clock source /512 + source from SMCLK
	 * Note that we WDT is running in interval mode. WDT will not trigger a reset on expire in this mode. */
	WDTCTL = WDTPW | WDTTMSEL | WDTCNTCL | WDT_DIV_BITS;
 
	/* WDT interrupt enable */
#ifdef __MSP430_HAS_SFR__
	SFRIE1 |= WDTIE;
#else
	IE1 |= WDTIE;
#endif	
}

void initClocks(void)
{
#ifdef __MSP430_HAS_BC2__
#if defined(CALBC1_16MHZ_) && F_CPU >= 16000000L
	BCSCTL1 = CALBC1_16MHZ;
	DCOCTL = CALDCO_16MHZ;
#elif defined(CALBC1_12MHZ_) && (F_CPU >= 12000000L)
	BCSCTL1 = CALBC1_12MHZ;
	DCOCTL = CALDCO_12MHZ;
#elif defined(CALBC1_8MHZ_) && (F_CPU >= 8000000L)
	BCSCTL1 = CALBC1_8MHZ;
	DCOCTL = CALDCO_8MHZ;
#elif defined(CALBC1_1MHZ_) && (F_CPU >= 1000000L)
	BCSCTL1 = CALBC1_1MHZ;
	DCOCTL = CALDCO_1MHZ;
#else
        #warning No Suitable Frequency found!
#endif
	/* SMCLK = DCO / DIVS = nMHz */
	BCSCTL2 &= ~(DIVS_0);
	/* ACLK = VLO = ~ 12 KHz */
        BCSCTL3 |= LFXT1S_2; 
#endif

#ifdef __MSP430_HAS_CS__
    CSCTL0 = CSKEY;                // Enable Access to CS Registers
  
    CSCTL2 &= ~SELM_7;             // Clear selected Main CLK Source
    CSCTL2 |= SELM__DCOCLK;        // Use DCO as Main Clock Source
    CSCTL3 &= ~(DIVM_3 | DIVS_3);  // clear DIVM Bits
#if F_CPU >= 24000000L
    CSCTL1 = DCOFSEL0 | DCOFSEL1 | DCORSEL;    //Level 2 / Range 1 : 24.0MHz
#elif F_CPU >= 16000000L
    CSCTL1 = DCORSEL;              //Level 0 / Range 1 : 16.0MHz
#elif F_CPU >= 12000000L
    CSCTL1 = DCOFSEL0 | DCOFSEL1 | DCORSEL;    //Level 2 / Range 1 : 24.0MHz
    CSCTL3 |= DIVM_1;              // Div = 2
#elif F_CPU >= 8000000L
    CSCTL1 = DCOFSEL0 | DCOFSEL1;  //Level 2 / Range 0 : 8.0MHz
#elif F_CPU >= 1000000L
    CSCTL1 = DCOFSEL0 | DCOFSEL1;  //Level 2 / Range 0 : 8.0MHz
    CSCTL3 |= DIVM_3;              // Div = 8
#else
        #warning No Suitable Frequency found!
#endif
//    CSCTL0 = 0;                    // Disable Access to CS Registers
#endif // __MSP430_HAS_CS__

#if defined(__MSP430_HAS_UCS_RF__)
    UCSCTL3 |= SELREF_2;              // Set DCO FLL reference = REFO
	UCSCTL4 |= SELA_2;                // Set ACLK = REFO

	__bis_status_register(SCG0);      // Disable the FLL control loop
	UCSCTL0 = 0x0000;                 // Set lowest possible DCOx, MODx
	UCSCTL1 = DCORSEL_5;              // Select DCO range 16MHz operation
	UCSCTL2 = FLLD_1 + 249;           // Set DCO Multiplier for 8MHz
	                                  // (N + 1) * FLLRef = Fdco
	                                  // (249 + 1) * 32768 = 8MHz
	                                  // Set FLL Div = fDCOCLK/2
	__bic_status_register(SCG0);      // Enable the FLL control loop

	// Worst-case settling time for the DCO when the DCO range bits have been
	// changed is n x 32 x 32 x f_MCLK / f_FLL_reference. See UCS chapter in 5xx
	// UG for optimization.
	// 32 x 32 x 8 MHz / 32,768 Hz = 250000 = MCLK cycles for DCO to settle
	delayClockCycles(250000);
#endif

}
volatile uint32_t wdtCounter = 0;

unsigned long micros()
{
    return (1000 * wdtCounter) / WDT_TICKS_PER_MILISECOND;
}

unsigned long millis()
{
        return wdtCounter / WDT_TICKS_PER_MILISECOND;
}

/* Delay for the given number of microseconds.  Assumes a 1, 8 or 16 MHz clock. */
void delayMicroseconds(unsigned int us)
{
#if F_CPU >= 20000000L
	/* For a one-microsecond delay, simply wait 2 cycle and return. The overhead
	 * of the function call yields a delay of exactly one microsecond. */
	__asm__ __volatile__ (
		"nop" "\n\t"
		"nop");
	if (--us == 0)
		return;

	/* The following loop takes a 1/5 of a microsecond (4 cycles)
	 * per iteration, so execute it five times for each microsecond of
	 * delay requested. */
	us = (us<<2) + us; // x5 us

	/* Account for the time taken in the preceeding commands. */
	us -= 2;

#elif F_CPU >= 16000000L
	/* For the 16 MHz clock on most boards */

	/* For a one-microsecond delay, simply return.  the overhead
	 * of the function call yields a delay of approximately 1 1/8 us. */
	if (--us == 0)
		return;

	/* The following loop takes a quarter of a microsecond (4 cycles)
	 * per iteration, so execute it four times for each microsecond of
	 * delay requested. */
	us <<= 2;

	/* Account for the time taken in the preceeding commands. */
	us -= 2;
#else
	/* For the 1 MHz */

	/* For a one- or two-microsecond delay, simply return.  the overhead of
	 * the function calls takes more than two microseconds.  can't just
	 * subtract two, since us is unsigned; we'd overflow. */
	if (--us == 0)
		return;
	if (--us == 0)
		return;

	/* The following loop takes 4 microsecond (4 cycles)
	 * per iteration, so execute it ones for each 4 microsecond of
	 * delay requested. */
	us >>= 2;

	/* Partially compensate for the time taken by the preceeding commands.
	 * we can't subtract any more than this or we'd overflow w/ small delays. */
	us--;
#endif

	/* Busy wait */
        __asm__ __volatile__ (
                /* even steven */
                "L1: nop \n\t"   
                /* 1 instruction */
                "dec.w %[us] \n\t"
                /* 2 instructions */
                "jnz L1 \n\t"
                : [us] "=r" (us) : "[us]" (us)
        );
}

/* (ab)use the WDT */
void delay(uint32_t milliseconds)
{
	uint32_t wakeTime = wdtCounter + (milliseconds * WDT_TICKS_PER_MILISECOND);
        while(wdtCounter < wakeTime)
                /* Wait for WDT interrupt in LMP0 */
                __bis_status_register(LPM0_bits+GIE);
}

void __inline__ delayClockCycles(register unsigned int n)
{
    __asm__ __volatile__ (
                "1: \n"
                " dec        %[n] \n"
                " jne        1b \n"
        : [n] "+r"(n));
}

__attribute__((interrupt(WDT_VECTOR)))
void watchdog_isr (void)
{
        wdtCounter++;
        /* Exit from LMP3 on reti (this includes LMP0) */
        __bic_status_register_on_exit(LPM3_bits);
}

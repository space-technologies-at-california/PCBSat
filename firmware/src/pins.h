/**
 * Copyright (c) 2015 panStamp <contact@panstamp.com>
 * 
 * This file is part of the panStamp project.
 * 
 * panStamp  is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * any later version.
 * 
 * panStamp is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with panStamp; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 
 * USA
 * 
 * Author: Daniel Berenguer
 * Creation date: 06/17/2015
 */


#ifndef _PINS_H
#define _PINS_H

#include <stdint.h>
#include <msp430.h>

#define __F_CPU                 1000000L

void setup_pins(void);

#define __SPRITE_VERSION_2_0__  1

// Enable pseudo-pin interrupts on P3
#define __ENABLE_P3_PIN_INTERRUPTS__  1

// I2C
#define pinI2Cmap()       P1MAP4 = PM_UCB0SCL; P1MAP3 = PM_UCB0SDA
#define pinI2Cconfig()    P1SEL |= BIT3 | BIT4      // Set P1.4 and P1.3 to USCI function

// PWM
#define pinPWMmap()       P2MAP1 = PM_TA1CCR2A
#define pinPWMconfig()    P2SEL |= BIT1 | BIT2
/*// SPI
#define pinSPImap()       P1MAP0 = PM_UCB0SIMO; P1MAP1 = PM_UCB0SOMI; P1MAP2 = PM_UCB0CLK
#define pinSPIconfig()    P1SEL |= BIT0 | BIT1 | BIT2  // Set P1.0, P1.1 and P1.2 to USCI function
<<<<<<< HEAD
*/// UART
#define pinUARTmap()      P1MAP6 = PM_UCA0TXD; P1MAP5 = PM_UCA0RXD
#define pinUARTconfig()   P1SEL |= BIT6 | BIT5      // Set P1.6 and P1.5 to USCI Mode

#endif

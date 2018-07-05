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

//void setup_pins(void);

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
// UART
#define pinUARTmap()      P1MAP6 = PM_UCA0TXD; P1MAP5 = PM_UCA0RXD
#define pinUARTconfig()   P1SEL |= BIT6 | BIT5      // Set P1.6 and P1.5 to USCI Mode
*/

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
static const uint8_t A0 = 8;
static const uint8_t A1 = 9;
static const uint8_t A2 = 10;
static const uint8_t A3 = 11;
static const uint8_t A4 = 12;
static const uint8_t A5 = 13;        // Connected to NTC
static const uint8_t A10 = 128 + 10; // special. This is the internal temp sensor
static const uint8_t A11 = 128 + 11; // special. This is Vcc/2

// On-board LED
#define ONBOARD_LED 23
#define LED      ONBOARD_LED
#define INIT_ONBOARD_LED()    P3DIR |= BIT7; P3OUT &= ~BIT7
// Preferred analog pin to measure battery voltage
#define BATT_VOLT_PIN   A0
// NTC power pin
#define NTC_POWER_PIN   22
// NTC ADC pin
#define NTC_PIN    A5
// ADC resolution
#define ADC_RESOLUTION  0xFFF

// LD-BOARD definitions
#define HGM     20
#define LNA_EN  19
#define PA_EN   18

//                            ANT
//                         GND | GND
//                       GND | | | GND
//                     GND | | | | | GND
//                       | | | | | | |
//                      +-------------+
//         (D17) P3.1  1|     ||||    |27  P3.2 (D18) (PWM)
//         (D16) P3.0  2|             |26  P3.3 (D19) (PWM)
//         (D15) P2.7  3|             |25  P3.4 (D20)
//         (D14) P2.6  4|             |24  P3.5 (D21)
//      (D13/A5) P2.5  5|             |23  P3.6 (D22)
//      (D12/A4) P2.4  6|             |22  P1.0 (SPI_MOSI) (D0)
//      (D11/A3) P2.3  7|             |21  P1.1 (SPI_MISO) (D1) (PWM)
//      (D10/A2) P2.2  8|             |20  P1.2 (SPI_SCK) (D2) (PWM)
//       (D9/A1) P2.1  9|             |19  P1.3 (I2C_SDA) (D3) (PWM)
//       (D8/A0) P2.0 10|             |18  P1.4 (I2C_SCL) (D4) (PWM)
//                      +-------------+
//                       | | | | | | |
//                     TST | | | | | P1.5 (D5/RXD)
//                       RST | | | P1.6 (D6/TXD)
//                         VCC | P1.7 (D7)
//                            GND
//
//                    ONBOARD_LED -- P3.7 (D23)


extern const uint16_t digital_pin_to_pmap[];

extern const uint16_t port_to_input[];

extern const uint16_t port_to_output[];

extern const uint16_t port_to_dir[];

extern const uint16_t port_to_ren[]; 

extern const uint16_t port_to_sel[];

extern const uint16_t port_to_pmap[];

extern const uint8_t digital_pin_to_timer[];

extern const uint8_t digital_pin_to_port[];

extern const uint8_t digital_pin_to_bit_mask[];

extern const uint16_t digital_pin_to_analog_in[];

#endif

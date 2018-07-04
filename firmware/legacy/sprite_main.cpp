#include <Arduino.h>
#include <HardwareSerial.h>
#include <SpriteRadio.h>
#include <Wire.h>
#include <lsm9ds1.h>
#include "gitrev.h"

/*
  This example code will configure the CC1101 radio core in the CC430 to
  repeatedly transmit a text message. The output signal will be MSK modulated
  at 64 kbps on a 437.24 MHz carrier and bits are encoded by alternating between
  two different 511 bit Gold codes.
*/

unsigned char prn2[64] = {
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

unsigned char prn3[64] = {
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

// Initialize the radio class, supplying the Gold Codes that correspond to 0 and 1
SpriteRadio m_radio = SpriteRadio(prn2, prn3);
lsm9ds1 lsm = lsm9ds1();

static void blink() {
    P3OUT |= BIT7;
    __delay_cycles(SYSTEM_CLK_FREQ / 5);
    P3OUT &= ~BIT7;
    __delay_cycles(SYSTEM_CLK_FREQ / 5);
}

void setupSensor()
{
  // 2.) Set the magnetometer sensitivity
  lsm.setupMag(lsm.LSM9DS1_MAGGAIN_4GAUSS);

  // 3.) Setup the gyroscope
  lsm.setupGyro(lsm.LSM9DS1_GYROSCALE_245DPS);
}

void setup() {
    blink();
    Serial.begin(4800);
    blink();
    Serial.println("\n\r\nsprite test firmware " GIT_REV);
    Serial.print("init I2C...");
    Wire.begin();
    Serial.println("ok");
    Serial.println("init LSM9DS1...");
    lsm.begin();
    setupSensor();
    Serial.println("ok");
    Serial.print("init radio...");
    m_radio.txInit();
    Serial.println("ok");
    blink();
}

void loop() {
     lsm.read();  /* ask it to read in the data */ 

    /* Get a new sensor event */ 
    sensors_event_t m, g; //temp;
        
    lsm.getEvent(&m, &g); 
     
/*    Serial.print("M X: "); Serial.print((int)(100*m.magnetic.x));
    Serial.print("\tY: "); Serial.print((int)(100*m.magnetic.y));  
    Serial.print("\tZ: "); Serial.print((int)(100*m.magnetic.z));
    Serial.println();

    Serial.print("G X: "); Serial.print((int)g.gyro.x); 
    Serial.print("\tY: "); Serial.print((int)g.gyro.y);    
    Serial.print("\tZ: "); Serial.print((int)g.gyro.z);  
    Serial.println();
*/
    __delay_cycles(5 * SYSTEM_CLK_FREQ);
    // Blink LED while transmitter is on
    P3OUT |= BIT7;
    Serial.println("TX");
    m_radio.transmit("Hello Earthlings\n", 17);
    P3OUT &= ~BIT7;
}

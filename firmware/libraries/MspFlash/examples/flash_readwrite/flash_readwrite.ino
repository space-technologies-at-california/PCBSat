/*
  flas_readwrite.h - Read/Write flash memory library example for MSP430 Energia 
  Copyright (c) 2012 Peter Brier.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

  Provide access to the MSP430 flash memory controller. 
  All flash memory can be read, erased and written (except SEGMENT_A, the LOCK bits are not in the code, for a good reason).
  Flash can only be erased per 512 byte segments (except the 4 special information segments, they are 64 bytes in size)
  
  The same flash locations can be written multiple times with new values, but flash bits can only be reset (from 1 to 0) and cannot
  change to a 1 (you need to flash erase the whole segment)

  functions:
  ~~~~~~~~~~
  erase():  Erase a flash segment, all bytes in the segment will read 0xFF after an erase
  
  read(): Read flash locations (actually just a proxy for memcpy)
  
  write(): Write flash locations (actually just a proxy for memcpy), the same location can be written multiple times, 
  but once a bit is reset, you cannot set it with a subsequent write, you need to flash the complete segment to do so.  
  
  constants:
  ~~~~~~~~~~
  SEGMENT_A   // pointer to 64 byte flash segments
  SEGMENT_B
  SEGMENT_C
  SEGMENT_D
  
  Macros:
  ~~~~~~~
  SEGPTR(x)  // Return pointer to first complete segment inside variable
  SEGMENT(n) // Return pointer to start of segment n (n=0..63)
  
  
  NOTE: you are flashing the program memory, you can modify EVERYTHING (program, data) this is usually not what you want. 
  Be carefull when flashing data. You may use SEG_B to SEG_D for normal use, they should not be filled by the compiler
  If you wish to use main memory, you need to inform the linker NOT to use the segments you wish to use in the  linker script 
  (this is not for the faint of heart). 
  
  An alternative approach is to allocate a static variable with a size of AT LEAST 2 SEGMENTS in your program. 
  This makes sure there is at least ONE COMPLETE SEGMENT in this static variable, so there is no colleteral damage when you flash this
  area. You need to find the pointer to the start of the next segment. There is a macro define to do this: SEGPTR(x)
  A example that makes 2 segments available for flashing by allocating 3 segments of constant data:


  Using the example:
  ~~~~~~~~~~~~~~~~~~

  On the launchpad; put the two UART jumpers in HARDWARE SERIAL position (horizontal position) and use the terminal window to connect
  to the board (9600baud). 

  'e' Erase the flash segment
  'w' Write "Hello World" to the flash
  'r' Read the contents of the flash, and print as byte values and characters. stop at the first NULL byte

  - When you program the launchpad and read the flash, a single "0" character should be read (mem contains zero values)
    Writing the flash before you have erased it is not possible (you cannot program OFF bits to ON bits)
  - When you erase the flash, 0xFF values will be read back
  - When you write the flash, "Hello World" will be read back


*/

#include "MspFlash.h"


// Two options to use for flash: One of the info flash segments, or a part of the program memory
// either define a bit of constant program memory, and pass a pointer to the start of a segment to the flash functions,

//*** Option 1: use program memory, uncomment these lines and you have 512 bytes of flash available (1024 bytes allocated) ****
//const unsigned char data[2*SEGMENT_SIZE] = {0};
//#define flash SEGPTR(data)
//

//*** Option 2: use one of the 64 byte info segments, uncomment this line. Use SEGMENT_B, SEGMENT_C or SEGMENT_D (each 64 bytes, 192 bytes in total)
#define flash SEGMENT_D
//


void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly: 
  if ( Serial.available() )
  {
    switch ( Serial.read() )
    { 
      case 'e': doErase(); break;  
      case 'r': doRead(); break; 
      case 'w': doWrite(); break; 
      case 10:
      case 13: break;
      default: doHelp();
    }
  }  
}

void doRead()
{
  unsigned char p = 0;
  int i=0;
  Serial.println("Read:");
  do
  {
    Flash.read(flash+i, &p, 1);
    Serial.write(p);
    Serial.print(":");    
    Serial.println(p);
  } while ( p && (i++ < 16) );
  Serial.println(".");
}


void doErase()
{
 Serial.println("Erase"); 
 Flash.erase(flash); 
 Serial.println("Done."); 
}

void doWrite()
{
 Serial.println("Write");
 Flash.write(flash, (unsigned char*) "Hello World!" ,13); 
 Serial.println("Done.");
}

void doHelp()
{
  int div = (F_CPU/400000L) & 63; 
  Serial.println("flash test: e, r, w");
  Serial.println(F_CPU);
  Serial.println(div); 
}

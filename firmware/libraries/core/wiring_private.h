/*
  ************************************************************************
  *	wiring_private.h
  *
  *	Arduino core files for MSP430
  *		Copyright (c) 2012 Robert Wessels. All right reserved.
  *
  *
  ***********************************************************************
  Derived from:
  wiring_private.h - Internal header file.
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

  $Id: wiring.h 239 2007-01-12 17:58:39Z mellis $
*/

#ifndef WiringPrivate_h
#define WiringPrivate_h

#include "Energia.h"

#ifdef __cplusplus
extern "C"{
#endif

// TODO: This is a hack and needs cleaning up. Not all pins are available on the board
//.Change this to a more intelligent number of interrupt selection

#if defined(__MSP430_HAS_PORT1_R__)
#define EXTERNAL_NUM_INTERRUPTS 8
#elif defined(__MSP430_HAS_PORT2_R__)
#define EXTERNAL_NUM_INTERRUPTS 16
#else
#define EXTERNAL_NUM_INTERRUPTS 8
#endif

typedef void (*voidFuncPtr)(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif

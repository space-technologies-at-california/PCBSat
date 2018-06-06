# You MUST override:
#DEVICE=msp430f5529
#MAIN=blink

# Override the following, if necessary:
MSPDEBUG?=mspdebug tilib
MSP430_TOOLCHAIN?=/usr/local/gcc-msp430-ti-toolchain-5.01.02.00
MSP430_SUPPORT_DIR?=${MSP430_TOOLCHAIN}/include

# The rest should be fine:
CC=${MSP430_TOOLCHAIN}/bin/msp430-elf-gcc
CXX=${MSP430_TOOLCHAIN}/bin/msp430-elf-g++
CFLAGS=-mmcu=${DEVICE} -I ${MSP430_SUPPORT_DIR}
LDFLAGS=-L ${MSP430_SUPPORT_DIR}
RM=rm -f

all: ${MAIN}

.SUFFIX: s

.c.s:
	${CC} -S ${CFLAGS} $<

.PHONY: clean prog verify

clean:
	${RM} ${MAIN}

prog: ${MAIN}
	${MSPDEBUG} 'prog ${MAIN}'

verify: ${MAIN}
	${MSPDEBUG} 'verify ${MAIN}'

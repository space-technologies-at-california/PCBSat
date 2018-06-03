# You MUST override:
#DEVICE=msp430f5529
#MAIN=blink

# Override the following, if necessary:
MSPDEBUG?=mspdebug tilib
MSP430_TOOLCHAIN?=/usr/msp430
MSP430_SUPPORT_DIR?=${MSP430_TOOLCHAIN}/include

# The rest should be fine:
CC=msp430-gcc
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

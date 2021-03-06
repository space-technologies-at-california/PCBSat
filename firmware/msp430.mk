# You MUST override:
DEVICE=cc430f5135
#MAIN=blink

# Override the following, if necessary:
MSPDEBUG?=mspdebug tilib
MSP430_SUPPORT_DIR?=${MSP430_TOOLCHAIN}/include

# The rest should be fine:
CC=${MSP430_TOOLCHAIN}/bin/msp430-elf-gcc
# CXX=${MSP430_TOOLCHAIN}/bin/msp430-elf-g++
CFLAGS+=-mmcu=${DEVICE} -I ${MSP430_SUPPORT_DIR} -mrelax -flto
LDFLAGS+=-L ${MSP430_SUPPORT_DIR}
RM?=rm -f

all: ${MAIN}

.SUFFIX: s c

.c.s:
	@mkdir -p $(@D)
	${CC} -S ${CFLAGS} $< 

.c.o:
	@mkdir -p $(@D)
	${CC} -c ${CFLAGS} $< -o $@ 

# .cpp.o:
#	${CXX} -c ${CFLAGS} $< -o $@

.PHONY: clean prog verify

clean:
	${RM} ${MAIN} ${OBJS} ${MAIN}.map

prog: ${MAIN}
	${MSPDEBUG} 'prog ${MAIN}'

verify: ${MAIN}
	${MSPDEBUG} 'verify ${MAIN}'

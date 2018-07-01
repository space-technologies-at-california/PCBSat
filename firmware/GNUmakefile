MAIN=sprite_main

CORE=arduino/cores/sprite
CORE_CFLAGS=-I . -I ${CORE}
CORE_OBJS:=$(shell ls ${CORE}/*.c* | sed -e 's/c$$/o/' -e 's/cpp$$/o/')

LIB_CFLAGS=-I arduino/libraries/SpriteRadio -I arduino/libraries/Wire -I arduino/libraries/lsm9ds1 -I arduino/libraries/Adafruit_Sensor
LIB_OBJS=arduino/libraries/SpriteRadio/SpriteRadio.o arduino/libraries/Wire/Wire.o arduino/libraries/lsm9ds1/lsm9ds1.o

CFLAGS=-pipe -O2 ${CORE_CFLAGS} ${LIB_CFLAGS} \
       -ffunction-sections -fdata-sections -Xlinker --gc-sections \
	   -Wl,-Map,${MAIN}.map

OBJS=${CORE_OBJS} ${LIB_OBJS} sprite_main.o

sprite_main: ${OBJS} gitrev.h
	${CXX} ${CFLAGS} ${LDFLAGS} -o $@ ${OBJS}

.PHONY: gitrev.h

gitrev.h:
	echo "#define GIT_REV \"`git describe --always || echo unknown`\"" \
		> gitrev.h

include msp430.mk

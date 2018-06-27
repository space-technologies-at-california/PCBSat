MAIN=sprite_main

CORE=arduino/cores/sprite
CORE_CFLAGS=-I . -I ${CORE}
CORE_OBJS:=$(shell ls ${CORE}/*.c* | sed -e 's/c$$/o/' -e 's/cpp$$/o/')

LIB_CFLAGS=-I arduino/libraries/SpriteRadio
LIB_OBJS=arduino/libraries/SpriteRadio/SpriteRadio.o

CFLAGS=-pipe -O2 ${CORE_CFLAGS} ${LIB_CFLAGS}

OBJS=${CORE_OBJS} ${LIB_OBJS} sprite_main.o

sprite_main: ${OBJS}
	${CXX} ${CFLAGS} ${LDFLAGS} -o $@ ${OBJS}

include msp430.mk

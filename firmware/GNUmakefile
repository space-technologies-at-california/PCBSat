MAIN=sprite_main

CORE=arduino/cores/sprite
CORE_CFLAGS=-I . -I ${CORE}
CORE_OBJS:=$(shell ls ${CORE}/*.c* | sed -e 's/c$$/o/' -e 's/cpp$$/o/')

LIB_CFLAGS=-I arduino/libraries/SpriteRadio -I arduino/libraries/Wire
LIB_OBJS=arduino/libraries/SpriteRadio/SpriteRadio.o arduino/libraries/Wire/Wire.o

CFLAGS=-pipe -O2 ${CORE_CFLAGS} ${LIB_CFLAGS}

OBJS=${CORE_OBJS} ${LIB_OBJS} sprite_main.o
PWM_OBJS=sprite_pwm.o

sprite_main: ${OBJS} gitrev.h
	${CXX} ${CFLAGS} ${LDFLAGS} -o $@ ${OBJS}

sprite_pwm: ${PWM_OBJS}
	${CXX} ${CFLAGS} ${LDFLAGS} -o $@ ${PWM_OBJS}

.PHONY: gitrev.h

gitrev.h:
	echo "#define GIT_REV \"`git describe --always || echo unknown`\"" \
		> gitrev.h

include msp430.mk

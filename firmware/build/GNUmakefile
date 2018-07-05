MAIN=main

SRC=../src
VPATH=${SRC}
INCLUDE=-I . -I ${SRC}
OBJS:=$(shell cd ${SRC} ; ls *.c* | sed -e 's/c$$/o/' -e 's/cpp$$/o/')

CFLAGS=-pipe -O2 ${INCLUDE}\
       -ffunction-sections -fdata-sections -Xlinker --gc-sections \
	   -Wl,-Map,${@F}.map

main: ${OBJS} gitrev.h
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ main.o 

.PHONY: gitrev.h

gitrev.h:
	echo "#define GIT_REV \"`git describe --always || echo unknown`\"" \
		> gitrev.h

include msp430.mk

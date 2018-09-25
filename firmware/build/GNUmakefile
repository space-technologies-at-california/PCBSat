MAIN = main

SRCDIR = ../src

INCLUDE = -I . -I ${SRCDIR} -I ${SRCDIR}/tilib/
CFLAGS = -DDEBUG -DDEBUGPRN \
		 -pipe -O2 ${INCLUDE} \
		 -ffunction-sections -fdata-sections -Xlinker --gc-sections \
         -Wl,-Map,${@F}.map

SRCS = \
	battery_mon.c \
	calc.c \
	cc430radio.c \
	cc430uart.c \
	drvr.c \
	i2c.c \
	lsm.c \
	main.c \
	pins.c \
	prn.c \
	radio.c \
	tilib/RF1A.c \
	tilib/adc12_a.c \
	tilib/flashctl.c \
	tilib/hal_pmm.c \
	tilib/ref.c \
	tilib/timer_a.c \
	tilib/ucs.c \
	tilib/wdt_a.c

VPATH = ${SRCDIR}
OBJS = $(SRCS:.c=.o)
DEPS = $(OBJS:.o=.d)

main: ${OBJS} gitrev.h
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ ${OBJS}

-include $(DEPS)

%.d: %.c
	$(CPP) $(CFLAGS) $< -MM -MT $(@:.d=.o) > $@

.PHONY: gitrev.h cleandep

gitrev.h:
	echo "#define GIT_REV \"`git describe --always || echo unknown`\"" \
		> gitrev.h

cleandep:
	$(RM) $(DEPS)

include msp430.mk

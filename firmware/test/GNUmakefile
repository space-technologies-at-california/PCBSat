SRCDIR = ../src

INCLUDE = -I ${SRCDIR}
CFLAGS = -pipe -O2 ${INCLUDE}

SRCS = \
	calc.c \
	test.c

VPATH = ${SRCDIR}
OBJS = $(SRCS:.c=.o)
DEPS = $(OBJS:.o=.d)

test: ${OBJS}
	${CC} ${OBJS} -o $@

-include $(DEPS)

%.d: %.c
	$(CPP) $(CFLAGS) $< -MM -MT $(@:.d=.o) > $@

.PHONY: gitrev.h cleandep

cleandep:
	$(RM) $(DEPS)

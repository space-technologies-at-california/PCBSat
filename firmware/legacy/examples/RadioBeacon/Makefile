DEVICE=cc430f5135
MAIN=RadioBeacon

RadioBeacon: RadioBeacon.c
	${CXX} ${CFLAGS} ${LDFLAGS} -I ../../libraries/SpriteRadio -I ../../libraries/core RadioBeacon.c ../../libraries/core/wiring_digital.c ../../libraries/core/wiring.c

include ../../msp430.mk

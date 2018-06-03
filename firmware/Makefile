DEVICE=cc430f5135
MAIN=RF_Toggle_LED_Demo

SRCS=RF1A.c RF_Toggle_LED_Demo.c RfRegSettings.c hal_pmm.c

RF_Toggle_LED_Demo: ${SRCS}
	${CC} ${CFLAGS} ${LDFLAGS} -g -DMHZ_868 ${SRCS} -o ${MAIN}

include msp430.mk

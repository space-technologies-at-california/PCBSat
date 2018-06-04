######################################
# Board specific
ARCH = msp430
MCU = cc430f5135
MCU_FLAG = -mmcu=$(MCU)
F_CPU = 1000000L
FLASH_SIZE = 16384
UPLOAD_COMMAND = $(MSPDEBUG) $(VERBOSE_UPLOAD) tilib --force-reset "prog build/$(SKETCH_NAME).bin"
###########

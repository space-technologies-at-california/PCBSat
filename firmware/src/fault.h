#include <stdint.h>

enum {
    FAULT_POWER = 1,
    FAULT_RADIO = 2,
};

extern uint8_t faults;

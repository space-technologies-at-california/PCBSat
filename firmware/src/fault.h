#include <stdint.h>

enum {
    FAULT_RECENT_POR = 1,
    FAULT_POWER = 2,
    FAULT_RADIO = 3,
    FAULT_LSM_SETUP = 4,
};

extern volatile uint8_t faults;

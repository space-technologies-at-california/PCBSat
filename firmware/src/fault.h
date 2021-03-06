#include <stdint.h>

enum {
    FAULT_RECENT_POR = 1,
    FAULT_POWER = 2,
    FAULT_RADIO = 4,
    FAULT_LSM_SETUP = 8,
    FAULT_MPPT = 16,
    FAULT_SAFEMODE = 32,
    FAULT_GPF = 64,
};

extern volatile uint8_t faults;

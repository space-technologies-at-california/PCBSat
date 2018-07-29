#ifndef _battery_mon
#define _battery_mon

#include <msp430.h>
#include "adc12_a.h"

extern volatile uint16_t batt_voltage;

extern ADC12_A_configureMemoryParam ADC_config;

extern void setup_bat_monitor(void);

extern void start_bat_monitor(void);

extern void stop_bat_monitor(void);

extern void shutdown_bat_monitor(void);

#endif

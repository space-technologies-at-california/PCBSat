#pragma once

#include "adc12_a.h"

extern volatile uint8_t batt_voltage;
extern volatile uint8_t temp_measure;
extern ADC12_A_configureMemoryParam ADC_config;

extern void setup_bat_monitor(void);
extern void shutdown_bat_monitor(void);
extern void start_bat_monitor(void);
extern void stop_bat_monitor(void);

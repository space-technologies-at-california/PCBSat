/*******************************************************
* Battery Monitor Code
*******************************************************/

#include <msp430.h>
#include "adc12_a.h"
#include "ref.h"

volatile uint16_t batt_voltage;

const ADC12_A_configureMemoryParam ADC_config = {ADC12_A_MEMORY_0,
                                                        ADC12_A_INPUT_BATTERYMONITOR,
                                                        ADC12_A_VREFPOS_INT,
                                                        ADC12_A_VREFNEG_AVSS,
                                                        ADC12_A_NOTENDOFSEQUENCE};

// This function sets up internal battery monitor functionality, 
// but does not start battery monitoring. Note calling this function
// and not using battery monitoring is wasteful as it turns on the 
// reference. 
void setup_bat_monitor(void) {
    // Turn on the Reference
    while (REF_ACTIVE == Ref_isRefGenBusy(REF_BASE));
    
    Ref_setReferenceVoltage(REF_BASE, REF_VREF1_5V);
    Ref_enableReferenceVoltage(REF_BASE);
    
    __delay_cycles(75);

    // The ADC will use ACLK = VLO ~ 10kHz
    // The input divider is set to 32, ADC_CLK = 312Hz
    // The ADC is pulse sampled, and a pulse occurs
    // every 1024 cycles. The effective sample rate is
    // 0.3Hz
    ADC12_A_init(ADC12_A_BASE, 
                 ADC12_A_SAMPLEHOLDSOURCE_SC,
                 ADC12_A_CLOCKSOURCE_ACLK,
                 ADC12_A_CLOCKDIVIDER_32);
    ADC12_A_enable(ADC12_A_BASE);
    ADC12_A_setResolution(ADC12_A_BASE, 
                          ADC12_A_RESOLUTION_8BIT);
    ADC12_A_setupSamplingTimer(ADC12_A_BASE,
                               ADC12_A_CYCLEHOLD_1024_CYCLES,
                               ADC12_A_CYCLEHOLD_1024_CYCLES,
                               ADC12_A_MULTIPLESAMPLESENABLE);
    ADC12_A_configureMemory(ADC12_A_BASE,
                            &ADC_config);

    // Set the buffer to be in lower power burst mode, only 
    // turning on when necessary
    ADC12_A_setReferenceBufferSamplingRate(ADC12_A_BASE,
                                           ADC12_A_MAXSAMPLINGRATE_50KSPS);
    ADC12_A_enableReferenceBurst(ADC12_A_BASE);
}

// This function enables battery monitoring functionality
void start_bat_monitor(void) {
    batt_voltage = 0;
    ADC12_A_clearInterrupt(ADC12_A_BASE, ADC12IFG0);
    ADC12_A_enableInterrupt(ADC12_A_BASE, ADC12IFG0);
    // We will use repeated single channel conversion mode to
    // get this working in the background
    ADC12_A_startConversion(ADC12_A_BASE,
                            ADC12_A_MEMORY_0,
                            ADC12_A_REPEATED_SINGLECHANNEL);
}

// This function disables battery monitoring functionality
void stop_bat_monitor(void) {
    ADC12_A_clearInterrupt(ADC12_A_BASE, ADC12IFG0);
    ADC12_A_disableInterrupt(ADC12_A_BASE, ADC12IFG0);
    ADC12_A_disableConversions(ADC12_A_BASE, ADC12_A_COMPLETECONVERSION);
}

// This function shuts down the ADC based battery monitor
void shutdown_bat_monitor(void) {
    ADC12_A_disable(ADC12_A_BASE);
    Ref_disableReferenceVoltage(REF_BASE);
}

void __interrupt_vec(ADC12_VECTOR) isr_adc() {
    uint8_t adc_result;
    switch (__even_in_range(ADC12IV, 0x24)) {
        case ADC12IV_ADC12IFG0:
            adc_result = ADC12_A_getResults(ADC12_A_BASE,
                                            ADC12_A_MEMORY_0);
            batt_voltage = adc_result * 100 / 255;
            break;
        default: break;
    }
}
#include <msp430.h>
#include <stdbool.h>

#include "cc430uart.h"
#include "drvr.h"
#include "battery_mon.h"
#include "proto.h"
#include "lsm.h"

static bool has_setup = false;

void setup_drvr() {
    // Configure DRVR pins to be outputs
    P2DIR |= BIT1 | BIT2 | BIT3;
    P5DIR |= BIT0 | BIT1;

    //Configure TimerA1
    // Use SMCLK, Input Divider=1, Mode=Up (count to TA1CCR0)
    TA1CTL |= TASSEL_2 | MC_0;

    P1OUT &= ~BIT0;  // REG_D0 LOW
    P3OUT |= BIT6;  // REG_D1 HIGH
    has_setup = true;
}

static void drvr_on(uint8_t drvr, int8_t pwm) {
    // First enable the specific Driver
    switch (drvr) {
        case XAXIS : 
            P2OUT |= BIT3;
            break;
        case YAXIS : 
            P5OUT |= BIT0;
            break;
        case ZAXIS : 
            P5OUT |= BIT1;
    }

    // Enable the timer
    if (pwm < 0) {
        TA1CCTL1 |= OUTMOD_3;
        pwm = -1*pwm; 
        TA1CCR1 = (uint8_t) pwm;
        P2SEL |= BIT2;
        P2DIR |= BIT1;
        P2OUT &= ~BIT1;
    } else {
        TA1CCTL2 |= OUTMOD_3;
        TA1CCR2 = pwm;
        P2SEL |= BIT1;
        P2DIR |= BIT2;
        P2OUT &= ~BIT2;
        // Need to turn on bit2, since bit 1 is mapped to TA1CCR2
    }

    TA1CCR0 = 100;
    TA1CTL |= MC_1 | TACLR;
}

static void drvr_off() {
    TA1CTL = TASSEL_1 | MC_0;
    P2OUT &= ~BIT3;
    P5OUT &= ~(BIT0 | BIT1);
    P2SEL &= ~(BIT1 | BIT2);
    P2OUT |= BIT1 | BIT2;
}

uint8_t run_actuation(uint8_t axis, int8_t power, struct vec3_s *data) {
#ifdef DEBUG
    uart_write("actuating\r\n", 11);
#endif
    if (!has_setup) {
        setup_drvr();
    }
    struct vec3_s data_gyro_init;
    struct vec3_s data_gyro_dump;
    readGyro(&data_gyro_init);
    drvr_on(axis, power);
    uint8_t i;
    for (i = 0; i < 100; i++) {
        if (batt_voltage < 179) {
            break;
        }
        readGyro(&data_gyro_dump);
        sleep(50, LPM3_bits);
    }
    drvr_off();
    struct vec3_s data_gyro_final;
    readGyro(&data_gyro_final);
    data->x = data_gyro_final.x - data_gyro_init.x;
    data->y = data_gyro_final.y - data_gyro_init.y;
    data->z = data_gyro_final.z - data_gyro_init.z;
#ifdef DEBUG
    if (i < 100) {
        uart_write("early\r\n", 11);
    } else { 
        uart_write("done\r\n", 11);
    }
#endif
    return i;
}

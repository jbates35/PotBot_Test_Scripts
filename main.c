#include "Peripheral_Headers/F2802x_device.h"
#include "F2807x_EPwm_defines.h"

#include "servo.h"
#include "adc.h"

/**
 * main.c
 */

int main(void)
{
    //For testing adc
    uint16_t adc_read1, adc_read2;

    DeviceInit();

    CpuTimer0Regs.PRD.all = mSec1;
    CpuTimer1Regs.PRD.all = mSec100;

    //Set servo
    float dc_min[8] = { 0.018, 0.018, 0.018, 0,0,0,0,0 };
    float dc_max[8] = { 0.118, 0.118, 0.118, 0,0,0,0,0 };

    servo_init(3, dc_min, dc_max); // initialize 3 servos

    adc_init(3,true); //Initialize 3 ADC channels, and turn temperature sensor on
    //adc_trigger_select(0, TRIGGER_EPWM1A);

    int32_t temperature;

    while(1) {
        if(CpuTimer1Regs.TCR.bit.TIF==1) {
            CpuTimer1Regs.TCR.bit.TIF=1;
            temperature = temp_sample(true); // Sample temperature, start conversion
        }
        if(CpuTimer0Regs.TCR.bit.TIF==1) {
            CpuTimer0Regs.TCR.bit.TIF=1;

            //Example of a adc sample where soc isn't needed (such as when there's a trigger set)
            //adc_read1 = adc_sample(0, false); //Sample ADCRESULT0, start conversion

            //Example of a adc sample where soc is needed (such as when there's no trigger set)
            adc_read1 = adc_sample(0, true); //Sample ADCRESULT0, start conversion

            //Example of setting servo
            servo_set(0, adc_read1); // Set servo, note adc_read MUST be uint16_t

        }
    }
	return 0;
}


/*
 * ADC pins:
 * A0 - J5-6
 * A1 - J5-5
 * A2 - J1-8
 * A3 - J5-4
 * A4 - J1-6
 *
 * B1 - J5-7
 * B2 - J1-9
 * B3 - J5-8
 * B4 - J1-10
 *
 * ePWM 1
 * a - GPIO0 (J6-1)
 * b - GPIO1 (J6-2)
 *
 * ePWM 2
 * a - GPIO2 (J6-3)
 * b - GPIO3 (J6-4)
 *
 * ePWM 3
 * a - GPIO4 (J6-5)
 * b - GPIO5 (J6-6)
 *
 * ePWM 4
 * a - GPIO6 (J2-8)
 * b - GPIO7 (J2-9)
 *
 */

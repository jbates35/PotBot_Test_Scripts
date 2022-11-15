#include "Peripheral_Headers/F2802x_device.h"
#include "F2807x_EPwm_defines.h"

#include "servo.h"
#include "adc.h"
#include "dsp.h"

/**
 * main.c
 */

//Stuff for input arrays
#define INPUT_SIZE 100 // Size of input array

int16_t adc_in_x[] = {[0 ... INPUT_SIZE*2-1] = 0}; // adc for x
int16_t adc_in_y[] = {[0 ... INPUT_SIZE*2-1] = 0}; // adc for y
int16_t out_x = 0; // output buffer for x
int16_t out_y = 0; // output buffer for y

int16_t fir_N;          //Length of array
int16_t fir_counter;    // Point of array where FIR is calculated from


N = INPUT_SIZE;
fir_counter = INPUT_SIZE-1;

//End stuff for input arrays



int main(void)
{
    //For testing adc
    int16_t adc_read0;

    DeviceInit();

    CpuTimer0Regs.PRD.all = mSec1 * 20;
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

            //Get N value
            adc_read0 = adc_sample(0, true); //Start soc and sample
            y_fit(&fir_N, &adc_read0, 0,4095, 1,100); // Change 0-4095->1-100

        }
        if(CpuTimer0Regs.TCR.bit.TIF==1) {
            CpuTimer0Regs.TCR.bit.TIF=1;

            //Example of a adc sample where soc isn't needed (such as when there's a trigger set)
            //adc_read1 = adc_sample(0, false); //Sample ADCRESULT0, start conversion

            //Example of a adc sample where soc is needed (such as when there's no trigger set)
            //adc_read1 = adc_sample(0, true); //Sample ADCRESULT0, start conversion

            //Start input array
            adc_in_x[fir_counter] = adc_sample(1, true); //Sample ADCRESULT0, start conversion
            adc_in_x[fir_counter+INPUT_SIZE] = adc_in_x[fir_counter];

            //Calculate moving average
            moving_average(&out_x, &adc_in_x, fir_N, fir_counter);

            //Example of setting servo
            servo_set(0, out_x); // Set servo, note adc_read MUST be uint16_t

            //Move counter of where to start from in FIR circular buffer
            if(fir_counter==0) fir_counter=INPUT_SIZE-1; //Rollover
            else fir_counter--;

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

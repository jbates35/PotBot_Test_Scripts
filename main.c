#include "Peripheral_Headers/F2802x_device.h"
#include "F2807x_EPwm_defines.h"

#include "servo.h"
#include "adc.h"
#include "dsp.h"
#include "uart.h"

/**
 * main.c
 */

//Stuff for input arrays

int16_t adc_in_x[] = {[0 ... FIR_INPUT_SIZE*2-1] = 0}; // adc for x
int16_t adc_in_y[] = {[0 ... FIR_INPUT_SIZE*2-1] = 0}; // adc for y
int16_t adc_out_x = 0; // output buffer for x
int16_t adc_out_y = 0; // output buffer for y

int16_t fir_N;          //Length of array
int16_t fir_counter;    // Point of array where FIR is calculated from


N = FIR_INPUT_SIZE;
fir_counter = FIR_INPUT_SIZE-1;

//End stuff for input arrays

//For testing adc
int16_t adc_result_FIR_N;
int16_t servo_degs;

//Stuff for UART module
char completed_string[UART_BUFF_SIZE]; //
char buffer_string[UART_BUFF_SIZE]; // puts uart rx into a string form
int buffer_index; // keeps track of parsed string position
int buffer_string_ready;

int main(void)
{
    DeviceInit();

    CpuTimer0Regs.PRD.all = mSec1 * 20;
    CpuTimer1Regs.PRD.all = mSec100;

    //Set servo
    float dc_min[8] = { 0.018, 0.018, 0.018, 0,0,0,0,0 };
    float dc_max[8] = { 0.118, 0.118, 0.118, 0,0,0,0,0 };

    servo_init(3, dc_min, dc_max); // initialize 3 servos

    adc_init(3,true); //Initialize 3 ADC channels, and turn temperature sensor on
    //adc_trigger_select(0, TRIGGER_EPWM1A);

    uart_init();

    //Stuff for servo
    //int16_t servo_degs = 0;


    int j;
    for (j = 0; j<UART_BUFF_SIZE; j++) buffer_string[j] = NULL;

    int32_t temperature;

    buffer_index=0;

    while(1) {
/*
        //UART INTERRUPT ROUTINE
        if(SciaRegs.SCIRXST.bit.RXRDY==1)
        {

            uint16_t buffer_int; //for dumping whole word into
            char buffer; // just the char part

            //Dump result into buffer, parse into string
            buffer_int = SciaRegs.SCIRXBUF.all;
            buffer = (char) buffer_int;

            //Ensure buffer string ready starts at 0
            buffer_string_ready = 0;

            //Make decision of indexing based on character
            if(buffer=='<') {
                //Restart buffer index
                buffer_index=0;

                //Clear string
                int i;
                for(i=0; i<UART_BUFF_SIZE; i++) buffer_string[i]=NULL;


            } else if(buffer=='>') {
                //End of uart buffer, put eol
                buffer_string[buffer_index]='\0';

                //Signal flag that string can be dumped and processed
                buffer_string_ready=1;
            } else {
                //dump buffer in appropriate spot in buffer string
                buffer_string[buffer_index] = buffer;

                //increment the index for next character
                buffer_index++;
            }
        }

        */

        uart_rx(&buffer_string, &buffer_string_ready);

        //If flag ready, dump buffer_string into character string
        if(buffer_string_ready==1) {
            int i;
            for(i=0; i<UART_BUFF_SIZE; i++) completed_string[i] = (char) buffer_string[i];
            buffer_string_ready=0;

            EALLOW;
            SciaRegs.SCICTL1.bit.SWRESET=1; // Reset SCI
            EDIS;
        }

        //END UART ISR


/*



            //If buffer string is completed, dump string and turn off flag
            if(buffer_string_ready == 1) {

                //Dumping string


                //Clear flag
                buffer_string_ready = 0;

                //Call post for parsing string
                //FOR RTOS
            }

            //Send acknowledgement
            uart_tx_char(RX_READY);

        }
*/
        if(CpuTimer0Regs.TCR.bit.TIF==1) {
            CpuTimer0Regs.TCR.bit.TIF=1;

            //Example of a adc sample where soc isn't needed (such as when there's a trigger set)
            //adc_read1 = adc_sample(0, false); //Sample ADCRESULT0, start conversion

            //Example of a adc sample where soc is needed (such as when there's no trigger set)
            //adc_read1 = adc_sample(0, true); //Sample ADCRESULT0, start conversion

/*


            //Start input array
            adc_in_x[fir_counter] = adc_sample(1, true); //Sample ADCRESULT0, start conversion
            adc_in_x[fir_counter+FIR_INPUT_SIZE] = adc_in_x[fir_counter];

            //Measure FIR:
            GpioDataRegs.GPASET.bit.GPIO6 = 1;

            //Calculate moving average
            moving_average(&adc_out_x, &adc_in_x, fir_N, fir_counter);

            //End measure FIR
            GpioDataRegs.GPACLEAR.bit.GPIO6 = 1;

            //Convert adc to degrees
            y_fit(&adc_out_x, &servo_degs, ADC_MIN, ADC_MAX, SERVO_MIN, SERVO_MAX);

            //Example of setting servo
            servo_set(0, servo_degs); // Set servo, note adc_read MUST be uint16_t

            //Move counter of where to start from in FIR circular buffer
            if(fir_counter==0) fir_counter=FIR_INPUT_SIZE-1; //Rollover
            else fir_counter--;

            //Get N value
            adc_result_FIR_N = adc_sample(0, true); //Start soc and sample
            y_fit(&adc_result_FIR_N, &fir_N, ADC_MIN, ADC_MAX, N_MIN, N_MAX); // Change 0-4095->10-100


 */
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

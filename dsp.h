/*
 * dsp.h
 *
 *  Created on: Nov. 14, 2022
 *      Author: Jimmy Bates and Tom Kuzma
 *
 * FIR module based on variable moving average
 *
 */

#ifndef DSP_H_
#define DSP_H_

#include "Peripheral_Headers/F2802x_device.h"

//****** moving_average ********//
//
// calculates moving average of input array, unsigned ints for adc
//
// Arguments:
// int16_t out - output buffer
// int16_t in - input array
// int16_t N - how many terms to be averaged
// int16_t start - position pointer should be at for input
//
// Return : None
//
//**************************//
void moving_average(int16_t *out, int16_t *in, int16_t N, int16_t start);


#endif /* DSP_H_ */

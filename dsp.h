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

//****** moving_average_adc ********//
//
// calculates moving average of input array, unsigned ints for adc
//
// Arguments:
// uint16_t out - output buffer
// uint16_t in - input array
// uint16_t N - how many terms to be averaged
// uint16_t start - position pointer should be at for input
//
// Return : None
//
//**************************//
void moving_average_adc(uint16_t *out, uint16_t *in, uint16_t N, uint16_t start);


void moving_average_degs(int16_t *out, int16_t *in, uint16_t N, uint16_t start);

#endif /* DSP_H_ */

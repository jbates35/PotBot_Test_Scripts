/*
 * dsp.c
 *
 *  Created on: Nov. 14, 2022
 *      Author: Jimmy Bates, Tom Kuzma
 */

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
void moving_average_adc(uint16_t *out, uint16_t *in, uint16_t N, uint16_t start) {
   // Pointer which holds input values
   int* p = NULL;

   //New variable for holding output (int32 to accept overflows)
   uint32_t temp_out;

   //Iterate through input
   for (p = in + start; p < in + (start + N); ++p) {
       temp_out += *p;
   }

   //Store output in buffer
   *out = (uint16_t) temp_out/N;
}


void moving_average_degs(int16_t *out, int16_t *in, uint16_t N, uint16_t start) {

}

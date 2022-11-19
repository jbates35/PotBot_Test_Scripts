/*
 * uart.c
 *
 *  Created on: Nov. 16, 2022
 *      Author: Jimmy Bates and Tom Kuzma
 *
 * Takes care of setting uart up, and then
 * contains functions to use the uart -
 * namely, being able to receive from the pi
 * a message and then parse through it
 *
 */

#include "uart.h"

uint16_t __uart_c_buffer_index; // index of uart array
char __uart_c_buffer_string[UART_BUFF_SIZE]; // character array to stuff vals in


//****** uart_init ********//
//
// Initializes all regs needed for uart
//
// Return : None
//
//**************************//
void uart_init(void) {

    //initialize uart globals
    __uart_c_buffer_index=0;

    //UART SETUP START
    EALLOW;

    SysCtrlRegs.PCLKCR0.bit.SCIAENCLK = 1;  // Enable SCI Clock
    SysCtrlRegs.LOSPCP.bit.LSPCLK = 0x02; // Sysclk (MIGHT NEED TO BE REMOVED IN RTOS VERSION)

    //UART RX
    GpioCtrlRegs.GPAPUD.bit.GPIO28 = 0;     // Enable Pull-up for GPO28
    GpioCtrlRegs.GPAMUX2.bit.GPIO28 = 1;    // Enable UART RX
    GpioCtrlRegs.GPAQSEL2.bit.GPIO28 = 3;   // Set as asynchronous input

    //UART TX
    GpioCtrlRegs.GPAPUD.bit.GPIO29 = 1;     // Disable Pull-up for GPIO29
    GpioCtrlRegs.GPAMUX2.bit.GPIO29 = 1;    // Enable UART TX

    SciaRegs.SCIFFTX.bit.SCIFFENA = 0;  //Disable FIFO
    SciaRegs.SCICCR.all =0x0007;    //No parity bit, 1 stop bit


    //Set baud rate of 115200bps (with low clock speed of 15MHz)
    SciaRegs.SCIHBAUD = 0; // Not needed
    SciaRegs.SCILBAUD = 0x000F; // BRR = LSPCLK/(Baud*8) - 1

    SciaRegs.SCICTL2.bit.RXBKINTENA = 1; // Enable SCI interrupt

    SciaRegs.SCICTL1.bit.TXENA = 1; // Enable TX
    SciaRegs.SCICTL1.bit.RXENA = 1; // Enable RX
    SciaRegs.SCICTL1.bit.SWRESET = 1; // Reset SCI

    EDIS;
    //END UART SETUP
}

//****** uart_tx_char ********//
//
// Sends a single character through uart
//
// Arguments:
// char input - single character being sent
//
// Return : None
//
//**************************//
void uart_tx_char(char input) {
    //Wait until other characters have been sent
    //while (SciaRegs.SCIFFTX.bit.TXFFST != 0)   {}
    while(SciaRegs.SCICTL2.bit.TXRDY == 0) { }
    SciaRegs.SCITXBUF=(uint16_t)input;
}

//****** uart_tx_str ********//
//
// Sends a single character through uart
//
// Arguments:
// char input - array of characters (string) being sent
//
// Return : None
//
//**************************//
void uart_tx_str(char *input) {
    //Iterate through loop until end of character
    int i=0; //Create index
    while(input[i] != '\0') uart_tx_char(input[i++]);
}

//****** uart_rx ********//
//
// Receive uart rx buffer and dump result into variable
//
// Arguments:
// char **input_string - string to put buffer into
// int **buff_i - index of the buffer string
// bool_t *ready - associate this with the flag, string is ready to be parsed
// Return : None
//
//**************************//
void uart_rx(char **input_string, int *ready) {

    //UART INTERRUPT ROUTINE
    if(SciaRegs.SCIRXST.bit.RXRDY==1) {
        strcpy(__uart_c_buffer_string, input_string);

        //Dump result into buffer, parse into string
        uint16_t buffer_int = SciaRegs.SCIRXBUF.all; //for dumping whole word into
        char buffer = (char) buffer_int; // just the char part

        //Ensure buffer string ready starts at 0
        *ready = 0;

        //Make decision of indexing based on character
        if(buffer=='<') {
            //Restart buffer index
            __uart_c_buffer_index=0;

            //Clear string
            int j;
            for(j=0; j<UART_BUFF_SIZE; j++) __uart_c_buffer_string[j]=NULL;

        } else if(buffer=='>') {
            //End of uart buffer, put eol
            __uart_c_buffer_string[__uart_c_buffer_index]='\0';

            //Signal flag that string can be dumped and processed
            *ready=1;
        } else {
            //dump buffer in appropriate spot in buffer string
            __uart_c_buffer_string[__uart_c_buffer_index] = (char) buffer;

            //increment the index for next character
            __uart_c_buffer_index++;
        }

        strcpy(input_string, __uart_c_buffer_string);
    }
}



//****** uart_rx ********//
//
// Take the rx buffer and parse through it, converting the string into x y and z values
//
// Arguments:
// char *buffer - variable to dump the rx buffer into
// int16_t *x - pointer of x position
// int16_t *y - pointer of y position
// bool_t *z - pointer of z state (true or false)
//
// Return : None
//
//**************************//
void parse_rx(char string[14], int16_t *x, int16_t *y, bool_t *z) {
}

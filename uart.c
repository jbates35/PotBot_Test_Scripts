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

//****** uart_init ********//
//
// Initializes all regs needed for uart
//
// Return : None
//
//**************************//
void uart_init(void) {

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

    //Enable FIFO
    SciaRegs.SCIFFTX.all=0xE040;
    SciaRegs.SCIFFRX.all=0x2044; // Note- has highest interrupt level
    SciaRegs.SCIFFCT.all=0x0; // This just means turn auto-detect baud off


    SciaRegs.SCICCR.all =0x0007;    //No parity bit, 1 stop bit

    //Set baud rate of 115200bps
    /******** IMPORTANT *************/
    // (with low speed clock at 15MHZ)
    SciaRegs.SCIHBAUD = 0; // Not needed
    SciaRegs.SCILBAUD = 0x000F; // BRR = LSPCLK/(Baud*8) - 1

    SciaRegs.SCICTL1.all =0x0023;  // Relinquish SCI from Reset

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
    while (SciaRegs.SCIFFTX.bit.TXFFST != 0)   {}
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
// char *buffer - variable to dump the rx buffer into
//
// Return : None
//
//**************************//
void uart_rx(char *buffer) {

    //create index
    int i = 0;
    char temp_buf[13];

    if(SciaRegs.SCIFFRX.bit.RXFFST==1)
        *buffer = "";

    while(SciaRegs.SCIFFRX.bit.RXFFST==1 || i<13) {
        temp_buf[i] = SciaRegs.SCIRXBUF.all;
        i++;
    }

    if(*buffer="") *buffer=temp_buf;
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
void parse_rx(char *buffer, int16_t *x, int16_t *y, bool_t *z) {
}

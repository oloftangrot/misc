/**
* @file $RCSfile: uart_t1.c,v $
* Copyright (c) 2004 Atmel.
* Please read file license.txt for copyright notice.
* @brief This file is an example to use uart with timer1.
* UART will echo a received data.
* This file can be parsed by Doxygen for automatic documentation
* generation.
* Put here the functional description of this file within the software
* architecture of your program.
* @version $Revision: 1.0 $ $Name: $
*/
/* @section I N C L U D E S */
#include <at89x51.h>

char uart_data;
/**
* FUNCTION_PURPOSE: This file set up uart in mode 1 (8 bits uart) with
* timer 1 in mode 2 (8 bits auto reload timer).
* FUNCTION_INPUTS: void
* FUNCTION_OUTPUTS: void
*/
void main (void)
{
	SCON = 0x50; /* uart in mode 1 (8 bit), REN=1 */
	TMOD = TMOD | 0x20 ; /* Timer 1 in mode 2 */
	TH1 = 0xFD; /* 9600 Bds at 11.059MHz */
	TL1 = 0xFD; /* 9600 Bds at 11.059MHz */
	ES = 1; /* Enable serial interrupt*/
	EA = 1; /* Enable global interrupt */
	TR1 = 1; /* Timer 1 run */
	while(1); /* endless */
}
/**
* FUNCTION_PURPOSE: serial interrupt, echo received data.
* FUNCTION_INPUTS: P3.0(RXD) serial input
* FUNCTION_OUTPUTS: P3.1(TXD) serial output
*/
void serial_IT(void) __interrupt( 4 )
{
	if (RI == 1) { /* if reception occur */
		RI = 0; /* clear reception flag for next reception */
		uart_data = SBUF; /* Read receive data */
		SBUF = uart_data; /* Send back same data on uart*/
	}
	else TI = 0;	/* if emission occur */
}					/* clear emission flag for next emission*/
/**
* @file $RCSfile:uart_multiproc_slave.c,v $
*
* Copyright (c) 2004 Atmel.
*
* Please read file license.txt for copyright notice.
*
* @brief This file is an example to use uart with timer in
* multiprocessor mode.
* Slave will echo a received data to master.
* This file can be parsed by Doxygen for automatic documentation
* generation.
* Put here the functional description of this file within the software
* architecture of your program.
*
* @version $Revision: 1.0 $ $Name: $
*/
/* @section I N C L U D E S */
#include <at89x51.h>

char uart_data;
__bit TxOK=0;
__bit echo=0;
/**
* FUNCTION_PURPOSE: This file set up uart in mode 3 (9 bits uart) with
* timer 1 in baud rate generator mode.
* FUNCTION_INPUTS: void
* FUNCTION_OUTPUTS: P3.1(TXD) serial output
*/
void main (void)
{
	SCON = 0xF0; /* uart in mode 3 (9 bit), REN=1 */
	SADDR=0x03; /* local address */
	SADEN=0xFF; /* address mask */
	TMOD = TMOD | 0x20 ; /* Timer 1 in mode 2 */
	TH1 = 0xFD; /* 9600 Bds at 11.059MHz */
	TL1 = 0xFD; /* 9600 Bds at 11.059MHz */
	ES = 1; /* Enable serial interrupt */
	EA = 1; /* Enable global interrupt */
	TR1 = 1; /* Timer 1 run */
	while(1) { /* endless */
		while(!echo); /* wait data to echo */
		echo = 0; /* disable echo */
		TB8 = 1; /* address mode */
		TxOK=1; /* set software flag */
		SBUF = 0x01; /* send master adress */
		while(TxOK); /* wait the stop bit transmition */
		TB8 = 0; /* data mode */
		TxOK=1; /* set software flag */
		SBUF = uart_data; /* send data */
		while(TxOK); /* wait the stop bit transmition */
	}
}
/**
* FUNCTION_PURPOSE: serial interrupt, receive data to master
* FUNCTION_INPUTS: P3.0(RXD) serial input
* FUNCTION_OUTPUTS: none
*/
void serial_IT(void) __interrupt( 4 )
{
	if (TI == 1) { /* if reception occur */
		TI=0; /* clear transmition flag for next transmition */
		TxOK=0; /* clear software transmition flag */
	}
	if (RI == 1) { /* if reception occur */
		RI = 0; /* clear reception flag for next reception */
		if(RB8) SM2=0; /* go into data mode */
		else {
			uart_data = SBUF; /* Read receive data */
			SM2=1; /* return into address mode after receive data */
			echo=1; /* enable echo */
		}
	}
}

/**
* @file $RCSfile: t0_m0_t_gh.c,v $
*
* Copyright (c) 2004 Atmel.
*
* Please read file license.txt for copyright notice.
*
* @brief This file is an example to use timer0 in mode 0.
*
* This file can be parsed by Doxygen for automatic documentation
* generation.
* Put here the functional description of this file within the software
* architecture of your program.
*
* @version $Revision: 1.0.0 $ $Name: $
*/
/* @section I N C L U D E S */
#include <at89x51.h>
/**
* FUNCTION_PURPOSE: This file set up timer 0 in mode 0 (13 bits timer)
* with a hardware gate.
* The 13-bits register consist of all 8 bits of TH0 and the lower 5 bits
* of TL0. The upper 3 bits of TL0 are undeterminate and are ignored.
* FUNCTION_INPUTS: P3.2(INT0)=1 : GATE Input
* FUNCTION_OUTPUTS: void
*/
void main(void)
{
	TMOD &= 0xF0; /* Timer 0 mode 0 with hardware gate */
	TMOD |= 0x08; /* GATE0=1; C/T0#=0; M10=0; M00=0; */
	TH0 = 0x00; /* init values */
	TL0 = 0x00;
	ET0=1; /* enable timer0 interrupt */
	EA=1; /* enable interrupts */
	TR0=1; /* timer0 run */
	while(1); /* endless */
}
/**
* FUNCTION_PURPOSE: timer0 interrupt
* FUNCTION_INPUTS: void
* FUNCTION_OUTPUTS: P1.0 toggle period = 2 * 8192 cycles
*/
void it_timer0( void ) __interrupt( 1 ) /* interrupt address is 0x000b */
{
	TF0 = 0; /* reset interrupt flag (already done by hardware)*/
//	P1_0 = ~P1_0;/* P1.0 toggle when interrupt using ones-complement operator. */ 
	P1_0 ^=1;
}
#include <at89x51.h>

//Global variables and definition
#define PWMPIN P1_0

unsigned char pwm_width;
__bit pwm_flag = 0;

void pwm_setup( void ) 
{
	TMOD = 0;
	pwm_width = 160;
	EA = 1;
	ET0 = 1;
	TR0 = 1;
}

void timer0( void ) __interrupt( 1 ) 
{
	if(!pwm_flag) { //Start of High level
		pwm_flag = 1;   //Set flag
		PWMPIN = 1;     //Set PWM o/p pin
		TH0 = pwm_width;        //Load timer
		TF0 = 0;                //Clear interrupt flag
		return;         //Return
	}
	else {  //Start of Low level
		pwm_flag = 0;   //Clear flag
		PWMPIN = 0;     //Clear PWM o/p pin
		TH0 = 255 - pwm_width;  //Load timer
		TF0 = 0;        //Clear Interrupt flag
		return;         //return
	}
}

void main( void ) 
{
	pwm_setup();
	while( 1 );
}

#include <at89x51.h> /* Include 89C2051 header file */

#define OUTPORT P1  /* Port to be used in the test. */

void main( void )
{
#if 0
	unsigned int i;
	OUTPORT = 0x00;
	for( i = 0; i < 50000; i++ );

	while (1)
	{
		OUTPORT = 0xF0;
		for( i = 0; i < 50000; i++ ) //Delay
		;
		for( i = 0; i < 50000; i++ )
		;
		OUTPORT = 0x0F;
		for( i = 0; i < 50000; i++ )
		;
		for( i = 0; i < 50000; i++ )
		;
		OUTPORT = 0x00;
		for( i = 0; i < 50000; i++ )
		;
		for( i = 0; i < 50000; i++ )
		;
		OUTPORT = 0xFF;
		for( i = 0; i < 50000; i++ )
		;
		for(i=0;i<50000;i++)
		;
		OUTPORT = 0x00;
		for( i = 0; i < 50000; i++ )
		;
		for( i = 0; i < 50000; i++ )
		;
	}
#else
	unsigned char i = 0;
	while ( 1 ) {
#if 1	
		OUTPORT = i++;
		__asm__("nop");
		__asm__("nop");
#else
		P0 = 0x55;
		P1 = 0x55;
		P3 = 0x55;
#endif
	}
#endif
}

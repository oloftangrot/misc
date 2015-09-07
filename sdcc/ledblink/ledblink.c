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
	unsigned char f = 0;
	int i, j;
	while ( 1 ) {
#if 1
		if ( f ) {
			f = 0;
			OUTPORT |= 1 << 2;
		}
		else {
			f = 1;
			OUTPORT &= ~( 1 << 2 );
		}
		for ( j = 0; j < 6000; j++ )		/* This loop should provide a 60 s delay*/
			for ( i = 0; i < 1000; i++ )	/* This is the 0.01 s delay loop	*/
				__asm__("nop");
#else
		P0 = 0x55;
		P1 = 0x55;
		P3 = 0x55;
#endif
	}
#endif
}

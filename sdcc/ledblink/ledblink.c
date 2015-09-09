#include <at89x51.h> /* Include 89C2051 header file */

#define OUTPORT P1  /* Port to be used in the test. */
#define RELAY_PIN (2)

void main( void )
{
	unsigned char minutes;
	int i, j;
	unsigned char f = 0;
#if 0
	OUTPORT |= 1 << RELAY_PIN; /* Activate the relay */
	/*
    Delay start
	*/
	for ( minutes = 0; minutes < 5; minutes++ ) /* This loop shall provide the delay counted in minutes. */
		for ( j = 0; j < 6000; j++ )		/* This loop should provide a 60 s delay*/
			for ( i = 0; i < 1000; i++ )	/* This is the 0.01 s delay loop	*/
				__asm__("nop");
	/*
 		Delay end
  */
	while ( 1 ) {
	  OUTPORT &= ~( 1 << RELAY_PIN ); /* Deactivate the relay */
		__asm__("nop");
	}
#else
	for ( ;; ) {
		if ( f ) {
			f = 0;
			OUTPORT |= 1 << RELAY_PIN; /* Activate the relay */
		}
		else {
			f = 1;
		  OUTPORT &= ~( 1 << RELAY_PIN ); /* Deactivate the relay */
		}
		for ( j = 0; j < 100; j++ )		/* This loop should provide a 1 s delay*/
			for ( i = 0; i < 1000; i++ )	/* This is the 0.01 s delay loop	*/
				__asm__("nop");
	}
#endif
}

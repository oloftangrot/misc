#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "async_port.h"

const char test_str[] = "Putte efter pappa";

int main( int argc, char* argv[] )
{
//	char tbuf[256];
	unsigned char c;
	int i, iSR = 0;

	if(argc < 3) {
		fprintf( stderr, "Usage:  %s [S|R] [devname]\n", argv[0] );
		return 1;
	}
	if(argv[1][0] == 'R' || argv[1][0]=='r') {
		iSR = -1;
	}
	else if(argv[1][0] == 'S' || argv[1][0]=='s') {
		iSR = 1;
	}
	else {
		fprintf( stderr, "Expected first argument to be in [S|s|R|r]\n" );		
		return 1;
	}

	asyncInit( argv[2] );
	if ( 1 == iSR ) { // Send
#if 0
		for ( i = 0; i < strlen( test_str ); i++ ) {
			async_putchar( test_str[i] );
		}

#else
		for ( i = 0; i < 256; i++ ) {
			async_putchar( i );
			sleep(1);
		}
#endif
	}
	if ( -1 == iSR ) { // Receive
		for ( i = 0; i < 1000; i++ ) {
			int res;
			res = async_getchar( &c );
			if ( res <= 0 ) { // Write something on timeouts
//				fprintf ( stderr, "." );
				sleep(1);
			
			}
			else {
				fprintf( stderr, "%d ", c );		
			}
		}
	}
	return 0;
}

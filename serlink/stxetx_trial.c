#include <stdio.h>
#include <string.h>
#include "stxetx.h"
#include "buffer.h"

unsigned char serial_link[256];
int pos= 0;

cBuffer myBuff;

void put_char( unsigned char c )
{
	printf( "%x ", c ); // Make a debug print
	bufferAddToEnd( &myBuff, c );
//	serial_link[pos++]; // Simulate adding the character to the stream.
}

int main( void )
{
	char str[] = "DEADBEEF";
	unsigned char *tmp;
	int i=0, res;

	bufferInit( &myBuff, serial_link, 256 );	
	stxetxInit( put_char );
	stxetxSend( 0, 0, strlen( str ), (unsigned char*) str );

	// Now parse the package byte by until the receive function claims success.
	printf( "\nStarting parsing" );
	res = stxetxProcess( &myBuff );		
#if 0
	do {
		res = stxetxProcess( &myBuff );
		printf( "." );
	} while ( res );
#endif
	printf( "\nFinnished parsing!\n" );

	tmp = stxetxGetRxPacketData();
	for ( i = 0; i < stxetxGetRxPacketDatalength(); i++ ) {
		printf( "%x ", *tmp ); // Show the received data
		tmp++; 
	}
	printf( "\n" );
	return 0;
}


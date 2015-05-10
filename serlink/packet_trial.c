#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "async_port.h"
#include "packet_driver.h"

const char test_str[] = "Putte efter pappa";
const char test2_str[] = "12345678" ;

unsigned char serial_link[256]; // Buffer for simualated data.
int pos= 0;

int put_char( unsigned char c ) // Simulated put_char
{
	printf( "%x ", c ); // Make a debug print
	serial_link[pos++] = c; // Simulate adding the character to the stream.
	return 1;
}

int main( int argc, char* argv[] )
{
	unsigned char c=0;
	int i, iSR = 0;

	if(argc < 3) {
		fprintf( stderr, "Usage:  %s [S|R|T] [devname]\n", argv[0] );
		return 1;
	}
	if(argv[1][0] == 'R' || argv[1][0]=='r') {
		iSR = -1;
	}
	else if(argv[1][0] == 'S' || argv[1][0]=='s') {
		iSR = 1;
	}
	else if(argv[1][0] == 'T' || argv[1][0]=='t') {
		iSR = 0;
	}
	else {
		fprintf( stderr, "Expected first argument to be in [S|s|R|r]\n" );		
		return 1;
	}
	if ( 1 == iSR ) { // Send
		asyncInit( argv[2] );
		packetDriverInit( async_putchar ); // Use asynchronous output.
		sendPacket( test2_str, 8 );	
	}
	if ( -1 == iSR ) { // Receive
		asyncInit( argv[2] );
		for ( i = 0; i < 50; i++ ) {
			int res=0;
			res = async_getchar( &c );
			if ( res <= 0 ) { // Write something on timeouts.
				fprintf ( stderr, "." );
				sleep(1);
			}
			else {
				parseBuffer( &c, 1 );
//				fprintf( stderr, "%d ", c );		
				if ( packetReady() ) {
					fprintf ( stderr, "Packet parsed sucessfully\n" );
				}
			}
		}
	}
	if ( 0 == iSR ) { // Test
		packetDriverInit( put_char ); // Use simulated driver
		sendPacket( test_str, 8 );
		printf( "\n" );
		i = parseBuffer( serial_link, 20 );
		printf( "Remainng data in buffer %d\n", i );
	}
	return 0;
}

// 
// The simple packet driver.
// Packet specification <STX><Data><CRC><ETX>, data length is fixed 8 bytes, CRC is 2 bytes
//
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "packet_driver.h"
#include "serlnk_crc_ccitt.h"

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))
#define PACKET_SIZE 8
#define CHECKSUM_SIZE 2
#define STX_CHAR 2
#define ETX_CHAR 3

const int packetLenght = 8;
enum parseState {
	waitForSTX,
	waitForData,
	waitForChecksum,
	waitForETX,
	packetPass,
	packetFail
};

struct {
	enum parseState pS;
	unsigned char buf[PACKET_SIZE];
	int cnt;
	uint16_t crc;
} packetDriverData;

static int (*dataOut)(unsigned char data); // function pointer to data output routine

void packetDriverInit( int (*dataOut_fn)(unsigned char data) ) 
{
	packetDriverData.cnt = 0;	
	packetDriverData.pS = waitForSTX;
	dataOut = dataOut_fn;
}

#define update_checksum crc_ccitt_update
/*
int update_checksum( unsigned char c )
{
	return 0;
}
*/
int parseBuffer(  unsigned char const * c, int len )
{
	int i;
	do {
		switch ( packetDriverData.pS ) {
			case waitForSTX:
				if ( STX_CHAR == *c++ ) { // Test content and advance pointer
					packetDriverData.pS = waitForData;
					fprintf( stderr, "waitForData " );
					crc_ccitt_init( &packetDriverData.crc ); // Time to initialize check sum.
				}
				len--; // Decrease the remaining packet content
				break;
			case waitForData:
				for ( i = 0; i < MIN(len, ( PACKET_SIZE - packetDriverData.cnt ) ); i++, c++ ) {
					crc_ccitt_update( &packetDriverData.crc, *c );
					packetDriverData.buf[packetDriverData.cnt+i] = *c; // Save data.
					fprintf( stderr, "[%d %x]", i, *c );
					
				}
				packetDriverData.cnt += i; 
				if ( PACKET_SIZE == packetDriverData.cnt ) {
					packetDriverData.pS = waitForChecksum;
					packetDriverData.cnt = 0; // Reuse counter for the checksum
					fprintf( stderr, "waitForChecksum " );
				}
				len -= i;
				break;
			case waitForChecksum:
				for ( i = 0; i < MIN(len, ( CHECKSUM_SIZE - packetDriverData.cnt ) ); i++, c++ ) {
					crc_ccitt_update( &packetDriverData.crc, *c );
					fprintf( stderr, "[%d %x]", i, *c );
				}
				packetDriverData.cnt += i; 
				if ( CHECKSUM_SIZE == packetDriverData.cnt ) {
					packetDriverData.pS = waitForETX;
					fprintf( stderr, "waitForEXT " );
				}
				len -= i;
				break;
			case waitForETX:
				if ( ETX_CHAR == *c++ ) { // Test content and advance pointer
					if ( 0 == packetDriverData.crc ) {
						packetDriverData.pS = packetPass;
						fprintf( stderr, "Pass\n" );
					}
					else {  // Checksum fail.
						packetDriverData.pS = packetFail;
						fprintf( stderr, "Checksum fail %x\n", packetDriverData.crc );
					}
				}
				else {  // Unexpected token.
					packetDriverData.pS = packetFail;
					fprintf( stderr, "Fail\n" );
				}
			case packetPass:
			case packetFail:
				// Coming here means that a complete package has been parsed but there is remaing data in the in buffer.
				goto parser_exit;
				break;
			default:
				fprintf( stderr, "Should not be here!\n" );
				break;
		}
	} while ( ( len > 0 ) );
parser_exit:
	return len; // Returning the remaining data to be parsed if a packet was found in the beginning of the buffer.
}

int packetReady( void )
{
	if ( packetPass == packetDriverData.pS ) return 1;
	else return 0;
}

int sendPacket( char const * buf, int len )
{
	int i;
	uint16_t checkSum;

	crc_ccitt_init( &checkSum ); // Res initialize check sum first!!, dont forget....
	(void) dataOut( STX_CHAR );
	for ( i = 0; i < MIN( len, PACKET_SIZE ); i++ ) {
		(void) dataOut( buf[i] );
		crc_ccitt_update( &checkSum, buf[i] );
	}
	if ( i < PACKET_SIZE ) { // Handle case when buffer data was too short.
		for ( ; i < PACKET_SIZE; i++ ) {
			(void) dataOut( 0 ); // Pad with zeros
			crc_ccitt_update( &checkSum, 0 );
		}
	}
	(void) dataOut ( (unsigned char) ( checkSum >> 8 ) );
	(void) dataOut ( (unsigned char) ( checkSum & 0xff ) );
	(void) dataOut ( ETX_CHAR );
	return 1;
}

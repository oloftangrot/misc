#include "serlnk_crc_ccitt.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>

char str[] = "Putte efter pappa" ;

char vec1[] = { '1', '2','3','4','5','6','7','8','9',0};
char vec2[] = { 0x12, 0x35, 0x56, 0x70, 0 }; // 0x12345670 = 0xB1E4

//char vec2[] = { 0x70, 0x56, 0x35, 0x12, 0 }; // 0x12345670 = 0xB1E4

//0x5A261977 = 0x1AAD


int main ( void )
{
	int i;
	uint16_t crc;


	crc_ccitt_init( &crc );
	for ( i = 0; i < strlen( vec1 ); i++ ) {
		crc_ccitt_update( &crc, vec1[i] );
		printf( "%d", i );
	}
	printf ( "Checksum Vector 1 (TI): %x expected 0x29b1\n", crc );
	crc_ccitt_update( &crc, 0x29 );
	crc_ccitt_update( &crc, 0xb1 );
	printf ( "Checksum run 2: %x expected 0\n", crc );
	
	crc_ccitt_init( &crc );
	for ( i = 0; i < strlen( vec2 ); i++ ) {
		crc_ccitt_update( &crc, vec2[i] );
		printf( "%d", i );
	}
	printf ( "Checksum Vector 2 (TI): %x expected 0xb1e4\n", crc );


	return 0;
}

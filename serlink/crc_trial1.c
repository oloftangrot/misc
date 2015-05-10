#include "crc_ccitt.h"

#include <stdio.h>
#include <string.h>

char str[] = "Putte efter pappa" ;

char vec1[] = { '1', '2','3','4','5','6','7','8','9',0};
char vec2[] = { 0x12, 0x35, 0x56, 0x70, 0 }; // 0x12345670 = 0xB1E4

//char vec2[] = { 0x70, 0x56, 0x35, 0x12, 0 }; // 0x12345670 = 0xB1E4

//0x5A261977 = 0x1AAD

static uint16_t crc; 

void
crc_ccitt_init2(void)
{
	crc = 0xffff;
}

void
crc_ccitt_update2(unsigned char x)
{
	uint16_t crc_new = (unsigned char)(crc >> 8) | (crc << 8);
	crc_new ^= x;
	crc_new ^= (unsigned char)(crc_new & 0xff) >> 4;
	crc_new ^= crc_new << 12;
	crc_new ^= (crc_new & 0xff) << 5;
	crc = crc_new;
}

uint16_t
crc_ccitt_crc2(void)
{
	return crc;
}

int main ( void )
{
	uint16_t crc_avr = 0xffff; // Initaliser
	int i;

	for ( i = 0; i < strlen( str ); i++ ) {
		crc_avr = crc_ccitt_update( crc_avr, str[i] );
	}
	printf ( "Checksum: %x\n", crc_avr );

	crc_avr = crc_ccitt_update( crc_avr, crc_avr & 0xff );
	crc_avr = crc_ccitt_update( crc_avr, crc_avr >> 8 );

	printf ( "Checksum: %x\n", crc_avr );


	crc_avr = 0xffff;
	for ( i = 0; i < strlen( vec1 ); i++ ) {
		crc_avr = crc_ccitt_update( crc_avr, vec1[i] );
		printf( "%d", i );
	}
	printf ( "Checksum Vector 1 (AVR): %x expected 0x29b1\n", crc_avr );;

	crc_ccitt_init2();
	for ( i = 0; i < strlen( vec1 ); i++ ) {
		crc_ccitt_update2( vec1[i] );
		printf( "%d", i );
	}
	printf ( "Checksum Vector 1 (TI): %x expected 0x29b1\n", crc_ccitt_crc2() );
	crc_ccitt_update2( 0x29 );
	crc_ccitt_update2( 0xb1 );
	printf ( "Checksum run 2: %x expected 0\n", crc_ccitt_crc2() );
	
	crc_ccitt_init2();
	for ( i = 0; i < strlen( vec2 ); i++ ) {
		crc_ccitt_update2( vec2[i] );
		printf( "%d", i );
	}
	printf ( "Checksum Vector 2 (TI): %x expected 0xb1e4\n", crc_ccitt_crc2() );

	crc_avr = 0xffff;
	crc_avr = crc_ccitt_update( crc_avr, 0 );
	printf ( "Checksum 0 (AVR): %x \n", crc_avr );;

	crc_ccitt_init2();
	crc_ccitt_update2( 0 );
	printf ( "Checksum 0 (TI): %x \n", crc_ccitt_crc2() );

	return 0;
}

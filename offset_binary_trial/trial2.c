#include <stdio.h>
#include <stdint.h>

int16_t foo ( uint16_t arg )
{
	return ( arg ^ 0x200 ) - 0x200;
}

int16_t bar ( uint16_t arg )
{
	arg ^= 0x200;
	arg += ( 0x0 ^ 0x8000 ) - ( 0x0^ 0x200 );
	arg ^= 0x8000;
	return arg;
}

int main( void )
{
	int16_t m;
	int n = 0x3fe; // -1 in 10 bit
	n = (n ^ 0x200) - 0x200;

	printf ( "Result %d \n", n );

	n = 0x3ff; // -1 in 10 bit
	m = ( (n ^ 0x200) + (0x0 ^ 0x8000) -( 0x0^ 0x200) ) ^ 0x8000 ;

	printf ( "Result %x \n", m );
	printf ( "Size of short %ld \n", sizeof( uint16_t ) );

	n = 0x3fe; // -1 in 10 bit
	m = (n ^ 0x200) - 0x200;

	printf ( "Result %d \n", m );

}

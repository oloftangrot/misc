#include <stdio.h>

int main( void )
{
	signed char sc;
	signed short b;
	
	signed short ss;
	unsigned short tmp, tmp2, offset;
	
	sc = -1;
	ss = -1;
	
	tmp = (unsigned char) sc;
	printf( "First look at the value a 8-bit negative number expressed as an 8-bit offset binary.\n" );
	printf( "Unsigned char value %x\n", tmp );
	tmp = 0x80 ^ tmp ;
	printf( "Offset binary value %x\n", tmp );
	
	tmp2 = ss;
	printf( "Second look what the same negtive number expressed as an 16 bit offset binary.\n" );
	printf( "Unsigned shor value %x\n", tmp2 );
	tmp2 = 0x8000 ^ tmp2 ;
	printf( "Offset binary value %x\n", tmp2 );
	offset = tmp2 - tmp;
	printf( "The offset difference was %x\n", offset );

	printf( "Now lets use xor add xor to make the conversion of -1.\n" );
	sc = -128;
	tmp = (unsigned char) sc;
	tmp = 0x80 ^ tmp;
	tmp = offset + tmp;
	tmp2 = 0x8000 ^ tmp ;
	printf ( "If conversion worked the result should be -128 -> %x\n", tmp2 );
	printf ( "Hard to tell when shown as hex, try a signed decimal -> %d\n", (signed short) tmp2 );

	printf ( "Lets try the positive max (127) too...\n" );
	sc = 127;
	tmp = (unsigned char) sc;
	tmp = 0x80 ^ tmp;
	tmp = offset + tmp;
	tmp2 = 0x8000 ^ tmp ;
	printf ( "If conversion worked the result should be 127 -> %x\n", tmp2 );
	printf ( "Hard to tell when shown as hex, try a signed decimal -> %d\n", (signed short) tmp2 );

	printf ( "Lets try -50 too...\n" );

	sc = -50;
	tmp = (unsigned char) sc;
	tmp = 0x80 ^ tmp;
	tmp = offset + tmp;
	tmp2 = 0x8000 ^ tmp ;
	printf ( "If conversion worked the result should be -50 -> %x\n", tmp2 );
	printf ( "Hard to tell when shown as hex, try a signed decimal -> %d\n", (signed short) tmp2 );

	return 0;
}

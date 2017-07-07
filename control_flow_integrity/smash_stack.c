#include <string.h>
#include <stdio.h>

void smashme( char* blah ) {
	char smash[16];
	printf( "Before %p\n", __builtin_return_address( 0 ) );
	strcpy( smash, blah );
	printf( "After %p\n", __builtin_return_address( 0 ) );

}

int main( int argc, char** argv ) {
	if ( argc > 1 ) {
		smashme( argv[ 1 ] );
	}
	return 0;
}


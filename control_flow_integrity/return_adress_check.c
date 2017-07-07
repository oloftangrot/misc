#include <stdio.h>

void foo( void ) {
	printf( "Stack frame return address %p\n", __builtin_return_address(0) );
}

int main( int argc, char * argv[] ) {
	int a;
	printf( "Hello world!\n" );
//	goto end;
	printf( "Hello again!\n" );
	foo();
end:
	printf( "Hello end! %p\n", &&end );
	return 0;
}

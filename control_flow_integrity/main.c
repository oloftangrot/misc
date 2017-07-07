#include <stdio.h>
int main ( void );
void b( void *caller );

//void a( (*caller)( void ) );

#if 1
void a( void *caller ) {
   void *me = ( void*) &a;

	 b( me );	
}
#endif 
void b( void *caller ) {
  void *me = ( void* ) &b;
	void * ret_address = __builtin_return_address( 0 );
	void * frame_pointer = __builtin_frame_address( 0 );
	void * addr;
	unsigned long l;
	printf( "Caller %p\n", caller );
	 
	if ( caller != (void (*)( void )) &main )
		printf( "Wrong caller\n" );
	else	
		printf( "Correct caller\n");

	printf( "Return address %p\n", ret_address );
	l = *( unsigned long *) (frame_pointer);
	printf( "Stack content %lx\n", l );
}


int main ( void ) {
   void *me = (void*) &main;

	b ( me );
	a ( me );

	return 0;
}

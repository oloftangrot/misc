#include "cpptk.h"
#include <iostream>
#include "RTSP.h"
#include "Parser.h"
#include <RTSP_Internals.h>

using namespace Tk;
using namespace std;

int buttonGroup = 2;
char const * const g2[] = {
	".f.a2",
	".f.b2",
  ".f.c2"
};

struct _a {
	char const * const path;
	char const * const text;
} group2[3] = { 
	{ ".fx.a2", "Unselected" },
	{ ".fx.b2", "Not Enabled" }, 
	{ ".fx.c2", "By Default" } 
};

void group3_callback( void ) ; // Forward declaration of call back for group 3

struct _fbg {
	size_t siz;
	int	value;
	int last_value;
	void (*cb)( void ); // callback function...
	struct _aa {
		char const * const path;
		char const * const text;
	} s[5];
} group3 = {
	3,
	0,
	0,
	group3_callback,
	{
		{ ".fx.a2", "Unselected 3" },
		{ ".fx.b2", "Not Enabled 3" }, 
		{ ".fx.c2", "By Default 3" }, 
		{ NULL, NULL }
	}
};

void group3_callback( void ) {
	group3.s[ group3.last_value ].path << ( configure() -state( "normal" ) ) ; // GUI fix in Ubuntu only??
	group3.last_value = group3.value ;
	cout << "Hello World! from " << group3.s[group3.value].text << endl;
}

void fbg_create( struct _fbg & fbg ) { // Create button group
	for( size_t i = 0; i < fbg.siz; i++ ) {
		if ( NULL != fbg.s[i].path ) {
			radiobutton( fbg.s[i].path ) 
				-text( fbg.s[i].text )
				-variable( fbg.value )
				-value( i ) 
				-command( fbg.cb );
		}
	}
	for( size_t i = 0; i < fbg.siz; i++ ) { // Add to the layout manager
		if ( NULL != fbg.s[i].path ) {
			pack( fbg.s[i].path ) -padx(20) -pady(6) -anchor( "w" );
		}
	}
	fbg.last_value = fbg.value; // Ensure faulty initial data does not cause problems later
}

void cmd( void )
{
	cout << "Hello World! from " << buttonGroup << group2[buttonGroup - 1].text << endl;
	".f.a" << ( configure() -state( "normal" ) ); // Adding these configure calls fixed faults
	".f.b" << ( configure() -state( "normal" ) ) ; // where the previous selected button 
	".f.c" << ( configure() -state( "normal" ) ) ; // remained visually selected together with the new one.
  update( );
}

int main( int argc, char *argv[] )
{
	try
	{
		init(argv[0]);
		frame( ".f" );
		pack( ".f" ) -Tk::fill( both ) -expand( 1 );

		radiobutton( ".f.a" ) -text( "Emergency Mode" ) -variable ( buttonGroup ) -value( 1 ) -command( cmd );
		radiobutton( ".f.b" ) -text( "Normal Mode" ) -variable ( buttonGroup ) -value( 2 ) -command( cmd ) ;
		radiobutton( ".f.c" ) -text( "Backup Mode" ) -variable ( buttonGroup ) -value( 3 ) -command( cmd ) ;

		pack( ".f.a" ) -padx(20) -pady(6) -anchor( "w" );
		pack( ".f.b" ) -padx(20) -pady(6) -anchor( "w" );
		pack( ".f.c" ) -padx(20) -pady(6) -anchor( "w" );

		frame( ".fx" ) -relief(raised) -borderwidth( 1 ) ;
		pack( ".fx" ) -Tk::fill( both ) -expand( 1 );
		fbg_create( group3 );

		runEventLoop();
	}
	catch (exception const &e)
	{
		cerr << "Error: " << e.what() << '\n';
	}
}



#include "cpptk.h"
#include <iostream>
#include <cmath>
#include <string.h>

using namespace Tk;
using namespace std;


string afterCommand;
const int delay   = 25;
const int h = 240;
const int _width = 400; 
const int h1 = h/2; // # canvas mid-height
const int h2 = h1 + 1;
const int h3 = ( h * 4 ) / 10  ; //# graph mid-height
const double accuracy  = 1.e-2;

int t = 0;

void nextStep ( void );

int main( int argc, char* argv[] )
{
	init(argv[0]);
	pack (canvas( ".c" ) - width( _width ) -height( h ) -xscrollincrement( 1 ) -bg ( "maroon" ) );
  afterCommand = callback(nextStep); // register the callback function
  after(delay, afterCommand);   // start animation
  runEventLoop();

	return 0;
}

void nextStep ( void ) {
	char buf[32];
	sprintf( buf, "%d", t );
	double x = t/50.;
	double vv = sin( x );
	int v = (int) ( ( vv * h3 ) + h1 );
	if ( abs ( vv ) < accuracy ) {
    ".c" <<  create( text, t, 0) -anchor( n ) -text( buf ) -Tk::fill( "gray" ) ;
		".c" <<  create( line, t, 0, t, h ) -Tk::fill( "gray" ) ;
	}
  ".c" << create( line, t, h1, t, h2 ) -Tk::fill( "gray" ) ;
  ".c" << create( rectangle, t, v, t, v ) ;
  t++;
  if ( t > _width ) { 
		".c" << xview( scroll, 1, units ) ; 
	}
  after( delay,  afterCommand ) ;

}


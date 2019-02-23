#include "solve.h"

double func( double v, double a, double t ) {
	double tmp = v*t+t*t*a/2;
	return tmp;
}

int main ( void )
{
	double x=0, v=0;
	solve( func, x, v, 10., 0.1 );

	return 0;
}

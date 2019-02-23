/* Second/Third order integrator for real-time problems
*
* This is the "NASA" integrator, specifically for systems
* with second-order dynamics.
*
*/
#include <iostream>

using namespace std;

void solve( double (*f)(double, double, double), double &x, double &v, double tf, double h )
{
	double t = 0;
	static double f1 = f(t, x, v );
	static double v1;
	double temp;
	while ( t < tf ) {
		cout << "t = " << t << "; x = " << x << "; v = " << v << endl;
		temp = f( t, x, v );
		v1 = v;
		v += h * ( 3 * temp - f1 ) / 2;
		f1 = temp;
		x += h * ( v + v1 ) / 2;
		t += h;
	}
}

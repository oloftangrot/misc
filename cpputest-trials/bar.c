#include "systime.h"

int bar(void) {
	unsigned int t = sysTime();
	if ( t > 65535 ) 
		return 0;
	else
		return (int) t;
}

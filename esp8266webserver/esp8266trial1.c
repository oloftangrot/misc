#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "../serlink/async_port.h"

int main ( int argc, char * argv[] )
{
	if(argc < 2) {
		fprintf( stderr, "Usage:  %s [devname]\n", argv[0] );
		return 1;
	}

	asyncInit( argv[1] );

	return 0;
}

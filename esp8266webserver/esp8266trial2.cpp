/*  Copyright (C) 2015  Olof Tångrot

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "../serlink/async_port.h"

char const * const esp8266atCmd[5] = {
  "AT\r\n",          /* Test AT modem precense */
  "AT+CWMODE=3\r\n",  /* Working mode: AP+STA */
	"AT+CIPMUX=1\r\n",	 /* Turn on multiple connection */
	"AT+CIPSERVER=1,9999\r\n", /* Start the server listening on socket 9999 */
	"+IPD," /* Some one connected on the socket and sent data. */
};

void write_buf( char const * buf, size_t len )
{
	for ( size_t i = 0; i < len; i++ ) {
		async_putchar( *buf++ );
	}
}

enum atParseState {
	waitForOk,
  waitSocketData,
  waitLength,
  waitData,
  waitForTail,
};

int main ( int argc, char * argv[] )
{
	enum atParseState atPS = waitForOk;
	unsigned char chan, c;	
	int res;

	if(argc < 2) {
		fprintf( stderr, "Usage:  %s [devname]\n", argv[0] );
		return 1;
	}

	asyncInit( argv[1] );

	// Initialize the modem, in each step wait for Ok.

	write_buf( esp8266atCmd[0], strlen( esp8266atCmd[0] ) );
	
	c = 0;
	do {
		res = async_getchar( &c );
		if ( res > 0 )
			putc( c, stdout );
		else 
			c = 0;
	} while ( c != 'K' );

	// Now wait for incomming data, store the incoming channel

	// Now read the incoming data and send it to the http_parser
	return 0;
}

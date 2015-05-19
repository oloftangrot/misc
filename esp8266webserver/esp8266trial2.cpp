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

#ifndef __cplusplus
typedef unsigned char boolean;
enum {
  false = 0,
	true = 1
};
#else
typedef bool boolean; // Use the c++ type
#endif


char const * const esp8266atCmd[5] = {
  "AT\r\n",          /* Test AT modem precense */
  "AT+CWMODE=3\r\n",  /* Working mode: AP+STA */
	"AT+CIPMUX=1\r\n",	 /* Turn on multiple connection */
	"AT+CIPSERVER=1,9999\r\n", /* Start the server listening on socket 9999 */
	"+IPD," /* Some one connected on the socket and sent data. */
};

#define NUM_AT_REPLY (2)
char const * const atReply[NUM_AT_REPLY] = {
	"OK\r\n",		/* Response when the command passed */
	"ERROR\r\n" /* Response when the command failed */
};

void write_buf( char const * buf, size_t len )
{
	for ( size_t i = 0; i < len; i++ ) {
		async_putchar( *buf++ );
	}
}

enum atParseState {
  waitForCmdEcho,
  waitForCmdReply,
  waitForSocketData,
  waitForLength,
  waitForData,
  waitForTail,
	resultParseFailure,
	resultOk,
	resultError
};

enum atParseState atPS;

enum atParseState replyRules[NUM_AT_REPLY] = {
	resultOk,
	resultError
};

static size_t cnt;
static unsigned char currentCmd;
static unsigned char atReplyId;
static unsigned char replyCnt;
static boolean flags[NUM_AT_REPLY];

void initAtParser( void )
{
	cnt = 0;
  for ( int i = 0; i < NUM_AT_REPLY; i++ ) {
		flags[i] = false;
	}
	atPS = waitForCmdEcho;
	replyCnt = NUM_AT_REPLY;
}

enum atParseState atParser( unsigned char in )
{
  switch ( atPS ) {
    case waitForCmdEcho:
      if ( in == esp8266atCmd[currentCmd][cnt] ) {
        cnt++;
        if ( cnt == strlen( esp8266atCmd[currentCmd] ) ) {
          printf ( "Found command %s!\n", esp8266atCmd[currentCmd] ); 
          cnt = 0; 
          atPS = waitForCmdReply;
        }
      }
      else
				atPS = resultParseFailure;
      break;
    case waitForCmdReply:
      for ( int i = 0; i < NUM_AT_REPLY; i++ ) {
        if ( flags[i] ) {
          if ( in == atReply[ i ][ cnt ] ) {
            if ( ( cnt + 1 ) == strlen( atReply[ i ] ) ) {
              printf( "Found reply " ); 
              printf( "%s\n", atReply[ i ] ); 
              cnt = -1; // Let the out of loop cnt++ increment to 0.
              atReplyId = i; // Save the found command
              atPS = replyRules[i];
              break; // Break out from the for loop
            }
          }
          else {
            flags[i] = false;
            replyCnt--;
            printf( "Reply ruled out %d\n", i );
          }
        }
      }
      cnt++;
      if ( 0 == replyCnt ) { 
        atPS = resultParseFailure; 
        cnt = 0;
        printf( "Reset reply search\n" ); // Some parse flag should be set.
      }
      break;
		case waitForData:
		case waitForSocketData:
		case waitForLength:
		case waitForTail:
		case resultParseFailure:
		case resultOk:
		case resultError:

			break;
  } /* Switch */
	return atPS;
}

int main ( int argc, char * argv[] )
{
	unsigned char chan, c;	
	int res;

	if(argc < 2) {
		fprintf( stderr, "Usage:  %s [devname]\n", argv[0] );
		return 1;
	}

	asyncInit( argv[1] );

	// Initialize the modem, in each step wait for Ok.

	atPS = waitForCmdEcho;
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

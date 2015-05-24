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
#include "wap.h"
#undef PARSE_DEBUG_CMD
#undef PARSE_DEBUG_REP1
#define PARSE_DEBUG_REP2

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


char const * const esp8266atCmd[6] = {
	"AT\r\n",          /* Test AT modem precense */
	"AT+CWMODE=3\r\n",  /* Working mode: AP+STA */
	"AT+CWJAP="ssid","passwd"\r\n",
	"AT+CIPMUX=1\r\n",	 /* Turn on multiple connection */
	"AT+CIPSERVER=1,9999\r\n", /* Start the server listening on socket 9999 */
	"+IPD," /* Some one connected on the socket and sent data. */
};

#define NUM_AT_REPLY (4)
char const * const atReply[NUM_AT_REPLY] = {
	"\n\r\nOK\r\n",			/* Response when the command passed */
	"\nno change\r\n",	/* Typical response to AT+CWMODE=3 */
	"\nError\r\n",			/* Response when the command failed */
	"\n\r\nFAIL\r\n"		/* Response when the AT+CWJAP failed */
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
	resultOk,
	resultError,
	resultError,
};

static size_t cnt;
static unsigned char currentCmd;
static unsigned char atReplyId;
static unsigned char replyCnt;
static boolean flags[NUM_AT_REPLY];

void initAtParser( unsigned char current )
{
	cnt = 0;
	for ( int i = 0; i < NUM_AT_REPLY; i++ ) {
		flags[i] = true;
	}
	currentCmd = current;
	atPS = waitForCmdEcho;
	replyCnt = NUM_AT_REPLY;
}

enum atParseState atParse( unsigned char in )
{
	boolean oneMatch;
	switch ( atPS ) {
		case waitForCmdEcho:
#ifdef PARSE_DEBUG_CMD
			printf( "waitForCmdEcho %ld %d %d\n", cnt, in, esp8266atCmd[currentCmd][cnt] );
#endif
			if ( ( in == esp8266atCmd[currentCmd][cnt] ) || 
			     ( ( in == 13 ) && ( esp8266atCmd[currentCmd][cnt] == 10 ) ) )
			{
				cnt++;
				if ( cnt == strlen( esp8266atCmd[currentCmd] ) ) {
#ifdef PARSE_DEBUG_CMD
					printf ( "Found command %s\n", esp8266atCmd[currentCmd] ); 
#endif
					cnt = 0; 
					atPS = waitForCmdReply;
				}
			}
			else {
				atPS = resultParseFailure;
			}
			break;
		case waitForCmdReply:
#ifdef PARSE_DEBUG_REP1
			printf( "waitForCmdReply %ld %d\n", cnt, in );
#endif
			oneMatch = false;
			for ( int i = 0; i < NUM_AT_REPLY; i++ ) {
				if ( flags[i] ) {
#ifdef PARSE_DEBUG_REP1
					printf( "   %d %d\n", i, atReply[ i ][ cnt ] );
#endif
					if ( in == atReply[ i ][ cnt ] ) {
						oneMatch = true;
						if ( ( cnt + 1 ) == strlen( atReply[ i ] ) ) {
#ifdef PARSE_DEBUG_REP1
							printf( "Found reply " ); 
#endif
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
#ifdef PARSE_DEBUG_REP2
						printf( "Reply ruled out %d\n", i );
#endif
					}
				}
      			}
			if ( oneMatch ) cnt++; // At least one matching reply string was found
			if ( 0 == replyCnt ) { 
				atPS = resultParseFailure; 
				cnt = 0;
#ifdef PARSE_DEBUG_REP2
				printf( "Reset reply search\n" ); // Some parse flag should be set.
#endif
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

boolean sendATcommand( int cmd ) {
	unsigned char c;	
	enum atParseState parseResult;
	int res;

	initAtParser( cmd );
	write_buf( esp8266atCmd[cmd], strlen( esp8266atCmd[cmd] ) );
	
	c = 0;
	do {
		res = async_getchar( &c );
		if ( res > 0 ) {
//			putc( c, stdout );
			parseResult = atParse( c );
		}
		else {
			c = 0;
		}
	} while ( ( parseResult != resultOk ) && 
		  			( parseResult != resultError ) && 
		  			( parseResult != resultParseFailure ) );
	return ( resultOk == parseResult );
}

int main ( int argc, char * argv[] )
{
//	unsigned char chan;	

	if(argc < 2) {
		fprintf( stderr, "Usage:  %s [devname]\n", argv[0] );
		return 1;
	}
	printf ( "%s \n", esp8266atCmd[2] );
	asyncInit( argv[1] );

	// Initialize the modem, in each step wait for Ok.
	printf( "Testing command 0...\n" );
	sendATcommand( 0 ); // First check if there is an AT modem connected.
	printf( "Testing command 1...\n" );
	sendATcommand( 1 ); // Set up access mode.
	printf( "Testing command 2...\n" );
	sendATcommand( 2 ); // Connect to as wireless access point.
	printf( "Testing command 3...\n" );
	sendATcommand( 3 ); // Allow multiple connections.
	printf( "Testing command 4...\n" );
	sendATcommand( 4 ); // Start a server.

	// Now wait for incomming data, store the incoming channel

	// Now read the incoming data and send it to the http_parser
	return 0;
}

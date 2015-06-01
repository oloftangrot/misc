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
#include "http_parser.h"

#undef PARSE_DEBUG_waitForCmdEcho
#undef PARSE_DEBUG_waitForCmdReply1
#undef PARSE_DEBUG_waitForCmdReply2
#define PARSE_DEBUG_waitForData
#define PARSE_DEBUG_waitForLength
#define PARSE_DEBUG_waitForConnectionId
#define PARSE_DEBUG_waitForSocketData
#define PARSE_DEBUG_waitForStringEcho
#undef PARSE_DEBUG_waitForCrCr

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

enum commandPosition { // Positions in the command string array.
	attest_cmd = 0,
	mode_cmd   = 1,
	join_cmd   = 2,
	ipmux_cmd  = 3,
	server_cmd = 4,
	data_cmd   = 5,
	send_cmd   = 6,
	close_cmd  = 7,
	crcr_end	 = 8,
};

char const *err_msg = "HTTP/1.0 404 Not Found\r\n\r\n";  /* Error response */
char const *reply_msg = "HTTP/1.1 200 OK\r\nServer: Apache/1.3.3.7 (Unix) (Red-Hat/Linux)\r\nContent-Length: 5\r\nConnection: close\r\nContent-Type: text/plain; charset=UTF-8\r\n\r\nTEST2";

char const * const esp8266atCmd[9] = {
	"AT\r\n",          /* Test AT modem precense */
	"AT+CWMODE=3\r\n",  /* Working mode: AP+STA */
	"AT+CWJAP="ssid","passwd"\r\n",
	"AT+CIPMUX=1\r\n",	 /* Turn on multiple connection */
	"AT+CIPSERVER=1,9999\r\n", /* Start the server listening on socket 9999 */
	"+IPD,", /* Some one connected on the socket and sent data. */
	"AT+CIPSEND=",
	"AT+CIPCLOSE=",
	"\r\r"
};

#define NUM_AT_REPLY (6)
char const * const atReply[NUM_AT_REPLY] = {
	"\n\r\nOK\r\n",			/* Response when the command passed. */
	"\nno change\r\n",	/* Typical response to AT+CWMODE=3. */
	"\nError\r\n",			/* Response when the command failed. */
	"\n\r\nFAIL\r\n",		/* Response when the AT+CWJAP failed. */
	"\n> ",							/* Response when AT+CIPSEND waits for socket data. */
	"\r\nOK\r\n"				/* Tailing response after +IPD data. */
};

void write_buf( char const * buf, size_t len )
{
	for ( size_t i = 0; i < len; i++ ) {
		async_putchar( *buf++ );
	}
}

enum atParseState {
	waitForStringEcho,
	waitForCrCr,
  waitForCmdEcho,
  waitForCmdReply,
  waitForSocketData,
  waitForLength,
	waitForConnectionId,
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
	resultOk,
	resultOk
};

static size_t cnt;
static unsigned char currentCmd;
static unsigned char atReplyId;
static unsigned char socketPort;
static unsigned char replyCnt;
static boolean flags[NUM_AT_REPLY];
static boolean url_f;
static char * currentString;

void initAtParser( unsigned char current )
{
	cnt = 0;
	for ( int i = 0; i < NUM_AT_REPLY; i++ ) {
		flags[i] = true;
	}
	if ( data_cmd == current ) {
		atPS = waitForData;
		// TODO reset any other counters....?
	}
	else if ( send_cmd == current ) {
	}
	else {
		currentCmd = current;
		atPS = waitForCmdEcho;
		replyCnt = NUM_AT_REPLY;
	}
}


enum atParseState atParse( unsigned char in )
{
	boolean oneMatch;
	switch ( atPS ) {
		case waitForStringEcho:
#ifdef PARSE_DEBUG_waitForStringEcho
			printf( "waitForStringEcho %ld %d %d\n", cnt, in, currentString[cnt] );
#endif
			if ( in == currentString[cnt] ) {
				cnt++;
				if ( cnt == strlen( currentString ) ) {
#ifdef PARSE_DEBUG_waitForStringEcho
					printf ( "String found %s\n", currentString ); 
#endif
					cnt = 0; 
					atPS = resultOk;
				}
			}
			else {
				atPS = resultError;
			}
			break;
		case waitForCrCr:
#ifdef PARSE_DEBUG_waitForCrCr
			printf( "waitForCrCr %ld %d %d\n", cnt, in, currentString[cnt] );
#endif
			if ( in == currentString[cnt] ) 
			{
				cnt++;
				if ( cnt == strlen( currentString ) ) {
#ifdef PARSE_DEBUG_waitForCrCr
					printf ( "CRCR line end found %s\n", currentString ); 
#endif
					cnt = 0; 
					atPS = waitForCmdReply;
				}
			}
			else {
				atPS = resultParseFailure;
			}
			break;
		case waitForCmdEcho:
#ifdef PARSE_DEBUG_waitForCmdEcho
			printf( "waitForCmdEcho %ld %d %d\n", cnt, in, currentString[cnt] );
#endif
			if ( ( in == currentString[cnt] ) || 
			     ( ( in == 13 ) && ( currentString[cnt] == 10 ) ) )
			{
				cnt++;
				if ( cnt == strlen( currentString ) ) {
#ifdef PARSE_DEBUG_waitForCmdEcho
					printf ( "Found command %s\n", currentString ); 
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
#ifdef PARSE_DEBUG_waitForCmdReply1
			printf( "waitForCmdReply %ld %d\n", cnt, in );
#endif
			oneMatch = false;
			for ( int i = 0; i < NUM_AT_REPLY; i++ ) {
				if ( flags[i] ) {
#ifdef PARSE_DEBUG_waitForCmdReply1
					printf( "   %d %d\n", i, atReply[ i ][ cnt ] );
#endif
					if ( in == atReply[ i ][ cnt ] ) {
						oneMatch = true;
						if ( ( cnt + 1 ) == strlen( atReply[ i ] ) ) {
#ifdef PARSE_DEBUG_waitForCmdReply1
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
#ifdef PARSE_DEBUG_waitForCmdReply2
						printf( "Reply ruled out %d\n", i );
#endif
					}
				}
			}
			if ( oneMatch ) cnt++; // At least one matching reply string was found
			if ( 0 == replyCnt ) { 
				atPS = resultParseFailure; 
				cnt = 0;
#ifdef PARSE_DEBUG_waitForCmdReply2
				printf( "Reset reply search\n" ); // Some parse flag should be set.
#endif
			}
      break;
		case waitForData:
#ifdef PARSE_DEBUG_waitForData
			printf( "waitForData %ld %d %d\n", cnt, in, esp8266atCmd[data_cmd][cnt] );
#endif
			if ( ( in == esp8266atCmd[data_cmd][cnt] ) || 
			     ( ( in == 13 ) && ( esp8266atCmd[data_cmd][cnt] == 10 ) ) )
			{
				cnt++;
				if ( cnt == strlen( esp8266atCmd[data_cmd] ) ) {
#ifdef PARSE_DEBUG_waitForData
					printf ( "Found command %s\n", esp8266atCmd[data_cmd] ); 
#endif
					cnt = 0; 
					atPS = waitForConnectionId;
				}
			}
			break;
		case waitForConnectionId:
#ifdef PARSE_DEBUG_waitForConnectionId
			printf( "waitForConnectionId %ld %d\n", cnt, in );
#endif
			if ( ( in >= '0' ) && ( in <= '9' ) ) { /* Handle id digits */
				cnt = cnt * 10 + ( in - '0' ); // Use the char counter as data length storage.
			}
			else if ( ',' == in ) { /* ',' is the separator between the id digits and length digits */
				socketPort = cnt;  /* Reuse the variable and save the reply id for later usage */
#ifdef PARSE_DEBUG_waitForConnectionId
				printf ( "Reply to channel Id %ld\n", cnt );
#endif
				atPS = waitForLength;
				cnt = 0; /* Clear the variable for new usage */
			}
			else {
				atPS = resultParseFailure;
			}
			break;
		case waitForLength:
#ifdef PARSE_DEBUG_waitForLength
			printf( "waitForLenght %ld %d %d\n", cnt, in, esp8266atCmd[data_cmd][cnt] );
#endif
			if ( ( in >= '0' ) && ( in <= '9' ) ) { /* Handle length digits */
				cnt = cnt * 10 + ( in - '0' ); // Use the char counter as data length storage.
			}
			else if ( ':' == in ) { /* ':' is the separator between the length and frame data */
#ifdef PARSE_DEBUG_waitForLength
				printf ( "Data to be received %ld\n", cnt );
#endif
				atPS = waitForSocketData;
				url_f = false;
			}
			else {
				atPS = resultParseFailure;
			}
			break;
		case waitForSocketData:
			cnt--;
			if ( false == url_f ) { /* Only try to parse the incoming data for a valid url as long as no match has been made. */
				if ( url_ok ==	httpParser( in ) ) {  /* The url has been accepted */
					url_f = true; /* Save the result before reading out the rest of the data */
				}
			}
//			putc( in, stdout );
			if ( 0 == cnt ) {
				atPS = waitForCmdReply;  
				// ...and when all data has been received the responce should be sent back!
				if ( url_ok ) {
				//	atPS = resultOk;
				}
				else {
				//	atPS = resultError;
				}
#ifdef PARSE_DEBUG_waitForSocketData
				printf( "All data has been recived!\n" );
#endif
			}
			break;	
		case waitForTail:
		case resultParseFailure:
		case resultOk:
		case resultError:
			break;
	} /* Switch */
	return atPS;
}

enum atParseState getAndParse( void )
{
	int res;
	unsigned char c;	
	enum atParseState parseResult;
	do {
		res = async_getchar( &c );
		if ( res > 0 ) {
//			putc( c, stdout );
//			printf( "+" );
			parseResult = atParse( c );
		}
		else {
			c = 0;
		}
	} while ( ( parseResult != resultOk ) && 
		  			( parseResult != resultError ) && 
		  			( parseResult != resultParseFailure ) );
	return parseResult;
}
boolean ipSend( void ) {
//	enum atParseState parseResult;
	char buf[16];
	cnt = 0;
	for ( int i = 0; i < NUM_AT_REPLY; i++ ) {
		flags[i] = true;
	}
	currentString = (char*) esp8266atCmd[send_cmd];
	atPS = waitForStringEcho;
	write_buf( currentString, strlen( currentString ) );
			printf( "x" );

	if ( resultOk != getAndParse() )
		return false;

	sprintf( buf, "%d,", socketPort ); // Send port
	currentString = buf;
	atPS = waitForStringEcho;
	write_buf( currentString, strlen( currentString ) );
	if ( resultOk != getAndParse() )
		return false;

	sprintf( buf, "%ld", strlen( (char *) reply_msg ) ); // Send data length
	atPS = waitForStringEcho;
	write_buf( currentString, strlen( currentString ) );
	if ( resultOk != getAndParse() )
		return false;

	currentString = (char*) esp8266atCmd[crcr_end];
	atPS = waitForCrCr;
	write_buf( "\r\n", 2 );
	if ( resultOk != getAndParse() )
		return false;

	printf ( "Sending back data to socket\n" );
	write_buf( reply_msg, strlen( (char *) reply_msg ) );

	write_buf( esp8266atCmd[close_cmd], strlen( (char *) esp8266atCmd[close_cmd] ) );
	sprintf( buf, "%d,", socketPort ); // Send port
	write_buf( buf, strlen( buf ) );
	write_buf( "\r\n", 2 );

	return true;
}

boolean sendATcommand( int cmd ) {
	unsigned char c;	
	enum atParseState parseResult;
	int res;

	currentString = (char *) esp8266atCmd[cmd];
	initAtParser( cmd );
	if ( data_cmd != cmd ) { // Don't write out the connect string
		write_buf( currentString, strlen( currentString ) );
	}
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

	if ( argc < 2 ) {
		fprintf( stderr, "Usage:  %s [devname]\n", argv[0] );
		return 1;
	}
	printf ( "%s \n", esp8266atCmd[2] );
	asyncInit( argv[1] );

	// Initialize the modem, in each step wait for Ok.
	printf( "Testing AT response...\n" );
	sendATcommand( 0 ); // First check if there is an AT modem connected.
	printf( "Set up access mode...\n" );
	sendATcommand( 1 ); // Set up access mode.
	printf( "Connect to wireless access point...\n" );
	sendATcommand( 2 ); // Connect to as wireless access point.
	printf( "Enable multiple connections...\n" );
	sendATcommand( 3 ); // Allow multiple connections.
	printf( "Start IP-server ...\n" );
	sendATcommand( 4 ); // Start a server.

	httpParserInit();

	printf( "Listen for link data ...\n" );
	sendATcommand( data_cmd ); // Listen for data.
#if 0
	for ( ;; ) {
		int res;
		unsigned char c;
		res = async_getchar( &c );
		if ( res > 0 ) {
			putc( c, stdout );
			res = c;
			printf ( " %d,", res );
		}
	}
#endif
	printf( "Send back data on the connecting socket...\n" );
	ipSend();
	// Now read the incoming data and send it to the http_parser
	return 0;
}

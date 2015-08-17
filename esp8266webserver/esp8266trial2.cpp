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
#define PARSE_DEBUG_waitForCmdReply1
#define PARSE_DEBUG_waitForCmdReply2
#undef PARSE_DEBUG_waitForData
#define PARSE_DEBUG_waitForLength
#define PARSE_DEBUG_waitForConnectionId
#undef PARSE_DEBUG_waitForSocketData
#define PARSE_DEBUG_waitForStringEcho
#define PARSE_DEBUG_waitForCrCr

#include "async_port.h"

#ifndef __cplusplus
typedef unsigned char boolean;
enum {
	false = 0,
	true = 1
};
#else
typedef bool boolean; // Use the c++ type
#endif

void readOutInBuffer( void );
boolean waitForReply( void );

boolean parse_debug = false;

enum commandPosition { // Positions in the command string array.
	noecho_cmd = 0,
	attest_cmd = 1,
	mode_cmd   = 2,
	join_cmd   = 3,
	ipmux_cmd  = 4,
	server_cmd = 5,
	data_cmd   = 6,
	send_cmd   = 7,
	close_cmd  = 8
};

//char const *err_msg = "HTTP/1.0 404 Not Found\r\n\r\n";  /* Error response */
char const *error_msg = "HTTP/1.0 404 Not Found\r\nServer: Apache/1.3.3.7 (Unix) (Red-Hat/Linux)\r\nContent-Length: 15\r\nConnection: close\r\nContent-Type: text/plain; charset=UTF-8\r\n\r\n[\"ERROR\",\"404\"]";  /* Error response */
char const *reply_msg = "HTTP/1.1 200 OK\r\nServer: Apache/1.3.3.7 (Unix) (Red-Hat/Linux)\r\nContent-Length: 5\r\nConnection: close\r\nContent-Type: text/plain; charset=UTF-8\r\n\r\nTEST2";

char const * const esp8266atCmd[9] = {
	"ATE0",				 /* Disable character echo */	
	"AT",          /* Test AT modem precense */
	"AT+CWMODE=3",  /* Working mode: AP+STA */
	"AT+CWJAP="ssid","passwd,
	"AT+CIPMUX=1",	 /* Turn on multiple connection */
	"AT+CIPSERVER=1,9999", /* Start the server listening on socket 9999 */
	"+IPD,", /* Some one connected on the socket and sent data. */
	"AT+CIPSEND=",
	"AT+CIPCLOSE=",
};

#define NUM_AT_REPLY (9)
char const * const atReply[NUM_AT_REPLY] = {
	"\n\r\nOK\r\n",			/* Response when the command passed. */
	"\nno change\r\n",	/* Typical response to AT+CWMODE=3. */
	"\nError\r\n",			/* Response when the command failed. */
	"\n\r\nERROR\r\n",	/* Response when the command failed. */
	"\n\r\nFAIL\r\n",		/* Response when the AT+CWJAP failed. */
	"\n> ",							/* Response when AT+CIPSEND waits for socket data. */
	"\r\nOK\r\n",				/* Tailing response after +IPD data. */
	"\r\r\nSEND OK\r\n",				/* Response when AT+CIPSEND has received all socket data. */
  "\r\r\nbusy s...\r\n\r\nSEND OK\r\nUnlink\r\n" /* Alternate response when AT+CIPSEND has received all socket data. */
//"\r\r\nbusy s...\r\n\r\nSEND OK\r\nLink\r\n" /* Alternate response when AT+CIPSEND has received all socket data, but favicon is to be fetched.... */
};

boolean writeCommandLineEnd( void );

void write_buf( char const * buf, size_t len )
{
	for ( size_t i = 0; i < len; i++ ) {
		async_putchar( *buf++ );
	}
}

enum atParseState {
	waitForStringEcho,
	waitForCrCr,
//  waitForCmdEcho,
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
	resultError,
	resultOk,
	resultOk,
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
//		atPS = waitForCmdEcho;
		replyCnt = NUM_AT_REPLY;
	}
}

enum atParseState atParse( unsigned char in )
{
	boolean oneMatch;
	switch ( atPS ) {
#if 1
		case waitForStringEcho:
#ifdef PARSE_DEBUG_waitForStringEcho
			if ( parse_debug ) printf( "waitForStringEcho %ld %d %d\n", cnt, in, currentString[cnt] );
#endif
			if ( in == currentString[cnt] ) {
				cnt++;
				if ( cnt == strlen( currentString ) ) {
#ifdef PARSE_DEBUG_waitForStringEcho
					if ( parse_debug ) printf ( "String found %s\n", currentString ); 
#endif
					cnt = 0; 
					atPS = resultOk;
				}
			}
			else {
				atPS = resultError;
			}
			break;
#endif
		case waitForCrCr:
#ifdef PARSE_DEBUG_waitForCrCr
			if ( parse_debug ) printf( "waitForCrCr %ld %d\n", cnt, in );
#endif
			if ( ( in == '\r' ) ) 
			{
				cnt++;
				if ( 2 == cnt ) {
#ifdef PARSE_DEBUG_waitForCrCr
					if ( parse_debug ) printf ( "CRCR line end found.\n" ); 
#endif
					cnt = 0; 
					atPS = waitForCmdReply;
				}
			}
			else {
				atPS = resultParseFailure;
			}
			break;
#if 0
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
#endif
		case waitForCmdReply:
#ifdef PARSE_DEBUG_waitForCmdReply1
			if ( parse_debug ) {
				printf( "waitForCmdReply %ld %d ", cnt, in );
				if ( in > 32 ) 
					putc( in, stdout );
				else {
					putc( '^', stdout );
					putc( 64 + in, stdout );
				}
				printf( "\n" );
			}
#endif
			oneMatch = false;
			for ( int i = 0; i < NUM_AT_REPLY; i++ ) {
				if ( flags[i] ) {
#ifdef PARSE_DEBUG_waitForCmdReply1
					if ( parse_debug ) printf( "   %d %d\n", i, atReply[ i ][ cnt ] );
#endif
					if ( in == atReply[ i ][ cnt ] ) {
						oneMatch = true;
						if ( ( cnt + 1 ) == strlen( atReply[ i ] ) ) {
#ifdef PARSE_DEBUG_waitForCmdReply1
							if ( parse_debug ) printf( "Found reply " ); 
							printf( "%s\n", atReply[ i ] ); 
#endif
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
						if ( parse_debug ) printf( "Ruled out reply: %d\n", i );
#endif
					}
				}
			}
			if ( oneMatch ) cnt++; // At least one matching reply string was found
			if ( 0 == replyCnt ) { 
				atPS = resultParseFailure; 
				cnt = 0;
#ifdef PARSE_DEBUG_waitForCmdReply2
				if ( parse_debug ) printf( "Reset reply search\n" ); // Some parse flag should be set.
#endif
			}
      break;
		case waitForData:
#ifdef PARSE_DEBUG_waitForData
			if ( parse_debug ) printf( "waitForData %ld %d %d\n", cnt, in, esp8266atCmd[data_cmd][cnt] );
#endif
			if ( ( in == esp8266atCmd[data_cmd][cnt] ) || 
			     ( ( in == 13 ) && ( esp8266atCmd[data_cmd][cnt] == 10 ) ) )
			{
				cnt++;
				if ( cnt == strlen( esp8266atCmd[data_cmd] ) ) {
#ifdef PARSE_DEBUG_waitForData
					if ( parse_debug ) printf ( "Found command %s\n", esp8266atCmd[data_cmd] ); 
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
	enum atParseState parseResult = waitForStringEcho; // Assign something that won't terminate the loop if the first read fails.
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
/*
* Send a string character by character without checking any echo.
*/
boolean stringSend( char * str, boolean check_LF_echo = true ) {
	boolean res = true;
	unsigned int len = strlen( str );
	printf ( "Length %d ", len );
	for ( unsigned int i = 0; i < len; i++ ) {
		async_putchar( *str );
		str++;
	}
	return res;
}
/*
* Send a stirng character by chararter and check the echo after each one.
*/
boolean stringSend_check_echo( char * str, boolean check_LF_echo = true ) {
	boolean res = true;
	unsigned char c;
	unsigned int len = strlen( str );
	printf ( "Length %d ", len );
	for ( unsigned int i = 0; i < len; i++ ) {
		async_putchar( *str );
		c = 0;
		if ( async_getchar( &c ) <= 0 ) {
			res = false;
			printf( "No echo w:%d @%d !\n", *str, i );
			if ( ( *str != 10 ) && check_LF_echo ) break; // // esp8266 provides no responce on the LF character.
		}
		else if ( c != *str ) { 
			printf( "Echo failed w: %d r:%d @%d !\n", *str, c, i );
			res = false;
			readOutInBuffer();
			break;
		}
		str++;
	}
	return res;
}

boolean ipSend( void ) {
//	enum atParseState parseResult;
	char buf[16];
	cnt = 0;
	for ( int i = 0; i < NUM_AT_REPLY; i++ ) {
		flags[i] = true;
	}
	printf( "Send command: " ) ;
	if ( stringSend( (char*) esp8266atCmd[send_cmd] ) ) printf( "Ok\n" );
	else {
		printf ( "Error!\n" );
		return false;
	}
	sprintf( buf, "%d,", socketPort ); // Send port
	printf( "Socket port: " ) ;
	if ( stringSend( (char*) buf ) ) printf( "Ok\n" ) ;
	else {
		printf ( "Error!\n" );
		return false;
	}
	sprintf( buf, "%ld", strlen( (char *) error_msg ) ); // Send data length
	printf( "Data length: " ) ;
	if ( stringSend( (char*) buf ) ) printf( "Ok\n" ) ;
	else {
		printf ( "Error!\n" );
		return false;
	}
  if ( writeCommandLineEnd() ) printf ( " Command responce Ok\n" ) ;

	printf ( "Sending back data to socket\n" );
	if ( stringSend( (char*) error_msg ), false ) printf( "Ok\n" ) ;

  parse_debug = true;
	if ( waitForReply() ) printf ( "Data sent Ok\n" );
	else printf ( "Data send error!\n" );
	// Here there should be some code that reads out the 'SEND OK' response from the modem before closing the socket...
  parse_debug = false;
#if 0
	printf( "Close command: " ) ; 
	if ( stringSend( (char*) esp8266atCmd[ close_cmd ] ) ) printf( "Ok\n" ) ;
	else {
		printf ( "Error!\n" );
//		return false; // Temporary fix...
	}
  if ( writeCommandLineEnd() ) printf ( " Command responce Ok\n" );
#endif
//	readOutInBuffer();
	return true;
}
/*
* Set up the stream parser to wait for the string received when a client connects 
* and sends data.
*/
boolean waitForServerConnection( void ) {
	unsigned char c;	
	enum atParseState parseResult;
	int res;

	cnt = 0;
	for ( int i = 0; i < NUM_AT_REPLY; i++ ) {
		flags[i] = true;
	}
	atPS = waitForData;
	currentString = (char *) esp8266atCmd[ data_cmd ];
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

void readOutInBuffer( void )
{
	unsigned char c;
	int res;
	int emptyReads = 10;

	sleep( 1 );
	do {
		res = async_getchar( &c );
		if ( res > 0 ) {
			if ( c > 32 ) 
				putc( c, stdout );
			else {
				putc( '^', stdout );
				putc( 64 + c, stdout );
			}
		}
		else {
			emptyReads--;
		}
	} while ( emptyReads );
}
/*
* Write the a command line end the esp8266 modem expects, then wait for any of the
* expected responses.
*/
boolean writeCommandLineEnd( void )
{
	cnt = 0;
	for ( int i = 0; i < NUM_AT_REPLY; i++ ) {
		flags[i] = true;
	}
	atPS = waitForCrCr;
	write_buf( "\r\n", 2 );
	if ( resultOk != getAndParse() ){
		printf ( "Error CrCr!\n" );
		return false;
	}
	return true;
}

/*
* Write the a command line end the esp8266 modem expects, then wait for any of the
* expected responses.
*/
boolean writeCommandLineEndNoEcho( void )
{
	cnt = 0;
	for ( int i = 0; i < NUM_AT_REPLY; i++ ) {
		flags[i] = true;
	}
	atPS = waitForCmdReply;
	write_buf( "\r\n", 2 );
	if ( resultOk != getAndParse() ){
		printf ( "Error writeCommandLineEndNoEcho!\n" );
		return false;
	}
	return true;
}

boolean waitForReply( void )
{
	cnt = 0;
	for ( int i = 0; i < NUM_AT_REPLY; i++ ) {
		flags[i] = true;
	}
	atPS = waitForCmdReply;
	write_buf( "\r\n", 2 );
	if ( resultOk != getAndParse() ){
		printf ( "Error waitForReply!\n" );
		return false;
	}
	return true;
}

#if 0
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
#endif

int main ( int argc, char * argv[] )
{
	unsigned char c;	

	if ( argc < 2 ) {
		fprintf( stderr, "Usage:  %s [devname]\n", argv[0] );
		return 1;
	}
//	printf ( "%s \n", esp8266atCmd[2] );
	asyncInit( argv[1] );
//	printf ( "Testing loop back of the 'error message' reply.	\n" );
//  stringSend( (char *) error_msg ); 
	// Initialize the modem, in each step wait for Ok.
	parse_debug = true;
	printf( "Deactivating echo..." );
	if ( stringSend( (char *) esp8266atCmd[ noecho_cmd ] ) ) printf ( " Command OK\n" );
	for ( unsigned int i = 0; i < strlen( esp8266atCmd[ noecho_cmd ] ); i++ ) async_getchar( &c );
 	if ( writeCommandLineEnd() ) printf ( " Command responce Ok\n" );

	printf( "Testing AT response..." );
	if ( stringSend( (char *) esp8266atCmd[ attest_cmd ] ) ) printf ( " Command OK\n" );
  if ( writeCommandLineEndNoEcho() ) printf ( " Command responce Ok\n" );

	printf( "Set up access mode... " );
	if ( stringSend( (char *) esp8266atCmd[ mode_cmd] ) ) printf ( " Command OK\n" );
  if ( writeCommandLineEndNoEcho() ) printf ( " Command responce Ok\n" );

	printf( "Connect to wireless access point..." );
	if ( stringSend( (char *) esp8266atCmd[ join_cmd ] ) ) printf ( " Command OK\n" );
  if ( writeCommandLineEndNoEcho() ) printf ( " Command responce Ok\n" );

	printf( "Enable multiple connections..." );
	if ( stringSend( (char *) esp8266atCmd[ ipmux_cmd ] ) ) printf ( " Command OK\n" );
  if ( writeCommandLineEnd() ) printf ( " Command responce Ok\n" );
	printf( "Start IP-server ..." );
//	sendATcommand( 4 ); // Start a server.
	if ( stringSend( (char *) esp8266atCmd[ server_cmd ] ) ) printf ( " Command OK\n" );
  if ( writeCommandLineEnd() ) printf ( " Command responce Ok\n" );

	httpParserInit();

	for ( ;; ) {
		printf( "Listen for link data ...\n" );
//		sendATcommand( data_cmd ); // Listen for data.
		waitForServerConnection();
//		printf( "Read out in buffer...\n" );
//		readOutInBuffer();
		printf( "Send back data on the connecting socket...\n" );
		ipSend();
	}
	return 0;
}

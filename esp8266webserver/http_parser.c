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
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "http_parser.h"

#define DEBUG_waitForGet
#define DEBUG_waitForCommand
#define DEBUG_waitForArg

enum parseState {
  waitForGet,
  waitForCommand,
  waitForArg,
  waitForTail,
	waitForArgumentSeparator
};

#ifndef __cplusplus
typedef unsigned char boolean;
enum {
  false = 0,
	true = 1
};
#else
typedef bool boolean; // Use the c++ type
#endif


const char * www[2] = {
  "GET /",                 /* HTTP request */
  "HTTP/1.0 404 \r\n\r\n"  /* Error response */
};

#define numCommands 5

char const * const commands[ numCommands ] = {
  "favicon.ico",
  "rb/",
  "wb/",
	"sbi/",
	"cbi/"
};

boolean flags[ numCommands ];
 
struct rules {
  int numArgs;
  enum parseState nextS;
} parseRules[ numCommands ] = {
  { 0, waitForTail },
  { 1, waitForArg },
  { 2, waitForArg },
  { 1, waitForArg },
  { 1, waitForArg }
};

static enum parseState pS;
static size_t cnt;
static int numArgs;
static int currentArg;
static int parsedArgument[2];

void httpParserInit( void )
{
//  Serial.begin(9600); // set up Serial library at 9600 baud
//  Serial.println("Hello from Arduino WEB-server"); 
//  Serial.println( strlen( www[0] ) );
	pS = waitForGet;
	cnt = 0;
}

static int commandCnt;
static int cmd;
/*
** Return 0-15 if input is a base 16 encoded ASCII char or -1 otherwise.
*/
char cxtoi( const char in )
{
	if ( 0 != isxdigit( in ) ) {
		if ( 0 != isdigit( in ) ) {
			return in - '0';
		}
		else if ( 0 != isupper( in ) ) {
			return  10 + in - 'A';
		}
		else {
			return  10 + in - 'a';
		}
	}
	return -1;
}

enum urlResult httpParser( unsigned char in )
{
	enum urlResult res = url_pending;
	char tmp;
  switch ( pS ) {
    case waitForGet:
      if ( in == www[0][cnt] ) {
        cnt++;
        if ( cnt == strlen( www[0] ) ) {
#ifdef DEBUG_waitForGet
          printf ("Found GET command!");
#endif
          cnt = 0; 
          pS = waitForCommand;
          for ( int i = 0; i < numCommands; i++ ) flags[i] = true;
          commandCnt = numCommands;
        }
      }
      else
        cnt = 0;
      break;
    case waitForCommand:
      for ( int i = 0; i < numCommands; i++ ) {
        if ( flags[i] ) {
          if ( in == commands[ i ][ cnt ] ) {
            if ( ( cnt + 1 ) == strlen( commands[ i ] ) ) {
#ifdef DEBUG_waitForCommand
              printf( "Found command " );
              printf( "%s\n", commands[ i ] ); 
#endif
              cnt = -1; /* Let the out of loop cnt++ increment to 0. */
              cmd = i; /* Save the found command */
              if ( 0 == parseRules[i].numArgs ) {
                pS = waitForGet;
								res = url_ok; /* Signal that parsing URL has finnished with a successful command */
              }
              else {
                pS = waitForArg ;
								numArgs = parseRules[i].numArgs;
								currentArg = 0;
								parsedArgument[0] = 0;
								parsedArgument[1] = 0;
              }
              break; /* Break out from the for loop */
            }
          }
          else {
            flags[i] = false;
            commandCnt--;
#ifdef DEBUG_waitForCommand
            printf( "Command ruled out " );
            printf( "%d\n", i );
#endif
          }
        }
      }
      cnt++;
      if ( 0 == commandCnt ) { 
        pS = waitForGet; 
        cnt = 0;
				res = url_error; /* Signal thar parsing the URL should result with an error message */
#ifdef DEBUG_waitForCommand
        printf( "Reset command search\n" );
#endif
      }
      break;
    case waitForTail:
      break;      
		case waitForArg:
			tmp = cxtoi( in );
			if ( tmp >= 0 ) {
				parsedArgument[ currentArg ] = parsedArgument[ currentArg ] * 16 + tmp;
				cnt++;
				if ( cnt > 1 ) {
#ifdef DEBUG_waitForArg
					printf ( "Current argument %d read %d.\n",currentArg, parsedArgument[ currentArg ] );
#endif
					if ( currentArg < numArgs ) {
						pS = waitForArgumentSeparator;
						currentArg++;
					}
					else {
		        pS = waitForGet;
						res = url_ok; /* Signal that parsing URL has finnished with a successful command */
						cnt = 0;
					}
				}
			}
			else {
				printf ( "Error, not a base-16 digit!\n" );
        pS = waitForGet; 
				cnt = 0;
				res = url_error; /* Signal thar parsing the URL should result with an error message */
			}
			break;
		case waitForArgumentSeparator:
			if ( '/' == in ) {
				pS = waitForArg;
				cnt = 0;
			}
			else {
				pS = waitForGet;
				cnt = 0;
				res = url_error;
			}
			break;
  } /* Switch */
	return res;
}

int getParsedCommand( void )
{
	return cmd;
}

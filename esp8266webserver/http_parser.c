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
#include "http_parser.h"

enum parseState {
  waitForGet,
  waitForCommand,
  waitForArg,
  waitForTail,
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

#define numCommands 4

char const * const commands[ numCommands ] = {
  "favicon.ico",
  "test1/",
  "test12/",
  "sirpa/"
};

boolean flags[ numCommands ];
 
struct rules {
  int numArgs;
  enum parseState nextS;
} parseRules[ numCommands ] = {
  { 0, waitForTail },
  { 1, waitForArg },
  { 2, waitForArg },
  { 0, waitForTail }
};

static enum parseState pS;
static size_t cnt;

void httpParserInit( void )
{
//  Serial.begin(9600); // set up Serial library at 9600 baud
//  Serial.println("Hello from Arduino WEB-server"); 
//  Serial.println( strlen( www[0] ) );
	pS = waitForGet;
	cnt = 0;
}

int in;
int commandCnt;
int cmd;

void httpParser( unsigned char in )
{
  switch ( pS ) {
    case waitForGet:
      if ( in == www[0][cnt] ) {
        cnt++;
        if ( cnt == strlen( www[0] ) ) {
          printf ("Found GET command!"); 
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
              printf( "Found command " ); 
              printf( "%s\n", commands[ i ] ); 
              cnt = -1; // Let the out of loop cnt++ increment to 0.
              cmd = i; // Save the found command
              if ( 0 == parseRules[i].numArgs ) {
                pS = waitForGet;
              }
              else {
                pS = waitForArg ;
              }
              break; // Break out from the for loop
            }
          }
          else {
            flags[i] = false;
            commandCnt--;
            printf( "Command ruled out " );
            printf( "%d\n", i );
          }
        }
      }
      cnt++;
      if ( 0 == commandCnt ) { 
        pS = waitForGet; 
        cnt = 0;
        printf( "Reset command search\n" );
      }
      break;
    case waitForTail:
      break;      
		case waitForArg:
			break;
  } /* Switch */
}

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#ifndef __USE_MISC
#define __USE_MISC
#endif
#ifndef __USE_BSD
#define __USE_BSD
#endif
#include <termios.h>
#include <fcntl.h>
//#include <sys/time.h>
#include <sys/ioctl.h> // for IOCTL definitions
#include <unistd.h>
#include <errno.h>

#include "async_port.h"

/* Loacal prototypes */
static void ttyconfig(int iFile, int iBaud, int iParity, int iBits, int iStop);

/* Private variables */
static int hSer = -1 ; // Invalid handle??

void asyncInit( char const * const devStr )
{
//  hSer = open( devStr, (O_RDWR | O_NONBLOCK), 0 );
//  hSer = open( devStr, (O_RDWR ), 0 );
  hSer = open( devStr, (O_RDWR | O_NOCTTY | O_NDELAY ) );
  if( hSer == -1 )
  {
    fprintf(stderr, "Unable to open %s \n", devStr );
    exit( 0 );
  }
  ttyconfig( hSer, 9600, 0, 8, 1 );
}

int async_getchar( unsigned char * const c )
{
	char buf[2]; // use size of '2' to avoid warnings about array size of '1'
	int res;
#if 0
	if(fcntl( hSer, F_SETFL, 0) == -1) { // set blocking mode
		static int iFailFlag = 0;

		if(!iFailFlag) {
			fprintf(stderr, "Warning:  'fcntl(O_NONBLOCK)' failed, errno = %d\n", errno);
			iFailFlag = 1;
		}
	}
#endif

	res = read(hSer, buf, 1);
	*c = buf[0];
//	fprintf(stderr, "Res %d \n", res );

	return res;
}

int async_putchar(unsigned char bVal)
{
	int iRval;
	char buf[2]; // use size of '2' to avoid warnings about array size of '1'
#if 0
	if(fcntl( hSer, F_SETFL, 0) == -1) { // set blocking mode
		static int iFailFlag = 0;

		if(!iFailFlag) {
			fprintf(stderr, "Warning:  'fcntl(O_NONBLOCK)' failed, errno = %d\n", errno);
			iFailFlag = 1;
		}
	}
#endif
	buf[0] = bVal; // in case args are passed by register
	iRval = write(hSer, buf, 1);
	return iRval;
}

/**
  * This is a sample tty config function to CORRECTLY set up a serial connection
  * to allow XMODEM transfer.  The important details here are the use of the
  * 'termios' structure and utility functions to DISABLE all of the things that
  * would otherwise cause trouble, like CRLF translation, CTRL+C handling, etc.
**/
static void ttyconfig(int iFile, int iBaud, int iParity, int iBits, int iStop)
{
	int i1;
	struct termios sIOS;

 	i1 = fcntl( iFile, F_GETFL );

 	i1 |= O_NONBLOCK; // i1 &= ~O_NONBLOCK); // turn OFF non-blocking?
// 	i1 &= ~O_NONBLOCK; // turn OFF non-blocking?

	fcntl(iFile, F_SETFL, 0);

 	if( !tcgetattr( iFile, &sIOS) ) {
#if 0
		cfsetspeed( &sIOS, iBaud );
		sIOS.c_cflag &= ~( CSIZE | PARENB | CS5 | CS6 | CS7 | CS8 );
		sIOS.c_cflag |= iBits == 5 ? CS5 : iBits == 6 ? CS6 : iBits == 7 ? CS7 : CS8; // 8 is default
		if( iStop == 2 ) {
			sIOS.c_cflag |= CSTOPB;
		}
		else {
	    sIOS.c_cflag &= ~CSTOPB;
		}

		sIOS.c_cflag &= ~CRTSCTS; // hardware flow control _DISABLED_ (so I can do the reset)

		if(!iParity) {
			sIOS.c_cflag &= ~(PARENB | PARODD);
		}
		else if(iParity > 0) { // odd 
			sIOS.c_cflag |= (PARENB | PARODD);
		}
		else { // even (negative)
			sIOS.c_cflag &= PARODD;
			sIOS.c_cflag |= PARENB;
		}
#endif
		//    sIOS.c_iflag |= IGNCR; // ignore CR
 		sIOS.c_cflag |= ( CLOCAL | CREAD ); // ignore any modem status lines

		// do not translate characters or xon/xoff and ignore break
//		sIOS.c_iflag &= ~(IGNBRK | INLCR | IGNCR | ICRNL | IXON | IXOFF | IXANY | IMAXBEL | ISTRIP); // turn these off

#if defined(__FreeBSD__)
//		sIOS.c_oflag &= ~(OPOST | ONLCR | OCRNL | TABDLY | ONOEOT | ONOCR | ONLRET); // FreeBSD version
    sIOS.c_oflag &= ~OPOST;
#else // Linux? YMMV
//		sIOS.c_oflag &= ~(OPOST | ONLCR | OCRNL | TABDLY | ONOCR | ONLRET); // turn these off too (see man termios)
    sIOS.c_oflag &= ~OPOST;

#endif // FBSD vs Linux

		// make sure echoing is disabled and control chars aren't translated or omitted
#if defined(__FreeBSD__)
//		sIOS.c_lflag &= ~(ECHO | ECHOKE | ECHOE | ECHONL | ECHOPRT | ECHOCTL | ICANON | IEXTEN | ISIG | ALTWERASE);
    sIOS.c_lflag     &= ~(ICANON | ECHO | ECHOE | ISIG);
#else // Linux? YMMV
//		sIOS.c_lflag &= ~(ECHO | ECHOKE | ECHOE | ECHONL | ECHOPRT | ECHOCTL | ICANON | IEXTEN | ISIG);
    sIOS.c_lflag     &= ~(ICANON | ECHO | ECHOE | ISIG);
#endif // FBSD vs Linux
		sIOS.c_cc[VMIN] = 0;  // Receive at least 1 character.
		sIOS.c_cc[VTIME] = 10; // 0.5 s timeout

		if(tcsetattr(iFile, TCSANOW, &sIOS)) {
			fprintf(stderr, "error %d setting attributes\n", errno);
		}
	}
	else {
		fprintf(stderr, "error %d getting attributes\n", errno);
	}
}


#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>

//char msg[] = "OLOF HEJ";
//char msg[] = "H  H  H  H  ";
char msg[] = " SA2KAA SA2KAA SA2KAA SA2KAA";
const int wpm = 12;

/* baudrate settings are defined in <asm/termbits.h>, which is
included by <termios.h> */
#define BAUDRATE B115200  

/* change this definition for the correct port */
#define MODEMDEVICE "/dev/ttyUSB0"

const static int wordSpace_ = 7;
const static int charSpace_ = 3;
const static int markSpace_ = 1;
const static int di_ = 1;
const static int da_ = 3;

char * digits[10] = {
 "-----", /* 0 */
 ".----", /* 1 */
 "..---", /* 2 */
 "...--", /* 3 */
 "....-", /* 4 */
 ".....", /* 5 */
 "-....", /* 6 */
 "--...", /* 7 */
 "---..", /* 8 */
 "----." /* 9 */
};

 char * alphas[35] = {
 ".-",    /* a */
 "-...",  /* b */
 "-.-.",  /* c */
 "-..",   /* d */
 "." ,    /* e */
 "..-.",  /* f */
 "--.",   /* g */
 "....",  /* h */
 "..",    /* i */
 ".---",  /* j */
 "-.-",   /* k */
 ".-..",  /* l */
 "--",    /* m */
 "-.",    /* n */
 "---",   /* o */
 ".--.",  /* p */
 "--.-",  /* q */
 ".-.",   /* r */
 "...",   /* s */
 "-",			/* t */
 "..-",    /* u */
 "...-",   /* v */
 "-..-",   /* x */
 "-.--",   /* y */
 "--..",   /* z */
 ".--.-",  /* å */
 ".-.-",   /* ä */
 "---.",   /* ö */
 ".-.-.-", /* . */
 "..--..", /* , */
 "-..-.",  /* / */
 "-...-",  /* = 'åtskillnadstecken' */
 "-....-", /* - 'bindestreck' */
 "-.---",  /* vänta */
};

const struct { int l; char * str;} wpmNorms[3] = {
	{50, "PARIS"},
  {54, "KANON" },
	{60, "CODEX" }
};

float getBaud(int wpm, int norm) {
	return wpm * wpmNorms[norm].l/60.;
}

float getDotTimeIn_ms(int wpm, int norm ) {
	return 60000./(wpm * wpmNorms[norm].l);
}

static int __attribute__((unused)) sprintfSeqNo_( char * buf, unsigned short c ) ;
static int __attribute__((unused)) sprintfTime_( char * buf, unsigned int c );
void asyncInit( int fd );

#define BUFSIZE 10000

char buf[BUFSIZE];

int main ( void )
{
	int fd; 
	int totalTime_ms = 0;
  int bytes, status;
	struct termios oldtio;
	unsigned short seqNo = 0;
  unsigned short diTime = getDotTimeIn_ms(wpm, 0);
	printf( "Baud %d %f\n", wpm, getBaud(wpm, 0) );
	printf( "Dot time %d %f\n", wpm, (float) diTime);
  printf( "Char rate %d\n", wpm * 5 );

	/* 
		Open modem device for reading and writing and not as controlling tty
		because we don't want to get killed if linenoise sends CTRL-C.
	*/
	fd = open(MODEMDEVICE, O_RDWR | O_NOCTTY );

	if ( fd < 0 ) {
		perror(MODEMDEVICE); exit( -1 ); 
	}

	tcgetattr( fd, &oldtio ); /* save current serial port settings */
	asyncInit( fd );

  ioctl(fd, TIOCMGET, &status);
	status |= TIOCM_DTR;
	ioctl(fd, TIOCMSET, status); // Ensure the Arduino is not held in reset

	printf("Init done\n");

	int n = 0;
	for ( int i = 0; i < strlen(msg); i++ ) {
		int j;
		if ( ' '  != msg[i] ) {
			char * p;
			if( isdigit( msg[i] ) ) p = digits[msg[i] - '0'];
		  else p = alphas[msg[i] - 'A'];	
	    for ( j = 0; j < strlen(p); j++ ) {
			  if ('.' == p[j] ) {
					printf ( "/%d", di_ );
					n += sprintf( buf+n, "/%d*", di_ * diTime );
					totalTime_ms += di_ * diTime;
				}
	      else if ( '-' == p[j] ) {
					printf ( "/%d", da_ );
					n += sprintf( buf+n, "/%d*", da_ * diTime );
					totalTime_ms += da_ * diTime;
				}
			  else printf ("? " );
			  if ( j < (strlen(p) - 1)) { 
					printf("\\%d", markSpace_ );
					n += sprintf( buf+n, "\\%d*", markSpace_ * diTime );
					totalTime_ms += markSpace_ * diTime;
				}
		  }
	  
	  	if ( msg[i+1] != ' ' ) {
				printf ("\\%d", charSpace_ ); 	
					n += sprintf( buf+n, "\\%d*", charSpace_ * diTime );
					totalTime_ms += charSpace_ * diTime;
			}
		}
		else {
			printf ("\\%d", wordSpace_ );
					n += sprintf( buf+n, "\\%d*", wordSpace_ * diTime );
					totalTime_ms += wordSpace_ * diTime;
		}
	}
#if 0
	char buf[256];
  sprintfSeqNo_(buf, 0xa5a5 );
	printf("Res: %s\n", buf );
  sprintfTime_(buf, 0x66a5a5 );
#endif
	printf("\nSequence count %d:\n%s\n", seqNo, buf );
	printf("Total time %f s\n", totalTime_ms/1000. );

	read( fd, buf, BUFSIZE );
	if ( '>' != buf[0] ) {
		printf( "Wrong start promt!\n" );
		exit( 0 );
  }
	sleep(1);
#if 0
  ioctl(fd, FIONREAD, &bytes);
	if (2 != bytes) {
		printf( "No start!\n");
		exit(0);
	}
#endif
//	for (int k= 0; k<100; k++)
		write( fd, buf, strlen(buf) );

	tcdrain( fd );
	sleep((totalTime_ms+500)/1000);
	

  ioctl(fd, FIONREAD, &bytes);
  printf( "Bytes in buffer %d\n", bytes );
	/* restore the old port settings */
	memset(buf, 0, BUFSIZE );
	read( fd, buf, BUFSIZE );
  printf ("Read from tty:\n");
  printf ("%s\n", buf);
	tcsetattr(fd, TCSANOW, &oldtio);

	return 0;
}

static int sprintfSeqNo_( char * buf, unsigned short c ) {
	return sprintf( buf, "%04x", c );
}

static int sprintfTime_( char * buf, unsigned int c ) {
	return sprintf( buf, "%06x", c );
}


void asyncInit( int fd ) {
	struct termios newtio;

	memset(&newtio, 0, sizeof(newtio)); /* clear struct for new port settings */
	/* 	
		BAUDRATE: Set bps rate. You could also use cfsetispeed and cfsetospeed.
		CRTSCTS : output hardware flow control (only used if the cable has
		all necessary lines. See sect. 7 of Serial-HOWTO)
		CS8: 8n1 (8bit,no parity,1 stopbit)
		CLOCAL  : local connection, no modem contol
		CREAD: enable receiving characters
	*/
	newtio.c_cflag = BAUDRATE | CS8| CREAD;

	/*
		IGNPAR  : ignore bytes with parity errors
		ICRNL: 	map CR to NL (otherwise a CR input on the other computer
						will not terminate input)
						otherwise make device raw (no other input processing)
	*/
	newtio.c_iflag = IGNPAR | IGNBRK | IXON | IXOFF | ICRNL;

	/*
		ICANON  : enable canonical input
							disable all echo functionality, and don't send signals to calling program
	*/
  newtio.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // Raw input.
	/* 
		initialize all control characters 
		default values can be found in /usr/include/termios.h, and are given
		in the comments, but we don't need them here
	*/
	newtio.c_cc[VINTR] = 0;/* Ctrl-c */ 
	newtio.c_cc[VQUIT] = 0;/* Ctrl-\ */
	newtio.c_cc[VERASE] = 0;/* del */
	newtio.c_cc[VKILL] = 0;/* @ */
	newtio.c_cc[VEOF]= 4;/* Ctrl-d */
	newtio.c_cc[VTIME]= 0;/* inter-character timer unused */
	newtio.c_cc[VMIN]= 1;/* blocking read until 1 character arrives */
	newtio.c_cc[VSWTC]= 0;/* '\0' */
	newtio.c_cc[VSTART]= 17;/* Ctrl-q */ /* 0 */
	newtio.c_cc[VSTOP]= 19;/* Ctrl-s */ /* 0 */
	newtio.c_cc[VSUSP]= 0;/* Ctrl-z */
	newtio.c_cc[VEOL]= 0;/* '\0' */
	newtio.c_cc[VREPRINT] = 0;/* Ctrl-r */
	newtio.c_cc[VDISCARD] = 0;/* Ctrl-u */
	newtio.c_cc[VWERASE]  = 0;/* Ctrl-w */
	newtio.c_cc[VLNEXT]= 0;/* Ctrl-v */
	newtio.c_cc[VEOL2]= 0;/* '\0' */

	/* 
		now clean the modem line and activate the settings for the port
	*/
	tcflush(fd, TCIFLUSH);
	tcsetattr(fd, TCSANOW, &newtio);
}


/**
 * Trainer Commands:
 * $run
 * $stop
 * /n* -  n time given as decimal ascii digits terminated by '*'
 * \n* -  n time given as decimal ascii digits terminated by '*'

		Trainer flow control:
  	XON
   	XOFF
   	BREAK

		Trainer data:

		$date
    Date text. For example: November 11, 2009.
		$end
		$version
		CW TRAINER
		$end
		$comment
   	Any comment text.
		$end
		$timescale 1ms $end
		$scope module logic $end
		$var wire 1 $ key $end
		$var wire 1 % out $end
		$var wire 1 ^ overflow $end
		$var wire 1 ( underflow $end
		$upscope $end
		$enddefinitions $end
		$dumpvars
		0$
		0%
		0^
		0(
		$end
		#0
		1$
		1%
		#2211
		0$
		#2296
		0%

 *
 */

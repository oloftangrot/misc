#include <stdio.h>
#include <string.h>

const static int speed = 10;

const static int wordSpace_ = 7;
const static int charSpace_ = 3;
const static int markSpace_ = 1;
const static int di_ = 1;
const static int da_ = 3;

 char * arr[34] = {
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
 "-,-",   /* k */
 ".-..",  /* l */
 "--",    /* m */
 "-.",    /* n */
 "---",   /* o */
 ".--.",  /* p */
 "--.-",  /* q */
 ".-.",   /* r */
 "...",   /* s */
 "-",			/* t */
 "--.",    /* u */
 "...-",   /* v */
 "-..-",   /* x */
 "-.--",   /* y */
 "--..",   /* z */
 ".--.-",  /* å */
 ".-.-",   /* ä */
 "---.",   /* ö */
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

char msg[] = "OLOF HEJ";

const int wpm = 10;

static int sprintfSeqNo_( char * buf, unsigned short c );
static int sprintfTime_( char * buf, unsigned int c );

char buf[10000];


int main ( void )
{
	unsigned short seqNo = 0;
  unsigned short diTime = getDotTimeIn_ms(wpm,0);
	printf( "Baud %d %f\n", wpm, getBaud(wpm,0));
	printf( "Dot time %d %f\n", wpm, (float) diTime);
  printf( "Char rate %d\n", wpm * 5 );

#if 0
	for ( int i = 0; i < 33; i++ )
	{
	  for ( int j = 0; j < strlen(arr[i]; j++ ) {
			
			printf (".");
		}
 	}
#endif
	int n = 0;
	for ( int i = 0; i < strlen(msg); i++ ) {
		int j;
		if ( ' '  != msg[i] ) {
		  char * p = arr[msg[i] - 'A'];	
	    for ( j = 0; j < strlen(p); j++ ) {
			  if ('.' == p[j] ) {
					printf ( "/%d", di_ );
					n += sprintf( buf+n, "/%04x:%06x*", seqNo++, di_ * diTime );
				}
	      else if ( '-' == p[j] ) {
					printf ( "/%d", da_ );
					n += sprintf( buf+n, "/%04x:%06x*", seqNo++, da_ * diTime );
				}
			  else printf ("? " );
			  if ( j < (strlen(p) - 1)) { 
					printf("\\%d", markSpace_ );
					n += sprintf( buf+n, "\\%04x:%06x*", seqNo++, markSpace_ * diTime );
				}
		  }
	  
	  	if ( msg[i+1] != ' ' ) {
				printf ("\\%d", charSpace_ ); 	
					n += sprintf( buf+n, "\\%04x:%06x*", seqNo++, charSpace_ * diTime );
			}
		}
		else {
			printf ("\\%d", wordSpace_ );
					n += sprintf( buf+n, "\\%04x:%06x*", seqNo++, wordSpace_ * diTime );
		}
	}
#if 0
	char buf[256];
  sprintfSeqNo_(buf, 0xa5a5 );
	printf("Res: %s\n", buf );
  sprintfTime_(buf, 0x66a5a5 );
#endif
	printf("\nSequence count %d:\n%s\n", seqNo, buf );

	return 0;
}

static int sprintfSeqNo_( char * buf, unsigned short c ) {
	return sprintf( buf, "%04x", c );
}

static int sprintfTime_( char * buf, unsigned int c ) {
	return sprintf( buf, "%06x", c );
}

/**
 * Commands:
 * > - send
 * < - reply
 * /[aaaa][bbbbbb] - high seq.no. time
 * \[aaaa][bbbbbb] - low seq.no time 
 * XON
 * XOFF
 * eo - error overflow
 * eu - error underflow
 *
 */

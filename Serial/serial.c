#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */

/*
 * 'open_port()' - Open serial port 1.
 *
 * Returns the file descriptor on success or -1 on error.
 */

const char dev[]="/dev/ttyUSB0";
//const char dev[]="/dev/ttyS0";
//const char dev[]="/dev/ttyACM0";

int open_port(void)
{
	int fd; /* File descriptor for the port */
	struct termios options;

	fd = open( dev, O_RDWR | O_NOCTTY | O_NDELAY); 

	if ( fd == -1 )
	{
		/*
		 * Could not open the port.
	   */
		perror("open_port: Unable to open ");
		perror( dev );
	}
	else {
		fcntl(fd, F_SETFL, 0);

    tcgetattr(fd, &options);	/* get the current options */

    /* set raw input, 1 second timeout */
    options.c_cflag     |= (CLOCAL | CREAD);
    options.c_lflag     &= ~(ICANON | ECHO | ECHOE | ISIG);
    options.c_oflag     &= ~OPOST;
    options.c_cc[VMIN]  = 0;
    options.c_cc[VTIME] = 10;

    tcsetattr(fd, TCSANOW, &options);	/* set the options */
	}
	return (fd);
}

int main ( void )
{
	int fd, n;
	char buf[1024];

	memset( buf, 0, 1024 );

	fd = open_port();

#if 1
	n = write(fd, "Hello", 5 );
  printf( "Wrote %d bytes.\n", n );
	if (n < 0)
		fputs("write() of 4 bytes failed!\n", stderr);
#endif
	n = read( fd, buf, 1 );
	printf( "Read %d bytes\n", n );
	printf( "Read <%s>\n", buf );

	sleep( 1 );

	n = read( fd, buf, 1 );
	printf( "Read %d bytes\n", n );
	printf( "Read <%s>\n", buf );

	close( fd ); /* Clean up before quiting! */
	return 0;
}

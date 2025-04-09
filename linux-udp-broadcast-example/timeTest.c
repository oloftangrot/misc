#include <stdio.h>
#include <sys/time.h>
#include <time.h>

int main()
{
    struct timeval tv;
    struct timezone tz;
    struct tm *today;
    int zone;


		for ( int i=0; i < 10; i++) {
		    gettimeofday(&tv,&tz);
				/* get time details */
				today = localtime(&tv.tv_sec);
				printf("It's %d:%0d:%0d.%ld\n",
					      today->tm_hour,
					      today->tm_min,
					      today->tm_sec,
					      tv.tv_usec
					    );
				printf("Milis sec %ld\n", tv.tv_usec/1000 );
				/* set time zone value to hours, not minutes */
				zone = tz.tz_minuteswest/60;
				/* calculate for zones east of GMT */
				if( zone > 12 )
					  zone -= 24;
				printf("Time zone: GMT %+d\n",zone);
				printf("Daylight Saving Time adjustment: %d\n",tz.tz_dsttime);
		}
    return(0);
}

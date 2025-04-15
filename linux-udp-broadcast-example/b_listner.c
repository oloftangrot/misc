#include <arpa/inet.h> 
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <linux/in.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <unistd.h>

#define IP_FOUND "IP_FOUND"
#define IP_FOUND_ACK "IP_FOUND_ACK"

#include "message.h"


char listnerName[MESSENGERNAME_LENGTH]= "DefaultName";
char tickerName[MESSENGERNAME_LENGTH];

struct timespec responceDelay;

void parseCommandLine(int argc, char * argv[]) {
	long delay = -1;

	if ( argc >= 2) {
		if ( strlen(argv[1]) < ( MESSENGERNAME_LENGTH - 1) ) {
			strcpy ( listnerName, argv[1] );
		}
		else {
			fprintf( stderr, "nodeName lenth exceeded!");
			exit(0);
		}
		delay = atoi( argv[2] ); // Assume time is given as miliseconds
		responceDelay.tv_sec = delay / 1000; // Take the seconds part.
		responceDelay.tv_nsec = ( delay % 1000 ) * 1000000;
		fprintf( stdout, "Node %s using delay %ld ms\n", listnerName, delay );
	}
	else
		fprintf(stderr, "Default settings used!\n" );
}

int main(int argc, char * argv[]) {
  int sock;
  struct sockaddr_in client_addr;
  struct sockaddr_in listner_addr;
  socklen_t addr_len;
  int count;
  int ret;
  fd_set readfd;
  char buffer[1024] = { 0 };
  union messengerMsg_t myMsg = { 0 };

  responceDelay.tv_sec = 5;
  responceDelay.tv_nsec = 0;
	parseCommandLine( argc, argv );

  sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock < 0) {
    perror("sock error\n");
    return -1;
  }

  addr_len = sizeof(struct sockaddr_in);

  memset( (void*)&listner_addr, 0, addr_len );
  listner_addr.sin_family = AF_INET;
  listner_addr.sin_addr.s_addr = htons(INADDR_ANY);
  listner_addr.sin_port = htons( COMMON_MESSAGE_PORT );

  ret = bind(sock, (struct sockaddr*)&listner_addr, addr_len);
  if (ret < 0) {
    perror("bind error\n");
    return -1;
  }
  while ( 1 ) {
    FD_ZERO(&readfd);
    FD_SET(sock, &readfd);

    ret = select( sock + 1, &readfd, NULL, NULL, 0 );
    if ( ret > 0 ) {
      if (FD_ISSET(sock, &readfd)) {
        count = recvfrom(sock, buffer, 1024, 0, (struct sockaddr*)&client_addr, &addr_len);
        memcpy( &myMsg, buffer, sizeof(myMsg) );
        printf("Received %d bytes at tick %d from %s\n", count, myMsg.data[TICK_COUNTER_POS], myMsg.str );
      }
    }
		usleep( 0 ); // Yield to the operating system.
  }
  return 0;
}

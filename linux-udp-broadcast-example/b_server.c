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

#define IP_FOUND "IP_FOUND"
#define IP_FOUND_ACK "IP_FOUND_ACK"
#define PORT 9999

#include "ticker.h"
#include "message.h"

char nodeName[MESSENGERNAME_LENGTH]= "DefaultName";
char tickerName[TICKERNAME_LENGTH];

struct timespec responceDelay;

void parseCommandLine(int argc, char * argv[]) {
	long delay = -1;

	if ( argc >= 2) {
		if ( strlen(argv[1]) < ( MESSENGERNAME_LENGTH - 1) ) {
			strcpy ( nodeName, argv[1] );
		}
		else {
			fprintf( stderr, "nodeName lenth exceeded!");
			exit(0);
		}
		delay = atoi( argv[2] ); // Assume time is given as miliseconds
		responceDelay.tv_sec = delay / 1000; // Take the seconds part.
		responceDelay.tv_nsec = ( delay % 1000 ) * 1000000;
		fprintf( stdout, "Node %s using delay %ld ms\n", nodeName, delay );
	}
	else
		fprintf(stderr, "Default settings used!\n" );
}

int main(int argc, char * argv[]) {
  int sock;
//  int yes = 1;
  struct sockaddr_in client_addr;
  struct sockaddr_in server_addr;
  struct sockaddr_in broadcast_addr;

  socklen_t addr_len, broadcast_addr_len;
  int count;
 	int yes = 1;
  int ret;
  fd_set readfd;
  char buffer[1024] = { 0 };
  union tickerMsg_t tickerMsg = { 0 };
	union messengerMsg_t myMsg = { 0 };
	
  responceDelay.tv_sec = 5;
  responceDelay.tv_nsec = 0;
	parseCommandLine( argc, argv );

  sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock < 0) {
    perror("sock error\n");
    return -1;
  }
  ret = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char*)&yes, sizeof(yes));
  if (ret == -1) {
    perror("setsockopt error");
    return 0;
  }
  addr_len = sizeof(struct sockaddr_in);

  memset((void*)&server_addr, 0, addr_len);
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htons(INADDR_ANY);
  server_addr.sin_port = htons(PORT);

  broadcast_addr_len = sizeof(struct sockaddr_in);

  memset((void*)&broadcast_addr, 0, broadcast_addr_len);
  broadcast_addr.sin_family = AF_INET;
  broadcast_addr.sin_addr.s_addr = htonl( INADDR_BROADCAST );
  broadcast_addr.sin_port = htons( COMMON_MESSAGE_PORT );


  ret = bind(sock, (struct sockaddr*)&server_addr, addr_len);
  if (ret < 0) {
    perror("bind error\n");
    return -1;
  }
  while ( 1 ) {
    FD_ZERO(&readfd);
    FD_SET(sock, &readfd);

    ret = select( sock + 1, &readfd, NULL, NULL, 0 );
    if ( ret > 0 ) {
			nanosleep( &responceDelay , NULL) ;
      if (FD_ISSET(sock, &readfd)) {
        count = recvfrom(sock, buffer, 1024, 0, (struct sockaddr*)&client_addr, &addr_len);
        memcpy( &tickerMsg, buffer, sizeof(tickerMsg) );
        printf("Received %d bytes at tick %d from %s\n", count, tickerMsg.data[TICK_COUNTER_POS], tickerMsg.str );
        if (strstr(buffer, "BusTicker")) {
//            memcpy(buffer, IP_FOUND_ACK, strlen(IP_FOUND_ACK) + 1);
            memcpy(myMsg.str, nodeName, strlen(nodeName) + 1);
            myMsg.data[TICK_COUNTER_POS] = tickerMsg.data[TICK_COUNTER_POS];
              count = sendto(sock, &myMsg, sizeof( myMsg ), 0, (struct sockaddr*)&broadcast_addr, broadcast_addr_len);
            (void) count; // Silence -Wall compiler warning.
	          printf("\nClient connection information:\n\t IP: %s, Port: %d\n",
	          				inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        }
        else
        	printf("Received from unknown!\n");
      }
    }
  }
  return 0;
}

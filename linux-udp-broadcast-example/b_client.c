#include <arpa/inet.h> 
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <linux/in.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include "ticker.h"

#define IP_FOUND "IP_FOUND"
#define IP_FOUND_ACK "IP_FOUND_ACK"
#define PORT 9999

const char clientName[] = "BusTicker";

struct timeval tv;
struct timezone tz;

const unsigned int tickTimeInSec = 1;

int main() {
  int sock;
 	int yes = 1;
  struct sockaddr_in broadcast_addr;
  struct sockaddr_in server_addr;
  socklen_t addr_len;
  int count;
  int ret;
  fd_set readfd;
  char buffer[1024] = { 0 };
  union tickerMsg_t  tickerMsg = { 0 };
  int i;
	time_t past=0;

	memcpy(tickerMsg.str, clientName, strlen(clientName) );
	
  sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock < 0) {
    perror("sock error");
    return -1;
  }
  ret = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char*)&yes, sizeof(yes));
  if (ret == -1) {
    perror("setsockopt error");
    return 0;
  }

  addr_len = sizeof(struct sockaddr_in);

  memset((void*)&broadcast_addr, 0, addr_len);
  broadcast_addr.sin_family = AF_INET;
  broadcast_addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
  broadcast_addr.sin_port = htons(PORT);

  for ( i = 0; i < 100; i++ ) {
  	tickerMsg.data[TICK_COUNTER_POS] = i ;
		
		do {
			usleep( 0 ); // Yield to the operating system.
			gettimeofday(&tv,&tz);
		} while ( tv.tv_sec < ( past + tickTimeInSec ) );
		past = tv.tv_sec;
		
    ret = sendto(sock, (void*) &tickerMsg, sizeof( tickerMsg ), 0, (struct sockaddr*) &broadcast_addr, addr_len);

    FD_ZERO(&readfd);
    FD_SET(sock, &readfd);

    ret = select(sock + 1, &readfd, NULL, NULL, NULL);
		memset( buffer, 0, 1024 ); // Empty buffer a stupid way instead of examine receive counter.
    if ( ret > 0 ) {
      if (FD_ISSET(sock, &readfd)) {
        count = recvfrom(sock, buffer, 1024, 0, (struct sockaddr*)&server_addr, &addr_len);
        (void) count; // Silence -Wall compiler warning.
        printf("\trecvmsg is %s\n", buffer);
        if (strstr(buffer, IP_FOUND_ACK)) {
          printf("\tfound server IP is %s, Port is %d\n", inet_ntoa(server_addr.sin_addr),htons(server_addr.sin_port));
        }
      }
    }
  }
}


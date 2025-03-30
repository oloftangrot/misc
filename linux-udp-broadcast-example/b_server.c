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

#define NODENAME_LENGTH (64)
char nodeName[NODENAME_LENGTH]= "DefaultName";
struct timespec responceDelay;

void parseCommandLine(int argc, char * argv[]) {
	long delay = -1;

	if ( argc >= 2) {
		if ( strlen(argv[1]) < ( NODENAME_LENGTH - 1) ) {
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
  socklen_t addr_len;
  int count;
  int ret;
  fd_set readfd;
  char buffer[1024] = { 0 };
  responceDelay.tv_sec = 5;
  responceDelay.tv_nsec = 0;
	parseCommandLine( argc, argv );

  sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock < 0) {
    perror("sock error\n");
    return -1;
  }

  addr_len = sizeof(struct sockaddr_in);

  memset((void*)&server_addr, 0, addr_len);
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htons(INADDR_ANY);
  server_addr.sin_port = htons(PORT);

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
        if (strstr(buffer, IP_FOUND)) {
            memcpy(buffer, IP_FOUND_ACK, strlen(IP_FOUND_ACK) + 1);
            count = sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr*)&client_addr, addr_len);
            (void) count; // Silence -Wall compiler warning.
	          printf("\nClient connection information:\n\t IP: %s, Port: %d\n",
	          				inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        }
      }
    }
  }
  return 0;
}

#ifndef PACKET_DRIVER_H
#define PACKET_DRIVER_H
#ifdef __cplusplus
extern "C" {
#endif
void packetDriverInit( int (*dataOut_fn)(unsigned char data) );
int parseBuffer( unsigned char const * c, int len );
int sendPacket( char const * buf, int len );
int packetReady( void );
#ifdef __cplusplus
}
#endif
#endif


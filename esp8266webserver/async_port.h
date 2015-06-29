#ifndef ASYNC_PORT_H
#define ASYNC_PORT_H
#ifdef __cplusplus
extern "C" {
#endif

void asyncInit( char const * const devStr );
int async_getchar( unsigned char * const c );
int async_putchar(unsigned char bVal);

#ifdef __cplusplus
}
#endif
#endif


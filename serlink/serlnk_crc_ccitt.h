#ifndef SERLNK_CRC_CCITT_H
#define SERLNK_CRC_CCITT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void crc_ccitt_init( uint16_t * const crc );
void crc_ccitt_update( uint16_t * const crc, unsigned char x );
// uint16_t crc_ccitt_crc(void);

#ifdef __cplusplus
}
#endif
#endif

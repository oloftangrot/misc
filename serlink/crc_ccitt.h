#ifndef CRC_CCITT_H
#define CRC_CCITT_H

#include <stdint.h>
#define lo8(a) (a & 0xff)
#define hi8(a) (0xff&(a >> 8) )

static __inline__  uint16_t
crc_ccitt_update (uint16_t crc, uint8_t data)
{
	data ^= lo8 (crc);
	data ^= data << 4;

        return ((((uint16_t)data << 8) | (hi8 (crc))) ^ (uint8_t)(data >> 4) ^ ((uint16_t)data << 3));
}

#endif

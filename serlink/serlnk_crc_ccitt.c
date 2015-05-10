#include "serlnk_crc_ccitt.h"

// static uint16_t crc; 

void
crc_ccitt_init( uint16_t * const crc)
{
	*crc = 0xffff;
}

void
crc_ccitt_update(uint16_t * const crc, unsigned char x)
{
	uint16_t crc_new = (unsigned char)(*crc >> 8) | (*crc << 8);
	crc_new ^= x;
	crc_new ^= (unsigned char)(crc_new & 0xff) >> 4;
	crc_new ^= crc_new << 12;
	crc_new ^= (crc_new & 0xff) << 5;
	*crc = crc_new;
}
/*
uint16_t
crc_ccitt_crc(void)
{
	return crc;
}
*/

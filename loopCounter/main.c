#include <avr/io.h>

#define SPIPORT PORTB
#define clkpinmask 4
#define mosipinmask 8

void spiWrite(uint8_t data)
{
 uint8_t bit;
 for(bit = 0x80; bit; bit >>= 1) {
  SPIPORT &= ~clkpinmask;
  if(data & bit) SPIPORT |= mosipinmask;
  else SPIPORT &= ~mosipinmask;
  SPIPORT |= clkpinmask;
 }
}

int main( void )
{
	unsigned char a = 0;
	for( ;; )
		spiWrite( a++ );
	return 0;
}

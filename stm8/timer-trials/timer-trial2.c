// Source code under CC0 1.0
#include <stdint.h>
#include <stdio.h>

/*
**Includes used by the old timer trial code
*/ 
#define PC_ODR	(*(volatile uint8_t *)0x500a)
#define PC_DDR	(*(volatile uint8_t *)0x500c)
#define PC_CR1	(*(volatile uint8_t *)0x500d)

#define PE_ODR	(*(volatile uint8_t *)0x5014)
#define PE_DDR	(*(volatile uint8_t *)0x5016)
#define PE_CR1	(*(volatile uint8_t *)0x5017)

#define CLK_ICKR	(*(volatile uint8_t *)0x50c0)
#define CLK_PCKENR2	(*(volatile uint8_t *)0x50c4)

#define TIM1_CR1	(*(volatile uint8_t *)0x5250)
#define TIM1_PCNTRH	(*(volatile uint8_t *)0x525e)
#define TIM1_PCNTRL	(*(volatile uint8_t *)0x525f)
#define TIM1_PSCRH	(*(volatile uint8_t *)0x5260)
#define TIM1_PSCRL	(*(volatile uint8_t *)0x5261)


/*
** Includes used by the serial uart
*/
#define CLK_DIVR        (*(volatile uint8_t *)0x50c6)
#define CLK_PCKENR1     (*(volatile uint8_t *)0x50c7)

#define UART1_SR        (*(volatile uint8_t *)0x5230)
#define UART1_DR        (*(volatile uint8_t *)0x5231)
#define UART1_BRR1      (*(volatile uint8_t *)0x5232)
#define UART1_BRR2      (*(volatile uint8_t *)0x5233)
#define UART1_CR2       (*(volatile uint8_t *)0x5235)
#define UART1_CR3       (*(volatile uint8_t *)0x5236)

#define UART_CR2_TEN (1 << 3)
#define UART_CR3_STOP2 (1 << 5)
#define UART_CR3_STOP1 (1 << 4)
#define UART_SR_TXE (1 << 7)

void uart_init( void ) 
{
	CLK_DIVR = 0x00; // Set the frequency to 16 MHz
	CLK_PCKENR1 = 0xFF; // Enable peripherals

	UART1_CR2 = UART_CR2_TEN; // Allow TX and RX
	UART1_CR3 &= ~(UART_CR3_STOP1 | UART_CR3_STOP2); // 1 stop bit
	UART1_BRR2 = 0x03; UART1_BRR1 = 0x68; // 9600 baud
}

void timer1_init( void ) 
{
	CLK_ICKR = 0x01; // Enable the interna RC 16 MHz oscillator
//	CLK_PCKENR2 |= 0x02; // Enable clock to timer

	// Configure timer
	// 1000 ticks per second
	TIM1_PSCRH = 0x3e;
	TIM1_PSCRL = 0x80;
	// Enable timer
	TIM1_CR1 = 0x01;
}

void putchar(char c)
{
        while(!(UART1_SR & UART_SR_TXE));
        UART1_DR = c;
}

unsigned int clock(void)
{
	unsigned char h = TIM1_PCNTRH;
	unsigned char l = TIM1_PCNTRL;
	return ( (unsigned int) (h) << 8 | l );
}

void main(void)
{
	int i;
	timer1_init();
	uart_init();
	for(;;)
	{
#if 1
		for ( i = 0; i < 20000; i++ ) ;
		printf( "%d\n", clock() ) ;
#endif
#if 0
		for ( i = 0; i < 10000; i++ ) ;
		putchar( 0x55 );
#endif
#if 0
		if ( ( clock() % 1000 ) == 0 )
			putchar( '3' );
#endif
#if 0
		if ( ( clock() % 5000 ) == 0 )
			putchar( '5' );
#endif
	}
}


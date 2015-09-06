#define SDA P0_0
#define SCL P0_1

void I2CInit( void )
{
	SDA = 1;
	SCL = 1;
}

void I2CStart( void )
{
	SCL = 1;
	SDA = 0;
	SCL = 0;
}

void I2CRestart( void ) 
{
	SCL = 0;
	SDA = 1;
	SCL = 1;
	SDA = 0;
}

void I2CStop( void )
{
	SCL = 0;
	SDA = 0;
	SCL = 1;
	SDA = 1;
}

void I2CAck( void )
{
	SDA = 0;
	SCL = 1;
	SCL = 0;
	SDA = 1;
}

void I2CNak( void )
{
	SDA = 1;
	SCL = 1;
	SCL = 0;
}

void I2CSend(unsigned char Data) 
{
	unsigned char i;
	for( i = 0; i < 8; i++ ) {
		SCL = 0;
		if( ( Data & 0x80 ) == 0 )
			SDA = 0;
		else
			SDA = 1;
		SCL = 1;
		Data<<= 1;
	}
	SCL = 0;
	SDA = 1;
}

unsigned char I2CRead( void ) {
	unsigned char i, Data=0;
	for( i = 0; i < 8; i++ ) {
		SCL = 0;
		SCL = 1;
		if( SDA )
			Data |= 1;
		Data<<= 1;
	}
	SCL = 0;
	SDA = 1;
	return Data;
}


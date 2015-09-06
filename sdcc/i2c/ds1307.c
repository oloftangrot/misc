void main(){
	unsigned char i, a[7];
	unsigned int j;
	I2CInit();
	LCD_init();

	while(1) {
		I2CStart();
		I2CSend(0xD0);
		I2CAck();
		I2CSend(0x00);
		I2CAck();
		I2CRestart();
		I2CSend(0xD1);
		I2CAck();
		for( i = 0; i < 7; i++ ) {
			a[i]=I2CRead();
			if(i==6)
				I2CNak();
			else
				I2CAck();
		}
		I2CStop();
		LCD_command(0x81);
		LCD_sendnum(a[2]);
		LCD_senddata(':');
		LCD_sendnum(a[1]);
		LCD_senddata(':');
		LCD_sendnum(a[0]);
		LCD_command(0xC0);
		sendday(a[3]);
		LCD_sendnum(a[4]);
		LCD_senddata('-');
		LCD_sendnum(a[5]);
		LCD_senddata('-');
		LCD_sendnum(a[6]);
		for ( j = 65535; j > 0; j-- );
	}
}
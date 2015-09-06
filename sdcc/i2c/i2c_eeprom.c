/*
** cODE WRITTEN FOR 24C04 EEPROM
*/

#include<reg51.h>
#include<stdio.h>
#include<intrins.h>

#define ACK      1
#define NO_ACK   0

unsigned char i;
unsigned char EData[5];
unsigned char Data;

void InitSerial(void);
void DelayMs(unsigned int);
void WriteI2C(unsigned char);
void Start(void);
void Stop(void);
void ReadBYTE(unsigned int);
void WriteBYTE(unsigned int);
unsigned char ReadI2C(bit);

sbit SCL = P2^0;    // connect to SCL pin (Clock)
sbit SDA = P2^1;    // connect to SDA pin (Data)

//---------------------------------------
// Main program
//---------------------------------------
void main(void)
{
	InitSerial();        // Initialize serial port
	putchar(0x0C);        // clear hyper terminal
	DelayMs(5);
	WriteBYTE(0x0000);
	WriteI2C('A');        //Write Data's Here
	WriteI2C('B');
	WriteI2C('C');
	WriteI2C('D');  
	WriteI2C('E');
	WriteI2C('F');  
	Stop(); 
	DelayMs(10);
	ReadBYTE(0x0000);
	EData[0] = ReadI2C(NO_ACK);   
	EData[1] = ReadI2C(NO_ACK);   
	EData[2] = ReadI2C(NO_ACK);   
	EData[3] = ReadI2C(NO_ACK);   
	EData[4] = ReadI2C(NO_ACK);   
	EData[5] = ReadI2C(NO_ACK);

	for(i=0;i<6;i++)
	{
		printf("value = %c\n",EData[i]);   // display data        */
		DelayMs(100);
    }
	while(1);
}

//---------------------------------------
// Initialize serial port
//---------------------------------------
void InitSerial(void)
{
  SCON = 0x52;   // setup serial port control
  TMOD = 0x20;   // hardware (9600 BAUD @11.0592MHZ)
  TH1 = 0xFD;   // TH1
  TR1 = 1;       // Timer 1 on
}

//-------------------------------
// start I2C
//-------------------------------
void Start(void)
{
   SDA = 1;
    SCL = 1;
    _nop_();_nop_();
    SDA = 0;
    _nop_();_nop_();
    SCL = 0;
    _nop_();_nop_();
}

//-------------------------------
// stop I2C
//-------------------------------
void Stop(void)
{
    SDA = 0;   
    _nop_();_nop_();
    SCL = 1;
    _nop_();_nop_();
    SDA = 1;
}

//-------------------------------
// Write I2C
//-------------------------------
void WriteI2C(unsigned char Data)
{  
    for (i=0;i<8;i++)
    {
       SDA = (Data & 0x80) ? 1:0;
         SCL=1;SCL=0;
         Data<<=1;
    }
  SCL = 1;
    _nop_();_nop_();
    SCL = 0;
}

//-------------------------------
// Read I2C
//-------------------------------
unsigned char ReadI2C(bit ACK_Bit)
{
	Start();
    WriteI2C(0xA1);
	SDA = 1;
    for (i=0;i<8;i++)
    {
		SCL   = 1;      
		Data<<= 1;
		Data = (Data | SDA);    
		SCL   = 0;
		_nop_();
    }
   if (ACK_Bit == 1)
    SDA = 0; // Send ACK     
    else    
    SDA = 1; // Send NO ACK               
    _nop_();_nop_();
    SCL = 1;    
    _nop_();_nop_();
    SCL = 0;
    Stop();
    return Data;
}

//-------------------------------
// Read 1 byte form I2C
//-------------------------------
void ReadBYTE(unsigned int Addr)
{
    Start();
    WriteI2C(0xA0);
    WriteI2C((unsigned char)(Addr>>8)&0xFF);
    WriteI2C((unsigned char)Addr&0xFF);
}

//-------------------------------
// Write 1 byte to I2C
//-------------------------------
void WriteBYTE(unsigned int Addr)
{
    Start();
    WriteI2C(0xA0);
    WriteI2C((unsigned char)(Addr>>8)&0xFF);    // send address high
    WriteI2C((unsigned char)Addr&0xFF);             // send address low
}

//---------------------------------------
// Delay mS function
//---------------------------------------
void DelayMs(unsigned int count)
{ // mSec Delay 11.0592 Mhz
   unsigned int i;                    // Keil v7.5a
   while(count)
    {
       i = 115;
         while(i>0) i--;
       count--;
   }
}


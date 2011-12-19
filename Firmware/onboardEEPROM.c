/*
 * This file is part of the Bus Pirate project (http://code.google.com/p/the-bus-pirate/).
 *
 * Written and maintained by the Bus Pirate project.
 *
 * To the extent possible under law, the project has
 * waived all copyright and related or neighboring rights to Bus Pirate. This
 * work is published from United States.
 *
 * For details see: http://creativecommons.org/publicdomain/zero/1.0/.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

#include "base.h"
#include "onboardEEPROM.h"

unsigned char eetest(void){
	unsigned char c,l;
	eei2cSetup();
	l=eeGetWP();
	
	eeEnableWrite();

	eeWriteByte(0xFF,0);
	bpDelayMS(10);
	
	eeWriteByte(0x10,0);
	bpDelayMS(10);
	if (eeReadByte(0)==0x10)
		c= 0;					//in selftest 0 = good.
	else
		c= 1;
		
	eeWriteByte(0xFF,0);
	
	BP_EE_WP = l;				//store last WP setting and post it.
	return c;
}

void eestart(void){
	// Enable a start condition
	I2C1CONbits.SEN = 1;
	while(I2C1CONbits.SEN==1);//wait
}

void eestop(void){
	I2C1CONbits.PEN = 1;
	while(I2C1CONbits.PEN==1);//wait	
}

unsigned char eegetack(void){
	return I2C1STATbits.ACKSTAT;
}

void eesendack(unsigned char ack){
	I2C1CONbits.ACKDT=ack;//send ACK (0) or NACK(1)?
	I2C1CONbits.ACKEN=1;
	while(I2C1CONbits.ACKEN==1);
}

void eewrite(unsigned char c){
	I2C1TRN=c;
	while(I2C1STATbits.TRSTAT==1);
}

unsigned char eeread(void){
	unsigned char c;

	I2C1CONbits.RCEN=1;
	while(I2C1CONbits.RCEN==1);
	c=I2C1RCV;
	return c;
}

void eei2cSetup(void){
	I2C1CONbits.A10M=0;
	I2C1CONbits.SCLREL=0;

	I2C1ADD=0;
	I2C1MSK=0;

	// Enable SMBus 
	I2C1CONbits.SMEN = 0; 

	// Baud rate setting
	I2C1BRG = 37;
	
	// Enable I2C module
	I2C1CONbits.I2CEN = 1;
}

/*
void eei2cSetup(void);
unsigned char eeread(void);
void eewrite(unsigned char c);
void eesendack(unsigned char ack);
unsigned char eegetack(void);
void eestop(void);
void eestop(void);
void eestart(void);
unsigned char eetest(void); */

unsigned char eeReadByte(unsigned long wAddr)
{
	char RetVal;

	eestart();
	eewrite(BP_EEPROM_ID_W);
	eewrite((wAddr >> 8));		//Sent write Address high bits first
	eewrite(wAddr);				//Send write address low bits section

	eestart();
	eewrite(BP_EEPROM_ID_R);

	RetVal = eeread();			//Write the single bit.
	
	eesendack(1);				//send nak?
	eestop();

	return RetVal;
}

unsigned int eeWriteByte(char wByte, unsigned long wAddr)
{
	char RetVal;
	eestart();
	eewrite(BP_EEPROM_ID_W);
	//test1=eegetack();
	
	eewrite((wAddr >> 8));		//Sent write Address high bits first
	eewrite(wAddr);				//Send write address low bits section
	//eegetack();
	eewrite(wByte);		//Write the single bit.
	RetVal = eegetack();
	eestop(); 

	return RetVal;
}

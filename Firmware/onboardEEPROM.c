#include "base.h"
//#include "onboardEEPROM.h"

unsigned char eetest(void){
	unsigned char c;
	eei2cSetup();
	eestart();
	eewrite(0xA0);//send address
	c=eegetack();
	eestop();
	I2C1CONbits.I2CEN = 0;//disable I2C module
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

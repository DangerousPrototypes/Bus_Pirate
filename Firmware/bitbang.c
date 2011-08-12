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
//Bus Pirate bitbang base library
//consolidates all bitbang code into one place

// The software i2c routines were written in c from public domain pseudo code:
/// **** I2C Driver V1.1 Written by V.Himpe. Released as Public Domain **** /
// http://www.esacademy.com/faq/i2c/general/i2cpseud.htm
#include "base.h"
#include "bitbang.h" //need own functions

#define	BB_5KHZSPEED_SETTLE 20 //~5KHz
#define	BB_5KHZSPEED_CLOCK 100
#define	BB_5KHZSPEED_HALFCLOCK BB_5KHZSPEED_CLOCK/2

#define	BB_50KHZSPEED_SETTLE 2 //~50KHz
#define	BB_50KHZSPEED_CLOCK 10
#define	BB_50KHZSPEED_HALFCLOCK BB_50KHZSPEED_CLOCK/2

#define	BB_100KHZSPEED_SETTLE 1 //~100KHz
#define	BB_100KHZSPEED_CLOCK 5
#define	BB_100KHZSPEED_HALFCLOCK 2

#define	BB_MAXSPEED_SETTLE 0 //~400KHz
#define	BB_MAXSPEED_CLOCK 0
#define	BB_MAXSPEED_HALFCLOCK 0

extern struct _modeConfig modeConfig;

struct _bitbang{
	unsigned char pins;
	unsigned int MOpin;
	unsigned int MIpin;
	unsigned char delaySettle;
	unsigned char delayClock;
	unsigned char delayHalfClock;
} bitbang;

void bbSetup(unsigned char pins, unsigned char speed){

	bitbang.pins=pins;

	//define pins for 2 or 3 wire modes (do we use a seperate input pin)
	if(pins==3){ //SPI-like
		bitbang.MOpin=MOSI;
		bitbang.MIpin=MISO;
	}else{ //I2C-like
		bitbang.MOpin=MOSI;
		bitbang.MIpin=MOSI;
	}
	
	
	//define delays for differnt speeds
	// I2C Bus timing in uS
	switch(speed){
		case 0:
			bitbang.delaySettle = BB_5KHZSPEED_SETTLE;
			bitbang.delayClock = BB_5KHZSPEED_CLOCK;
			bitbang.delayHalfClock = BB_5KHZSPEED_HALFCLOCK;
			break;
		case 1:
			bitbang.delaySettle = BB_50KHZSPEED_SETTLE;
			bitbang.delayClock = BB_50KHZSPEED_CLOCK;
			bitbang.delayHalfClock = BB_50KHZSPEED_HALFCLOCK;
			break;
		case 2:
			bitbang.delaySettle = BB_100KHZSPEED_SETTLE;
			bitbang.delayClock = BB_100KHZSPEED_CLOCK;
			bitbang.delayHalfClock = BB_100KHZSPEED_HALFCLOCK;
			break;
		default:
			bitbang.delaySettle = BB_MAXSPEED_SETTLE;
			bitbang.delayClock = BB_MAXSPEED_CLOCK;
			bitbang.delayHalfClock = BB_MAXSPEED_HALFCLOCK;
			break;
	}


}

//
// HELPER functions
//

int bbI2Cstart(void){
	int error=0;
//http://www.esacademy.com/faq/i2c/busevents/i2cstast.htm
	//setup both lines high first
	bbH(MOSI+CLK, bitbang.delayClock);

	//check bus state, return error if held low
	if(BP_CLK==0 || BP_MOSI==0) error=1;

	//now take data low while clock is high
	bbL(MOSI, bitbang.delayClock);

	//next take clock low too
	bbL(CLK, bitbang.delayClock);	
	
	//example suggests returning SDA to high
	bbH(MOSI, bitbang.delayClock);
	
	return error;

}


int bbI2Cstop(void){
//http://www.esacademy.com/faq/i2c/busevents/i2cstast.htm

	//setup both lines low first
	//example suggests just SDA, but some chips are flakey.
	bbL(MOSI+CLK, bitbang.delayClock);

	//take clock high
	bbH(CLK, bitbang.delayClock);

	//with clock high, bring data high too
	bbH(MOSI, bitbang.delayClock);

	//return clock low, importatnt for raw2wire smartcard
	//bbL(CLK, bitbang.delayClock);
	return 0;
}

//
//BYTE functions
//

// ** Read with write for 3-wire protocols ** //

//unsigned char bbReadWriteByte(unsigned char c){
unsigned int bbReadWriteByte(unsigned int c){
	unsigned int i,bt,tem,di,dat=0;

	//begin with clock low...	
	bt=1<<(modeConfig.numbits-1);

	tem=c;//????
//	for(i=0;i<8;i++){
	for(i=0;i<modeConfig.numbits;i++){
		bbPins((tem&bt), MOSI, bitbang.delaySettle); //set data out
		bbH(CLK,bitbang.delayClock);//set clock high
		di=bbR(MISO); //read data pin	
		bbL(CLK,bitbang.delayClock);;//set clock low

		//get MSB first
		tem=tem<<1;  //shift data output bits
		dat=dat<<1;  //shift the data input byte bits
		if(di)dat++; //if datapin in is high, set LBS
	}

	return dat;
}

// ** Seperate read/write for 2-wire protocols ** //

void bbWriteByte(unsigned int c){
	unsigned int i,bt,tem;

	//bbo();//prepare for output

	//bt=0x80;
	bt=1<<(modeConfig.numbits-1);

	tem=c;//????
	for(i=0;i<modeConfig.numbits;i++){
		//if( (b & d)== 0) bbL(bitbang.MOpin,bitbang.delaySettle); else bbH(bitbang.MOpin,bitbang.delaySettle);//setup the data pin
		bbPins((tem&bt), MOSI, bitbang.delaySettle );
		bbH(CLK,bitbang.delayClock);
		bbL(CLK,bitbang.delayClock);

		tem=tem<<1; //next output bit

	}
}

unsigned int bbReadByte(void){
	unsigned int i,di,dat=0;

	//bbi();//prepare for input
	bbR(MOSI); //setup for input

	for(i=0;i<modeConfig.numbits;i++){
		bbH(CLK,bitbang.delayClock);//set clock high
		di=bbR(MOSI); //same as MISO on 2-wire
		bbL(CLK,bitbang.delayClock);;//set clock low

		//get MSB first
		dat=dat<<1;//shift the data input byte bits
		if(di)dat++;//if datapin in is high, set LBS
	}
	return dat;
}

//
// BIT functions
//

unsigned char bbReadBit(void){
	unsigned char c;

	bbR(bitbang.MIpin); //setup for input
	bbH(CLK,bitbang.delayClock);//set clock high
	c=bbR(bitbang.MIpin);
	bbL(CLK,bitbang.delayClock);;//set clock low
	return c;
}

void bbWriteBit(unsigned char c){

	bbPins(c,bitbang.MOpin, bitbang.delaySettle);

	bbH(CLK,bitbang.delayClock);
	bbL(CLK,bitbang.delayClock);
}

void bbClockTicks(unsigned char c){
	unsigned char i;

	for(i=0;i<c;i++){
		bbH(CLK,bitbang.delayClock);
		bbL(CLK,bitbang.delayClock);
	}

}

//
// PIN functions
//
void bbMOSI(unsigned char dir){ bbPins(dir, bitbang.MOpin, bitbang.delaySettle);}
void bbCLK(unsigned char dir){ bbPins(dir, CLK, bitbang.delaySettle);}
void bbCS(unsigned char dir){ bbPins(dir, CS, bitbang.delaySettle);}
unsigned char bbMISO (void){ return bbR(bitbang.MIpin);}

//
// BASE IO functions
//
void bbH(unsigned int pins, unsigned char delay){
	if(modeConfig.HiZ==0){
		IOLAT |= pins;//normal output high
		IODIR &=(~pins);//direction to output
	}else{
		IODIR |= pins;//open collector output high
	}
	bpDelayUS(delay);//delay
}

void bbL(unsigned int pins, unsigned char delay){
	IOLAT &=(~pins); //pins to 0
	IODIR &=(~pins);//direction to output
	bpDelayUS(delay);//delay	
}

void bbPins(unsigned int dir, unsigned int pins, unsigned char delay){
	if(dir==0){
		IOLAT &=(~pins); //pins to 0
		IODIR &=(~pins);//direction to output
	}else{
		if(modeConfig.HiZ==0){
			LATB |= pins;//normal output high
			IODIR &=(~pins);//direction to output
		}else{
			IODIR |= pins;//open collector output high
		}
	}
	bpDelayUS(delay);//delay	
}

unsigned char bbR(unsigned int pin){
	IODIR |= pin; //pin as input
	Nop();
	Nop();
	Nop();
	if(IOPOR & pin) return 1; else return 0;//clear all but pin bit and return result
}




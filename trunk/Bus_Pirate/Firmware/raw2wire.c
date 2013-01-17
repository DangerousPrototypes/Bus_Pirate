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
//#include "raw2wire.h"
#include "bitbang.h"
#include "AUXpin.h"

#include "procMenu.h"		// for the userinteraction subs


#define R2WCLK_TRIS 	BP_CLK_DIR
#define R2WCLK 			BP_CLK 

#define R2WDIO_TRIS 	BP_MOSI_DIR
#define R2WDIO 			BP_MOSI

#define MENU 0
#define ISO78133ATR 1
#define ISO78133ATR_PARSE 2

extern struct _modeConfig modeConfig;
extern struct _command bpCommand;

void r2wMacro_78133Read(void);
void r2wMacro_78133Write(void);

/*
// move into a .h or other .c??? 
int getnumber(int def, int max); // everything to make the compiler happy *dubbelzucht*
int getint(void);
int getrepeat(void);
void consumewhitechars(void);
extern int cmderror;
*/


unsigned int R2Wread(void)
{	return (bbReadByte());
}

unsigned int R2Wwrite(unsigned int c)
{	bbWriteByte(c);//send byte
	return 0x100;
}

void R2Wstart(void)
{	bbI2Cstart();
	//bpWstring("(\\-/_\\)");
	BPMSG1138;
	//bpWmessage(MSG_I2C_START);
	BPMSG1062;
}

void R2Wstop(void)
{	bbI2Cstop();
	//bpWstring("(_/-\\)");
	BPMSG1140;
	//bpWmessage(MSG_I2C_STOP);
	BPMSG1063;
}

unsigned int R2Wbitr(void)
{	return (bbReadBit());
	//bpWmessage(MSG_BIT_NOWINPUT);
}

unsigned int R2Wbitp(void)
{	return (bbMISO());
	//bpWmessage(MSG_BIT_NOWINPUT);
}
		
void R2Wclkl(void)
{	bbCLK(0);
}

void R2Wclkh(void)
{	bbCLK(1);
}

void R2Wclk(void)
{	bbClockTicks(1);
}

void R2Wdatl(void)
{	bbMOSI(0);
}

void R2Wdath(void)
{	bbMOSI(1);
}

void R2Wsettings(void)
{	//bpWstring("R2W (spd hiz)=( ");
	BPMSG1143;
	bpWdec(modeConfig.speed); bpSP;
	bpWdec(modeConfig.HiZ); bpSP;
	//bpWline(")\r\n");
	BPMSG1162;
}


void R2Wsetup(void)
{	int speed, output;

	consumewhitechars();
	speed=getint();
	consumewhitechars();
	output=getint();

	// check for userinput (and sanitycheck it!!)
	if((speed>0)&&(speed<=4))
	{	modeConfig.speed=speed-1;
	}
	else	
	{	speed=0;					// when speed is 0 we ask the user
	}
	if((output>0)&&(output<=2))
	{	modeConfig.HiZ=(~(output-1));
	}
	else	
	{	speed=0;					// when speed is 0 we ask the user
	}

	if(speed==0)
	{	cmderror=0;
		//bpWmessage(MSG_OPT_BB_SPEED);
		BPMSG1065;
		modeConfig.speed=(getnumber(1,1,4,0)-1);
		//bpWmessage(MSG_OPT_OUTPUT_TYPE);
		BPMSG1142;
		modeConfig.HiZ=(~(getnumber(1,1,2,0)-1));
	}
	else
	{	R2Wsettings();
	}
}
void R2Wsetup_exc(void)
{
    //writes to the PORTs write to the LATCH
	R2WCLK=0;			//B8 scl 
	R2WDIO=0;			//B9 sda
	R2WDIO_TRIS=1;//data input
	R2WCLK_TRIS=0;//clock output
	bbSetup(2, modeConfig.speed);    
}    

void R2Wmacro(unsigned int c)
{
	switch(c)
	{	case MENU:
			//bpWstring(OUMSG_R2W_MACRO_MENU);
			BPMSG1144;
			break;
		case ISO78133ATR:
			r2wMacro_78133Write();
		case ISO78133ATR_PARSE:
			r2wMacro_78133Read();
			break;
		default:
			//bpWmessage(MSG_ERROR_MACRO);
			BPMSG1016;
	}
}

void R2Wpins(pins) {
        #if defined(BUSPIRATEV4)
        BPMSG1261; //bpWline("-\t-\tSCL\tSDA");
        #else
       	BPMSG1231; //bpWline("SCL\tSDA\t-\t-");
        #endif
}

//
// R2W macros

//ISO 7813-3 Answer to reset macro for smartcards
// syntax: a0%255@^a
// **depricated, some cards are MSB first** forces LSB format for easy use
// now uses CS pin instead of AUX pin because 1,2,3 have build in pullups on CS but not AUX
void r2wMacro_78133Write(void){

	//bpWline("ISO 7816-3 ATR (RESET on CS)");
	//bpWline("RESET HIGH, CLOCK TICK, RESET LOW");
	BPMSG1145;
	
	//Reset needs to start low
	bbCS(0); //bpAuxLow();
	bpDelayUS(0xff);
	
	//RESET HIGH
	bbCS(1);

	//clock tick
	bbClockTicks(1);

	//reset low again
	bbCS(0); //bpAuxLow();
}
	
void r2wMacro_78133Read(void){	
	unsigned char m[4];//macro buffer...
	unsigned char c;
	unsigned int i;

	//bpWstring("ISO 7816-3 reply (uses current LSB setting): ");
	BPMSG1146;

	//read and display ISO 7813-3 bytes
	for(i=0; i<4; i++){
		m[i]=bbReadByte();
		bpWhex(m[i]);
		bpSP;
	}
	bpWBR;

	//modeConfig.lsbEN=c;//return to previous LSB setting

	//parse the first two bytes for 7813-3 atr header info
	//bits8:5 8=serial, 9=3wire, 10=2wire 0xf=RFU
	//c=(m[0]>>4);
	//bpWstring("Protocol: ");
	BPMSG1147;
	switch((m[0]>>4)){
		case 0x08:
			//bpWline("serial");
			BPMSG1148;
			break;
		case 0x09:
			//bpWline("3 wire");
			BPMSG1149;
			break;
		case 0x0A:
			//bpWline("2 wire");
			BPMSG1150;
			break;
		case 0x0F:
			//bpWline("RFU");
			BPMSG1151;
			break;
		default:
			//bpWline("unknown");
			BPMSG1152;
			break;
	}
	bpBR;
	//bits4 RFU
	//bits 3:1 structure, x00 reserved, 010 general, 110 proprietary, x01 x11, special
	
	//bit 8 Supports random read lengths (0=false)
	//bpWstring("Read type: ");
	BPMSG1153;
	//c=(m[1]>>7);
	if((m[1]>>7)==0){
		//bpWline("to end");
		BPMSG1154;
	}else{
		//bpWline("variable length");
		BPMSG1155;
	}
	bpBR;
	//bit 7:4 data units (0001=128, 0010 = 256, 0011=512, etc, 1111=RFU)
	//bpWstring("Data units: ");
	BPMSG1156;
	c=((m[1]&(~0b10000111))>>3);
	if(c==0){
		//bpWstring("no indication");
		BPMSG1157;
	}else if(c==0b1111){
		//bpWstring("RFU");
		BPMSG1151;
	}else{
		i=64;//no 0...
		for(m[0]=0;m[0]<c;m[0]++)i*=2;//multiply by two each time
		bpWintdec(i);
	}
	bpBR;
	
	//bit 3:1 length of data units in bits (2^(3:1))
	//bpWstring("Data unit length (bits): ");
	BPMSG1158;
	c=(m[1]&(~0b11111000));
	i=1;
	for(m[0]=0;m[0]<c;m[0]++)i*=2;//multiply by two each time
	bpWdec(i);
	bpBR;
}

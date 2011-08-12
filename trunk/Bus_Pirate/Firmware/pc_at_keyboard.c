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
//#include "pc_at_keyboard.h"

#define KBCLK_TRIS 	BP_CLK_DIR
#define KBCLK 			BP_CLK

#define KBDIO_TRIS 	BP_MOSI_DIR
#define KBDIO 			BP_MOSI

extern struct _modeConfig modeConfig;
extern struct _command bpCommand;

#ifdef BP_USE_PCATKB

void kbSetup(void);
unsigned char kbReadBit(void);
unsigned char kbReadCode(void);
unsigned char kbReadByte(void);
unsigned char kbWriteByte(unsigned char c);
unsigned char kbWriteBit(unsigned char c);
void kbScancodeResults(unsigned char c);
unsigned char kbWaitClock(unsigned char c);

struct _kbframe{
	unsigned char startbit:1;
	unsigned char code;
	unsigned char paritybit:1;
	unsigned char stopbit:1;
	unsigned char parityerror:1;
} kbScancode;


void KEYBsetup(void)
{	modeConfig.HiZ=1;//yes, always HiZ
	kbSetup();
}

unsigned int KEYBread(void)
{	kbScancodeResults(kbReadByte());
	return kbScancode.code;
}

unsigned int KEYBwrite(unsigned int c)
{	unsigned char temp;

	temp=kbWriteByte(c);//send to bus
	if(temp==0) //ack bit
	{	//bpWmessage(MSG_ACK);
		BPMSG1060;
	}
	else if (temp==1)
	{	//bpWmessage(MSG_NACK);
		BPMSG1061;
	}
	else
	{	//bpWstring(OUMSG_KB_TIMEOUT);
		BPMSG1237;
	}
	return 0x100;
}

void KEYBmacro(unsigned int c)
{	switch(c)
	{	case 0:	//bpWline(OUMSG_KB_MACRO_MENU);
				BPMSG1238;
				break;
		case 1:	//bpWline(OUMSG_KB__MACRO_LIVE);
				BPMSG1239;
				BPMSG1250;
				while(1)
				{	if(kbReadByte()==0)
					{	bpWbyte(kbScancode.code);
						bpSP;
					}
					if(UART1RXRdy == 1) //any key pressed, exit
					{	c=UART1RX /* JTR usb port; */;
						bpBR;
						break;
					}						
				}
				break;
		default: BPMSG1016;
	}
}

void kbScancodeResults(unsigned char c)
{	switch(c)
	{	case 0:		// all ok
			//bpWbyte(kbScancode.code);
			//bpSP;		
			break;
		case 1:
			//bpWstring(OUMSG_KB_ERROR_STARTBIT);
			BPMSG1240;				
			break;
		case 2:
			//bpWbyte(kbScancode.code);
			//bpWstring(OUMSG_KB_ERROR_PARITY);
			BPMSG1241;
			break;
		case 3:
			//bpWbyte(kbScancode.code);
			//bpWstring(OUMSG_KB_ERROR_STOPBIT);
			BPMSG1242;
			break;
		case 4:
			//bpWstring(OUMSG_KB_ERROR_TIMEOUT);
			bpSP;
			BPMSG1237;
			break;
		case 0xff://no data
			//bpWstring(OUMSG_KB_ERROR_NONE);				
			BPMSG1243;
			break;
		default:
			//bpWstring(OUMSG_KB_ERROR_UNK);				
			BPMSG1244;
			break;
	}
}

void kbSetup(void){
	//writes to the PORTs write to the LATCH
	KBDIO_TRIS=1;//data input
	KBCLK_TRIS=0;//clock output/low
	KBCLK=0;			//B8 scl 
	KBDIO=0;			//B9 sda
}


unsigned char kbReadBit(void){
	unsigned char j;

	if(kbWaitClock(0)!=0) return 2; //wait for clock High->LOW (ACTIVE)

	j=KBDIO; //read the data pin

	if(kbWaitClock(1)!=0) return 2; //wait for clock low->HIGH (IDLE)

	return j;
}

//reads scancode into kbScancode structure
//returns 0 - success
//1 - startbit error
//2 - parity error
//3 - stopbit error
//4 - read bit timeout error
unsigned char kbReadCode(void){
	unsigned char i, par=0,c=0, b;

	//get startbit
	kbScancode.startbit=KBDIO;
	if(kbScancode.startbit!=0) return 1;//startbit should be 0

	//while(KBCLK==0);//wait for clock HIGH (IDLE)
	if(kbWaitClock(1)!=0) return 4;

	//grab the eight databits LSB
	for(i=0;i<8;i++){
		c=c>>1;
		b=kbReadBit();
		if(b==2) return 4; //bit timeout
		if(b==1){
			c+=0b10000000;
			par^=1;
		}
	}
	kbScancode.code=c;

	//get parity bit and check
	b=kbReadBit();
	if(b==2) return 4; //bit timeout
	kbScancode.paritybit=b;
	//odd parity:if these are the same, there is an error
	//condense to bitwise op...
	if(kbScancode.paritybit==par){
		kbScancode.parityerror=1;
		return 2; //error
 	}else{
		kbScancode.parityerror=0;//ok
	}

	//get stopbit
	b=kbReadBit();
	if(b==2) return 4; //bit timeout
	kbScancode.stopbit=b;//stopbit should be 1
	KBCLK_TRIS=0;//set clock low again to idle keyboard
	if(kbScancode.stopbit!=1) return 3;
	
	//success!
	return 0;
}

unsigned char kbReadByte(void){
	unsigned char i,c=0xff;

	//let clk float, clk and data will drop if there is data	
	KBDIO_TRIS=1;//data input
	KBCLK_TRIS=1;//clock input/high (keyboard will go low if scan code ready)

	for(i=0;i<255;i++){//do a loop, wait for change
	//while(1){//for debugging
		if(KBCLK==0){//check clock status
			c=kbReadCode();//read the scancode
			if(c==0 || c==4)break;//success or error, break the loop
		}
		bpDelayUS(5);//delay
	}

	//disable any further data by dropping clock line
	KBCLK_TRIS=0;//set clock low again

	return c;
}

//writes a bit using the KB clock signal.
// 0=success, 2=timeout
unsigned char kbWriteBit(unsigned char c){

	if(kbWaitClock(1)!=0) return 2; //wait for low-high transition
	KBDIO_TRIS=c;//set data direction
	if(kbWaitClock(0)!=0) return 2; //wait for high-low transition
	return 0;	//data now entered, you have a tiny bit of time before next bit
}

//@c - byte to send
//@returns - ack bit
unsigned char kbWriteByte(unsigned char c){
	unsigned char par=0;
	unsigned int i;

	//take clk low to own the bus
	//clock should already be low
	KBCLK_TRIS=0;//clock low
	KBCLK=0;

	bpDelayUS(60);//delay at least 1 bit period (60us), interrupt any other data transfer

	//data low to signal host->kb transfer
	KBDIO=0;
	KBDIO_TRIS=0;//data low

	//take clock high again, KB will start own clock
	KBCLK_TRIS=1;//clock high/input
	bpDelayUS(1);

	//while(KBCLK==1);//wait for first falling edge
	if(kbWaitClock(0)!=0) return 4; //timeout

	//bang out the bits LSB first out using the kb clock signal
	for(i=0;i<8;i++){
		if((c&1)==1){
			if(kbWriteBit(1)!=0)return 4;//send LSB first
			par^=1;//track parity bit
		}else{
			if(kbWriteBit(0)!=0)return 4;//send LSB first
		}		
		c=c>>1;//-- Shift next bit into position		
	}
	
	//write parity bit
	par^=1;//shift parity bit once more so it is opposite
	if(kbWriteBit(par)!=0)return 4;//send parity bit

	KBDIO_TRIS=1;//configure data for input
	KBDIO=0;

	//while(KBCLK==0);//wait for low->high transition to align with a readbit 
	if(kbWaitClock(1)!=0) return 4; //timeout

	if(kbReadBit()==2) return 4;//junk bit (stopbit/high)
	i=kbReadBit();//ack bit
	if(i==2) return 4;

	KBCLK_TRIS=0;//set clock low again to halt data
	KBCLK=0;

	return i;//return ack bit
}


unsigned char kbWaitClock(unsigned char c){
	unsigned int i=0xffff;
	while(1){
		if(KBCLK==c)return 0; //clock changed
		i--;
		if(i==0)return 1;//timeout
	}
}

#endif

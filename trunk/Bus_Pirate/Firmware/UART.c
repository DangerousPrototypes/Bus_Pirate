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

#ifdef BP_USE_HWUART

#include "uart2io.h"
#include "binIOhelpers.h"

#include "procMenu.h"		// for the userinteraction subs


extern struct _modeConfig modeConfig;
extern struct _command bpCommand;

/*
// move into a .h or other .c??? 
int getnumber(int def, int max); // everything to make the compiler happy *dubbelzucht*
int getint(void);
int getrepeat(void);
void consumewhitechars(void);
extern int cmderror;
*/


struct _UART{
	unsigned char dbp:2; //databits and parity
	unsigned char sb:1;//stop bits
	unsigned char rxp:1;//receive polarity
	unsigned char eu:1;//echo uart
} uartSettings;

static unsigned int UART2speed[]={13332,3332,1666,832,416,207,103,68,34,127};//BRG:300,1200,2400,4800,9600,19200,38400,57600,115200, 31250,

unsigned int UARTread(void)
{	unsigned int c;
	if(UART2RXRdy())
	{	if(U2STAbits.PERR) BPMSG1194;	//bpWstring("-p "); //show any errors
		if(U2STAbits.FERR) BPMSG1195;	//bpWstring("-f ");
		c=UART2RX();

		if(U2STAbits.OERR)
		{	//bpWstring("*Bytes dropped*");
			BPMSG1196;
			U2STA &= (~0b10); //clear overrun error if exists
		}
		return c;
	}
	else
	{	//bpWline(OUMSG_UART_READ_FAIL);
		BPMSG1197;	
	}
	return 0;
}

unsigned int UARTwrite(unsigned int c)
{	UART2TX(c);				//send byte
	return 0;
}

// todo: read from cmdline for now it is ok

void UARTsettings(void)
{	//bpWstring("UART (spd dbp sb rxp hiz)=( ");
	BPMSG1202;
	bpWdec(modeConfig.speed); bpSP;
	if(modeConfig.speed==9)
	{	bpWintdec(U2BRG); bpSP;
	}
	else
	{	bpWintdec(UART2speed[modeConfig.speed]); bpSP;
	}
	bpWdec(uartSettings.dbp); bpSP;
	bpWdec(uartSettings.sb); bpSP;
	bpWdec(uartSettings.rxp); bpSP;
	bpWdec(modeConfig.HiZ); bpSP;
	//bpWline(")\r\n");
	BPMSG1162;
}	


void UARTsetup(void)
{	int speed, dbp, sb, rxp, output, brg=0;

	consumewhitechars();
	speed=getint();
	if(speed==10)
	{	consumewhitechars();
		brg=getint();
	}
	consumewhitechars();
	dbp=getint();
	consumewhitechars();
	sb=getint();
	consumewhitechars();
	rxp=getint();
	consumewhitechars();
	output=getint();
 
	if((speed>0)&&(speed<=10))
	{	modeConfig.speed=speed-1;
	}
	else	
	{	speed=0;					// when speed is 0 we ask the user
	}
	if((dbp>0)&&(dbp<=4))
	{	uartSettings.dbp=dbp-1;
	}
	else	
	{	speed=0;					// when speed is 0 we ask the user
	}
	if((sb>0)&&(sb<=2))
	{	uartSettings.sb=sb-1;
	}
	else	
	{	speed=0;					// when speed is 0 we ask the user
	}
	if((rxp>0)&&(rxp<=2))
	{	uartSettings.rxp=rxp-1;
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

		//bpWmessage(MSG_OPT_UART_BAUD); //write text (baud rate)
		BPMSG1133;
		//BPMSG1198;
		modeConfig.speed=getnumber(1,1,10,0)-1; //get user reply

		if(modeConfig.speed==9)
		{	BPMSG1248;
			brg=getnumber(34,1,32767,0);
		}
		
		//bpWstring("Data bits and parity:\x0D\x0A 1. 8, NONE *default \x0D\x0A 2. 8, EVEN \x0D\x0A 3. 8, ODD \x0D\x0A 4. 9, NONE \x0D\x0A");
		//bpWline(OUMSG_UART_DATABITS_PARITY); //write text (data bit and parity)
		BPMSG1199;
		uartSettings.dbp=getnumber(1,1,4,0)-1;
	
		//bpWstring("Stop bits:\x0D\x0A 1. 1 *default\x0D\x0A 2. 2 \x0D\x0A");
		//bpWline(OUMSG_UART_STOPBITS); //write text 
		BPMSG1200;
		uartSettings.sb=getnumber(1,1,2,0)-1;
	
		//string("Receive polarity:\x0D\x0A 1. Idle 1 *default\x0D\x0A 2. Idle 0\x0D\x0A");				
		//bpWline(OUMSG_UART_RXPOLARITY); //write text 
		BPMSG1201;
		uartSettings.rxp=getnumber(1,1,2,0)-1;
	
		//bpWmessage(MSG_OPT_OUTPUT_TYPE);
		BPMSG1142;
		modeConfig.HiZ=(~(getnumber(1,1,2,0)-1));

	}
	else
	{	UARTsettings();
	}

	if(modeConfig.speed==9)
	{	UART2Setup(brg,modeConfig.HiZ, uartSettings.rxp, uartSettings.dbp, uartSettings.sb );
	}
	else
	{	UART2Setup(UART2speed[modeConfig.speed],modeConfig.HiZ, uartSettings.rxp, uartSettings.dbp, uartSettings.sb );
	}

	if(uartSettings.dbp==3)		// 9 bits
	{	modeConfig.numbits=9;
	}

	UART2Enable();

	if(U2BRG<U1BRG) BPMSG1249;

}


void UARTcleanup(void)
{	UART2Disable();

}

void UARTmacro(unsigned int macro)
{
	switch(macro){
		case 0://menu
			//bpWline(OUMSG_UART_MACRO_MENU);
			BPMSG1203;
			break;
		#if defined(BUSPIRATEV25) || defined(BUSPIRATEV3)
		case 3://UART bridge with flow control
			//setup RTS CTS on FTDI chip side
			FTDI_CTS_DIR=0; //CTS (PIC output to FTDI)
			FTDI_RTS_DIR=1; //RTS (PIC input from FTDI)
			//PORTAbits.RA4=0;
			//PORTAbits.RA5=0;
			BP_CS_DIR=1;//external CTS (PIC input from external circuit)
			BP_CLK_DIR=0;//external RTS (PIC output mirrors output from FTDI)
			//BP_CS=0;//external CTS (PIC input from external circuit)
			//BP_CLK=0;//external RTS (PIC mirrors output from FTDI)
		#endif
		case 1://transparent UART
			//bpWline("UART bridge");
			BPMSG1204;
			//bpWline("Reset to exit");
			BPMSG1205; 
			if(!agree()) break;
			// could use a lot of improvement
			//buffers for baud rate differences
			//it's best to adjust the terminal to the same speed you want to use to avoid buffer overuns
			//it will fail silently
			U2STA &= (~0b10); //clear overrun error if exists
			while(1){//never ending loop, reset Bus Pirate to get out
				if((U2STAbits.URXDA==1)&& (U1STAbits.UTXBF == 0)){
						U1TXREG = U2RXREG; //URXDA doesn't get cleared untill this happens
				}
				if((UART1RXRdy()==1)&& (U2STAbits.UTXBF == 0)){
						U2TXREG = UART1RX(); /* JTR usb port; */; //URXDA doesn't get cleared untill this happens
				}
				if(U2STAbits.OERR || U1STAbits.OERR){
   					U2STA &= (~0b10); //clear overrun error if exists
   					U1STA &= (~0b10); //clear overrun error if exists
					BP_LEDMODE=0;//MODE LED off to signify overrun error
				}
				#if defined(BUSPIRATEV25) || defined(BUSPIRATEV3)
				if(macro==3){
					//pass RTS/CTS
					BP_CLK=FTDI_RTS;
					FTDI_CTS=BP_CS;								
				}
				#endif
			}
			break;
		case 2: //Watch raw UART
			//bpWline("Raw UART input");
			BPMSG1206;
			BPMSG1250; //any key to exit
			
			// could use a lot of improvement
			//buffers for baud rate differences
			//it's best to adjust the terminal to the same speed you want to use to avoid buffer overuns
			//it will fail silently
			U2STA &= (~0b10); //clear overrun error if exists
			while(1){//never ending loop, reset Bus Pirate to get out
				if((U2STAbits.URXDA==1)&& (U1STAbits.UTXBF == 0)){
						U1TXREG = U2RXREG; //URXDA doesn't get cleared untill this happens
				}
				if (UART1RXRdy()==1){//escape
				 	macro=UART1RX(); /* JTR usb port; */;//read it to discard the byte
					break;//leave the loop
				}
			}
			break;
		//case 2://auto UART baud rate
		//	break;			
		default:
			//bpWmessage(MSG_ERROR_MACRO);
			BPMSG1016;
	}
}


void UARTstart(void)
{	U2STA &= (~0b10); //clear overrun error if exists
	uartSettings.eu=1;//open uart
	modeConfig.periodicService=1;//start periodic service calls
	//bpWline(OUMSG_UART_LIVE_DISPLAY_ON);
	BPMSG1207;
}

void UARTstop(void)
{	uartSettings.eu=0;// uart
	modeConfig.periodicService=0;//start periodic service calls
	//bpWline(OUMSG_UART_LIVE_DISPLAY_OFF);
	BPMSG1208;
}

unsigned int UARTperiodic(void)
{	unsigned int temp;

	temp=0;
	while(UART2RXRdy())			//data ready
	{	if(uartSettings.eu==1)
		{	bpWBR;
			//bpWmessage(MSG_READ); //bpWstring(OUMSG_UART_READ);
			BPMSG1102;
			if(U2STAbits.PERR) BPMSG1194;	//bpWstring("-p "); //show any errors
			if(U2STAbits.FERR) BPMSG1195;	//bpWstring("-f ");
			bpWbyte(UART2RX());
			if(U2STAbits.OERR)
			{	//bpWstring("*Bytes dropped*");
				BPMSG1196;
	 			U2STA &= (~0b10); //clear overrun error if exists
			}	
			bpWBR;
		}else
		{	UART2RX();//clear the buffer....
		}
		temp=1;
	}
	return temp;
}

void UARTpins(void) {
        #if defined(BUSPIRATEV4)
        BPMSG1260; //bpWline("-\tRxD\t-\tTxD");
        #else
       	BPMSG1230; //bpWline("-\tTxD\t-\tRxD");
        #endif
}

/*
databits and parity (2bits)
1. 8, NONE *default \x0D\x0A 2. 8, EVEN \x0D\x0A 3. 8, ODD \x0D\x0A 4. 9, NONE
Stop bits:\x0D\x0A 1. 1 *default\x0D\x0A 2. 2 \x0D\x0A
Receive polarity:\x0D\x0A 1. Idle 1 *default\x0D\x0A 2. Idle 0\x0D\x0A
output type (hiz or regular
peripheral settings

# 00000000//reset to BBIO
# 00000001 � mode version string (ART1)
# 00000010 � UART start echo uart RX
# 00000011 � UART stop echo uart RX
# 00000111 - UART speed manual config, 2 bytes (BRGH, BRGL)
# 00001111 - bridge mode (reset to exit)
# 0001xxxx � Bulk transfer, send 1-16 bytes (0=1byte!)
# 0100wxyz � Set peripheral w=power, x=pullups, y=AUX, z=CS
# 0101wxyz � read peripherals 
# 0110xxxx - Set speed,0000=300,0001=1200,10=2400,4800,9600,19200,31250, 38400,57600,1010=115200,
# 0111xxxx - Read speed, 
# 100wxxyz � config, w=output type, xx=databits and parity, y=stop bits, z=rx polarity (default :00000)
# 101wxxyz � read config
*/
static unsigned int binUARTspeed[]={13332,3332,1666,832,416,207,127,103,68,34,};//BRG:300,1200,2400,4800,9600,19200,31250,38400,57600,115200

void binUARTversionString(void){bpWstring("ART1");}

void binUART(void){
	static unsigned char inByte, rawCommand,i;
	static unsigned int BRGval;

	uartSettings.dbp=0; //startup defaults
	uartSettings.sb=0;
	uartSettings.rxp=0;
	modeConfig.HiZ=1;
	BRGval=binUARTspeed[0]; //start at 300bps
	uartSettings.eu=0;
	UART2Setup(BRGval,modeConfig.HiZ, uartSettings.rxp, uartSettings.dbp, uartSettings.sb );
	UART2Enable();
	binUARTversionString();

	while(1){

		//check for incomming bytes on UART2
		//if echo enabled, send to USB
		//else, just clear the buffer
		if( UART2RXRdy()){
			if(uartSettings.eu==1){ 
				UART1TX(UART2RX());
			}else{
				UART2RX();//clear the buffer....
			}
		}
		if(U2STAbits.OERR) U2STA &= (~0b10); //clear overrun error if exists

		//process commands
		if(UART1RXRdy() == 1){//wait for a byte
			inByte=UART1RX(); /* JTR usb port; */; //grab it
			rawCommand=(inByte>>4);//get command bits in seperate variable
			
			switch(rawCommand){
				case 0://reset/setup/config commands
					switch(inByte){
						case 0://0, reset exit
							UART2Disable();
							return; //exit
							break;
						case 1://reply string
							binUARTversionString();
							break;
						case 2://00000010 � Show UART input
							UART1TX(1);
							if(U2STAbits.OERR) U2STA &= (~0b10); //clear overrun error if exists
							uartSettings.eu=1;
							break;
						case 3://00000011 � Don't output UART input
							uartSettings.eu=0;
							UART1TX(1);
							break;
						case 7://00000111 - UART speed manual config, 2 bytes (BRGH, BRGL)
							UART1TX(1);
							UART2Disable();
							BRGval=(unsigned int)(UART1RX()<<8);
							UART1TX(1);
							BRGval|=UART1RX(); /* JTR usb port; */
							UART2Setup(BRGval,modeConfig.HiZ, uartSettings.rxp, uartSettings.dbp, uartSettings.sb );
							UART2Enable();
							UART1TX(1);
							break;
						case 15://00001111 - bridge mode (reset to exit)
							UART1TX(1);
							U2STA &= (~0b10); //clear overrun error if exists
							while(1){//never ending loop, reset Bus Pirate to get out
								if((U2STAbits.URXDA==1)&& (U1STAbits.UTXBF == 0)){
										U1TXREG = U2RXREG; //URXDA doesn't get cleared untill this happens
								}
								if((UART1RXRdy()==1)&& (U2STAbits.UTXBF == 0)){
										U2TXREG = UART1RX(); /* JTR usb port; */; //URXDA doesn't get cleared untill this happens
								}
							}
						default:
							UART1TX(0);
							break;
					}	
					break;
				case 0b0001://get x+1 bytes
					inByte&=(~0b11110000); //clear command portion
					inByte++; //increment by 1, 0=1byte
					UART1TX(1);//send 1/OK		
	
					for(i=0;i<inByte;i++){
						UART2TX(UART1RX()); // JTR usb port;
						UART1TX(1);
					}
	
					break;
				case 0b0100: //configure peripherals w=power, x=pullups, y=AUX, z=CS
					binIOperipheralset(inByte);	
					UART1TX(1);//send 1/OK		
					break;
				case 0b0110://set speed 
					//0110xxxx - Set speed,0000=300,0001=1200,10=2400,4800,9600,19200,31250, 38400,57600,1010=115200,
					inByte&=(~0b11110000);//clear command portion
					if(inByte>0b1010) inByte=0b1010; //safe default if out of range
					BRGval=binUARTspeed[inByte];
					UART2Disable();
					UART2Setup(BRGval,modeConfig.HiZ, uartSettings.rxp, uartSettings.dbp, uartSettings.sb );
					UART2Enable();
					UART1TX(1);//send 1/OK	
					break;
				case 0b1000: //set config
				case 0b1001: //set config
					//100wxxyz � config, w=output type, xx=databits and parity, y=stop bits, z=rx polarity (default :00000)
					uartSettings.dbp=0;
					uartSettings.sb=0;
					uartSettings.rxp=0;
					modeConfig.HiZ=0;
					if(inByte&0b1000) uartSettings.dbp|=0b10;//set 
					if(inByte&0b100) uartSettings.dbp|=0b1;//set 
					if(inByte&0b10) uartSettings.sb=1;//set 	
					if(inByte&0b1) uartSettings.rxp=1;//set 
					if((inByte&0b10000)==0) modeConfig.HiZ=1;//hiz output if this bit is 1
					UART2Disable();
					UART2Setup(BRGval,modeConfig.HiZ, uartSettings.rxp, uartSettings.dbp, uartSettings.sb );
					UART2Enable();
					UART1TX(1);//send 1/OK	
					break;
				default:
					UART1TX(0x00);//send 0/Error
					break;
			}//command switch
		}//if inbyte
	}//while loop
}//function

#endif

/*
 * This file is part of the Bus Pirate project (http://code.google.com/p/the-bus-pirate/).
 *
 * We claim no copyright on our code, but there may be different licenses for some of the code in this file.
 *
 * To the extent possible under law, the Bus Pirate project has
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

#ifdef BP_USE_1WIRE

#include "1wire_lib.h"

#ifndef BP_MOSI
 #error BP_MOSI Must Be Defined for 1-wire Bus I/O
#endif

#ifndef BP_MOSI_DIR
 #error BP_MOSI_DIR Must be defined for 1-wire Bus I/O
#endif


#if(USE_OWGLOBALS)
// OWShort: If 1; there is a short on the bus (set during OWReset)
static unsigned char OWShort=0;
// OWDevice: If 1; There is atleast one device on the bus (set during OWReset)
static unsigned char OWDevice=0;
#endif


/* ***********************************************************************************
   Function: OWReset
   Args[0]: Void
   Return: Made it mimic the old source.
		0 = OK
		1 = Short
		2 = No device.
   
   Desc: OneWire reset bus procedure. See return values for details.
*********************************************************************************** */
unsigned char OWReset(void){
	unsigned int Presence=0,i;	

	OW_bpResPins(); 		//found out i needed this after all.

	SDA_TRIS=0;				//go low

							//Maxim says a minimum of 480. 
	for(i=0;i<34;i++) {		//BP Timing seems off; adjusting it looks like each FOR takes 4us. 
		bpDelayUS(10);		//So 10+4=14 ## I want ~490 so (((TIMEWANTED)/(DELAY_TIME)+(ADJUST))=(LOOPS_NEEDED)) OR (490/(10+4))=(35)
	}						//-- above is old. 35x10=506. So im setting it at 34 which is about 490 (turns out to be 491 :)
	
	SDA_TRIS=1;				//release. My logic analyzer says we stayed low for exatly 488us. Thats pretty damn near perfect.
		
	bpDelayUS(65);			// ADJUSTED. Timing looks great now with the odd numbers.
	if(SDA)					// if lines still high, then no device
		Presence=2;			// if no device then return 2.
		
	for(i=0;i<35;i++) {		//delay for 506us (found above) which is way after the 480 reccomended. thats ok.
		bpDelayUS(10);
	}
	
	if(SDA==0)				// if lines still low; then theres a short
	{
		#if(USE_OWGLOBALS)
			OWShort=1;
		#endif
		return 1;
	}

	#if(USE_OWGLOBALS)
		if(Presence==0)
			OWDevice=1;
	#endif

	return Presence;
}


/* ***********************************************************************************
   Function: OWBit
   Args[0]: uChar [Bit to send. Logic 1 or 0] - or 1 to recieve
   Return: Returns bit value on bus
   
   Desc: OWBit works as both a sending and reciving of 1 bit value on the OWBus.
		 To get a bit value send a logical 1 (OWBit 1) This will pulse the line
		 as needed then release the bus and wait a few US before sampling if the
		 OW device has sent data.
*********************************************************************************** */
unsigned char OWBit(unsigned char OWbit){
    OW_bpResPins();					// I found not including this leaves the lines odd... :/

    SDA_TRIS=0;
    bpDelayUS(4);
	if(OWbit) {
		SDA_TRIS=1;
	}
    bpDelayUS(8);

	if(OWbit) {						// This is where the magic happens. If a OWbit value of 1 is sent to this function
		OWbit = SDA;				// well thats the same timing needed to get a value (not just send it) so why not
    	bpDelayUS(32);				// perform both? So sending one will not only send 1 bit; it will also read one bit
	} else {						// it all depends on what the iDevice is in the mood to do. If its in send mode then
		bpDelayUS(25);				// it will sends its data, if its in recive mode. Then we will send ours.
		SDA_TRIS=1;					//    magical, i know. :)
		bpDelayUS(7);
	}

	bpDelayUS(5); 					//Just an adjust. 70us perfect.

    return OWbit;
}






/* ***********************************************************************************
   Function: OWByte
   Args[0]: uChar [Byte to send to bus] - or 0xFF to recieve
   Return: Returns a byte from the OWBus
   
   Desc: Like OWBit; OWByte works for both sending and getting. OWTime slots are the same
		 for sending and getting; so only one function is needed to perform both tasks.

*********************************************************************************** */
unsigned char OWByte(unsigned char OWbyte){
	unsigned char i, t=0;		// nothing much to say about this function. pretty standard; do this 8 times and collect results.
								// except that sends and GETS data. Sending a value of 0xFF will have the OWBit function return
	for(i=0;i<8;i++)			// bits; this will collect those returns and spit them out. Same with send. It all depends on
	{							// what the iDevice is looking for at the time this command is sent.
		t = OWBit(OWbyte&1);
		OWbyte>>=1;
		if(t) { OWbyte |= 0x80; }
	} 
	bpDelayUS(8);
	return OWbyte;
}






/* ***********************************************************************************
   Function: OWShortOnBus
   Args[0]:
   Return: 1=Yes there is a short (bad) or 0=No short (good)
   
   Desc: Just threw this in for usage in 1wire.c
*********************************************************************************** */
#if(USE_OWGLOBALS)
#if(OW_INCLUDE_BUSSHORT)
unsigned char OWShortOnBus(void)
{
	return OWShort;
}
#endif
#endif






/* BrentBXR [2011] - No License, No Nothing! -Enjoy :) For questions join the Dangerous Prototypes Forums */
#endif

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
//#include "raw3wire.h"
#include "bitbang.h"
#include "procMenu.h"		// for the userinteraction subs

#define R3WMOSI_TRIS 	BP_MOSI_DIR
#define R3WCLK_TRIS 	BP_CLK_DIR
#define R3WMISO_TRIS 	BP_MISO_DIR
#define R3WCS_TRIS 		BP_CS_DIR


#define R3WMOSI 		BP_MOSI
#define R3WCLK 			BP_CLK 
#define R3WMISO 		BP_MISO 
#define R3WCS 			BP_CS 

// should this come from an .h?
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


struct _R3W{
//	unsigned char wwr:1;
	unsigned char csl:1;
} r3wSettings;


unsigned int R3Wread(void)
{	return (bbReadWriteByte(0xff));
}

unsigned int R3Wwrite(unsigned int c)
{	c=bbReadWriteByte(c);
	if(modeConfig.wwr==1)
	{	return c;
	}
	return 0x00;
}

void R3Wstartr(void)
{	modeConfig.wwr=1;
	if(r3wSettings.csl)
	{	bbCS(0);
	}
	else
	{	bbCS(1);
	}
	//bpWmessage(MSG_CS_ENABLED);
	if(r3wSettings.csl) UART1TX('/');
	BPMSG1159;
}
void R3Wstart(void)
{	modeConfig.wwr=0;
	if(r3wSettings.csl)
	{	bbCS(0);
	}
	else
	{	bbCS(1);
	}
	//bpWmessage(MSG_CS_ENABLED);
	if(r3wSettings.csl) UART1TX('/');
	BPMSG1159;
}
void R3Wstop(void)
{	modeConfig.wwr=0;
	if(r3wSettings.csl)
	{	bbCS(1);
	}
	else
	{	bbCS(0);
	}
	//bpWmessage(MSG_CS_DISABLED);
	if(r3wSettings.csl) UART1TX('/');
	BPMSG1160;
}
unsigned int R3Wbitr(void)
{	return (bbReadBit());
}
unsigned int R3Wbitp(void)
{	return (bbMISO());
}
void R3Wclk(void)
{	bbClockTicks(1);
}
void R3Wclkh(void)
{	bbCLK(1);				// same as r2wire?
}
void R3Wclkl(void)
{	bbCLK(0);				// same as r2wire?
}
void R3Wdath(void)
{	bbMOSI(1);				// same as r2wire?
}
void R3Wdatl(void)
{	bbMOSI(0);				// same as r2wire?
}

void R3Wsettings(void)
{	//bpWstring("R3W (spd hiz)=( ");
	BPMSG1161;
	bpWdec(modeConfig.speed); bpSP;
	bpWdec(r3wSettings.csl); bpSP;
	bpWdec(modeConfig.HiZ); bpSP;
	bpWline(")");
}


void R3Wsetup(void)
{	int speed, output, cslow;

	consumewhitechars();
	speed=getint();
	consumewhitechars();
	cslow=getint();
	consumewhitechars();
	output=getint();

	if((speed>0)&&(speed<=4))
	{	modeConfig.speed=speed-1;
	}
	else	
	{	speed=0;					// when speed is 0 we ask the user
	}
	if((cslow>0)&&(cslow<=2))
	{	r3wSettings.csl=(cslow-1);
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
	{	//bpWmessage(MSG_OPT_BB_SPEED);
		BPMSG1065;
		modeConfig.speed=(getnumber(1,1,4,0)-1);

		//bpWline("CS:\r\n 1. CS\r\n 2. /CS *default");
		BPMSG1253;
		r3wSettings.csl=getnumber(2,1,2,0)-1;

		//bpWmessage(MSG_OPT_OUTPUT_TYPE);
		BPMSG1142;
		modeConfig.HiZ=(~(getnumber(1,1,2,0)-1));
		cmderror=0;
	}
	else
	{	R3Wsettings();
	}

	//reset the write with read variable
	modeConfig.wwr=0;
	modeConfig.int16=0; //8 bit 
	
	bbSetup(3, modeConfig.speed); //setup the bitbang library, must be done before calling bbCS below
	//setup pins (pins are input/low when we start)
	//MOSI output, low
	//clock output, low
	//MISO input
	//CS output, high
	R3WMOSI_TRIS=0;
	R3WCLK_TRIS=0;
	R3WMISO_TRIS=1;

	// set cs the way the user wants
	bbCS(r3wSettings.csl);//takes care of custom HiZ settings too
}

void R3Wpins(void)
{	BPMSG1225;
}




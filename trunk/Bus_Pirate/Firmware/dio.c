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
#include "binIO.h"

extern struct _modeConfig modeConfig;
extern struct _command bpCommand;

#ifdef BP_USE_DIO

void DIOsetup(void)
{	//modeConfig.HiZ=1;//yes, always HiZ
	//kbSetup();
}

unsigned int DIOread(void)
{	//kbScancodeResults(kbReadByte());
	//return kbScancode.code;
	return PORTB;
}

unsigned int DIOwrite(unsigned int c)
{
	if((c&0b10000000)==0){
		return binBBpindirectionset(c);
	}else{
		return binBBpinset(c);
	}
	//return 0x100;
}
#endif

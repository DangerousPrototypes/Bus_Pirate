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



 ///////////////////////////
// USE_OWGLOBALS:   Define non-zero to enable OWDevice and OWShort globals.
//          DESC:   I am not sure the point of these; but the other source had them
//					so really im only adding them because of that. I am disabling
//					them as default. 
#define USE_OWGLOBALS 0
////////////////////////////
// OW_INCLUDE_BUSSHORT
//          DESC:	If this is a non-zero value then a 'OWShortOnBus' function is
//					enabled; using which will bring a true/false (1/0) value back
//					if the OWShort global has been set in OWReset or not.
//					NOTE> USE_OW_GLOBALS must also be non-zero to enable this function
#define OW_INCLUDE_BUSSHORT 0


 ///////////////////////////
// SOURCE DEFINES & FUNCTION DEFINES
//          NOTE:	These are not settings. do not edit unless you know what your doing. :)
#define SDA BP_MOSI
#define SDA_TRIS BP_MOSI_DIR
#define OW_bpResPins() SDA_TRIS=1; SDA=0

unsigned char OWReset(void);
unsigned char OWBit(unsigned char c);
unsigned char OWByte(unsigned char OWbyte);

void OWTest(void);

#if(USE_OWGLOBALS)
#if(OW_INCLUDE_BUSSHORT)
unsigned char OWShortOnBus(void);
#endif
#endif

 //////////////////////////
// Back Compatability defines.
//           DESC:	With these defines the new m_1wire_213 files are drop-in compatible
//    				with the original 1wire.c file and the bus pirate software.
#define OWWriteByte(d) OWByte(d)
#define OWReadByte() OWByte(0xFF)
#define OWReadBit() OWBit(1)
#define OWWriteBit(b) OWBit(b)

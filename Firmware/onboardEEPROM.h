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
 

// Addressing the BP eeprom
#define BP_EEPROM_ID 0xA0
#define BP_EEPROM_ID_W 0xA0
#define BP_EEPROM_ID_R 0xA1

void eei2cSetup(void);
unsigned char eeread(void);
void eewrite(unsigned char c);
void eesendack(unsigned char ack);
unsigned char eegetack(void);
void eestop(void);
void eestop(void);
void eestart(void);
unsigned char eetest(void);

unsigned char eeReadByte(unsigned long wAddr);
unsigned int eeWriteByte(char wByte, unsigned long wAddr);

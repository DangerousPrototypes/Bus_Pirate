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

void r2wProcess(void);

//these need to be visible to the MACROs, add as needed.
unsigned char r2wReadByte(void);
void r2wClockTicks(unsigned char c);


void R2Wstart(void);
void R2Wstop(void);
unsigned int R2Wwrite(unsigned int c);
unsigned int R2Wread(void);
void R2Wclkl(void);
void R2Wclkh(void);
void R2Wdath(void);
void R2Wdatl(void);
unsigned int R2Wbitp(void);
void R2Wclk(void);
unsigned int R2Wbitr(void);
void R2Wmacro(unsigned int c);
void R2Wsetup(void);
void R2Wpins(void);
void R2Wsettings(void);


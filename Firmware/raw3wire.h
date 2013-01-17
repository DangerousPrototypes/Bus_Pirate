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

void r3wProcess(void);

unsigned int R3Wread(void);
unsigned int R3Wwrite(unsigned int c);
void R3Wstartr(void);
void R3Wstart(void);
void R3Wstop(void);
unsigned int R3Wbitr(void);
unsigned int R3Wbitp(void);
void R3Wclk(void);
void R3Wclkh(void);
void R3Wclkl(void);
void R3Wdath(void);
void R3Wdatl(void);
void R3Wsetup(void);
void R3Wsetup_exc(void);
void R3Wpins(void);
void R3Wsettings(void);


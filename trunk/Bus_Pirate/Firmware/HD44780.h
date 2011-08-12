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
//void HD44780Process(void);

unsigned int LCDread(void);
unsigned int LCDwrite(unsigned int c);
void LCDstart(void);
void LCDstop(void);
void LCDsetup(void);
void LCDmacro(unsigned int c);
void LCDpins(void);


// to satisfy the compiler when split firmware
#ifndef BP_MAIN
void spiDisable(void);
#endif





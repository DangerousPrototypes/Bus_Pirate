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

void DS1wireProcess(void);
unsigned char PROBE_CHECKSUM (unsigned char *BYTE, int size_of_BYTE);
void bin1WIRE(void);

unsigned int OWread(void);
unsigned int OWwrite(unsigned int c);
unsigned int OWbitr(void);
void OWbitclk(void);
void OWdatl(void);
void OWdath(void);
void OWsetup(void);
void OWmacro(unsigned int macro);
void OWpins(void);
unsigned int OWstate(void);

// from m_1wire123.h anything to make the compiler.linker happy *zucht*
void DS1wireReset(void);





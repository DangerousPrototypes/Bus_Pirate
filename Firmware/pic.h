
/*
 * This file is part of the Bus Pirate project (http://code.google.com/p/the-bus-pirate/).
 *
 * Initial written by Chris van Dongen, 2010.
 *
 * To the extent possible under law, the project has
 * waived all copyright and related or neighboring rights to Bus Pirate.
 *
 * For details see: http://creativecommons.org/publicdomain/zero/1.0/.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

/*

these are lowlevel interface subs for programming pics

supported platforms (should) are pic10, pic12, pic14, pic16 and pic18

{ 0xff } or [ 0xff] sends 4 byte cmd (pic18) or 6 byte cmd (pic10, 12, 14, 16)

*/


#define PICMODE6	6
#define PICMODE4	4

#define PICSPEED	100		// +- 5KHz

#define PICCMD		1
#define PICDATA		0
#define PICMODEMSK	0xFE
#define PICCMDMSK	0x01

void picinit(void);
void picstart(void);
void picstop(void);
unsigned int picread(void);
unsigned int picwrite(unsigned int c);
void piccleanup(void);
void binpic(void);
void picmacro(unsigned int macro);
void picpins(void);



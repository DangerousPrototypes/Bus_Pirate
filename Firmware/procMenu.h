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

#define CMDHISTORY	15
#define BELL	0x07

#define CMDBUFLEN	256
#define CMDLENMSK	(CMDBUFLEN-1)

extern char cmdbuf[CMDBUFLEN];
extern unsigned int cmdend;
extern unsigned int cmdstart;
extern int cmderror;
//extern int currentproto;
//extern proto protos[MAXPROTO];

//prints version, used internally and in main.c
void versionInfo(void); 

void serviceuser(void);


int getint(void);
int getrepeat(void);
int getnumbits(void);
void consumewhitechars(void);
void changemode(void);
int cmdhistory(void);
int getnumber(int def, int min, int max, int x);	


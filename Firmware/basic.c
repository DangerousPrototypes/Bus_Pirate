
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


#include "base.h"
#include "basic.h"
#include "AUXpin.h"
#include "busPirateCore.h"
#include "procMenu.h"
#include "bitbang.h"


extern struct _bpConfig bpConfig;
extern struct _modeConfig modeConfig;
extern struct _command bpCommand;
extern proto protos[MAXPROTO];

#ifdef BP_USE_BASIC

int vars[26];					// var a-z
int stack[GOSUBMAX];				// max 5 gosubs
struct forloop forloops[FORMAX];			// max 2 nested forloop
int pc;							//programcounter
int fors;						// current for
int gosubs;						// current gosubs
int datapos;					// read pointer.


char *tokens[NUMTOKEN+1]=
{	STAT_LET,		//0x80
	STAT_IF,		//0x81
	STAT_THEN,		//0x82
	STAT_ELSE,		//0x83
	STAT_GOTO,		//0x84
	STAT_GOSUB,		//0x85
	STAT_RETURN,	//0x86
	STAT_REM,		//0x87
	STAT_PRINT,		//0x88
	STAT_INPUT,		//0x89
	STAT_FOR,		//0x8A
	STAT_TO,		//0x8b
	STAT_NEXT,		//0x8c
	STAT_READ,		//0x8d
	STAT_DATA,		//0x8e
	STAT_STARTR,	//0x8f
	STAT_START,		//0x90
	STAT_STOPR,		//0x91
	STAT_STOP,		//0x92
	STAT_SEND,		//0x93
	STAT_RECEIVE,	//0x94
	STAT_CLK,		//0x95
	STAT_DAT,		//0x96
	STAT_BITREAD,	//0x97
	STAT_ADC,		//0x98
	STAT_AUXPIN,	//0x99
	STAT_PSU,		//0x9a
	STAT_PULLUP,	//0x9b
	STAT_DELAY,		//0x9c 
	STAT_AUX,		//0x9d
	STAT_FREQ,		//0x9e
	STAT_DUTY,		//0x9f

	STAT_MACRO,		//0xA0
	STAT_END,		//0xa1
};




// insert your favarite basicprogram here:
unsigned char pgmspace[PGMSIZE]; /*={	// 1kb basic memory

// basic basic test :D
#ifdef BASICTEST
TOK_LEN+10, 0, 100, TOK_REM, 'b', 'a', 's', 'i', 'c', 't', 'e', 's', 't',
TOK_LEN+ 7, 0, 110, TOK_LET, 'A', '=', 'C', '+', '1', '6',
TOK_LEN+ 6, 0, 120, TOK_FOR, 'B', '=', '1', TOK_TO, '3',
TOK_LEN+ 6, 0, 125, TOK_FOR, 'D', '=', '0', TOK_TO, '1',
TOK_LEN+23, 0, 130, TOK_PRINT, '\"', 'A', '=', '\"', ';', 'A', ';', '\"', ' ', 'B', '=', '\"', ';', 'B', ';', '\"', ' ', 'D', '=', '\"', ';', 'D', //';',
TOK_LEN+ 2, 0, 135, TOK_NEXT, 'D',
TOK_LEN+ 2, 0, 140, TOK_NEXT, 'B',
TOK_LEN+12, 0, 200, TOK_INPUT, '\"', 'E', 'n', 't', 'e', 'r', ' ', 'C', '\"', ',','C',
TOK_LEN+ 2, 0, 201, TOK_READ, 'C',
TOK_LEN+ 5, 0, 202, TOK_GOSUB, '1', '0', '0', '0',
TOK_LEN+ 2, 0, 203, TOK_READ, 'C',
TOK_LEN+ 5, 0, 204, TOK_GOSUB, '1', '0', '0', '0',
TOK_LEN+ 2, 0, 205, TOK_READ, 'C',
TOK_LEN+ 5, 0, 206, TOK_GOSUB, '1', '0', '0', '0',
TOK_LEN+ 2, 0, 207, TOK_READ, 'C',
TOK_LEN+ 5, 0, 210, TOK_GOSUB, '1', '0', '0', '0',
TOK_LEN+26, 0, 220, TOK_IF, 'C', '=', '2', '0', TOK_THEN, TOK_PRINT, '\"', 'C', '=', '2', '0', '!', '!', '\"', ';', TOK_ELSE, TOK_PRINT, '\"', 'C', '!', '=', '2', '0', '"', ';',
TOK_LEN+ 1, 0, 230, TOK_END,
TOK_LEN+ 7, 3, 232, TOK_PRINT, '\"', 'C', '=', '\"', ';', 'C',
TOK_LEN+ 1, 3, 242, TOK_RETURN,
TOK_LEN+ 6, 7, 208, TOK_DATA, '1', ',', '2', ',', '3',
TOK_LEN+ 3, 7, 218, TOK_DATA, '2', '0',
TOK_LEN+ 1, 255, 255, TOK_END,
#endif


// I2C basic test (24lc02)

#ifdef BASICTEST_I2C
TOK_LEN+18, 0, 100, TOK_REM, 'I', '2', 'C', ' ', 't', 'e', 's', 't', ' ', '(', '2', '4', 'l', 'c', '0', '2', ')',
TOK_LEN+ 2, 0, 110, TOK_PULLUP, '1',
TOK_LEN+ 2, 0, 120, TOK_PSU, '1',
TOK_LEN+ 4, 0, 130, TOK_DELAY, '2', '5', '5',
TOK_LEN+ 1, 0, 140, TOK_STOP,
TOK_LEN+ 5, 0, 150, TOK_GOSUB, '1', '0', '0', '0',
TOK_LEN+ 1, 0, 200, TOK_START,
TOK_LEN+ 4, 0, 210, TOK_SEND, '1', '6', '0', 
TOK_LEN+ 2, 0, 220, TOK_SEND, '0',
TOK_LEN+ 6, 0, 230, TOK_FOR, 'A', '=', '1', TOK_TO, '8',
TOK_LEN+ 2, 0, 240, TOK_READ, 'B',
TOK_LEN+ 2, 0, 250, TOK_SEND, 'B', 
TOK_LEN+ 2, 0, 200, TOK_NEXT, 'A',
TOK_LEN+ 1, 1,   4, TOK_STOP,
TOK_LEN+ 4, 1,  14, TOK_DELAY, '2', '5', '5',
TOK_LEN+ 5, 1,  24, TOK_GOSUB, '1', '0', '0', '0',
TOK_LEN+ 2, 1,  34, TOK_PSU, '0',
TOK_LEN+ 2, 1,  44, TOK_PULLUP, '0',
TOK_LEN+ 1, 1,  54, TOK_END,
TOK_LEN+13, 3, 232, TOK_REM, 'D', 'u', 'm', 'p', ' ', '8', ' ', 'b', 'y', 't', 'e', 's',
TOK_LEN+ 1, 3, 242, TOK_START,
TOK_LEN+ 4, 3, 252, TOK_SEND, '1', '6', '0',
TOK_LEN+ 2, 4,   6, TOK_SEND, '0',
TOK_LEN+ 1, 4,  16, TOK_START,
TOK_LEN+ 4, 4,  26, TOK_SEND, '1', '6', '1',
TOK_LEN+ 7, 4,  36, TOK_PRINT, TOK_RECEIVE, ';', '"', ' ', '"', ';',
TOK_LEN+ 7, 4,  46, TOK_PRINT, TOK_RECEIVE, ';', '"', ' ', '"', ';',
TOK_LEN+ 7, 4,  56, TOK_PRINT, TOK_RECEIVE, ';', '"', ' ', '"', ';',
TOK_LEN+ 7, 4,  66, TOK_PRINT, TOK_RECEIVE, ';', '"', ' ', '"', ';',
TOK_LEN+ 7, 4,  76, TOK_PRINT, TOK_RECEIVE, ';', '"', ' ', '"', ';',
TOK_LEN+ 7, 4,  86, TOK_PRINT, TOK_RECEIVE, ';', '"', ' ', '"', ';',
TOK_LEN+ 7, 4,  96, TOK_PRINT, TOK_RECEIVE, ';', '"', ' ', '"', ';',
TOK_LEN+ 2, 4, 106, TOK_PRINT, TOK_RECEIVE,
TOK_LEN+ 1, 4, 116, TOK_STOP,
TOK_LEN+ 1, 4, 116, TOK_RETURN, 
TOK_LEN+16, 7, 208, TOK_DATA, '2', '5' ,'5', ',', '2', '5' ,'5', ',','2', '5' ,'5', ',','2', '5' ,'5', 
TOK_LEN+16, 7, 218, TOK_DATA, '2', '5' ,'5', ',', '2', '5' ,'5', ',','2', '5' ,'5', ',','2', '5' ,'5',
#endif

// UART test (serial rfid reader from seed)
#ifdef BASICTEST_UART
TOK_LEN+15, 0, 100, TOK_REM, 'U', 'A', 'R', 'T', ' ', 't', 'e', 's', 't', ' ', 'r', 'f', 'i', 'd',
TOK_LEN+ 2, 0, 110, TOK_PSU, '1',
TOK_LEN+ 4, 0, 120, TOK_DELAY, '2', '5', '5',
TOK_LEN+ 5, 0, 130, TOK_GOSUB, '1', '0', '0', '0',
TOK_LEN+ 3, 0, 135, TOK_DELAY, '1', '0', 
//TOK_LEN+12, 0, 140, TOK_IF, TOK_RECEIVE, '!', '=', '5', '2', TOK_THEN, TOK_GOTO, '2', '0', '2', '0', 
//TOK_LEN+ 2, 0, 145, TOK_DELAY, '1', 
//TOK_LEN+12, 0, 150, TOK_IF, TOK_RECEIVE, '!', '=', '5', '4', TOK_THEN, TOK_GOTO, '2', '0', '2', '0', 
//TOK_LEN+ 2, 0, 155, TOK_DELAY, '1',
//TOK_LEN+12, 0, 160, TOK_IF, TOK_RECEIVE, '!', '=', '4', '8', TOK_THEN, TOK_GOTO, '2', '0', '2', '0', 
//TOK_LEN+ 2, 0, 165, TOK_DELAY, '1',
//TOK_LEN+12, 0, 170, TOK_IF, TOK_RECEIVE, '!', '=', '4', '8', TOK_THEN, TOK_GOTO, '2', '0', '2', '0', 
//TOK_LEN+ 2, 0, 175, TOK_DELAY, '1',
//TOK_LEN+12, 0, 180, TOK_IF, TOK_RECEIVE, '!', '=', '5', '4', TOK_THEN, TOK_GOTO, '2', '0', '2', '0', 
//TOK_LEN+ 2, 0, 185, TOK_DELAY, '1',
//TOK_LEN+12, 0, 190, TOK_IF, TOK_RECEIVE, '!', '=', '5', '3', TOK_THEN, TOK_GOTO, '2', '0', '2', '0', 
//TOK_LEN+ 2, 0, 195, TOK_DELAY, '1',
//TOK_LEN+12, 0, 200, TOK_IF, TOK_RECEIVE, '!', '=', '5', '5', TOK_THEN, TOK_GOTO, '2', '0', '2', '0', 
//TOK_LEN+ 2, 0, 205, TOK_DELAY, '1',
//TOK_LEN+12, 0, 210, TOK_IF, TOK_RECEIVE, '!', '=', '5', '5', TOK_THEN, TOK_GOTO, '2', '0', '2', '0', 
//TOK_LEN+ 2, 0, 215, TOK_DELAY, '1',
//TOK_LEN+12, 0, 220, TOK_IF, TOK_RECEIVE, '!', '=', '5', '7', TOK_THEN, TOK_GOTO, '2', '0', '2', '0', 
//TOK_LEN+ 2, 0, 225, TOK_DELAY, '1',
//TOK_LEN+12, 0, 230, TOK_IF, TOK_RECEIVE, '!', '=', '5', '4', TOK_THEN, TOK_GOTO, '2', '0', '2', '0', 
//TOK_LEN+ 2, 0, 235, TOK_DELAY, '1',
//TOK_LEN+12, 0, 240, TOK_IF, TOK_RECEIVE, '!', '=', '6', '7', TOK_THEN, TOK_GOTO, '2', '0', '2', '0', 
//TOK_LEN+ 2, 0, 245, TOK_DELAY, '1',
//TOK_LEN+12, 0, 250, TOK_IF, TOK_RECEIVE, '!', '=', '5', '0', TOK_THEN, TOK_GOTO, '2', '0', '2', '0', 
TOK_LEN+ 6, 0, 140, TOK_PRINT, TOK_RECEIVE, '"', ' ', '"', ';',
//TOK_LEN+ 3, 0, 145, TOK_DELAY, '5', '0', 
TOK_LEN+ 6, 0, 150, TOK_PRINT, TOK_RECEIVE, '"', ' ', '"', ';',
//TOK_LEN+ 3, 0, 155, TOK_DELAY, '5', '0', 
TOK_LEN+ 6, 0, 160, TOK_PRINT, TOK_RECEIVE, '"', ' ', '"', ';',
//TOK_LEN+ 3, 0, 165, TOK_DELAY, '5', '0', 
TOK_LEN+ 6, 0, 170, TOK_PRINT, TOK_RECEIVE, '"', ' ', '"', ';',
//TOK_LEN+ 3, 0, 175, TOK_DELAY, '5', '0', 
TOK_LEN+ 6, 0, 180, TOK_PRINT, TOK_RECEIVE, '"', ' ', '"', ';',
//TOK_LEN+ 3, 0, 185, TOK_DELAY, '5', '0', 
TOK_LEN+ 6, 0, 190, TOK_PRINT, TOK_RECEIVE, '"', ' ', '"', ';',
//TOK_LEN+ 3, 0, 195, TOK_DELAY, '5', '0', 
TOK_LEN+ 6, 0, 200, TOK_PRINT, TOK_RECEIVE, '"', ' ', '"', ';',
//TOK_LEN+ 3, 0, 205, TOK_DELAY, '5', '0', 
TOK_LEN+ 6, 0, 210, TOK_PRINT, TOK_RECEIVE, '"', ' ', '"', ';',
//TOK_LEN+ 3, 0, 215, TOK_DELAY, '5', '0', 
TOK_LEN+ 6, 0, 220, TOK_PRINT, TOK_RECEIVE, '"', ' ', '"', ';',
//TOK_LEN+ 3, 0, 225, TOK_DELAY, '5', '0', 
TOK_LEN+ 6, 0, 230, TOK_PRINT, TOK_RECEIVE, '"', ' ', '"', ';',
//TOK_LEN+ 3, 0, 235, TOK_DELAY, '5', '0', 
TOK_LEN+ 6, 0, 240, TOK_PRINT, TOK_RECEIVE, '"', ' ', '"', ';',
//TOK_LEN+ 3, 0, 245, TOK_DELAY, '5', '0', 
TOK_LEN+ 6, 0, 250, TOK_PRINT, TOK_RECEIVE, '"', ' ', '"', ';',
//TOK_LEN+ 5, 1,   4, TOK_GOTO, '2', '0', '0', '0', 
TOK_LEN+ 4, 1,  14, TOK_GOTO, '1', '3', '0',
TOK_LEN+13, 3, 232, TOK_REM, 'W', 'a', 'i', 't', ' ', 'f', 'o', 'r', ' ', 'S', 'T', 'X',
TOK_LEN+ 4, 3, 242, TOK_LET, 'A', '=', TOK_RECEIVE,
TOK_LEN+12, 3, 252, TOK_IF, 'A', '=', '2', TOK_THEN, TOK_RETURN, TOK_ELSE, TOK_GOTO, '1', '0', '1', '0',
//TOK_LEN+ 8, 7, 208, TOK_PRINT, '"', 'V', 'A', 'L', 'I', 'D', '"', 
//TOK_LEN+ 4, 7, 218, TOK_GOTO, '1', '3', '0',
//TOK_LEN+10, 7, 228, TOK_PRINT, '"', 'I', 'N', 'V', 'A', 'L', 'I', 'D', '"', 
//TOK_LEN+ 4, 7, 238, TOK_GOTO, '1', '3', '0',
#endif

// raw3wire test (atiny85)
#ifdef BASICTEST_R3W
TOK_LEN+22, 0, 10, TOK_REM, 'r', '2', 'w', 'i', 'r', 'e', ' ', 't', 'e', 's', 't', ' ', '(', 'a', 't', 'i', 'n', 'y', '8', '5', ')',
TOK_LEN+ 2, 0, 100, TOK_PULLUP, '1',
TOK_LEN+ 2, 0, 110, TOK_CLK, '0',
TOK_LEN+ 2, 0, 120, TOK_DAT, '0',
TOK_LEN+ 2, 0, 130, TOK_AUX, '0',
TOK_LEN+ 2, 0, 140, TOK_PSU, '1',
TOK_LEN+ 4, 0, 150, TOK_DELAY, '2', '5', '5', 
TOK_LEN+ 1, 0, 160, TOK_STARTR,
TOK_LEN+ 7, 0, 170, TOK_LET, 'A', '=', TOK_SEND, '1', '7', '2',
TOK_LEN+ 6, 0, 180, TOK_LET, 'B', '=', TOK_SEND, '8', '3',
TOK_LEN+ 5, 0, 190, TOK_LET, 'C', '=', TOK_SEND, '0',
TOK_LEN+ 5, 0, 200, TOK_LET, 'D', '=', TOK_SEND, '0',
TOK_LEN+ 8, 0, 210, TOK_IF, 'C', '!', '=', '8', '3', TOK_THEN, TOK_END,
TOK_LEN+15, 0, 220, TOK_PRINT, '"', 'F', 'O', 'U', 'N', 'D', ' ', 'D', 'E', 'V', 'I', 'C', 'E', '"',
TOK_LEN+13, 0, 230, TOK_PRINT, '"', 'd', 'e', 'v', 'i', 'c', 'e', 'I', 'D', ':', '"', ';',
TOK_LEN+ 6, 0, 240, TOK_LET, 'A', '=', TOK_SEND, '4', '8', 
TOK_LEN+ 5, 0, 250, TOK_LET, 'B', '=', TOK_SEND, '0',
TOK_LEN+ 5, 1,   4, TOK_LET, 'C', '=', TOK_SEND, '0',
TOK_LEN+ 5, 1,  14, TOK_LET, 'D', '=', TOK_SEND, '0',
TOK_LEN+ 7, 1,  24, TOK_PRINT, 'D', ';', '"', ' ', '"', ';', 
TOK_LEN+ 6, 1,  34, TOK_LET, 'A', '=', TOK_SEND, '4', '8', 
TOK_LEN+ 5, 1,  44, TOK_LET, 'B', '=', TOK_SEND, '0',
TOK_LEN+ 5, 1,  54, TOK_LET, 'C', '=', TOK_SEND, '1',
TOK_LEN+ 5, 1,  64, TOK_LET, 'D', '=', TOK_SEND, '0',
TOK_LEN+ 7, 1,  74, TOK_PRINT, 'D', ';', '"', ' ', '"', ';', 
TOK_LEN+ 6, 1,  84, TOK_LET, 'A', '=', TOK_SEND, '4', '8', 
TOK_LEN+ 5, 1,  94, TOK_LET, 'B', '=', TOK_SEND, '0',
TOK_LEN+ 5, 1, 104, TOK_LET, 'C', '=', TOK_SEND, '2',
TOK_LEN+ 5, 1, 114, TOK_LET, 'D', '=', TOK_SEND, '0',
TOK_LEN+ 2, 1, 124, TOK_PRINT, 'D',
TOK_LEN+14, 1, 134, TOK_PRINT, '"', 'd', 'e', 'v', 'i', 'c', 'e', ' ', 'i', 's', ' ', '"', ';',
TOK_LEN+ 6, 1,  84, TOK_LET, 'A', '=', TOK_SEND, '8', '8', 
TOK_LEN+ 5, 1,  94, TOK_LET, 'B', '=', TOK_SEND, '0',
TOK_LEN+ 5, 1, 104, TOK_LET, 'C', '=', TOK_SEND, '2',
TOK_LEN+ 5, 1, 114, TOK_LET, 'D', '=', TOK_SEND, '0',
TOK_LEN+26, 1, 124, TOK_IF, 'D', '=', '3', TOK_THEN, TOK_PRINT, '"', 'U', 'N', 'L', 'O', 'C', 'K', 'E', 'D', '"', TOK_ELSE, TOK_PRINT, '"', 'L', 'O', 'C', 'K', 'E', 'D','"',
TOK_LEN+ 1, 1, 134, TOK_END,
#endif

#ifdef BASICTEST_PIC10

TOK_LEN+15, 0, 100, TOK_REM, 'P', 'R', 'O', 'G', 'R', 'A', 'M', ' ', 'P', 'I', 'C', '1', '0', 'F',
TOK_LEN+ 7, 0, 110, TOK_FOR, 'A', '=', '1', TOK_TO, '2', '4',
TOK_LEN+ 1, 0, 120, TOK_START,
TOK_LEN+ 2, 0, 130, TOK_SEND, '2',
TOK_LEN+ 1, 0, 140, TOK_STOP,
TOK_LEN+ 2, 0, 150, TOK_READ, 'B',
TOK_LEN+ 2, 0, 160, TOK_SEND, 'B',
TOK_LEN+ 1, 0, 170, TOK_START,
TOK_LEN+ 2, 0, 180, TOK_SEND, '8', 
TOK_LEN+ 2, 0, 190, TOK_DELAY, '2',
TOK_LEN+ 3, 0, 200, TOK_SEND, '1', '4',
TOK_LEN+ 2, 0, 210, TOK_SEND, '6',
TOK_LEN+ 1, 0, 210, TOK_STOP,
TOK_LEN+ 2, 0, 220, TOK_NEXT, 'A',
TOK_LEN+ 1, 0, 230, TOK_START,
TOK_LEN+ 9, 0, 240, TOK_FOR, 'A', '=', '2', '5', TOK_TO, '5', '1', '2', 
TOK_LEN+ 2, 0, 250, TOK_SEND, '6',
TOK_LEN+ 2, 1,   4, TOK_NEXT, 'A',
TOK_LEN+ 2, 1,  14, TOK_SEND, '2',
TOK_LEN+ 1, 1,  24, TOK_STOP,
TOK_LEN+ 2, 1,  34, TOK_READ, 'B',
TOK_LEN+ 2, 1,  44, TOK_SEND, 'B',
TOK_LEN+ 1, 1,  54, TOK_START,
TOK_LEN+ 2, 1,  64, TOK_SEND, '8',
TOK_LEN+ 2, 1,  74, TOK_DELAY, '2',
TOK_LEN+ 3, 1,  84, TOK_SEND, '1', '4',
TOK_LEN+ 1, 1,  94, TOK_STOP,
TOK_LEN+ 1, 1, 104, TOK_END,
//TOK_LEN+11, 3, 232, TOK_REM, 'C', 'O', 'N', 'F', 'I', 'G', 'W', 'O', 'R', 'D',
TOK_LEN+ 5, 3, 232, TOK_DATA, '4', '0', '7', '5',
//TOK_LEN+ 5, 7, 208, TOK_REM, 'M', 'A', 'I', 'N',
TOK_LEN+26, 7, 208, TOK_DATA, '3', '7', ',', '1', '0', '2', '9', ',', '2', '5', '7', '3', ',', '3', '3', '2', '2', ',', '4', '9', ',', '3', '3', '2', '7',
TOK_LEN+26, 7, 218, TOK_DATA, '5', '0', ',', '7', '5', '3', ',', '2', '5', '7', '0', ',', '2', '0', '4', '8', ',', '7', '5', '4', ',', '2', '5', '7', '0',
TOK_LEN+25, 7, 228, TOK_DATA, '2', '5', '6', '7', ',', '3', '2', '6', '4', ',', '2', ',', '3', '0', '7', '2', ',', '3', '8', ',', '3', '3', '2', '3',
TOK_LEN+21, 7, 238, TOK_DATA, '6', ',', '3', '0', '7', '6', ',', '4', '2', '2', ',', '2', '3', '0', '7', ',', '2', '5', '7', '9',
//TOK_LEN+ 7,11, 184, TOK_REM, 'O', 'S', 'C', 'C', 'A', 'L',
TOK_LEN+ 5,11, 184, TOK_DATA, '3', '0', '9', '6', 

// data statements and rems aren't mixing well!
#endif
#ifdef BASICTEST_PIC10_2

TOK_LEN+12, 0, 100, TOK_REM, 'D', 'U', 'M', 'P', ' ', 'P', 'I', 'C', '1', '0', 'F',
TOK_LEN+ 8, 0, 110, TOK_FOR, 'A', '=', '1', TOK_TO, '5', '1', '8',
TOK_LEN+ 1, 0, 120, TOK_START,
TOK_LEN+ 2, 0, 130, TOK_SEND, '4',
TOK_LEN+ 1, 0, 140, TOK_STOP, 
TOK_LEN+ 7, 0, 150, TOK_PRINT, TOK_RECEIVE, ';', '"', ',', '"', ';', 
TOK_LEN+ 1, 0, 160, TOK_START, 
TOK_LEN+ 2, 0, 170, TOK_SEND, '6',
TOK_LEN+ 2, 0, 180, TOK_NEXT, 'A', 
TOK_LEN+ 1, 0, 190, TOK_END, 
#endif


0x00,0x00,
};
*/



// basicinterpreter starts here

void handleelse(void)
{	if(pgmspace[pc]==TOK_ELSE)
	{	pc++;
		while(pgmspace[pc]<=TOK_LEN)
		{	pc++;
		}
	}
}

int searchlineno(unsigned int line)
{	int i;
	int len;
	int lineno;

	i=0;

	//bpWintdec(line);
	//UART1TX('?');

	while(1)
	{	if(pgmspace[i]<=TOK_LEN)
		{	return -1;
		}
		len=pgmspace[i]-TOK_LEN;
		lineno=(pgmspace[i+1]<<8)+pgmspace[i+2];
//		if(i>PGMSIZE)
//		{	return -1;
//		}
		if(line==lineno)
		{	return i;
		}
		i+=len+3;
		//bpWintdec(i); bpSP;
		//bpWintdec(lineno); bpSP;
	}
	return -1;
}

int getnumvar(void)
{	int temp;
	temp=0;

	if((pgmspace[pc]=='('))
    {
		pc++;
		temp=assign();
//        temp=evaluate();
		if((pgmspace[pc]==')'))
		{	pc++;
		}
	}
	else if((pgmspace[pc]>='A')&&(pgmspace[pc]<='Z'))
	{	//bpWstring("var ");
		//bpWhex(pgmspace[pc]);
		//bpSP;
		return vars[pgmspace[pc++]-'A'];				//increment pc
	}
	else if(pgmspace[pc]>TOKENS)			// looks for tokens like aux, clk and dat
	{	switch(pgmspace[pc++])					// increment pc
		{	case TOK_RECEIVE:	//temp=protoread();
							temp=protos[bpConfig.busMode].protocol_read();
							break;
			case TOK_SEND:	//temp=protoread();
							temp=protos[bpConfig.busMode].protocol_send(assign());
							break;
			case TOK_AUX:	//temp=protogetaux();
							temp=bpAuxRead();
							break;
			case TOK_DAT:	//temp=protogetdat();
							temp=protos[bpConfig.busMode].protocol_dats();
							break;
			case TOK_BITREAD:	//temp=protobitr();
							temp=protos[bpConfig.busMode].protocol_bitr();
							break;
			case TOK_PSU:	temp=BP_VREGEN; //modeConfig.vregEN;
							break;
#ifndef BUSPIRATEV1A
			case TOK_PULLUP:	temp=(~BP_PULLUP); //modeConfig.pullupEN;
							break;
#endif
			case TOK_ADC:	//temp=bpADC(BP_ADC_PROBE);
							ADCON(); // turn ADC ON
							temp=bpADC(BP_ADC_PROBE);
							ADCOFF(); // turn ADC OFF

//							temp=1234;
							break;
			default:		temp=0;
		}
	}
	else
	{	while((pgmspace[pc]>='0')&&(pgmspace[pc]<='9'))
		{	temp*=10;
			temp+=pgmspace[pc]-'0';
			pc++;
		}
	}
	//bpWstring("int ");
	//bpWinthex(temp);
	//bpSP;
	return temp;
}

int getmultdiv(void)
{	int temp;
	temp=getnumvar();
    while (1) {
		if((pgmspace[pc]!='*')&&(pgmspace[pc]!='/')&&(pgmspace[pc]!='&')&&(pgmspace[pc]!='|'))
		{	return temp;
		}
		else									// assume operand
		{	//bpWstring("op ");
			//UART1TX(pgmspace[pc]);
			//bpSP;
			switch(pgmspace[pc++])
			{	case '*': 	//UART1TX('*');
							temp*=getnumvar();
							break;
				case '/':	//UART1TX('/');
							temp/=getnumvar();
							break;
				case '&':	//UART1TX('/');
							temp&=getnumvar();
							break;
				case '|':	//UART1TX('/');
							temp|=getnumvar();
							break;
				default:	break;
			}
		}
	}


}

int assign(void)
{	unsigned int temp;
	
	//pc+=2;
	temp=getmultdiv();

    while (1) {
		if((pgmspace[pc]!='-')&&(pgmspace[pc]!='+')&&(pgmspace[pc]!='<')&&(pgmspace[pc]!='>')&&(pgmspace[pc]!='='))
		{	return temp;
		}
		else									// assume operand
		{	//bpWstring("op ");
			//UART1TX(pgmspace[pc]);
			//bpSP;
			switch(pgmspace[pc++])
			{	case '-': 	//UART1TX('-');
							temp-=getmultdiv();
							break;
				case '+':	//UART1TX('+');
							temp+=getmultdiv();
							break;
				case '>':	//UART1TX('+');
							if(pgmspace[pc+1]=='=')
							{	temp=(temp>=getmultdiv()?1:0);
								pc++;
							}
							else
							{	temp=(temp>getmultdiv()?1:0);
							}
							break;
				case '<':	//UART1TX('+');
							if(pgmspace[pc+1]=='>')
							{	temp=(temp!=getmultdiv()?1:0);
								pc++;
							}
							else if(pgmspace[pc+1]=='=')
							{	temp=(temp<=getmultdiv()?1:0);
								pc++;
							}
							else
							{	temp=(temp<getmultdiv()?1:0);
							}
							break;
				case '=':	//UART1TX('+');
							temp=(temp==getmultdiv()?1:0);
				default:	break;
			}
		}
	}
}

/*
int evaluate(void)
{	int left;
	int right;
	int i;

	char op[2];		// operand

	left=vars[pgmspace[pc++]-'A'];

	i=0;

	while((!((pgmspace[pc]>='A')&&(pgmspace[pc]<='Z')))&&(!((pgmspace[pc]>='0')&&(pgmspace[pc]<='9'))))
	{	op[i]=pgmspace[pc++];
		i++;
	}

	right=assign();

	//bpSP; 
	//bpWinthex(left); bpSP;
	//UART1TX(op[0]); 
	//UART1TX(op[1]); bpSP; 
	//bpWinthex(right); bpSP; 

	
	switch (op[0])
	{	case '=':	return (left==right);
		case '>':	if(op[1]=='=') 
					{	return (left>=right);
					}
					else
					{	return (left>right);
					}
		case '<':	if(op[1]=='=') 
					{	return (left<=right);
					}
					else
					{	return (left<right);
					}
		case '!':	if(op[1]=='=')	
					{	return (left!=right);
					}
		default:	return 0;				
	}
	return 0;
}
*/

void interpreter(void)
{	int len;
	unsigned int lineno;
	int i;
//	int pc; 			//became global
	int stop;
	int pcupdated;
	int ifstat;

	int temp;

	// init
	pc=0;
	stop=0;
	pcupdated=0;
	ifstat=0;
	fors=0;
	gosubs=0;
	datapos=0;
	lineno=0;
	bpConfig.quiet=1;				// turn echoing off

	for(i=0; i<26; i++) vars[i]=0;

	while(!stop)
	{	if(!ifstat)
		{	if(pgmspace[pc]<TOK_LEN)
			{	stop=NOLEN;
				break;
			}

			len=pgmspace[pc]-TOK_LEN;
			lineno=((pgmspace[pc+1]<<8)|pgmspace[pc+2]);

	/*		bpBR;
			bpWintdec(pc);
			bpSP;
			bpWintdec(lineno);
			bpSP;
			bpWdec(len);
			bpSP; */
		}

		ifstat=0;

		switch(pgmspace[pc+3])					// first token
		{	case TOK_LET:	//bpWstring(STAT_LET);
							//bpSP;
							//for(i=4; i<len+3; i++)
							//{	UART1TX(pgmspace[pc+i]);
							//}

							pcupdated=1;
							pc+=6;		// 4(len lineno lineno token) +2 ("A=")

							vars[pgmspace[pc-2]-0x41]=assign();
							handleelse();
							
							//bpBR;
							break;
			case TOK_IF:	//bpWstring(STAT_IF);
							//bpSP;
							//i=4;
							//while(pgmspace[pc+i]<0x80)
							//{	UART1TX(pgmspace[pc+i]);		// print if condition
							//	i++;
							//}
							//bpSP;
							//if(pgmspace[pc+i]==TOK_THEN)
							//{	bpWstring(STAT_THEN);
							//	bpSP;
							//	i++;
							//	while((pgmspace[pc+i]!=TOK_ELSE)&&(i<len))
							//	{	UART1TX(pgmspace[pc+i]);
							//		i++;
							//	}
							//}
							//if(pgmspace[pc+i]==TOK_ELSE)
							//{	bpWstring(STAT_ELSE);
							//	bpSP;
							//	i++;
							//	for( ; i<len+3; i++)
							//	{	UART1TX(pgmspace[pc+i]);
							//	} 
							//}

							pcupdated=1;
							pc+=4;
		
							//temp=pc;
		
							//if(evaluate())
							if(assign())
							{	//bpWline("TRUE");	// execute statement then
								if(pgmspace[pc++]==TOK_THEN)
								{	//bpWstring ("THEN");
									ifstat=1;
									pc-=3;					// simplest way (for now)
								}
							}
							else
							{	//bpWline("FALSE");
								while((pgmspace[pc]!=TOK_ELSE)&&(pgmspace[pc]<=TOK_LEN))
								{	pc++;
								}
								if(pgmspace[pc]==TOK_ELSE)
								{	//bpWstring ("ELSE");
									ifstat=1;
									pc-=2;					// simplest way (for now)
								}
							}
	
							//bpWstring(" endif ");
							break;
			case TOK_GOTO:	//bpWstring(STAT_GOTO);
							//bpSP;
							//for(i=4; i<len+3; i++)
							//{	UART1TX(pgmspace[pc+i]);
							//} 

							pcupdated=1;
							pc+=4;
	
							//bpConfig.quiet=0;
							temp=searchlineno(assign());
							//bpSP; bpWintdec(temp); bpSP;
							if(temp!=-1)
							{	pc=temp;
							}
							else
							{	stop=GOTOERROR;
							}
							//bpConfig.quiet=1;

							break;
			case TOK_GOSUB:	//bpWstring(STAT_GOSUB);
							//bpSP;
							//for(i=4; i<len+3; i++)
							//{	UART1TX(pgmspace[pc+i]);
							//} 

							pcupdated=1;
							pc+=4;
	
							if(gosubs<GOSUBMAX)
							{	stack[gosubs]=pc+len-1;
								gosubs++;
								temp=searchlineno(assign());
								//bpSP; bpWinthex(temp); bpSP;
								if(temp!=-1)
								{	pc=temp;
								}
								else
								{	stop=GOTOERROR;
								}
							}
							else
							{	stop=STACKERROR;
							}
							//bpBR;

							break;
			case TOK_RETURN:	bpWstring(STAT_RETURN);
						
							pcupdated=1;
							if(gosubs)
							{	pc=stack[--gosubs];
							}
							else
							{	stop=RETURNERROR; 
							}
							//bpBR
							break;
			case TOK_REM:	//bpWstring(STAT_REM);
							//bpSP;
							//for(i=4; i<len+3; i++)
							//{	UART1TX(pgmspace[pc+i]);
							//} 
							//bpBR;

							pcupdated=1;
							pc+=len+3;
							break;
			case TOK_PRINT:	//bpWstring(STAT_PRINT);
							//bpSP;
							//for(i=4; i<len+3; i++)
							//{	UART1TX(pgmspace[pc+i]);
							//} 

							pcupdated=1;
							pc+=4;
							while((pgmspace[pc]<TOK_LEN)&&(pgmspace[pc]!=TOK_ELSE))
							{	if(pgmspace[pc]=='\"')						// is it a string?
								{	pc++;
									while(pgmspace[pc]!='\"')
									{	bpConfig.quiet=0;
										UART1TX(pgmspace[pc++]);
										bpConfig.quiet=1;
									}
									pc++;
								}
								else if(((pgmspace[pc]>='A')&&(pgmspace[pc]<='Z'))||((pgmspace[pc]>=TOKENS)&&(pgmspace[pc]<TOK_LEN)))
								{	temp=assign();
									bpConfig.quiet=0;
									bpWintdec(temp);
									bpConfig.quiet=1;
								}
								else if(pgmspace[pc]==';')						// spacer
								{	pc++;
								}
							}
							if(pgmspace[pc-1]!=';')
							{	bpConfig.quiet=0;
								bpBR;
								bpConfig.quiet=1;
							}
							handleelse();
							break;
			case TOK_INPUT:	//bpWstring(STAT_INPUT);
							//bpSP;
							//for(i=4; i<len+3; i++)
							//{	UART1TX(pgmspace[pc+i]);
							//} 

							pcupdated=1;
							bpConfig.quiet=0;		// print prompt
							pc+=4;
	
							if(pgmspace[pc]=='\"')						// is it a string?
							{	pc++;
								while(pgmspace[pc]!='\"')
								{	UART1TX(pgmspace[pc++]);
								}
								pc++;
							}
							if(pgmspace[pc]==',')
							{	pc++;
							}
							else
							{	stop=SYNTAXERROR;
							}
	
							vars[pgmspace[pc]-'A']=getnumber(0, 0, 0x7FFF, 0);
							pc++;
							handleelse();
							bpConfig.quiet=1;
							break;
			case TOK_FOR:	//bpWstring(STAT_FOR);
							//bpSP;
							//i=0;
							//while(pgmspace[pc+4+i]<0x80)
							//{	UART1TX(pgmspace[pc+4+i]);		// print for condition
							//	i++;
							//}
							//bpSP;
							//if(pgmspace[pc+4+i]==TOK_TO)
							//{	bpWstring(STAT_TO);
							//	bpSP;
							//}
							//i++;
							//while(len>(1+i))
							//{	UART1TX(pgmspace[pc+4+i]);		// print to
							//	i++;
							//}

							pcupdated=1;
							pc+=4;
							if(fors<FORMAX)
							{	fors++;	
							}
							else
							{	stop=FORERROR;				// to many nested fors
							}
							forloops[fors].var=(pgmspace[pc++]-'A')+1;
	
							if(pgmspace[pc]=='=')
							{	pc++;
								vars[(forloops[fors].var)-1]=assign();
							}
							else
							{	stop=SYNTAXERROR;
							}
							if(pgmspace[pc++]==TOK_TO)
							{	//bpWstring(STAT_TO);
								forloops[fors].to=assign();
							}
							else
							{	stop=SYNTAXERROR;
							}
							if(pgmspace[pc]>=TOK_LEN)
							{	forloops[fors].forstart=pc;
							}
							else
							{	stop=SYNTAXERROR;
							}
							//bpSP;
							//bpWinthex(forloops[fors].var); bpSP;
							//bpWinthex(forloops[fors].to); bpSP;
							//bpWinthex(forloops[fors].forstart); bpSP;
							//bpBR;

							handleelse();

							break;
			case TOK_NEXT:	//bpWstring(STAT_NEXT);
							//bpSP;
							//for(i=4; i<len+3; i++)
							//{	UART1TX(pgmspace[pc+i]);
							//}

							pcupdated=1;
							pc+=4;
	
							temp=(pgmspace[pc++]-'A')+1;
							stop=NEXTERROR;
	
							for(i=0; i<=fors; i++)
							{	if(forloops[i].var==temp)
								{	if(vars[temp-1]<forloops[i].to)
									{	vars[temp-1]++;
										pc=forloops[i].forstart;
									}
									else
									{	fors--;
									}
									stop=0;
								}
							}
							handleelse();
							break;
			case TOK_READ:	pcupdated=1;
							pc+=4;

							if(datapos==0)
							{	i=0;
								while((pgmspace[i+3]!=TOK_DATA)&&(pgmspace[i]!=0x00))
								{	i+=(pgmspace[i]-TOK_LEN)+3;
									//bpWinthex(i); bpSP;
								}
								//bpWinthex(pgmspace[i]);
								if(pgmspace[i])
								{	datapos=i+4;
								}
								else
								{	stop=DATAERROR;
								}
							}
							//bpWstring("datapos ");
							//bpWinthex(datapos); bpSP; 
							//UART1TX(pgmspace[datapos]); bpSP;
							temp=pc;
							pc=datapos;
							vars[pgmspace[temp]-'A']=getnumvar();
							datapos=pc;
							pc=temp+1;
	
							if(pgmspace[datapos]==',')
							{	datapos++;
							}
							if(pgmspace[datapos]>TOK_LEN)
							{	if(pgmspace[datapos+3]!=TOK_DATA)
								{	datapos=0;							// rolover
								}
								else
								{	datapos+=4;
								}
							}						
	
							handleelse();

							break;
			case TOK_DATA:	pcupdated=1;
							pc+=len+3;
							break;


// buspirate subs

			case TOK_START:	pcupdated=1;
							pc+=4;
		
							protos[bpConfig.busMode].protocol_start();
							handleelse();
							//protostart();
							break;
			case TOK_STARTR:	pcupdated=1;
							pc+=4;
		
							protos[bpConfig.busMode].protocol_startR();
							//protostartr();
							handleelse();
							break;
			case TOK_STOP:	pcupdated=1;
							pc+=4;
		
							protos[bpConfig.busMode].protocol_stop();
							//protostop();
							handleelse();
							break;
			case TOK_STOPR:	pcupdated=1;
							pc+=4;
		
							protos[bpConfig.busMode].protocol_stopR();
							//protostopr();
							handleelse();
							break;
			case TOK_SEND:	pcupdated=1;
							pc+=4;
							//protowrite(getnumvar());
							protos[bpConfig.busMode].protocol_send((int)assign());
							handleelse();
							break;
			case TOK_AUX:	pcupdated=1;
							pc+=4;

							if(assign())
							{	//protoauxh();
								bpAuxHigh();
							}
							else
							{	//protoauxl();
								bpAuxLow();
							}
							handleelse();
							
							break;
			case TOK_PSU:	pcupdated=1;
							pc+=4;

							if(assign())
							{	//protopsuon();
								BP_VREG_ON();
								//modeConfig.vregEN=1;
							}
							else
							{	//protopsuoff();
								BP_VREG_OFF();
								//modeConfig.vregEN=0;
							}
							handleelse();
						
							break;

			case TOK_AUXPIN:	pcupdated=1;
							pc+=4;

							if(assign())
							{	modeConfig.altAUX=1;
							}
							else
							{	modeConfig.altAUX=1;
							}
							handleelse();
						
							break;
			case TOK_FREQ:	pcupdated=1;
							pc+=4;

							PWMfreq=assign();
							if(PWMfreq<0)	PWMfreq=0;
							if(PWMfreq>4000)	PWMfreq=4000;
							if(PWMduty<2)	PWMduty=2;
							if(PWMduty>99)	PWMduty=99;

							updatePWM();
							handleelse();

							break;
			case TOK_DUTY:	pcupdated=1;
							pc+=4;

							PWMduty=assign();
							if(PWMfreq<0)	PWMfreq=0;
							if(PWMfreq>4000)	PWMfreq=4000;
							if(PWMduty<2)	PWMduty=2;
							if(PWMduty>99)	PWMduty=99;

							updatePWM();
							handleelse();
						
							break;


			case TOK_DAT:	pcupdated=1;
							pc+=4;

							if(assign())
							{	//protodath();
								protos[bpConfig.busMode].protocol_dath();
							}
							else
							{	//protodatl();
								protos[bpConfig.busMode].protocol_datl();
							}
							handleelse();
						
							break;
			case TOK_CLK:	pcupdated=1;
							pc+=4;

							switch(assign())
							{	case 0:	//protoclkl();
										protos[bpConfig.busMode].protocol_clkl();
										break;
								case 1:	//protoclkh();
										protos[bpConfig.busMode].protocol_clkh();
										break;
								case 2: //protoclk();
										protos[bpConfig.busMode].protocol_clk();
										break;
							}
							handleelse();
						
							break;
			case TOK_PULLUP:	pcupdated=1;
							pc+=4;

#ifndef BUSPIRATEV1A
//#if(0)
							if(assign())
							{	//protopullupon();
								BP_PULLUP_ON(); 
								//modeConfig.pullupEN=1;
							}
							else
							{	//protopullupoff();
								BP_PULLUP_OFF();
								//modeConfig.pullupEN=0;
							}
#else
							pc+=len-1; 	// fail silently
#endif							
							handleelse();

							break;
			case TOK_DELAY:	pcupdated=1;
							pc+=4;
							temp=assign();
							bpDelayMS(temp);
							handleelse();

							break;
			case TOK_MACRO:	pcupdated=1;
							pc+=4;
							temp=assign();
							protos[bpConfig.busMode].protocol_macro(temp);
							handleelse();
							break;
			case TOK_END:	//bpWstring(STAT_END);
							stop=1; 
							break;
			default:		stop=SYNTAXERROR;
							break;	
		}

		if(!pcupdated)
		{	pc+=len+3;
			//bpBR;
		}
		pcupdated=0;
	}

	bpConfig.quiet=0; 		// display on 

	if(stop!=NOERROR)
	{	//bpWstring("Error(");
		BPMSG1047;
		bpWintdec(stop);
		//bpWstring(") @line:");
		BPMSG1048;
		bpWintdec(lineno);
		//bpWstring(" @pgmspace:");
		BPMSG1049;
		bpWintdec(pc);
		bpBR;
	}


	//bpWinthex(vars[0]); bpSP;
	//bpWinthex(vars[1]); bpSP;
	//bpWinthex(vars[2]); bpSP;
}

void printstat(char *s)
{	bpSP;
	bpWstring(s);
	bpSP;
}

void list(void)
{	unsigned char c;
	unsigned int lineno;

	pc=0;

	while(pgmspace[pc])
	{	c=pgmspace[pc];
		if(c<TOK_LET)
		{	UART1TX(c);
		}
		else if(c>TOK_LEN)
		{	bpBR;
			//bpWintdec(pc); bpSP;
			lineno=(pgmspace[pc+1]<<8)+pgmspace[pc+2];
			pc+=2;
			bpWintdec(lineno);
			bpSP;
		}
		else
		{	bpSP;
			bpWstring(tokens[c-TOKENS]);
			bpSP;
		}
		pc++;
	}
	bpBR;
	bpWintdec(pc-1);
	//bpWline(" bytes.");
	BPMSG1050;
}

int compare(char *p)
{	int oldstart;
	
	oldstart=cmdstart;
	while(*p)
	{	if(*p!=cmdbuf[cmdstart])
		{	cmdstart=oldstart;
			return 0;
		}
		cmdstart=(cmdstart+1)&CMDLENMSK;

		p++;
	}
	return 1;
}  

unsigned char gettoken(void)
{	int i;

	for(i=0; i<NUMTOKEN; i++)
	{	if(compare(tokens[i]))
		{	return TOKENS+i;
		}
	}
	return 0;
}

void basiccmdline(void)
{	int i, j, temp;
	int pos, end, len, string;
	unsigned char line[35];

	unsigned int lineno1, lineno2;


	//convert to everyhting to uppercase
	for(i=cmdstart; i!=cmdend; )
	{	if((cmdbuf[i]>='a')&&(cmdbuf[i]<='z')) cmdbuf[i]&=0xDF;
		i++;
		i&=CMDLENMSK;
	}

	i=0;
	// command or a new line?
	if((cmdbuf[cmdstart]>='0')&&(cmdbuf[cmdstart]<='9'))
	{	//bpWline("new line");

		for(i=0; i<35; i++)
		{	line[i]=0;
		}

		temp=getint();
		line[1]=temp>>8;
		line[2]=temp&0xFF;

		//bpWstring("search for line ");
		//bpWintdec(temp); bpBR;

		pos=searchlineno(temp);
		//bpWstring("pos=");
		//bpWintdec(pos); bpSP;
		if(pos!=-1)							// if it already exist remove it first
		{	//bpWstring("replace/remove line @");
			//bpWintdec(pos); bpBR
			len=(pgmspace[pos]-TOK_LEN)+3;
			//bpWstring("pos=");
			//bpWintdec(pos); bpSP; 
			for(i=pos; i<PGMSIZE-len; i++)
			{	pgmspace[i]=pgmspace[i+len];	// move everyhting from pos len bytes down
			}
			//bpWstring("i=");
			//bpWintdec(i); bpBR;
			for( ; i<PGMSIZE; i++)
			{	pgmspace[i]=0x00;
			}
		}

		i=3;
		string=0;

		consumewhitechars();
		while(cmdstart!=cmdend)
		{	if(!string)
			{	consumewhitechars();
			}

			if(!string)
			{	temp=gettoken();
			}
			else
			{	temp=0;
			}

			if(temp)
			{	line[i]=temp;
				if(temp==TOK_REM) string=1;			// allow spaces in rem statement
			}
			else
			{	if(cmdbuf[cmdstart]=='"') string^=0x01;
				line[i]=cmdbuf[cmdstart];
				cmdstart=(cmdstart+1)&CMDLENMSK;
			}
			i++;
			if(i>35) 
			{	//bpWline("Too long!");
				BPMSG1051;
				return;
			}
		}
		//bpWstring("i=");
		//bpWintdec(i); bpSP;

		if(i==3) return;		// no need to insert an empty line
		if(i==4) return;		// no need to insert an empty line
 
		line[0]=TOK_LEN+(i-4);	

		//UART1TX('[');
		//for(i=0; i<35; i++)
		//{	UART1TX(line[i]);
		//}
		//UART1TX(']');

		i=0;
		end=0;
		pos=0;
		
		while(!end)
		{	if(pgmspace[i]>TOK_LEN)			// valid line
			{	len=pgmspace[i]-TOK_LEN;
				//bpWstring("len=");
				//bpWintdec(len); bpSP;

				lineno1=(pgmspace[i+1]<<8)+pgmspace[i+2];
				lineno2=(line[1]<<8)+line[2];
				if(lineno1<lineno2)
				{	pos=i+len+3;
					//bpWstring("pos=");
					//bpWintdec(pos); bpSP;
				}
				i+=(len+3);
			}
			else
			{	end=i;				// we found the end! YaY!  
			}

			temp=(pgmspace[i+1]<<8)+pgmspace[i+2];
			//bpWstring("#");
			//bpWintdec(temp); bpSP;
			//bpWstring("i=");
			//bpWintdec(i); bpSP;

		}

		//bpWstring("pos=");
		//bpWintdec(pos); bpSP;
		//bpWstring("end=");
		//bpWintdec(end);
		//bpBR;

		temp=(line[0]-TOK_LEN)+3;

		//for(i=end+temp; i>=pos; i--)
		for(i=end; i>=pos; i--)
		{	pgmspace[i+temp]=pgmspace[i];		// move every thing from pos temp 
		}
		for(i=0; i<temp; i++)					// insert line
		{	pgmspace[pos+i]=line[i];
		}

		//cmdstart=cmdend;
	}
	else
	{	if(compare("RUN"))
		{	interpreter();
			bpBR;
		}
		else if(compare("LIST"))
		{	list();
		}
		else if(compare("EXIT"))
		{	bpConfig.basic=0;
		}
#ifdef BP_USE_BASICI2C
		else if(compare("FORMAT"))
		{	format();
		}
		else if(compare("SAVE"))
		{	save();
		}
		else if(compare("LOAD"))
		{	load();
		}
#endif
		else if(compare("DEBUG"))
		{	for(i=0; i<PGMSIZE; i+=16)
			{	for(j=0; j<16; j++)
				{	bpWhex(pgmspace[i+j]); bpSP;
				}
			}
		}
		else if(compare("NEW"))
		{	initpgmspace();
		}
		else
		{	//bpWline("Syntax error");
			BPMSG1052;
		}
	}
//	cmdstart=cmdend;
}

void initpgmspace(void)
{	int i;

	for(i=0; i<PGMSIZE ;i++)
	{	pgmspace[i]=0;
	}
	pgmspace[0]=TOK_LEN+1;
	pgmspace[1]=0xFF;
	pgmspace[2]=0xFF;
	pgmspace[3]=TOK_END;
}



#ifdef BP_USE_BASICI2C

// i2c eeprom interaction
// need to incorperate it in bitbang or r2wire!
// CvD: I stole most off it from bitbang.c/h

#define BASSDA		1
#define BASSCL		2
#define BASI2CCLK	100

#define EEP24LC256

#ifdef EEP24LC256

#define I2CADDR		0xA0
#define EEPROMSIZE	0x8000
#define EEPROMPAGE	64

#endif

//globals
int eeprom_lastprog;
unsigned int eeprom_lastmem;



void HIZbbL(unsigned int pins, int delay)
{	IOLAT &=(~pins); //pins to 0
	IODIR &=(~pins);//direction to output
	bpDelayUS(delay);//delay	
}
void HIZbbH(unsigned int pins, int delay)
{	IODIR |= pins;//open collector output high
	bpDelayUS(delay);//delay
}
unsigned char HIZbbR(unsigned int pin)
{	IODIR |= pin; //pin as input
	Nop();
	Nop();
	Nop();
	if(IOPOR & pin) return 1; else return 0;  //clear all but pin bit and return result
}

void basi2cstart(void)
{	HIZbbH((BASSDA|BASSCL), BASI2CCLK);		// both hi
	HIZbbL(BASSDA, BASI2CCLK);				// data down
	HIZbbL(BASSCL, BASI2CCLK);				// clk down
	HIZbbH(BASSDA, BASI2CCLK);				// data up
}

void basi2cstop(void)
{	HIZbbL((BASSDA|BASSCL), BASI2CCLK);	
	HIZbbH(BASSCL, BASI2CCLK);
	HIZbbH(BASSDA, BASI2CCLK);
}

unsigned char basi2cread(int ack)
{	int i;
	unsigned char c;

	c=0;
	HIZbbR(BASSDA);

	for(i=0; i<8; i++)
	{	HIZbbL(BASSCL, BASI2CCLK/5);
		HIZbbH(BASSCL, BASI2CCLK);
		c<<=1;
		c|=HIZbbR(BASSDA);

		HIZbbL(BASSCL, BASI2CCLK);
	}

	if(ack)
	{	HIZbbL(BASSDA, BASI2CCLK/5);
	}
	HIZbbH(BASSCL, BASI2CCLK);
	HIZbbL(BASSCL, BASI2CCLK);

	return c;
}

int basi2cwrite(unsigned char c)
{	int i;
	unsigned char mask;

	mask=0x80;

	for(i=0; i<8; i++)
	{	if(c&mask)
		{	HIZbbH(BASSDA, BASI2CCLK/5);
			//bpWstring("W1");
		}
		else
		{	HIZbbL(BASSDA, BASI2CCLK/5);
			//bpWstring("W0");
		}
		HIZbbH(BASSCL, BASI2CCLK);
		HIZbbL(BASSCL, BASI2CCLK);
		mask>>=1;
	}

	HIZbbH(BASSCL, BASI2CCLK);
	i=HIZbbR(BASSDA);
	HIZbbL(BASSCL, BASI2CCLK);

	return (i^0x01);
}

int checkeeprom(void)
{	// just to be sure
	basi2cstop();
	basi2cstop();
	basi2cstart();
	if(!basi2cwrite(I2CADDR))
	{	//bpWline("No EEPROM");
		BPMSG1053;
		return 0;
	}
	basi2cwrite(0x00);
	basi2cwrite(0x00);
	basi2cstart();
	if(basi2cread(1)==0x00)					// check for any data
	{	bpWline("No EEPROM");			// if 0 prolly no pullup and eeprom (PROLLY!) 
		BPMSG1053;
		return 0;
	}
	basi2cstop();
	return 1;
}

void format(void)
{	int i,j;

	basi2cstop();
	basi2cstart();
	if(!basi2cwrite(I2CADDR))
	{	//bpWline("No EEPROM");
		BPMSG1053;
		return;
	}
	basi2cstop();

	//bpWstring("Erasing");
	BPMSG1054;
	for(i=0; i<EEPROMSIZE; i+=EEPROMPAGE)
	{	basi2cstart();
		basi2cwrite(I2CADDR);
		basi2cwrite((i>>8));
		basi2cwrite((i&0x0FF));
		for(j=0; j<EEPROMPAGE; j++)
		{	basi2cwrite(0xFF);
		}
		basi2cstop();
		UART1TX('.');
		waiteeprom();
	}
	//bpWline("done");
	BPMSG1055;
}

void waiteeprom(void)
{	int wait;
	wait=1;
	while(wait)
	{	basi2cstart();
		wait=basi2cwrite(I2CADDR);
		basi2cstop();
	}
}

void save(void)
{	int i,j;
	int slot;

	consumewhitechars();
	slot=getint();

	if(slot==0)
	{	//bpWline("Syntax error");
		BPMSG1052;
		return;
	}

	//bpWstring("Saving to slot ");
	BPMSG1056;
	bpWdec(slot);
	bpBR;

	if(slot>(EEPROMSIZE/PGMSIZE))
	{	//bpWline("Invalid slot");
		BPMSG1057;
		return;
	}

	if(!checkeeprom())
	{	return;
	}

	slot*=PGMSIZE;

	basi2cstop();
	basi2cwrite(I2CADDR);
	basi2cwrite(slot>>8);
	basi2cwrite(slot&0x0FF);
	basi2cstart();
	basi2cwrite(I2CADDR+1);

	slot*=EEPROMPAGE; 

	for(i=0; i<PGMSIZE; i+=EEPROMPAGE)		// we assume that pgmsize is dividable by eeprompage
	{	basi2cstart();
		basi2cwrite(I2CADDR);
		basi2cwrite((slot+i)>>8);
		basi2cwrite((slot+i)&0x0FF);
		for(j=0; j<EEPROMPAGE; j++)
		{	basi2cwrite(pgmspace[i+j]);
		}
		basi2cstop();
		UART1TX('.');
		waiteeprom();
	}
}

void load(void)
{	int i;
	int slot;

	consumewhitechars();
	slot=getint();

	if(slot==0)
	{	//bpWline("Syntax error");
		BPMSG1052;
		return;
	}

	//bpWstring("Loading from slot ");
	BPMSG1058;
	bpWdec(slot);
	bpBR;

	if(slot>(EEPROMSIZE/PGMSIZE))
	{	//bpWline("Invalid slot");
		BPMSG1057;
		return;
	}

	if(!checkeeprom())
	{	return;
	}

	slot*=PGMSIZE;

	basi2cstop();
	basi2cwrite(I2CADDR);
	basi2cwrite(slot>>8);
	basi2cwrite(slot&0x0FF);
	basi2cstart();
	basi2cwrite(I2CADDR+1);

	for(i=0; i<PGMSIZE; i++)
	{	if(!(i%EEPROMPAGE))	UART1TX('.');		// pure estetic
		pgmspace[i]=basi2cread(1);
	}
}

#endif // BP_USE_BASICI2C
#endif // BP_USE_BASIC

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


// tokens
#define TOKENS		0x80
#define	TOK_LET		0x80
#define TOK_IF		0x81
#define TOK_THEN	0x82
#define TOK_ELSE	0x83
#define TOK_GOTO	0x84
#define TOK_GOSUB	0x85
#define TOK_RETURN	0x86
#define TOK_REM		0x87
#define TOK_PRINT	0x88
#define TOK_INPUT	0x89
#define TOK_FOR		0x8A
#define TOK_TO		0x8B
#define TOK_NEXT	0x8C
#define TOK_READ	0x8D
#define TOK_DATA	0x8E
#define TOK_STARTR	0x8F
#define TOK_START	0x90
#define TOK_STOPR	0x91
#define TOK_STOP	0x92
#define TOK_SEND	0x93
#define TOK_RECEIVE	0x94
#define TOK_CLK		0x95
#define TOK_DAT		0x96
#define TOK_BITREAD	0x97
#define TOK_ADC		0x98
#define TOK_AUXPIN	0x99
#define TOK_PSU		0x9A
#define TOK_PULLUP	0x9B
#define TOK_DELAY	0x9C

#define TOK_AUX		0x9D
#define TOK_FREQ	0x9E
#define TOK_DUTY	0x9F

#define TOK_MACRO	0xA0
#define TOK_END		0xA1 	

#define TOK_LEN		0xE0

#define NUMTOKEN	(TOK_END-TOKENS)+1


// statements
#define	STAT_LET	"LET"	
#define STAT_IF		"IF"
#define STAT_THEN	"THEN"
#define STAT_ELSE	"ELSE"
#define STAT_GOTO	"GOTO"
#define STAT_GOSUB	"GOSUB"
#define STAT_RETURN	"RETURN"
#define STAT_REM	"REM"
#define STAT_PRINT	"PRINT"
#define STAT_INPUT	"INPUT"
#define STAT_FOR	"FOR"
#define STAT_TO		"TO"
#define STAT_NEXT	"NEXT"
#define STAT_END	"END"
#define STAT_READ	"READ"
#define STAT_DATA	"DATA"
#define STAT_START	"START"
#define STAT_STARTR	"STARTR"
#define STAT_STOP	"STOP"
#define STAT_STOPR	"STOPR"
#define STAT_SEND	"SEND"
#define STAT_RECEIVE	"RECEIVE"
#define STAT_CLK	"CLK"
#define STAT_DAT	"DAT"
#define STAT_BITREAD	"BITREAD"
#define STAT_ADC	"ADC"
#define STAT_AUX	"AUX"
#define STAT_PSU	"PSU"
#define STAT_PULLUP	"PULLUP"
#define STAT_DELAY	"DELAY"

#define STAT_AUXPIN	"AUXPIN"
#define STAT_FREQ	"FREQ"
#define STAT_DUTY	"DUTY"
#define STAT_MACRO	"MACRO"

// runtime vars
#define PGMSIZE		1024
#define FORMAX		4
#define GOSUBMAX	10

// errors
#define NOERROR		1
#define NOLEN		2
#define SYNTAXERROR	3
#define FORERROR	4
#define NEXTERROR	5
#define GOTOERROR	6
#define STACKERROR	7
#define RETURNERROR 8
#define DATAERROR	9

// structs
struct forloop {
	int	forstart;
	int var;
	int to;
};

struct token 
{	char *statement;
	unsigned char tok;
};

// functions
void list(void);
void interpreter(void);
void handleelse(void);
int searchlineno(unsigned int line);
int getnumvar(void);
int getmultdiv(void);
int assign(void);
int evaluate(void);	
void interpreter(void);
void printstat(char *s);
void list(void);
void initpgmspace(void);
void basiccmdline(void);

void directory(void);
void save(void);
void format(void);
void load(void);
void waiteeprom(void);


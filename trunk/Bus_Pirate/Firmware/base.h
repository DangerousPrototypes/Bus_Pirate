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
#ifndef BASE 
#define BASE
#include <p24Fxxxx.h>

typedef unsigned char u8;
typedef unsigned int u16;
typedef unsigned long u32;
typedef unsigned char BYTE;


//this sets the hardware version
//#define BUSPIRATEV1A //http://hackaday.com/2009/01/22/how-to-bus-pirate-v1-improved-universal-serial-interface/
//#define BUSPIRATEV3 //also v2go
#define BUSPIRATEV4

#ifdef BUSPIRATEV4
	#define DOUBLE_BUFFER //use USB double buffer, else single buffer is used
	#define USB_INTERRUPT //use USB interrupts, otherwise use timer 1
	#include "cdc_config.h"
	//#include "picusb.h"
	#include "usb_stack.h"
	#include "cdc.h"
	#include "onboardEEPROM.h"
	#include "hardwarev4a.h"
#endif
#if defined(BUSPIRATEV1A)
	#include "hardwarev1a.h"
#elif defined(BUSPIRATEV3)
	#define BUSPIRATEV2 //v25 (2go) and v3 are about the same, enable the same featue set for both
	#include "hardwarev3.h"
#elif defined(BUSPIRATEV4)

#else
#error "No hardware defined in base.h."
#endif

#include "baseIO.h"
#include "baseUI.h"


#define BP_FIRMWARE_STRING "Firmware v6.0-a6 "

#define LANGUAGE_EN_US
//#define LANGUAGE_IT_IT
//#define LANGUAGE_ES_ES
//#define LANGUAGE_DE_DE

//include/exclude modules here
// standard protocols
//#define BP_USE_RAW2WIRE
//#define BP_USE_RAW3WIRE

#define BP_MAIN
//#define BP_ADDONS

#if defined(BP_MAIN)

	#define BP_USE_1WIRE
	//#ifndef BUSPIRATEV4
	#define BP_USE_HWUART //hardware uart (now also MIDI)
	//#endif
	#define BP_USE_I2C
	//#define BP_USE_I2C_HW
	#define BP_USE_HWSPI //hardware spi
	#define BP_USE_RAW2WIRE
	#define BP_USE_RAW3WIRE
	#define BP_USE_LCD // include HD44780 LCD library
	
	#if defined(BUSPIRATEV4)
		#define BP_USE_PCATKB
		#define BP_USE_PIC
		#define BP_USE_BASIC 
	#endif
	
	#define BP_USE_DIO //binary mode
	
#elif defined(BP_ADDONS)
	// most used protos
	//#define BP_USE_1WIRE
	//#define BP_USE_HWUART //hardware uart (now also MIDI)
	//#define BP_USE_I2C
	//#define BP_USE_I2C_HW
	//#define BP_USE_HWSPI //hardware spi
	#define BP_USE_RAW2WIRE
	#define BP_USE_RAW3WIRE
	#define BP_USE_PCATKB
	#define BP_USE_LCD // include HD44780 LCD library
	#define BP_USE_PIC
	#define BP_USE_DIO //binary mode
#else
#error "No Bus Pirate configuration defined."
#endif


//#define BP_USE_BASIC   // basic subsystem
//#define BP_USE_BASICI2C  // use an i2ceeprom for storing


// only 1 should be uncommented
//#define BASICTEST
//#define BASICTEST_I2C
//#define BASICTEST_UART
//#define BASICTEST_R3W
//#define BASICTEST_PIC10			// program blink a led
//#define BASICTEST_PIC10_2			// read whole pic

//sets the address in the bootloader to jump to on the bootloader command
//must be defined in asm
asm (".equ BLJUMPADDRESS, 0xABF8");

//
//
//
// END CONFIG AREA
//
//
//

//// left out for now
/*	my italian and spanish aint that good ;)
#elif defined(LANGUAGE_IT_IT)
        #include "translations/it-IT.h"
//	#include "translations/en-US.h"
#elif defined(LANGUAGE_ES_ES)
        #include "translations/es-ES.h"
 */
 
 
// If BUSPIRATEv4 then use the en_US.h otherwise
// use the v3 one. The version3 one does not include
// the modes that the v3 cannot fit. This way we dont
// waste space on strings we wont see.
//
// Also note; the BPV4 project file uses the en_US.s file
// the buspurate v3 uses the other one...
#if defined(BUSPIRATEV4)
	#if defined(LANGUAGE_EN_US)
		#include "translations/en_US.h"
	#elif defined(LANGUAGE_DE_DE)
		#include "translations/de_DE.h"
	#else
		#error "No language defined in base.h."
	#endif
#else
	#if defined(LANGUAGE_EN_US)
		#include "translations/v3_en_US.h"
	#elif defined(LANGUAGE_DE_DE)
		#include "translations/v3_de_DE.h"
	#else
		#error "No language defined in base.h."
	#endif
#endif






//these settings are destroyed between modes.
//this is used to pass stuff from protocol libraries to BP
//for example, allowpullup determines if the P menu is available
//TO DO: add global scratch buffer to setting array for use in 
//	1-Wire enumeration, JTAG input buffer, etc...

struct _modeConfig {
    unsigned char speed;
    unsigned char numbits;
    unsigned char buf[16];
    unsigned char altAUX : 2; // there are 4 AUX on BUSPIRATEV4
    unsigned char periodicService : 1;
    unsigned char lsbEN : 1;
    unsigned char HiZ : 1;
    unsigned char int16 : 1; // 16 bits output?
    unsigned char wwr : 1; // write with read
};

struct _command {
    unsigned char cmd;
    unsigned int num;
    unsigned int repeat;
};

unsigned int bpRevByte(unsigned int c);

//reset all peripherals before configuring a new bus protocol
void bpInit(void);

//take an ADC measurement on channel c
unsigned int bpADC(unsigned char c);
//takes a measurement from the ADC probe and prints the result to the user terminal
void bpADCprobe(void);
void bpADCCprobe(void);

//print byte c to the user terminal in the format 
//  specified by the bpConfig.displayMode setting
void bpWbyte(unsigned int c);

//delays used by many libraries
//void bpDelayMS(const unsigned char delay);
//void bpDelayUS(const unsigned char delay);
void bpDelayMS(int delay);
void bpDelayUS(int delay);
#endif

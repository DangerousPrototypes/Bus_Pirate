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

/* Binary access modes for Bus Pirate scripting */

#include "base.h"
#include "bitbang.h"
#include "selftest.h"
#include "SPI.h"
#include "I2C.h"
#include "UART.h"
#include "1wire.h"
#include "binwire.h"
#include "OpenOCD.h"
#include "pic.h"
#include "binIO.h"
#include "AUXpin.h"
#if defined (BUSPIRATEV4)
#include "smps.h"
#endif

extern struct _modeConfig modeConfig;

//unsigned char binBBpindirectionset(unsigned char inByte);
//unsigned char binBBpinset(unsigned char inByte);
void binBBversion(void);
void binSelfTest(unsigned char jumperTest);
void binReset(void);
unsigned char getRXbyte(void);

/*
Bitbang is like a player piano or bitmap. The 1 and 0 represent the pins. 
So for the four Bus Pirate pins we use the the bits as follows:
COMMAND|POWER|PULLUP|AUX|CS|MISO|CLK|MOSI.

The Bus pirate also responds to each write with a read byte showing the current state of the pins.

The bits control the state of each of those pins when COMMAND=1. 
When COMMAND=0 then up to 127 command codes can be entered on the lower bits.
0x00 resets the Bus Pirate to bitbang mode.

Data:
1xxxxxxx //COMMAND|POWER|PULLUP|AUX|MOSI|CLK|MISO|CS

Commands:
00000000 //Reset to raw BB mode, get raw BB version string
00000001 //enter rawSPI mode
00000010 //enter raw I2C mode
00000011 //enter raw UART mode
00000100 // enter raw 1-wire
00000101 //enter raw wire mode
00000110 // enter openOCD
00000111 // pic programming mode
00001000 // SMPS setting mode for v4
00001111 //reset, return to user terminal
00010000 //short self test
00010001 //full self test with jumpers
00010010 // setup PWM
00010011 // clear PWM
00010100 //ADC measurement
010xxxxx //set input(1)/output(0) pin state (returns pin read)
 */
void binBBversion(void) {
    bpWstring("BBIO1");
}

void binBB(void) {
    static unsigned char inByte;
    unsigned int i;

    BP_LEDMODE = 1; //light MODE LED
    binReset();
    binBBversion(); //send mode name and version

    while (1) {

        inByte = getRXbyte();

        if ((inByte & 0b10000000) == 0) {//if command bit cleared, process command
            if (inByte == 0) {//reset, send BB version
                binBBversion();
            } else if (inByte == 1) {//goto SPI mode
                binReset();
#ifdef BP_USE_HWSPI
                binSPI(); //go into rawSPI loop
#endif
                binReset();
                binBBversion(); //say name on return
            } else if (inByte == 2) {//goto I2C mode
                binReset();
#ifdef BP_USE_I2C
                binI2C();
#endif
                binReset();
                binBBversion(); //say name on return
            } else if (inByte == 3) {//goto UART mode
                binReset();
#ifdef BP_USE_HWUART
                binUART();
#endif
                binReset();
                binBBversion(); //say name on return
            } else if (inByte == 4) {//goto 1WIRE mode
                binReset();
#ifdef BP_USE_1WIRE
                bin1WIRE();
#endif
                binReset();
                binBBversion(); //say name on return
            } else if (inByte == 5) {//goto RAW WIRE mode
                binReset();
                binwire();
                binReset();
                binBBversion(); //say name on return
            } else if (inByte == 6) {//goto OpenOCD mode
                binReset();
#ifndef BUSPIRATEV4
                binOpenOCD();
#endif
                binReset();
                binBBversion(); //say name on return
            } else if (inByte == 7) {//goto pic mode
                binReset();
#ifdef BP_USE_PIC
                binpic();
#endif
                binReset();
                binBBversion(); //say name on return
            } else if (inByte == 8) {//goto SMPS mode
                binReset();
#ifdef BUSPIRATEV4
                binSMPS();
#endif
                binReset();
                binBBversion(); //say name on return
            } else if (inByte == 0b1111) {//return to terminal
                UART1TX(1);
                BP_LEDMODE = 0; //light MODE LED
                WAITTXEmpty(); //wait untill TX finishes
#ifndef BUSPIRATEV4
                asm("RESET");
#endif
#ifdef BUSPIRATEV4 //cannot use ASM reset on BPv4
                binReset();
				return;
#endif
                //self test is only for v2go and v3
#ifndef BUSPIRATEV1A 
            } else if (inByte == 0b10000) {//short self test
                binSelfTest(0);
            } else if (inByte == 0b10001) {//full self test with jumpers
                binSelfTest(1);
#endif
            } else if (inByte == 0b10010) {//setup PWM

                //cleanup timers from FREQ measure
                T2CON = 0; //16 bit mode
                T4CON = 0;
                OC5CON = 0; //clear PWM settings

                BP_AUX_RPOUT = OC5_IO; //setup pin

                //get one byte
                i = getRXbyte();
                if (i & 0b10) T2CONbits.TCKPS1 = 1; //set prescalers
                if (i & 0b1) T2CONbits.TCKPS0 = 1;

                //get two bytes
                i = (getRXbyte() << 8);
                i |= getRXbyte();
                OC5R = i; //Write duty cycle to both registers
                OC5RS = i;
                OC5CON = 0x6; // PWM mode on OC, Fault pin disabled

                //get two bytes
                i = (getRXbyte() << 8);
                i |= getRXbyte();
                PR2 = i; // write period

                T2CONbits.TON = 1; // Start Timer2
                UART1TX(1);
            } else if (inByte == 0b10011) {//clear PWM
                T2CON = 0; // stop Timer2
                OC5CON = 0;
                BP_AUX_RPOUT = 0; //remove output from AUX pin
                UART1TX(1);
                //ADC only for v1, v2, v3
            } else if (inByte == 0b10100) {//ADC reading (x/1024)*6.6volts
                AD1CON1bits.ADON = 1; // turn ADC ON
                i = bpADC(BP_ADC_PROBE); //take measurement
                AD1CON1bits.ADON = 0; // turn ADC OFF
                UART1TX((i >> 8)); //send upper 8 bits
                UART1TX(i); //send lower 8 bits
            } else if (inByte == 0b10101) {//ADC reading (x/1024)*6.6volts
                AD1CON1bits.ADON = 1; // turn ADC ON
                while (1) {
                    i = bpADC(BP_ADC_PROBE); //take measurement
                    WAITTXEmpty();
                    UART1TX((i >> 8)); //send upper 8 bits
                    //while(UART1TXRdy==0);
                    UART1TX(i); //send lower 8 bits

                    if (UART1RXRdy() == 1) {//any key pressed, exit
                        i = UART1RX(); // /* JTR usb port; */;
                        break;
                    }
                }
                AD1CON1bits.ADON = 0; // turn ADC OFF
			}else if (inByte==0b10110){ //binary frequency count access
				unsigned long l;
				l=bpBinFreq();
				UART1TX((l>>(8*3)));
				UART1TX((l>>(8*2)));
				UART1TX((l>>(8*1)));
				UART1TX((l));
            } else if ((inByte >> 5)&0b010) {//set pin direction, return read
                UART1TX(binBBpindirectionset(inByte));
            } else {//unknown command, error
                UART1TX(0);
            }

        } else {//data for pins
            UART1TX(binBBpinset(inByte));
        }//if
    }//while
}//function

unsigned char getRXbyte(void) {
    while (UART1RXRdy() == 0); //wait for a byte
    return UART1RX(); ///* JTR usb port; */ //grab it
}

void binReset(void) {
    binBBpindirectionset(0xff); //pins to input on start
    binBBpinset(0); //startup everything off, pins at ground
#if defined(BUSPIRATEV1A) //aux2 pin to input on v1a
    BP_AUX2_IN();
#endif
}

unsigned char binBBpindirectionset(unsigned char inByte) {
    unsigned char i;
    //setup pin TRIS
    //using this method is long and nasty,
    //but it makes it work for all hardware versions
    //without special adjustments
    i = 0;
    if (inByte & 0b10000)i = 1;
    BP_AUX_DIR = i;

    i = 0;
    if (inByte & 0b1000)i = 1;
    BP_MOSI_DIR = i;

    i = 0;
    if (inByte & 0b100)i = 1;
    BP_CLK_DIR = i;

    i = 0;
    if (inByte & 0b10)i = 1;
    BP_MISO_DIR = i;

    i = 0;
    if (inByte & 0b1)i = 1;
    BP_CS_DIR = i;

    //delay for a brief period
    bpDelayUS(5);

    //return PORT read
    inByte &= (~0b00011111);
    if (BP_AUX != 0)inByte |= 0b10000;
    if (BP_MOSI != 0)inByte |= 0b1000;
    if (BP_CLK != 0)inByte |= 0b100;
    if (BP_MISO != 0)inByte |= 0b10;
    if (BP_CS != 0)inByte |= 0b1;

    return inByte; //return the read
}

unsigned char binBBpinset(unsigned char inByte) {
    unsigned char i;

    if (inByte & 0b1000000) {
        BP_VREG_ON(); //power on
    } else {
        BP_VREG_OFF(); //power off
    }

#ifndef BUSPIRATEV1A 
    if (inByte & 0b100000) {
        BP_PULLUP_ON(); //pullups on
    } else {
        BP_PULLUP_OFF();
    }
#endif

    //set pin LAT
    //using this method is long and nasty,
    //but it makes it work for all hardware versions
    //without special adjustments
    i = 0;
    if (inByte & 0b10000)i = 1;
    BP_AUX = i;

    i = 0;
    if (inByte & 0b1000)i = 1;
    BP_MOSI = i;

    i = 0;
    if (inByte & 0b100)i = 1;
    BP_CLK = i;

    i = 0;
    if (inByte & 0b10)i = 1;
    BP_MISO = i;

    i = 0;
    if (inByte & 0b1)i = 1;
    BP_CS = i;

    //delay for a brief period
    bpDelayUS(5);

    //return PORT read
    inByte &= (~0b00011111);
    if (BP_AUX != 0)inByte |= 0b10000;
    if (BP_MOSI != 0)inByte |= 0b1000;
    if (BP_CLK != 0)inByte |= 0b100;
    if (BP_MISO != 0)inByte |= 0b10;
    if (BP_CS != 0)inByte |= 0b1;

    return inByte; //return the read
}

#ifndef BUSPIRATEV1A

void binSelfTest(unsigned char jumperTest) {
    static volatile unsigned int tick = 0;
    unsigned char errors, inByte;

    errors = selfTest(0, jumperTest); //silent self-test
    if (errors) BP_LEDMODE = 1; //light MODE LED if errors
    UART1TX(errors); //reply with number of errors

    while (1) {
        //echo incoming bytes + errors
        //tests FTDI chip, UART, retrieves results of test
        if (UART1RXRdy()) {
            inByte = UART1RX(); //check input
            if (inByte != 0xff) {
                UART1TX(inByte + errors);
            } else {
                UART1TX(0x01);
                return; //exit if we get oxff, else send back byte+errors
            }
        }

        if (!errors) {
            if (tick == 0) {
                tick = 0xFFFF;
                BP_LEDMODE ^= 1; //toggle LED
            }
            tick--;
        }

    }

}
#endif

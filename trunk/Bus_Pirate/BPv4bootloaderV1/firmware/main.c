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

//BOOTLOADER PROJECT MAIN FILE

#include "globals.h"
#include "configwords.h"	// JTR only included in main.c
#include "descriptors.h"	// JTR Only included in main.c

void usb_start(void);
void initCDC(void);
void usb_init(ROMPTR const unsigned char*, ROMPTR const unsigned char*, ROMPTR const unsigned char*, int);
void Initialize(void);
void bootloader(void);

extern volatile unsigned char usb_device_state;

#pragma code

int main(void) {
	volatile int i;
    //asm("DISI #16"); //Disable interrupts

	//configure PGC PGD for jumper test
	//PGC is output
	#define PGC_OUT LATBbits.LATB6
	#define PGC_TRIS TRISBbits.TRISB6
	#define PGC_PU CNPU2bits.CN24PUE
	#define PGC_IN PORTBbits.RB6

	#define PGD_IN PORTBbits.RB7
	#define PGD_TRIS TRISBbits.TRISB7
	#define PGD_PU CNPU2bits.CN25PUE
	
	AD1PCFGL=0xFFFF;//all digital

	//PGD is input with pullup
	//TRISB|=0b10000000; //
	PGD_TRIS=1; //input
	//CNPU2|=0b1000000000; //
	PGD_PU=1;	//pullup on

	PGC_OUT=0;
	PGC_TRIS=0;

	i=5000;
	while(i--);

	for(i=0; i<20; i++){
//(PORTCbits.RC14==1)||
		//if((PORTBbits.RB7==1)){ //go to user space on first mis-match
		if((PGD_IN==1)){ //go to user space on first mis-match
			//continue to bootloader, or exit
			asm (".equ BLJUMPADDRESS, 0x2000");
			asm volatile ("mov #BLJUMPADDRESS, w1 \n" //bootloader location
						  "goto w1 \n");
	
		}
	}

	PGD_PU=0;	//pullup off
	PGC_TRIS=1;//input

    Initialize(); //setup bus pirate
	LedSetup();
    uLedOff();
    mLedOff();
    vLedOn();

    usb_init(cdc_device_descriptor, cdc_config_descriptor, cdc_str_descs, USB_NUM_STRINGS); // TODO: Remove magic with macro
    usb_start();
    initCDC(); // Setup CDC defaults.
    vLedOff();
    //wait for the USB connection to enumerate
    do {
        //if (!TestUsbInterruptEnabled()) //JTR3 added

        usb_handler(); ////service USB tasks Guaranteed one pass in polling mode even when usb_device_state == CONFIGURED_STATE
        //if ((usb_device_state < DEFAULT_STATE)) { // JTR2 no suspendControl available yet || (USBSuspendControl==1) ){
       // } else if (usb_device_state < CONFIGURED_STATE) {
       // }
    } while (usb_device_state < CONFIGURED_STATE); // JTR addition. Do not proceed until device is configured.
    vLedOn();
    bootloader();
    return 0;
}

void Initialize(void) {
    CORCONbits.PSV = 1;
    PSVPAG = 0; //
    CLKDIV = 0x0000; // Set PLL prescaler (1:1)
    TBLPAG = 0;

	//all high-z to protect everything
	AD1PCFGL=0xFFFF;//all digital
    AD1PCFGH = 0x2;
	//TRISA=0xFFFF;
	TRISB=0xFFFF;
	TRISC=0xFFFF;
	TRISD=0xFFFF;
	TRISE=0xFFFF;
	TRISF=0xFFFF;
	TRISG=0xFFFF;
    OSCCONbits.SOSCEN = 0;
}

#ifdef USB_INTERRUPT
#pragma interrupt _USB1Interrupt

void __attribute__((interrupt, no_auto_psv)) _USB1Interrupt() {

    //USB interrupt
    //IRQ enable IEC5bits.USB1IE
    //IRQ flag	IFS5bits.USB1IF
    //IRQ priority IPC21<10:8>
    //{
    usb_handler();
    IFS5bits.USB1IF = 0; //	PIR2bits.USBIF = 0;
    //}
}
#endif






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
    //asm("DISI #16"); //Disable interrupts

    Initialize(); //setup bus pirate
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

    AD1PCFGL = 0x7FD8; //BPv4 has five analog pins b0, b1, b2, b5, b15
    AD1PCFGH = 0x2;
    TRISB = 0x8027; // JTR Analog pins as inputs.
    TRISC = 0x0000;
    TRISD = 0x0000;
    TRISE = 0x0000;
    TRISF = 0x0000;
    TRISG = 0x0000;
    LATB = 0x0000;
    LATC = 0x0000;
    LATD = 0x0000;
    LATE = 0x0000;
    LATF = 0x0000;
    LATG = 0x0000;
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






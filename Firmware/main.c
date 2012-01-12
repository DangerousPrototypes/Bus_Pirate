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
 
 
    ///////////////////////////////////////////
   /////////////[ NOTE ]//////////////////////////////////////
  ///    The settings and configuration are on [base.h] please open that file if you are new to
 /// compiling the Bus Pirate Firmware.
 ///
 ///                                                                      DangerousPrototypes.com
 ////                                                                 For support and information
 /////                                                        visit the DangerousPrototypes Forum
 ///////////
 
 
#include "globals.h"
#include "busPirateCore.h"
#include "procMenu.h"
//#include "procSyntax.h"
#include "selftest.h"
//#include "binIO.h"
//#include "SUMP.h"
#include "basic.h"


#if defined (BUSPIRATEV4)
#include "configwords.h"	// JTR only included in main.c
#include "descriptors.h"	// JTR Only included in main.c

void user_configured_init(void);
void init(void); //hardware init
void usb_start(void); //JTR added prototype was missing or something else funny or altered that I cannot remember??
/* Initialize usb cdc acm subsystem */
void initCDC(void);
void usb_init(ROMPTR const unsigned char*, ROMPTR const unsigned char*, ROMPTR const unsigned char*, int);

extern BYTE cdc_In_buffer[64];
extern BYTE CDC_In_count;
extern BYTE *InPtr;
extern BYTE *OutPtr;

//void USBbufFlush(void);

extern volatile unsigned char usb_device_state; // JTR added
extern unsigned char CDC_trf_state;

#endif

#if defined (BUSPIRATEV2) || defined (BUSPIRATEV1A)
//set custom configuration for PIC 24F (now always set in bootloader page, not needed here)
_CONFIG2(FNOSC_FRCPLL & OSCIOFNC_ON &POSCMOD_NONE & I2C1SEL_PRI)		// Internal FRC OSC = 8MHz
_CONFIG1(JTAGEN_OFF & GCP_OFF & GWRP_OFF & COE_OFF & FWDTEN_OFF & ICS_PGx1) //turn off junk we don't need
#endif

unsigned char irqFlag = 0;
//void _T1Interrupt(void);
void ISRTable(); //Pseudo function to hold ISR remap jump table
void Initialize(void);

static unsigned char __attribute__((section(".bss.end"))) _buffer[TERMINAL_BUFFER];
struct _bpConfig bpConfig = {.terminalInput = _buffer}; //holds persistant bus pirate settings (see busPirateCore.h)
struct _modeConfig modeConfig; //holds mode info, cleared between modes
struct _command bpCommand; //holds the current active command so we don't ahve to put so many variables on the stack

unsigned char binmodecnt = 0; //, terminalInput[TERMINAL_BUFFER];
#if defined (BUSPIRATEV4)
void _USB1Interrupt(void);
#endif

#pragma code
//this loop services user input and passes it to be processed on <enter>

int main(void) {
    Initialize(); //setup bus pirate

    //wait for the USB connection to enumerate
#if defined (BUSPIRATEV4)

	#ifdef USB_INTERRUPT
	    EnableUsbInterrupt(USB_STALL + USB_IDLE + USB_TRN + USB_ACTIV + USB_SOF + USB_UERR + USB_URST);
	    //EnableUsbInterrupt(USB_TRN + USB_SOF + USB_UERR + USB_URST);
	    EnableUsbInterrupts();
	#endif

    usbbufflush(); //setup the USB byte buffer
	BP_USBLED_ON();
    do {
		#ifndef USB_INTERRUPT
        //if (!TestUsbInterruptEnabled()) //JTR3 added
        	usb_handler(); ////service USB tasks Guaranteed one pass in polling mode even when usb_device_state == CONFIGURED_STATE
		#endif

        if ((usb_device_state < DEFAULT_STATE)) { // JTR2 no suspendControl available yet || (USBSuspendControl==1) ){

        } else if (usb_device_state < CONFIGURED_STATE) {

        }

    } while (usb_device_state < CONFIGURED_STATE); // JTR addition. Do not proceed until device is configured.

	BP_USBLED_OFF();

	#ifdef DOUBLE_BUFFER
   		ArmCDCInDB(); // Set up CDC IN double buffer
	#endif

    //enable timer 1 with interrupts,
    //service with function in main.c.
    IEC0bits.T1IE = 1;
    IFS0bits.T1IF = 0;
    PR1 = 0x0FFF;
    T1CON = 0x8000; //8010
#endif
    serviceuser();
    return 0;
}

//bus pirate initialization
//setup clock, terminal UART, pins, LEDs, and display version info

void Initialize(void) {
#if defined (BUSPIRATEV2)
	unsigned char i;
#endif
#if defined (BUSPIRATEV2) || defined (BUSPIRATEV1A)
    CLKDIVbits.RCDIV0 = 0; //clock divider to 0
    AD1PCFG = 0xFFFF; // Default all pins to digital
#elif defined (BUSPIRATEV4)
    CLKDIV = 0x0000; // Set PLL prescaler (1:1)
    //	BP_LEDUSB_DIR=0;	// output
    CORCONbits.PSV = 1; // JTR PIC24 fixup ?? PSV not being initialized. May have been done by c_init though.
    PSVPAG = 0; //
    AD1PCFGL = 0x7FD8; //BUSPIRATEV4 has five analog pins b0, b1, b2, b5, b15
    AD1PCFGH = 0x2;
#endif

    OSCCONbits.SOSCEN = 0;

    //set pin configuration using peripheral pin select
#if defined (BUSPIRATEV2) || defined (BUSPIRATEV1A)
    BP_TERM_RX = BP_TERM_RX_RP; //Inputs UART1 RX RPINR18bits.U1RXR=4;
    BP_TERM_TX_RP = BP_TERM_TX; // Outputs UART1 TX RPOR1bits.RP3R=U1TX_IO;
#endif

    //put startup values in config (do first)
    bpConfig.termSpeed = 8; //default PC side port speed, startup in 115200, or saved state (later)....
    bpConfig.displayMode = HEX;

    bpInit(); //put startup values in config (do first)clean up, exit in HI-Z

#if defined (BUSPIRATEV2) || defined (BUSPIRATEV1A)
    InitializeUART1(); //init the PC side serial port
#elif defined (BUSPIRATEV4)

    //    USBDeviceInit();//setup usb
    initCDC(); // JTR this function has been highly modified It no longer sets up CDC endpoints.
    usb_init(cdc_device_descriptor, cdc_config_descriptor, cdc_str_descs, USB_NUM_STRINGS); // TODO: Remove magic with macro
    usb_start();

#endif


#if defined (BUSPIRATEV2)
    //find the Bus Pirate revision
    //pullup on, do it now so it can settle during the next operations
    CNPU1bits.CN6PUE = 1;
    CNPU1bits.CN7PUE = 1;
#endif
    //#ifndef BUSPIRATEV4
    // Get the chip type and revision
    bpConfig.dev_type = bpReadFlash(DEV_ADDR_UPPER, DEV_ADDR_TYPE);
    bpConfig.dev_rev = bpReadFlash(DEV_ADDR_UPPER, DEV_ADDR_REV);
    //#endif

#if defined (BUSPIRATEV2)
    //now check the revision
	//Version | RB3 | RB2
	//2go, 3a | 1   |  1
	//v3b     | 1   |  0
	//v3.5    | 0   |  0
	i=PORTB; //get settings
	i=i>>2; //remove unused
	i&=(~0b11111100); //clear others
    if (i==0b11) {
        bpConfig.HWversion = 'a';
    } else if(i==0b10){
        bpConfig.HWversion = 'b';
    }else if(i==0){
        bpConfig.HWversion = '5';
	}
    //pullup off
    CNPU1bits.CN6PUE = 0;
    CNPU1bits.CN7PUE = 0;
#else
    bpConfig.HWversion = 0;
#endif

    bpConfig.quiet = 0; // turn output on (default)
    modeConfig.numbits = 8;
#ifdef BP_USE_BASIC
    initpgmspace();
#endif

    TBLPAG = 0; // we need to be in page 0 (somehow this isn't set)

#ifndef BUSPIRATEV4
    bpWBR; //send a line feed
    versionInfo(); //prints hardware and firmware version info (base.c)
#endif

}

//Interrupt Remap method 2:  Using Goto and jump table
/*
void __attribute__ ((interrupt,no_auto_psv)) _T1Interrupt(){
        IFS0bits.T1IF = 0;
        IEC0bits.T1IE = 0;
        PR1 = 0xFFFF;
        T1CON = 0;
        irqFlag=1;
}
 */

#ifdef BUSPIRATEV4
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
#pragma code
#endif



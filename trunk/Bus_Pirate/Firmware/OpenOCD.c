#include "base.h"
#include "baseIO.h"
#include "busPirateCore.h"
#include "binIOhelpers.h"

extern struct _modeConfig modeConfig;
extern struct _bpConfig bpConfig;
// tris registers
#define OOCD_TDO_TRIS   BP_MISO_DIR
#define OOCD_TMS_TRIS   BP_CS_DIR
#define OOCD_CLK_TRIS   BP_CLK_DIR
#define OOCD_TDI_TRIS   BP_MOSI_DIR
#define OOCD_SRST_TRIS  BP_AUX0_DIR
#if defined (BUSPIRATEV3)
#define OOCD_TRST_TRIS  BP_PGD_DIR
#endif

// io ports
#define OOCD_TDO        BP_MISO
#define OOCD_TMS        BP_CS 
#define OOCD_CLK        BP_CLK 
#define OOCD_TDI        BP_MOSI 
#define OOCD_SRST       BP_AUX0
#if defined (BUSPIRATEV3)
#define OOCD_TRST       BP_PGD
#endif

// open-drain control
#define OOCD_TDO_ODC    BP_MISO
#define OOCD_TMS_ODC    BP_CS 
#define OOCD_CLK_ODC    BP_CLK 
#define OOCD_TDI_ODC    BP_MOSI 
#define OOCD_SRST_ODC   BP_AUX0
#if defined (BUSPIRATEV3)
#define OOCD_TRST_ODC   BP_PGD
#endif

#define CMD_UNKNOWN       0x00
#define CMD_PORT_MODE     0x01
#define CMD_FEATURE       0x02
#define CMD_READ_ADCS     0x03
//#define CMD_TAP_SHIFT     0x04 // old protocol
#define CMD_TAP_SHIFT     0x05
#define CMD_ENTER_OOCD    0x06 // this is the same as in binIO
#define CMD_UART_SPEED    0x07
#define CMD_JTAG_SPEED    0x08

static void binOpenOCDString(void);
static void binOpenOCDPinMode(unsigned char mode);
static void binOpenOCDHandleFeature(unsigned char feat, unsigned char action);
static void binOpenOCDAnswer(unsigned char *buf, unsigned int len);
extern void binOpenOCDTapShiftFast(unsigned char *in_buf, unsigned char *out_buf, unsigned int bits, unsigned int delay);


enum {
	FEATURE_LED=0x01,
	FEATURE_VREG=0x02,
	FEATURE_TRST=0x04,
	FEATURE_SRST=0x08,
	FEATURE_PULLUP=0x10
};

enum {
	SERIAL_NORMAL=0,
	SERIAL_FAST=1
};

enum {
	MODE_HIZ=0,
	MODE_JTAG=1,
	MODE_JTAG_OD=2, // open-drain outputs
};


static unsigned int OpenOCDJtagDelay;

static void binOpenOCDString(void){bpWstring("OCD1");}

void binOpenOCD(void){
	unsigned char *buf = bpConfig.terminalInput; // for simplicity :)
	unsigned int i,j;
	unsigned char inByte;
	unsigned char inByte2;

	OpenOCDJtagDelay = 1;

	binOpenOCDString();

	while(1){
/*
this will misbehave when polling is turned off in OpenOCD

		// enable timeout timer (1sec) taken from AUX measurement
		T4CON=0;
		TMR5HLD=0x00; // reset the counter
		TMR4=0x00;
		T4CON=0b1000; // 32bit timer
		PR5=0x1e8;  // 0xf4;
		PR4=0x4800; // 0x2400;
		IFS1bits.T5IF=0;

		// enable timer
		T4CONbits.TON=1;

		// wait for byte or timeout 
		while(U1STAbits.URXDA == 0 && IFS1bits.T5IF==0);
		
		if (IFS1bits.T5IF==1) {
			// disable timer
			T4CONbits.TON=0;
			// we timeouted, set serial to normal speed
			UART1Speed(UART_NORMAL_SPEED);
			return;
		}

		// disable timer
		T4CONbits.TON=0;

		// read the byte
		inByte=U1RXREG; 
*/
		inByte=UART1RX();

		switch(inByte){
			case CMD_UNKNOWN:
				return;
			case CMD_ENTER_OOCD:
				binOpenOCDString();
				break;
			case CMD_READ_ADCS:
				buf[0] = CMD_READ_ADCS;
				buf[1] = 8;
				AD1CON1bits.ADON = 1; // turn ADC ON
				i=bpADC(12); // ADC pin
				buf[2] = (unsigned char)(i>>8);
				buf[3] = (unsigned char)(i);
				i=bpADC(11); // VEXT pin
				buf[4] = (unsigned char)(i>>8);
				buf[5] = (unsigned char)(i);
				i=bpADC(10); // V33 pin
				buf[6] = (unsigned char)(i>>8);
				buf[7] = (unsigned char)(i);
				i=bpADC(9); // V50 pin
				buf[8] = (unsigned char)(i>>8);
				buf[9] = (unsigned char)(i);
				AD1CON1bits.ADON = 0; // turn ADC OFF
				binOpenOCDAnswer(buf, 10);
				break;
			case CMD_PORT_MODE:
				inByte=UART1RX();
				binOpenOCDPinMode(inByte);
				break;
			case CMD_FEATURE:
				inByte=UART1RX();
				inByte2=UART1RX();
				binOpenOCDHandleFeature(inByte, inByte2);
				break;
			case CMD_JTAG_SPEED:
				inByte=UART1RX();
				inByte2=UART1RX();
				OpenOCDJtagDelay = (inByte << 8) | inByte2;
				break;
			case CMD_UART_SPEED:
				inByte=UART1RX();
				i = inByte;
				if (inByte == SERIAL_FAST) {
					UART1Speed(UART_FAST_SPEED);
				} else {
					UART1Speed(UART_NORMAL_SPEED);
				}

				inByte=UART1RX();
				inByte2=UART1RX();
				if ((inByte != 0xAA) || (inByte2 != 0x55)) {
					i = SERIAL_NORMAL;
					UART1Speed(UART_NORMAL_SPEED);
				}

				buf[0] = CMD_UART_SPEED;
				buf[1] = (unsigned char)i;
				binOpenOCDAnswer(buf, 2);
				break;
			case CMD_TAP_SHIFT:
				inByte=UART1RX();
				inByte2=UART1RX();

				IFS0bits.U1RXIF = 0; // reset the RX flag

				j = (inByte << 8) | inByte2; // number of bit sequences

				// this fixes possible buffer overflow
				if (j > 0x2000) 
					j = 0x2000;

				i = (j+7)/8; // number of bytes used
				buf[0] = CMD_TAP_SHIFT;
				buf[1] = inByte;
				buf[2] = inByte2;
				binOpenOCDAnswer(buf, 3);

				// prepare the interrupt transfer
				UART1RXBuf = (unsigned char*)bpConfig.terminalInput;
				UART1RXToRecv = 2*i;
				UART1RXRecvd = 0;

				UART1TXBuf = (unsigned char*)(bpConfig.terminalInput + 2100); // 2048 bytes + 3 command header + to be sure
				UART1TXSent = 0;
				UART1TXAvailable = 0;

				// enable RX interrupt
				IEC0bits.U1RXIE = 1;

				binOpenOCDTapShiftFast(UART1RXBuf, UART1TXBuf, j, OpenOCDJtagDelay);
				break;
			default:
				buf[0] = 0x00; // unknown command
				buf[1] = 0x00;
				binOpenOCDAnswer(buf, 1);
				break;
		}
	}			
}

static void binOpenOCDPinMode(unsigned char mode) {
	// reset all pins
	OOCD_TMS=0;
	OOCD_TDI=0;
	OOCD_CLK=0;
	OOCD_SRST=0;
#if defined (BUSPIRATEV3)
	OOCD_TRST=0;
#endif
	// setup open-drain if necessary
	if (mode == MODE_JTAG_OD) {
		OOCD_TMS_ODC=1;
		OOCD_CLK_ODC=1;
		OOCD_TDI_ODC=1;
		OOCD_SRST_ODC=1;
#if defined (BUSPIRATEV3)
		OOCD_TRST_ODC=1;
#endif
	} else {
		OOCD_TMS_ODC=0;
		OOCD_CLK_ODC=0;
		OOCD_TDI_ODC=0;
		OOCD_SRST_ODC=0;
#if defined (BUSPIRATEV3)
		OOCD_TRST_ODC=0;
#endif
	}
	// make pins output
	if (mode == MODE_JTAG || mode == MODE_JTAG_OD) {
		OOCD_TMS_TRIS=0;
		OOCD_TDI_TRIS=0;
		OOCD_CLK_TRIS=0;
		OOCD_SRST_TRIS=0;
#if defined (BUSPIRATEV3)
		OOCD_TRST_TRIS=0;
#endif
		OOCD_TDO_TRIS=1;
	} else {
		OOCD_TMS_TRIS=1;
		OOCD_TDI_TRIS=1;
		OOCD_CLK_TRIS=1;
		OOCD_SRST_TRIS=1;
#if defined (BUSPIRATEV3)
		OOCD_TRST_TRIS=1;
#endif
		OOCD_TDO_TRIS=1;
	}
}

static void binOpenOCDAnswer(unsigned char *buf, unsigned int len) {
	unsigned int i;
	for (i=0; i < len; i++ ){
		UART1TX(buf[i]);
	}
}

static void binOpenOCDHandleFeature(unsigned char feat, unsigned char action) {
	switch (feat) {
		case FEATURE_LED:
			BP_LEDMODE_DIR=0; //LED to output
			BP_LEDMODE=action;
			break;
		case FEATURE_VREG:
			if (action) {
				BP_VREG_ON();
			} else {
				BP_VREG_OFF();
			}
			break;
		case FEATURE_PULLUP:
#if defined (BUSPIRATEV3)
			if (action) {
				BP_PULLUP_ON();
			} else {
				BP_PULLUP_OFF();
			}
#endif
			break;
#if defined (BUSPIRATEV3)
		case FEATURE_TRST:
			OOCD_TRST=action;
			break;
#endif
		case FEATURE_SRST:
			OOCD_SRST=action;
			break;
		default:
			break;
	}
}

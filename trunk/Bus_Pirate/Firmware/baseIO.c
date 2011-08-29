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
#include "globals.h"
#include "busPirateCore.h"//need access to bpConfig
extern struct _bpConfig bpConfig; //holds persistant bus pirate settings (see base.h) need hardware version info

//add byte to buffer, pause if full
//uses PIC 4 byte UART FIFO buffer

#if defined (BUSPIRATEV4)
//this struct buffers the USB input because the stack doesn't like 1 byte reads
static unsigned char lock = 0, fcnt = 0;
static struct _usbbuffer {
    unsigned char inBuf[64];
    unsigned char cnt;
    unsigned char rdptr;
} ubuf;

extern BYTE cdc_In_buffer[64];
BYTE CDC_In_count=0;
extern BYTE *InPtr;
extern BYTE *OutPtr;

//USB output buffer
#define USB_OUT_BUF 64
unsigned char buf[USB_OUT_BUF];
unsigned char uartincnt = 0;

void _T1Interrupt(void);
//void usbbufservice(void);
//unsigned char usbbufgetbyte(unsigned char* c);
#endif

//echo ASCII 0 or 1, given unsigned char c

void bpEchoState(unsigned int c) {
    if (c) UART1TX('1');
    else UART1TX('0');
}

//
//
//functions for terminal output of various types and lengths
//
//

//Write a string to the user terminal

void bpWstring(char *s) {
    char c;
    while ((c = *s++)) UART1TX(c);
}

//write a string to the user terminal, finish with a line break

void bpWline(char *s) {
    char c;
    while ((c = *s++)) UART1TX(c);
    UART1TX(0x0d);
    UART1TX(0x0a);

}

//output an 8bit/byte binary value to the user terminal

void bpWbin(unsigned char c) {
    unsigned char i, j;
    j = 0b10000000;

    bpWstring("0b");

    for (i = 0; i < 8; i++) {
        if (c & j) {
            UART1TX('1');
        } else {
            UART1TX('0');
        }
        j >>= 1;
    }

}

//output an 32bit/long decimal value to the user terminal

void bpWlongdec(unsigned long l) {
    unsigned long c, m;
    unsigned char j, k = 0;

    c = 100000000;
    for (j = 0; j < 8; j++) {
        m = l / c;
        if (k || m) {
            UART1TX(m + '0');
            l = l - (m * c);
            k = 1;
        }
        c /= 10;
    }
    UART1TX(l + '0');

}

// userfriendly printing of looooonng ints

void bpWlongdecf(unsigned long l) {
    unsigned long int temp;
    int mld, mil;

    mld = 0;
    mil = 0;
    temp = 0;

    if (l >= 1000000) {
        temp = l / 1000000;
        bpWintdec(temp);
        UART1TX(',');
        l %= 1000000;
        if (l < 1000) bpWstring("000,");
        mld = 1;
        mil = 1;
    }
    if (l >= 1000) {
        temp = l / 1000;
        if (temp > 100) {
            bpWintdec(temp);
        } else if (mld) {
            if (temp > 10) {
                UART1TX('0'); // 1 leading zero
            } else {
                bpWstring("00");
            }
            bpWintdec(temp);
        } else bpWintdec(temp);
        UART1TX(',');
        l %= 1000;
        mil = 1;
    }
    if (l > 100) {
        bpWintdec(l);
    } else if (mil) {
        if (l > 10) {
            UART1TX('0'); // 1 leading zero
        } else {
            bpWstring("00");
        }
        bpWintdec(l);
    } else bpWintdec(l);
}


//output an 16bit/integer decimal value to the user terminal

void bpWintdec(unsigned int i) {
    unsigned int c, m;
    unsigned char j, k = 0;

    c = 10000;
    for (j = 0; j < 4; j++) {
        m = i / c;
        if (k || m) {
            UART1TX(m + '0');
            i = i - (m * c);
            k = 1;
        }
        c /= 10;
    }
    UART1TX(i + '0');
}

//output an 8bit/byte decimal value to the user terminal

void bpWdec(unsigned char c) {
    unsigned char d, j, m, k = 0;

    d = 100;
    for (j = 0; j < 2; j++) {
        m = c / d;
        if (k || m) {
            UART1TX(m + '0');
            c = c - (m * d);
            k = 1;
        }
        d /= 10;
    }
    UART1TX(c + '0');
}

//output an 8bit/byte hex value to the user terminal
const unsigned char HEXASCII[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

void bpWhex(unsigned int c) {
    unsigned int b;

    bpWstring("0x");
    b = (c >> 4) & 0x0F;
    UART1TX(HEXASCII[b]);
    b = c & 0x0F;
    UART1TX(HEXASCII[b]);
    return;
}

void bpWhexBuf(unsigned int c) {
    unsigned int b;

    UARTbuf('0');
    UARTbuf('x');
    b = (c >> 4) & 0x0F;
    UARTbuf(HEXASCII[b]);
    b = c & 0x0F;
    UARTbuf(HEXASCII[b]);
    return;
}

// output a 16bit hex value to the user terminal

void bpWinthex(unsigned int c) {
    unsigned int b;

    bpWstring("0x");
    b = (c >> 12) & 0x0F;
    UART1TX(HEXASCII[b]);
    b = (c >> 8) & 0x0F;
    UART1TX(HEXASCII[b]);
    b = (c >> 4) & 0x0F;
    UART1TX(HEXASCII[b]);
    b = c & 0x0F;
    UART1TX(HEXASCII[b]);
    return;
}


//print an ADC measurement in decimal form

void bpWvolts(const unsigned int adc) {
    unsigned char c;

    // input voltage is divided by two and compared to 3.3V
    // volt      = adc / 1024 * 3.3V * 2
    // centivolt = adc / 1024 * 3.3 * 2 * 100 = adc * 165 / 256
    // This is approximately (adc * 29 / 45), making the calculation
    // fit in an unsigned int. The error is less than 1mV.
    const unsigned int centivolt = (adc * 29) / 45;

    bpWdec(centivolt / 100);

    UART1TX('.');

    c = centivolt % 100;

    if (c < 10) // need extra zero?
        UART1TX('0');

    bpWdec(centivolt % 100);
}



// Read the lower 16 bits from programming flash memory

unsigned int bpReadFlash(unsigned int page, unsigned int addr) {
    unsigned int tblold;
    unsigned flash;

    tblold = TBLPAG;
    TBLPAG = page;
    flash = (__builtin_tblrdh(addr) << 8) | __builtin_tblrdl(addr);
    TBLPAG = tblold;

    return flash;
}

#if defined (BUSPIRATEV2) || defined (BUSPIRATEV1A)
//
//
// Base user terminal UART functions
//
//


//Initialize the terminal UART for the speed currently set in bpConfig.termSpeed
static unsigned int UARTspeed[] = {13332, 3332, 1666, 832, 416, 207, 103, 68, 34,}; //BRG:300,1200,2400,4800,9600,19200,38400,57600,115200

void InitializeUART1(void) { // if termspeed==9 it is custom
    // dunno if the additional settings are needed but justin kaas!
    if (bpConfig.termSpeed != 9) U1BRG = UARTspeed[bpConfig.termSpeed]; //13332=300, 1666=2400, 416=9600, 34@32mhz=115200....
    U1MODE = 0;
    U1MODEbits.BRGH = 1;
    U1STA = 0;
    U1MODEbits.UARTEN = 1;
    U1STAbits.UTXEN = 1;
    IFS0bits.U1RXIF = 0;
}
#define UART1TXRdy() U1STAbits.UTXBF

unsigned char UART1RXRdy(void) {
    return U1STAbits.URXDA;
}

//new UART ring buffer
//uses user terminal input buffer to buffer UART output
//any existing user input will be destroyed
//best used for binary mode and sniffers
//static struct _UARTRINGBUF{
static unsigned int writepointer;
static unsigned int readpointer;
//}ringBuf;

void UARTbufSetup(void) {
    //setup ring buffer pointers
    readpointer = 0;
    writepointer = 1;
    bpConfig.overflow = 0;
}

void UARTbufService(void) {
    unsigned int i;
    if (U1STAbits.UTXBF == 0) {//check first for free slot

        i = readpointer + 1;
        if (i == TERMINAL_BUFFER) i = 0; //check for wrap
        if (i == writepointer) return; //buffer empty,
        readpointer = i;
        U1TXREG = bpConfig.terminalInput[readpointer]; //move a byte to UART
    }
}

void UARTbufFlush(void) {
    unsigned int i;

    while (1) {
        i = readpointer + 1;
        if (i == TERMINAL_BUFFER) i = 0; //check for wrap
        if (i == writepointer) return; //buffer empty,

        if (U1STAbits.UTXBF == 0) {//free slot, move a byte to UART
            readpointer = i;
            U1TXREG = bpConfig.terminalInput[readpointer];
        }
    }
}

void UARTbuf(char c) {
    if (writepointer == readpointer) {
        BP_LEDMODE = 0; //drop byte, buffer full LED off
        bpConfig.overflow = 1;
    } else {
        bpConfig.terminalInput[writepointer] = c;
        writepointer++;
        if (writepointer == TERMINAL_BUFFER) writepointer = 0; //check for wrap
    }
}

//get a byte from UART

unsigned char UART1RX(void) {
    while (U1STAbits.URXDA == 0);
    return U1RXREG;
}

void WAITTXEmpty(void) {
    while (U1STAbits.TRMT == 0);
}
//add byte to buffer, pause if full
//uses PIC 4 byte UART FIFO buffer

void UART1TX(char c) {
    if (bpConfig.quiet) return;
    while (U1STAbits.UTXBF == 1); //if buffer is full, wait
    U1TXREG = c;
}

void UART1Speed(unsigned char brg) {
    U1BRG = brg;
}

unsigned char CheckCommsError(void) {
    return U1STAbits.OERR; //check for user terminal buffer overflow error
}

void ClearCommsError(void) {
    U1STA &= (~0b10); //clear overrun error if exists  
}

/* interrupt transfer related stuff */
unsigned char __attribute__((section(".bss.filereg"))) * UART1RXBuf;
unsigned int __attribute__((section(".bss.filereg"))) UART1RXToRecv;
unsigned int __attribute__((section(".bss.filereg"))) UART1RXRecvd;
unsigned char __attribute__((section(".bss.filereg"))) * UART1TXBuf;
unsigned int __attribute__((section(".bss.filereg"))) UART1TXSent;
unsigned int __attribute__((section(".bss.filereg"))) UART1TXAvailable;

void UART1TXInt() {
    if (IEC0bits.U1TXIE == 1)
        return;
    if (UART1TXAvailable == UART1TXSent)
        return;

    while (U1STAbits.UTXBF == 1); //if buffer is full, wait

    IFS0bits.U1TXIF = 0;

    IEC0bits.U1TXIE = 1;

    U1TXREG = UART1TXBuf[UART1TXSent];
}

// UART1 ISRs

void __attribute__((interrupt, no_auto_psv)) _U1RXInterrupt(void) {
    UART1RXBuf[UART1RXRecvd] = U1RXREG;
    UART1RXRecvd++;

    if (UART1RXRecvd == UART1RXToRecv) {
        // disable UART1 RX interrupt
        IEC0bits.U1RXIE = 0;
    }

    IFS0bits.U1RXIF = 0;
}

void __attribute__((interrupt, no_auto_psv)) _U1TXInterrupt(void) {
    UART1TXSent++;
    if (UART1TXSent == UART1TXAvailable) {
        // if everything is sent  disale interrupts
        IEC0bits.U1TXIE = 0;
    } else {
        // feed next byte
        U1TXREG = UART1TXBuf[UART1TXSent];
    }

    IFS0bits.U1TXIF = 0;
}


#endif



#if defined(BUSPIRATEV4)

//#define ECHO_TEST //enables terminal echo test instead of bus pirate
void UART1TX(char c) {
#ifdef ECHO_TEST
return;
#endif
    if (bpConfig.quiet) return;
    lock = 1;
    *InPtr = c;
    InPtr++;
    CDC_In_count++;
    if (CDC_In_count > 62) {//62
        SendCDC_In_ArmNext(CDC_In_count);
        FAST_usb_handler();
    }
    lock = 0;
    //setup timer to throw data if the buffer doesn't fill
    fcnt = 0;
}

void UARTbuf(char c) {
    UART1TX(c);
}

void WAITTXEmpty(void) {
    WaitInReady();
}

unsigned char UART1TXRdy(void) {
    WaitInReady();
    return getInReady();
}

//is data available in RX buffer?
unsigned char UART1RXRdy(void) {
    return(usbbufservice()); //service USB buffer system
}

//get a byte from UART

unsigned char UART1RX(void) {
    unsigned char c = 0;

	while(!UART1RXRdy());

 	usbbufgetbyte(&c);
#ifdef ECHO_TEST
   *InPtr = c;
    InPtr++;
    CDC_In_count++;
    if (CDC_In_count > 62) {//62
        SendCDC_In_ArmNext(CDC_In_count);
        FAST_usb_handler();
		CDC_In_count=0;
    }
#endif

	return c;
}

void UARTbufSetup(void) {
    // Not required for USB
}

void UARTbufService(void) {
    // serial TX/IN Not required for USB
}

void UARTbufFlush(void) {
    WaitInReady();
    SendCDC_In_ArmNext(CDC_In_count);
    fcnt = 0;
}

unsigned char CheckCommsError(void) {
    return 0; //check for user terminal buffer overflow error
}

void ClearCommsError(void) {
    //clear overrun error if exists  
}

void InitializeUART1(void) {
}

void UART1Speed(unsigned char brg) {
}

//Interrupt Remap method 1:  Using direct interrupt address

void __attribute__((interrupt, address(0xF00), no_auto_psv)) _T1Interrupt() {
    IFS0bits.T1IF = 0;
	usb_handler();
    if (CDC_In_count > 0) {
        if (lock == 0 && fcnt > 5 && getInReady()) {
            SendCDC_In_ArmNext(CDC_In_count);
            CDC_In_count = 0;
            fcnt = 0;
        } else {
            fcnt++;
        }
    }
}

#endif




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

#include "SPI.h"
#include "base.h"
#include "busPirateCore.h"
#include "binIOhelpers.h"

#include "procMenu.h"		// for the userinteraction subs

//#define USE_SPICS //the CS hardware pin on silicone REV 3 doesn't work, optionally enable it here

#ifdef BP_USE_HWSPI

//direction registers
#define SPIMOSI_TRIS 	BP_MOSI_DIR	
#define SPICLK_TRIS 	BP_CLK_DIR	
#define SPIMISO_TRIS 	BP_MISO_DIR	
#define SPICS_TRIS 		BP_CS_DIR	

//pin control registers
#define SPIMOSI 		BP_MOSI
#define SPICLK 			BP_CLK	
#define SPIMISO 		BP_MISO	
#define SPICS 			BP_CS	

//open drain control registers for OUTPUT pins
#define SPIMOSI_ODC 		BP_MISO_ODC	
#define SPICLK_ODC 			BP_CLK_ODC	
#define SPICS_ODC 			BP_CS_ODC	

#define SPICS_RPIN		BP_CS_RPIN

extern struct _modeConfig modeConfig;
extern struct _command bpCommand;
extern struct _bpConfig bpConfig; //we use the big buffer

void binSPIversionString(void);
//void spiSetup(unsigned char spiSpeed);
//void spiDisable(void);
//unsigned char spiWriteByte(unsigned char c);
void spiSlaveDisable(void);
void spiSlaveSetup(void);
void spiSniffer(unsigned char csState, unsigned char termMode);

struct _SPI {
    unsigned char ckp : 1;
    unsigned char cke : 1;
    unsigned char smp : 1;
    //	unsigned char wwr:1;
    unsigned char csl : 1; // to /CS or  not to CS
} spiSettings;

static unsigned char SPIspeed[] = {0b00000, 0b11000, 0b11100, 0b11101}; //30,125,250,1000khz; datasheet pg 142

/*
// move into a .h or other .c??? 
int getnumber(int def, int max); // everything to make the compiler happy *dubbelzucht*
int getint(void);
int getrepeat(void);
void consumewhitechars(void);
extern int cmderror;
 */

void SPIstartr(void) {
    modeConfig.wwr = 1;
    if (spiSettings.csl) {
        SPICS = 0;
    } else {
        SPICS = 1;
    }
    ///bpWmessage(MSG_CS_ENABLED);
    if (spiSettings.csl) UART1TX('/');
    BPMSG1159;
}

void SPIstart(void) {
    modeConfig.wwr = 0;
    if (spiSettings.csl) {
        SPICS = 0;
    } else {
        SPICS = 1;
    }
    //bpWmessage(MSG_CS_ENABLED);
    if (spiSettings.csl) UART1TX('/');
    BPMSG1159;
}

void SPIstop(void) {
    if (spiSettings.csl) {
        SPICS = 1;
    } else {
        SPICS = 0;
    }

    //bpWmessage(MSG_CS_DISABLED);
    if (spiSettings.csl) UART1TX('/');
    BPMSG1160;
}

unsigned int SPIread(void) {
    return (spiWriteByte(0xff));
}

unsigned int SPIwrite(unsigned int c) {
    unsigned char r;

    r = spiWriteByte(c);
    if (modeConfig.wwr == 1) {
        return r;
    }
    //FIXME what to return if wwr=0? we need an uint here
    return 0; //JTR just to get rid of the warning msg
}

void SPIsettings(void) { //bpWstring("SPI (spd ckp ske smp hiz)=( ");
    BPMSG1191;
    bpWdec((modeConfig.speed + 1));
    bpSP;
    bpWdec(spiSettings.ckp);
    bpSP;
    bpWdec(spiSettings.cke);
    bpSP;
    bpWdec(spiSettings.smp);
    bpSP;
    bpWdec(spiSettings.csl);
    bpSP;
    bpWdec(modeConfig.HiZ);
    bpSP;
    //bpWline(")\r\n");
    BPMSG1162;
}

void SPIsetup(void) {
    int speed, clkpol, clkedge, sample, output, cslow;

    consumewhitechars();
    speed = getint();
    consumewhitechars();
    clkpol = getint();
    consumewhitechars();
    clkedge = getint();
    consumewhitechars();
    sample = getint();
    consumewhitechars();
    cslow = getint();
    consumewhitechars();
    output = getint();

    //	bpWdec(speed); bpSP;
    //	bpWdec(clkpol); bpSP;
    //	bpWdec(clkedge); bpSP;
    //	bpWdec(sample); bpSP;
    //	bpWdec(output); bpBR;

    // check for userinput (and sanitycheck it!!)
    if ((speed > 0) && (speed <= 4)) {
        modeConfig.speed = speed - 1;
    } else {
        speed = 0; // when speed is 0 we ask the user
    }

    if ((clkpol > 0) && (clkpol <= 2)) {
        spiSettings.ckp = clkpol - 1;
    } else {
        speed = 0; // when speed is 0 we ask the user
    }

    if ((clkedge > 0) && (clkedge <= 2)) {
        spiSettings.cke = clkedge - 1;
    } else {
        speed = 0; // when speed is 0 we ask the user
    }

    if ((sample > 0) && (sample <= 2)) {
        spiSettings.smp = sample - 1;
    } else {
        speed = 0; // when speed is 0 we ask the user
    }

    if ((cslow > 0) && (cslow <= 2)) {
        spiSettings.csl = (cslow - 1);
    } else {
        speed = 0; // when speed is 0 we ask the user
    }


    if ((output > 0) && (output <= 2)) {
        modeConfig.HiZ = (~(output - 1));
    } else {
        speed = 0; // when speed is 0 we ask the user
    }


    if (speed == 0) // no (valid) cmdline options found
    {
        cmderror = 0; // reset errorflag because of no cmdlineinput

        //bpWstring("Set speed:\x0D\x0A 1. 30KHz\x0D\x0A 2. 125KHz\x0D\x0A 3. 250KHz\x0D\x0A 4. 1MHz\x0D\x0A");
        //bpWline(OUMSG_SPI_SPEED);
        BPMSG1187;
        //modeConfig.speed=(bpUserNumberPrompt(1, 4, 1)-1);
        modeConfig.speed = getnumber(1, 1, 4, 0) - 1;

        //bpWstring("Clock polarity:\x0D\x0A 1. Idle low *default\x0D\x0A 2. Idle high\x0D\x0A");
        //bpWmessage(MSG_OPT_CKP);
        BPMSG1188;
        //spiSettings.ckp=(bpUserNumberPrompt(1, 2, 1)-1);
        spiSettings.ckp = getnumber(1, 1, 2, 0) - 1;

        //bpWstring("Output clock edge:\x0D\x0A 1. Idle to active\x0D\x0A 2. Active to idle *default\x0D\x0A");
        //bpWmessage(MSG_OPT_CKE);
        BPMSG1189;
        //spiSettings.cke=(bpUserNumberPrompt(1, 2, 2)-1);
        spiSettings.cke = getnumber(2, 1, 2, 0) - 1;

        //bpWstring("Input sample phase:\x0D\x0A 1. Middle *default\x0D\x0A 2. End\x0D\x0A");
        //bpWmessage(MSG_OPT_SMP);
        BPMSG1190;
        //spiSettings.smp=(bpUserNumberPrompt(1, 2, 1)-1);
        spiSettings.smp = getnumber(1, 1, 2, 0) - 1;

        //bpWline("CS:\r\n 1. CS\r\n 2. /CS *default");
        BPMSG1253;
        spiSettings.csl = getnumber(2, 1, 2, 0) - 1;

        //bpWmessage(MSG_OPT_OUTPUT_TYPE);
        BPMSG1142;
        //modeConfig.HiZ=(~(bpUserNumberPrompt(1, 2, 1)-1));
        modeConfig.HiZ = (~(getnumber(1, 1, 2, 0) - 1));
    } else {
        SPIsettings();
    }
    modeConfig.wwr = 0;
    //do SPI peripheral setup
    spiSetup(SPIspeed[modeConfig.speed]);

    // set cs the way the user wants
    SPICS = spiSettings.csl;
}

void SPIcleanup(void) {
    spiDisable();
}

void SPImacro(unsigned int macro) {

    switch (macro) {
        case 0:
            //bpWline(OUMSG_SPI_MACRO_MENU);
            BPMSG1192;
            break;
        case 1://sniff CS low
            BPMSG1071; //moved to a more generic message
            BPMSG1250;
            spiSniffer(0, 1); //configure for terminal mode
            break;
        case 2://sniff all
            BPMSG1071; //moved to a more generic message
            BPMSG1250;
            spiSniffer(1, 1); //configure for terminal mode
            break;
        case 3: //sniff CS high
            break;
        case 10:
            spiSettings.ckp = 0;
            goto SPImacro_settings_cleanup;
        case 11:
            spiSettings.ckp = 1;
            goto SPImacro_settings_cleanup;
        case 12:
            spiSettings.cke = 0;
            goto SPImacro_settings_cleanup;
        case 13:
            spiSettings.cke = 1;
            goto SPImacro_settings_cleanup;
        case 14:
            spiSettings.smp = 0;
            goto SPImacro_settings_cleanup;
        case 15:
            spiSettings.smp = 1;
SPImacro_settings_cleanup:
            SPI1CON1bits.CKP = spiSettings.ckp;
            SPI1CON1bits.CKE = spiSettings.cke;
            SPI1CON1bits.SMP = spiSettings.smp;
            SPIsettings();
            break;
        default:
            //bpWmessage(MSG_ERROR_MACRO);
            BPMSG1016;
    }
}

void SPIpins(void) {
	#if defined(BUSPIRATEV4)
        BPMSG1258; //bpWline("CS\tMISO\tCLK\tMOSI");
        #else
       	BPMSG1225; //bpWline("CLK\tMOSI\tCS\tMISO");
        #endif
}

void spiSetup(unsigned char spiSpeed) {
    SPI1STATbits.SPIEN = 0; //disable, just in case...

    //use open drain control register to
    //enable Hi-Z mode on hardware module outputs
    //inputs are already HiZ
    if (modeConfig.HiZ == 1) {
        SPIMOSI_ODC = 1;
        SPICLK_ODC = 1;
        SPICS_ODC = 1;
    } else {
        SPIMOSI_ODC = 0;
        SPICLK_ODC = 0;
        SPICS_ODC = 0;
    }

    // Inputs
    RPINR20bits.SDI1R = 7; //B7 MISO
    // Outputs
    RPOR4bits.RP9R = SDO1_IO; //B9 MOSI
    RPOR4bits.RP8R = SCK1OUT_IO; //B8 CLK

    SPICS = 1; //B6 cs high
    SPICS_TRIS = 0; //B6 cs output

    //pps configures pins and this doesn't really matter....
    SPICLK_TRIS = 0; //B8 sck output
    SPIMISO_TRIS = 1; //B7 SDI input
    SPIMOSI_TRIS = 0; //B9 SDO output

    /* CKE=1, CKP=0, SMP=0 */
    SPI1CON1 = spiSpeed; //(SPIspeed[modeConfig.speed]); // CKE (output edge) active to idle, CKP idle low, SMP data sampled middle of output time.
    SPI1CON1bits.MSTEN = 1;
    SPI1CON1bits.CKP = spiSettings.ckp;
    SPI1CON1bits.CKE = spiSettings.cke;
    SPI1CON1bits.SMP = spiSettings.smp;
    SPI1CON2 = 0;
    SPI1STAT = 0; // clear SPI
    SPI1STATbits.SPIEN = 1;
}

void spiDisable(void) {
    SPI1STATbits.SPIEN = 0;
    RPINR20bits.SDI1R = 0b11111; //B7 MISO
    RPOR4bits.RP9R = 0; //B9 MOSI
    RPOR4bits.RP8R = 0; //B8 CLK
    //disable all open drain control register bits
    SPIMOSI_ODC = 0;
    SPICLK_ODC = 0;
    SPICS_ODC = 0;
    //make all input maybe???
}

unsigned char spiWriteByte(unsigned char c) {

    SPI1BUF = c;
    while (!IFS0bits.SPI1IF);
    c = SPI1BUF;
    IFS0bits.SPI1IF = 0;
    return c;
}

//
//
//	SPI Sniffer 
//
//
#define USE_SPICS //enabled SPI CS pin

void spiSniffer(unsigned char csState, unsigned char termMode) {
    unsigned char c, lastCS;

spiSnifferStart:
    lastCS = 1;

    UARTbufSetup();
    spiDisable();
    spiSlaveSetup();

    if (csState == 0) { //mode 0, use CS pin
        SPI1CON1bits.SSEN = 1; //CS pin active
        SPI2CON1bits.SSEN = 1; //CS pin active
    }

    if (csState < 2) { //mode 0 & 1, always on
        SPI1STATbits.SPIEN = 1;
        SPI2STATbits.SPIEN = 1;
    }

    while (1) {

        //detect when CS changes. works independently of the data interrupts
        if (lastCS == 0 && SPICS == 1) {
            UARTbuf(']'); //bpWBR; //cs disabled
            lastCS = 1;
        }

        if (SPI1STATbits.SRXMPT == 0 && SPI2STATbits.SRXMPT == 0) {//rx buffer NOT empty, get and display byte
            c = SPI1BUF;

            if (lastCS == 1) {
                UARTbuf('['); //bpWBR; //CS enabled
                lastCS = 0; //SPICS;
            }

            if (termMode) { //show hex output in terminal mode
                bpWhexBuf(c);
            } else { //escaped byte value in binary mode
                UARTbuf('\\');
                UARTbuf(c);
            }

            c = SPI2BUF;

            if (termMode) { //show hex output in terminal mode
                UARTbuf('('); //only show the () in terminal mode
                bpWhexBuf(c);
                UARTbuf(')');
            } else { //binary mode
                UARTbuf(c);
            }

        }

        if (SPI1STATbits.SPIROV == 1 || SPI2STATbits.SPIROV == 1 || bpConfig.overflow == 1) {//we weren't fast enough, buffer overflow

            if (bpConfig.overflow == 0) UARTbufFlush();
            SPI1STAT = 0;
            SPI2STAT = 0;

            if (termMode) {
                bpWline("Couldn't keep up");
                goto spiSnifferStart;
            }

            BP_LEDMODE = 0;
            break;
        }

        UARTbufService();
        if (UART1RXRdy() == 1) {//any key pressed, exit
            c = UART1RX();
            /* JTR usb port; */;
            if (termMode) bpBR; //fixed in 5.1: also sent br to binmode
            break;
        }
    }
    spiSlaveDisable();

    spiSetup(SPIspeed[modeConfig.speed]);
}

//configure both SPI units for slave mode on different pins
//use current settings

void spiSlaveSetup(void) {
    //	unsigned char c;
    //SPI1STATbits.SPIEN=0; //SPI module off

    //assign pins for SPI slave mode
    SPICS_TRIS = 1; //B6 cs input
    SPICLK_TRIS = 1; //B8 sck input
    SPIMISO_TRIS = 1; //B7 SDI input
    SPIMOSI_TRIS = 1; //b9 SDO input

    //#ifdef USE_SPICS
    RPINR21bits.SS1R = 6; //SPICS_RPIN; //assign CS function to bus pirate CS pin
    RPINR23bits.SS2R = 6;
    //#endif
    RPINR20bits.SDI1R = 9; //B9 MOSI
    RPINR20bits.SCK1R = 8; //SPICLK_RPIN; //assign SPI1 CLK input to bus pirate CLK pin
    RPINR22bits.SDI2R = 7; //B7 MiSo
    RPINR22bits.SCK2R = 8; //SPICLK_RPIN; //assign SPI2 CLK input to bus pirate CLK pin

    //clear old SPI settings first
    SPI1CON1 = (SPIspeed[modeConfig.speed]); // CKE (output edge) active to idle, CKP idle low, SMP data sampled middle of output time.
    SPI1CON1bits.CKP = spiSettings.ckp;
    SPI1CON1bits.CKE = spiSettings.cke;
    //SPI1CON1bits.SMP=spiSettings.smp;
    SPI1CON2 = 0;
    SPI1STAT = 0; // clear SPI

    SPI2CON1 = (SPIspeed[modeConfig.speed]); // CKE (output edge) active to idle, CKP idle low, SMP data sampled middle of output time.
    SPI2CON1bits.CKP = spiSettings.ckp;
    SPI2CON1bits.CKE = spiSettings.cke;
    SPI2CON2 = 0;
    SPI2STAT = 0; // clear SPI

    //To set up the SPI module for the Enhanced Buffer
    //Slave mode of operation:
    //1. Clear the SPIxBUF register.
    SPI1BUF = 0;
    SPI2BUF = 0;
    //3. Write the desired settings to the SPIxCON1 and SPIxCON2 registers with MSTEN (SPIxCON1<5>) = 0.
    //4. Clear the SMP bit.
    SPI1CON1bits.SMP = 0;
    SPI2CON1bits.SMP = 0;

    //	SPI1CON1bits.SSEN=1; //CS pin active
    //	SPI2CON1bits.SSEN=1; //CS pin active

    SPI1CON1bits.DISSDO = 1; //Disable SDO pin in slave mode
    SPI1CON1bits.MSTEN = 0;
    SPI2CON1bits.DISSDO = 1; //Disable SDO pin in slave mode
    SPI2CON1bits.MSTEN = 0;
    //5. If the CKE bit is set, then the SSEN bit must be set, thus enabling the SSx pin.
    //6. Clear the SPIROV bit (SPIxSTAT<6>).
    SPI1STATbits.SPIROV = 0;
    SPI2STATbits.SPIROV = 0;
    //7. Select Enhanced Buffer mode by setting the SPIBEN bit (SPIxCON2<0>).
    SPI1CON2bits.SPIBEN = 1;
    SPI2CON2bits.SPIBEN = 1;
    //8. Enable SPI operation by setting the SPIEN bit(SPIxSTAT<15>).
    //SPI1STATbits.SPIEN=1;

}

void spiSlaveDisable(void) {
    SPI1STATbits.SPIEN = 0; //SPI module off
    SPI1CON1bits.DISSDO = 0; //restore SDO pin
    SPI2STATbits.SPIEN = 0; //SPI module off
    SPI2CON1bits.DISSDO = 0; //restore SDO pin
    //#ifdef USE_SPICS
    RPINR21bits.SS1R = 0b11111; //assign CS input to none
    RPINR23bits.SS2R = 0b11111; //assign CS input to none
    //#endif
    RPINR20bits.SDI1R = 0b11111;
    RPINR20bits.SCK1R = 0b11111; //assign CLK input to none
    RPINR22bits.SDI2R = 0b11111;
    RPINR22bits.SCK2R = 0b11111; //assign CLK input to none
    //SPI1CON1bits.SMP=spiSettings.smp;	//restore SMP setting (done in spiSetup()
}

/*
rawSPI mode:
 * 00000000 � Enter raw bitbang mode, reset to raw bitbang mode
 * 00000001 � SPI mode/rawSPI version string (SPI1)
 * 00000010 � CS low (0)
 * 00000011 � CS high (1)
 * Sniffers
 * 0001xxxx � Bulk SPI transfer, send 1-16 bytes (0=1byte!)
 * 0100wxyz � Configure peripherals, w=power, x=pullups, y=AUX, z=CS
 * 01100xxx � Set SPI speed, 30, 125, 250khz; 1, 2, 2.6, 4, 8MHz
 * 1000wxyz � SPI config, w=output type, x=idle, y=clock edge, z=sample
	
 */
static unsigned char binSPIspeed[] = {0b00000, 0b11000, 0b11100, 0b11101, 0b00011, 0b01000, 0b10000, 0b11000}; //00=30,01=125,10=250,11=1000khz, 100=2mhz,101=2.667mhz,  110=4mhz, 111=8mhz; datasheet pg 142

void binSPIversionString(void) {
    bpWstring("SPI1");
}

void binSPI(void) {
    static unsigned char inByte, rawCommand, i;
    unsigned int j, fw, fr;

    //useful default values
    /* CKE=1, CKP=0, SMP=0 */
    modeConfig.speed = 1;
    spiSettings.ckp = 0;
    spiSettings.cke = 1;
    spiSettings.smp = 0;
    modeConfig.HiZ = 1;
    spiSetup(binSPIspeed[modeConfig.speed]); //start with 250khz (30,125,250,1000khz)
    binSPIversionString(); //1 - SPI setup and reply string

    while (1) {

        inByte = UART1RX(); /* JTR usb port; */ //grab it
        rawCommand = (inByte >> 4); //get command bits in seperate variable

        switch (rawCommand) {
            case 0://reset/setup/config commands
                switch (inByte) {
                    case 0://0, reset exit
                        spiDisable();
                        return; //exit
                        break;
                    case 1://1 - SPI setup and reply string
                        binSPIversionString();
                        break;
                    case 2:
                        IOLAT &= (~CS); //SPICS=0; //cs enable/low
                        UART1TX(1);
                        break;
                    case 3:
                        IOLAT |= CS; //SPICS=1; //cs disable/high
                        UART1TX(1);
                        break;
                    case 0b1101: //all traffic 13
                        UART1TX(1);
                        spiSniffer(1, 0);
                        break;
                    case 0b1110://cs low 14
                        UART1TX(1);
                        spiSniffer(0, 0);
                        break;
                        //case 0b1111://cs high
                        //	spiSniffer(1, 0);
                        //	UART1TX(1);
                        //	break;
                    case 4: //write-then-read, with !CS/CS
                    case 5: //write-then-read, NO CS!
                        //get the number of commands that will follow
                        fw = UART1RX();
                        /* JTR usb port; */; //get byte
                        fw = fw << 8;
                        fw |= UART1RX();
                        /* JTR usb port; */; //get byte

                        //get the number of reads to do
                        fr = UART1RX();
                        /* JTR usb port; */; //get byte
                        fr = fr << 8;
                        fr |= UART1RX();
                        /* JTR usb port; */; //get byte


                        //check length and report error
                        if (fw > TERMINAL_BUFFER || fr > TERMINAL_BUFFER) {
                            UART1TX(0);
                            break;
                        }

                        //get bytes
                        for (j = 0; j < fw; j++) {
                            bpConfig.terminalInput[j] = UART1RX();
                            /* JTR usb port; */;
                        }

                        if (inByte == 4) SPICS = 0;
                        for (j = 0; j < fw; j++) {
                            spiWriteByte(bpConfig.terminalInput[j]);
                        }
                        bpDelayUS(1);
                        for (j = 0; j < fr; j++) { //read bulk bytes from SPI
                            bpConfig.terminalInput[j] = spiWriteByte(0xff);
                        }
                        if (inByte == 4) SPICS = 1;

                        UART1TX(1); //send 1/OK

                        for (j = 0; j < fr; j++) { //send the read buffer contents over serial
                            UART1TX(bpConfig.terminalInput[j]);
                        }

                        break;
                    default:
                        UART1TX(0);
                        break;
                }
                break;
            case 0b0001://get x+1 bytes
                inByte &= (~0b11110000); //clear command portion
                inByte++; //increment by 1, 0=1byte
                UART1TX(1); //send 1/OK

                for (i = 0; i < inByte; i++) {
                    UART1TX(spiWriteByte(UART1RX()));
                }

                break;
            case 0b0100: //configure peripherals w=power, x=pullups, y=AUX, z=CS
                binIOperipheralset(inByte);
                UART1TX(1); //send 1/OK
                break;
            case 0b0110://set speed
                inByte &= (~0b11111000); //clear command portion
                modeConfig.speed = inByte;
                spiSetup(binSPIspeed[modeConfig.speed]); //resetup SPI
                UART1TX(1); //send 1/OK
                break;
            case 0b1000: //set SPI config
                //wxyz //w=HiZ(0)/3.3v(1), x=CKP idle (low=0), y=CKE clock edge (active to idle=1), z=SMP sample (middle=0)
                spiSettings.ckp = 0;
                spiSettings.cke = 0;
                spiSettings.smp = 0;
                modeConfig.HiZ = 0;
                if (inByte & 0b100) spiSettings.ckp = 1; //set idle
                if (inByte & 0b10) spiSettings.cke = 1; //set edge
                if (inByte & 0b1) spiSettings.smp = 1; //set sample time
                if ((inByte & 0b1000) == 0) modeConfig.HiZ = 1; //hiz output if this bit is 1
                spiSetup(binSPIspeed[modeConfig.speed]); //resetup SPI
                UART1TX(1); //send 1/OK
                break;
            default:
                UART1TX(0x00); //send 0/Error
                break;
        }//command switch
    }//while loop
}//function

#endif

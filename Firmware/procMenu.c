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
#include "base.h"
#include "AUXpin.h"
#include "busPirateCore.h"
#include "procMenu.h" //need our public versionInfo() function
#ifndef BUSPIRATEV1A
#include "selftest.h"
#endif
#include "binIO.h"
#include "SUMP.h"
#include "basic.h"

extern struct _bpConfig bpConfig;
extern struct _modeConfig modeConfig;
extern struct _command bpCommand;
extern proto protos[MAXPROTO];

#ifdef BUSPIRATEV4
extern volatile BYTE cdc_Out_len;
#endif

void walkdungeon(void);

void setMode(void); //change protocol/bus mode
void setDisplayMode(void); //user terminal number display mode dialog (eg HEX, DEC, BIN, RAW)
void setPullups(void); //pullup resistor dialog
void measureSupplyVoltages(void); //display supply voltage measurements for 3.3, 5, and vpullup
void setBitOrder(void); //LSB/MSB menu for modes that allow it
void setAltAuxPin(void); //configure AUX syntax to control AUX or CS pin
void setBaudRate(void); //configure user terminal side UART baud rate
void statusInfo(void); //display properties of the current bus mode (pullups, vreg, lsb, output type, etc)
void convert(void); //convert input to HEX/DEC/BIN
void pinDirection(unsigned int pin);
void pinState(unsigned int pin);
void pinStates(void);

#ifdef BUSPIRATEV4
void setPullupVoltage(void); // onboard Vpu selection
#endif

//global vars    move to bpconfig structure?
char cmdbuf[CMDBUFLEN];
unsigned int cmdend;
unsigned int cmdstart;
int cmderror;

//int currentproto;	// port to the other way :)

#define USRMACROS	5
#define USRMACROLEN	32

char usrmacros[USRMACROS][USRMACROLEN];
int usrmacro;

void serviceuser(void) {
    int cmd, stop;
    int newstart;
    int oldstart;
    unsigned int sendw, received;
    int repeat;
    unsigned char c;
    int temp;
    int temp2;
    int binmodecnt;
    int numbits;
    unsigned int tmpcmdend, histcnt, tmphistcnt;

    // init
    cmd = 0;
    cmdstart = 0;
    cmdend = 0;
    tmpcmdend = cmdend;
    histcnt = 0;
    tmphistcnt = 0;
    bpConfig.busMode = HIZ;
    temp2 = 0;
    cmderror = 0; // we don't want to start with error do we?
    binmodecnt = 0;

    for (repeat = 0; repeat < USRMACROS; repeat++) {
        for (temp = 0; temp < 32; temp++) {
            usrmacros[repeat][temp] = 0;
        }
    }
    usrmacro = 0;

    while (1) {
        bpWstring(protos[bpConfig.busMode].protocol_name);
        if (bpConfig.basic) { //bpWstring("(BASIC)");
            BPMSG1084;
        }
        bpWstring(">");
        while (!cmd) {
            if (usrmacro) {
                usrmacro--;
                temp = 0;
                while (usrmacros[usrmacro][temp]) {
                    cmdbuf[cmdend] = usrmacros[usrmacro][temp];
                    UART1TX(usrmacros[usrmacro][temp]);
                    cmdend++;
                    temp++;
                    cmdend &= CMDLENMSK;
                }
                usrmacro = 0;
            }

            while (!UART1RXRdy()) // as long as there is no user input poll periodicservice
            {
                if (modeConfig.periodicService == 1) {
                    if (protos[bpConfig.busMode].protocol_periodic()) // did we print something?
                    {
                        bpWstring(protos[bpConfig.busMode].protocol_name);
                        bpWstring(">");
                        if (cmdstart != cmdend) {
                            for (temp = cmdstart; temp != cmdend; temp++) {
                                UART1TX(cmdbuf[temp]);
                                temp &= CMDLENMSK;
                            }
                        }
                    }
                }
#ifdef BUSPIRATEV4
                if (!BP_BUTTON) { // button pressed
                } else { // button not pressed
                }
#endif
            }

            if (CheckCommsError()) { //check for user terminal buffer overflow error
                ClearCommsError();
                continue; //resume getting more user input
            } else {
                c = UART1RX(); //no error, process byte
            }

            switch (c) {
                case 0x08: // backspace(^H)
                    if (tmpcmdend != cmdstart) // not at begining?
                    {
                        if (tmpcmdend == cmdend) // at the end?
                        {
                            cmdend = (cmdend - 1) & CMDLENMSK;
                            cmdbuf[cmdend] = 0x00; // add end marker
                            tmpcmdend = cmdend; // update temp
                            bpWstring("\x08 \x08"); // destructive backspace ian !! :P
                        } else // not at end, left arrow used
                        {
                            repeat = 0; // use as temp, not valid here anyway
                            tmpcmdend = (tmpcmdend - 1) & CMDLENMSK;
                            bpWstring("\x1B[D"); // move left
                            for (temp = tmpcmdend; temp != cmdend; temp = (temp + 1) & CMDLENMSK) {
                                cmdbuf[temp] = cmdbuf[temp + 1];
                                if (cmdbuf[temp]) // not NULL
                                    UART1TX(cmdbuf[temp]);
                                else
                                    UART1TX(0x20);
                                repeat++;
                            }
                            cmdend = (cmdend - 1) & CMDLENMSK; // end pointer moves left one
                            bpWstring("\x1B["); // move left
                            bpWdec(repeat); // to original
                            bpWstring("D"); // cursor position
                        }
                    } else {
                        UART1TX(BELL); // beep, at begining
                    }
                    break;
                case 0x04: // delete (^D)
                case 0x7F: // delete key
                    if (tmpcmdend != cmdend) // not at the end
                    {
                        repeat = 0; // use as temp, not valid here anyway
                        for (temp = tmpcmdend; temp != cmdend; temp = (temp + 1) & CMDLENMSK) {
                            cmdbuf[temp] = cmdbuf[temp + 1];
                            if (cmdbuf[temp]) // not NULL
                                UART1TX(cmdbuf[temp]);
                            else
                                UART1TX(0x20);
                            repeat++;
                        }
                        cmdend = (cmdend - 1) & CMDLENMSK; // end pointer moves left one
                        bpWstring("\x1B["); // move left
                        bpWdec(repeat); // to original
                        bpWstring("D"); // cursor position
                    } else {
                        UART1TX(BELL); // beep, at end
                    }
                    break;
                case 0x1B: // escape
                    c = UART1RX(); // get next char
                    if (c == '[') // got CSI
                    {
                        c = UART1RX(); // get next char
                        switch (c) {
                            case 'D': // left arrow
                                goto left;
                                break;
                            case 'C': // right arrow
                                goto right;
                                break;
                            case 'A': // up arrow
                                goto up;
                                break;
                            case 'B': // down arrow
                                goto down;
                                break;
                            case '1': // VT100+ home key (example use in PuTTY)
                                c = UART1RX();
                                if (c == '~') goto home;
                                break;
                            case '4': // VT100+ end key (example use in PuTTY)
                                c = UART1RX();
                                if (c == '~') goto end;
                                break;
                        }
                    }
                    break;
left:
                case 0x02: // ^B (left arrow) or SUMP
                    if (binmodecnt >= 5) {
                        SUMP();
                        binmodecnt = 0; // do we get here or not?
                    } else // ^B (left arrow)
                    {
                        if (tmpcmdend != cmdstart) // at the begining?
                        {
                            tmpcmdend = (tmpcmdend - 1) & CMDLENMSK;
                            bpWstring("\x1B[D"); // move left
                        } else {
                            UART1TX(BELL); // beep, at begining
                        }
                    }
                    break;
right:
                case 0x06: // ^F (right arrow)
                    if (tmpcmdend != cmdend) // ^F (right arrow)
                    { // ensure not at end
                        tmpcmdend = (tmpcmdend + 1) & CMDLENMSK;
                        bpWstring("\x1B[C"); // move right
                    } else {
                        UART1TX(BELL); // beep, at end
                    }
                    break;
up:
                case 0x10: // ^P (up arrow)
                    tmphistcnt = 0; // reset counter
                    for (temp = (cmdstart - 1) & CMDLENMSK; temp != cmdend; temp = (temp - 1) & CMDLENMSK) {
                        if (!cmdbuf[temp] && cmdbuf[(temp - 1) & CMDLENMSK]) { // found previous entry, temp is old cmdend
                            tmphistcnt++;
                            if (tmphistcnt > histcnt) {
                                histcnt++;
                                if (cmdstart != cmdend) { // clear partially entered cmd line
                                    while (cmdend != cmdstart) {
                                        cmdbuf[cmdend] = 0x00;
                                        cmdend = (cmdend - 1) & CMDLENMSK;
                                    }
                                    cmdbuf[cmdend] = 0x00;
                                }
                                repeat = (temp - 1) & CMDLENMSK;
                                while (repeat != cmdend) {
                                    if (!cmdbuf[repeat]) {
                                        temp2 = (repeat + 1) & CMDLENMSK;
                                        /* start of old cmd */
                                        break;
                                    }
                                    repeat = (repeat - 1) & CMDLENMSK;
                                }
                                bpWstring("\x1B[2K\x0D"); // clear line, CR
                                bpWstring(protos[bpConfig.busMode].protocol_name);
                                if (bpConfig.basic) BPMSG1084;
                                bpWstring(">");
                                for (repeat = temp2; repeat != temp; repeat = (repeat + 1) & CMDLENMSK) {
                                    UART1TX(cmdbuf[repeat]);
                                    cmdbuf[cmdend] = cmdbuf[repeat];
                                    cmdend = (cmdend + 1) & CMDLENMSK;
                                }
                                cmdbuf[cmdend] = 0x00;
                                tmpcmdend = cmdend; // resync
                                break;
                            }
                        }
                    }
                    if (temp == cmdend) UART1TX(BELL); // beep, top
                    break;
down:
                case 0x0E: // ^N (down arrow)
                    tmphistcnt = 0; // reset counter
                    for (temp = (cmdstart - 1) & CMDLENMSK; temp != cmdend; temp = (temp - 1) & CMDLENMSK) {
                        if (!cmdbuf[temp] && cmdbuf[(temp - 1) & CMDLENMSK]) { // found previous entry, temp is old cmdend
                            tmphistcnt++;
                            if (tmphistcnt == (histcnt - 1)) {
                                histcnt--;
                                if (cmdstart != cmdend) { // clear partially entered cmd line
                                    while (cmdend != cmdstart) {
                                        cmdbuf[cmdend] = 0x00;
                                        cmdend = (cmdend - 1) & CMDLENMSK;
                                    }
                                    cmdbuf[cmdend] = 0x00;
                                }
                                repeat = (temp - 1) & CMDLENMSK;
                                while (repeat != cmdend) {
                                    if (!cmdbuf[repeat]) {
                                        temp2 = (repeat + 1) & CMDLENMSK;
                                        /* start of old cmd */
                                        break;
                                    }
                                    repeat = (repeat - 1) & CMDLENMSK;
                                }
                                bpWstring("\x1B[2K\x0D"); // clear line, CR
                                bpWstring(protos[bpConfig.busMode].protocol_name);
                                if (bpConfig.basic) BPMSG1084;
                                bpWstring(">");
                                for (repeat = temp2; repeat != temp; repeat = (repeat + 1) & CMDLENMSK) {
                                    UART1TX(cmdbuf[repeat]);
                                    cmdbuf[cmdend] = cmdbuf[repeat];
                                    cmdend = (cmdend + 1) & CMDLENMSK;
                                }
                                cmdbuf[cmdend] = 0x00;
                                tmpcmdend = cmdend; // resync
                                break;
                            }
                        }
                    }
                    if (temp == cmdend) {
                        if (histcnt == 1) {
                            bpWstring("\x1B[2K\x0D"); // clear line, CR
                            bpWstring(protos[bpConfig.busMode].protocol_name);
                            if (bpConfig.basic) BPMSG1084;
                            bpWstring(">");
                            while (cmdend != cmdstart) {
                                cmdbuf[cmdend] = 0x00;
                                cmdend = (cmdend - 1) & CMDLENMSK;
                            }
                            cmdbuf[cmdend] = 0x00;
                            tmpcmdend = cmdend; // resync
                            histcnt = 0;
                        } else UART1TX(BELL); // beep, top
                    }
                    break;
home:
                    case 0x01: // ^A (goto begining of line)
                    if (tmpcmdend != cmdstart) {
                        repeat = (tmpcmdend - cmdstart) & CMDLENMSK;
                        bpWstring("\x1B["); // move left
                        bpWdec(repeat); // to start
                        bpWstring("D"); // of command line
                        tmpcmdend = cmdstart;
                    } else {
                        UART1TX(BELL); // beep, at start
                    }
                    break;
end:
                    case 0x05: // ^E (goto end of line)
                    if (tmpcmdend != cmdend) {
                        repeat = (cmdend - tmpcmdend) & CMDLENMSK;
                        bpWstring("\x1B["); // move right
                        bpWdec(repeat); // to end
                        bpWstring("C"); // of command line
                        tmpcmdend = cmdend;
                    } else {
                        UART1TX(BELL); // beep, at end
                    }
                    break;
                case 0x0A: // Does any terminal only send a CR?
                case 0x0D: // Enter pressed (LF)
                    cmd = 1; // command received
                    histcnt = 0; // reset counter
                    cmdbuf[cmdend] = 0x00; // use to find history
                    cmdend = (cmdend + 1) & CMDLENMSK;
                    tmpcmdend = cmdend; // resync
                    bpWline("");
                    break;
                case 0x00:
                    binmodecnt++;
                    if (binmodecnt == 20) {
                        binBB();
#if defined (BUSPIRATEV4)
                        binmodecnt = 0; //no reset, cleanup manually 
                        goto bpv4reset; //versionInfo(); //and simulate reset for dependent apps (looking at you AVR dude!)
#endif
                    }
                    break;
                default:
                    if ((((cmdend + 1) & CMDLENMSK) != cmdstart) && (c >= 0x20) && (c < 0x7F)) { // no overflow and printable
                        if (cmdend == tmpcmdend) // adding to the end
                        {
                            UART1TX(c); // echo back
                            cmdbuf[cmdend] = c; // store char
                            cmdend = (cmdend + 1) & CMDLENMSK;
                            cmdbuf[cmdend] = 0x00; // add end marker
                            tmpcmdend = cmdend; // update temp
                        } else // not at end, left arrow used
                        {
                            repeat = (cmdend - tmpcmdend) & CMDLENMSK;
                            bpWstring("\x1B["); // move right
                            bpWdec(repeat); // to end
                            bpWstring("C"); // of line
                            temp = cmdend;
                            while (temp != ((tmpcmdend - 1) & CMDLENMSK)) {
                                cmdbuf[temp + 1] = cmdbuf[temp];
                                if (cmdbuf[temp]) // not NULL
                                {
                                    UART1TX(cmdbuf[temp]);
                                    bpWstring("\x1B[2D"); // left 2
                                }
                                temp = (temp - 1) & CMDLENMSK;
                            }
                            UART1TX(c); // echo back
                            cmdbuf[tmpcmdend] = c; // store char
                            tmpcmdend = (tmpcmdend + 1) & CMDLENMSK;
                            cmdend = (cmdend + 1) & CMDLENMSK;
                        }
                    } else {
                        UART1TX(BELL); // beep, overflow or non printable
                    } //default:
            } //switch(c)
        } //while(!cmd)

        newstart = cmdend;
        oldstart = cmdstart;
        cmd = 0;

        //		bpWline("\x0a\x0dreceived enter, processing input");
        //		bpWline("cmdbuf[]=");

        //		for(i=0; i<CMDBUFLEN; i++)						// print ringbuffer
        //		{	if(cmdbuf[i]) UART1TX(cmdbuf[i]);
        //				else UART1TX('.');
        //		}

        //		bpWline("");
        //		bpWstring("cmdstart = ");
        //		bpWinthex(cmdstart);
        //		bpWstring(" cmdend = ");
        //		bpWinthex(cmdend);
        //		bpWline("");

        stop = 0;
        cmderror = 0;

#ifdef BP_USE_BASIC
        if (bpConfig.basic) {
            basiccmdline();
            //bpWline("Ready.");
            BPMSG1085;
            stop = 1;
        }
#endif

        //		for(i=0; i<CMDBUFLEN; i++)						// print ringbuffer
        //		{	if(cmdbuf[i]) UART1TX(cmdbuf[i]);
        //				else UART1TX('.');
        //		}

        //		bpWline("");
        //		bpWstring("cmdstart = ");
        //		bpWinthex(cmdstart);
        //		bpWstring(" cmdend = ");
        //		bpWinthex(cmdend);
        //		bpWline("");
        unsigned char oldDmode=0;//temperarly holds the defaout display mode, while a differend display read is preformed
        unsigned char newDmode=0;
        while (!stop) {
            c = cmdbuf[cmdstart];

            switch (c) { // generic commands (not bus specific)
                case 'h': //bpWline("-command history");
#if defined(BP_ENABLE_HISTORY)
                    if (!cmdhistory()) {
                        oldstart = cmdstart;
                        newstart = cmdend;
                    }
#endif
                    break;

                case '?': //bpWline("-HELP");
                    printHelp();
                    break;
                case 'i': //bpWline("-Status info");
                    versionInfo(); //display hardware and firmware version string
                    if (bpConfig.busMode != HIZ) {
                        statusInfo();
                    }
                    break;
                case 'm': //bpWline("-mode change");
                    changemode();
                    break;
                case 'b': //bpWline("-terminal speed set");
                    setBaudRate();
                    break;
                case 'o': //bpWline("-data output set");
                    setDisplayMode();
                    break;
                case 'v': //bpWline("-check supply voltage");
                    pinStates();
                    //measureSupplyVoltages();
                    break;
                case 'f': //bpWline("-frequency count on AUX");
                    bpFreq();
                    break;
                case 'g': //bpWline("-frequency generate on AUX");
                    /*if((cmdbuf[(cmdstart+1)&CMDLENMSK])=='o')
                    {	cmdstart+=2;
                            cmdstart&=CMDLENMSK;
                            walkdungeon();
                            break;
                    }
                    else*/
                    if (bpConfig.busMode == HIZ) { //bpWmessage(MSG_ERROR_MODE);
                        BPMSG1088;
                    } else {
                        bpPWM();
                    }
                    break;
                case 'c': //bpWline("-aux pin assigment");
                    modeConfig.altAUX = 0;
                    //bpWmessage(MSG_OPT_AUXPIN_AUX);
                    BPMSG1086;
                    break;
                case 'C': //bpWline("-aux pin assigment");
                    modeConfig.altAUX = 1;
                    //bpWmessage(MSG_OPT_AUXPIN_CS);
                    BPMSG1087;
                    break;
#ifdef BUSPIRATEV4
                case 'k': modeConfig.altAUX = 2;
                    //bpWline("AUX1 selected");
                    BPMSG1263;
                    break;
                case 'K': modeConfig.altAUX = 3;
                    //bpWline("AUX2 selected");
                    BPMSG1264;
                    break;
#endif
                case 'L': //bpWline("-bit order set (MSB)");
                    //if(bpConfig.busMode==HIZ)
                    //{	//bpWmessage(MSG_ERROR_MODE);
                    //	BPMSG1088;
                    //}
                    //else
                    //{
                    modeConfig.lsbEN = 1;
                    BPMSG1124;
                    bpBR;
                    //}
                    break;
                case 'l': //bpWline("-bit order set (LSB)");
                    //if(bpConfig.busMode==HIZ)
                    //{	//bpWmessage(MSG_ERROR_MODE);`
                    //	BPMSG1088;
                    //}
                    //else
                    //{
                    modeConfig.lsbEN = 0;
                    BPMSG1123;
                    bpBR;
                    //}
                    break;
#ifndef BUSPIRATEV1A
                case 'p': //bpWline("-pullup resistors off");


                    //don't allow pullups on some modules. also: V0a limitation of 2 resistors
                    if (bpConfig.busMode == HIZ) { //bpWmessage(MSG_ERROR_MODE);
                        BPMSG1088;
                    } else {
                        BP_PULLUP_OFF(); //pseudofunction in hardwarevx.h
                        //								modeConfig.pullupEN=0;
                        //bpWmessage(MSG_OPT_PULLUP_OFF);
                        BPMSG1089;
                        bpBR;
                    }
                    break;
                case 'P': //bpWline("-pullup resistors on");
                    //don't allow pullups on some modules. also: V0a limitation of 2 resistors
                    if (bpConfig.busMode == HIZ) { //bpWmessage(MSG_ERROR_MODE);
                        BPMSG1088;
                    } else {
                        if (modeConfig.HiZ == 0) { //bpWmessage(MSG_ERROR_NOTHIZPIN);
                            BPMSG1209;
                        }
                        BP_PULLUP_ON(); //pseudofunction in hardwarevx.h
                        //								modeConfig.pullupEN=1;
                        //bpWmessage(MSG_OPT_PULLUP_ON);
                        BPMSG1091;
                        bpBR;

                        ADCON();
                        if (bpADC(BP_ADC_VPU) < 0x50) { //no pullup voltage detected
                            bpWline("Warning: no voltage on Vpullup pin");
                        }
                        ADCOFF();
                    }
                    break;
#endif
#ifdef BUSPIRATEV4
                case 'e': setPullupVoltage();
                    break;
#endif
                case '=': //bpWline("-HEX/BIN/DEC convertor");
                    cmdstart = (cmdstart + 1) & CMDLENMSK;
                    consumewhitechars();
                    temp = getint();
                    bpWhex(temp);
                    bpWstring(" = ");
                    bpWdec(temp);
                    bpWstring(" = ");
                    bpWbin(temp);
                    bpBR;
                    break;
                case '|': //bpWline("-HEX/BIN/DEC convertor");
                    cmdstart = (cmdstart + 1) & CMDLENMSK;
                    consumewhitechars();
                    temp = getint();
                    temp = bpRevByte((unsigned char) temp);
                    bpWhex(temp);
                    bpWstring(" = ");
                    bpWdec(temp);
                    bpWstring(" = ");
                    bpWbin(temp);
                    bpBR;
                    break;
#ifndef BUSPIRATEV1A 
                case '~': //bpWline("-selftest");
                    if (bpConfig.busMode == HIZ) {
                        selfTest(1, 1); //self test, showprogress in terminal
                    } else {
                        //bpWline(OUMSG_PM_SELFTEST_HIZ);
                        BPMSG1092;
                    }
                    break;
#endif
                case '#': //bpWline("-reset BP");
                    //removed confirmation in v5.9
                    //ruined AVRdude compatibility
                    //if(agree())
                    //{	//bpWline(OUMSG_PM_RESET);
#if defined(BUSPIRATEV4)
                    //bpWline("* No Software Reset on v4, Use the reset button."); //TRANSLATE-NEEDED
                    //BPMSG1113;
                    bpWstring("RESET\r\n");
bpv4reset:
                    versionInfo();
                    //bpWstring("\r\nHiZ>"); //was printed twice
#else
                    BPMSG1093;
                    while (0 == UART1TXRdy()); //wait untill TX finishes
                    asm("RESET");
                    //}
#endif
                    break;
                case '$': //bpWline("-bootloader jump");
                    if (agree()) { //bpWline("BOOTLOADER");
                        BPMSG1094;
                        bpDelayMS(100);
                        bpInit(); // turn off nasty things, cleanup first needed?
                        while (0 == UART1TXRdy()); //wait untill TX finishes
                        asm volatile ("mov #BLJUMPADDRESS, w1 \n" //bootloader location
                                    "goto w1 \n");
                    }
                    break;
                case 'a': //bpWline("-AUX low");
                    repeat = getrepeat() + 1;
                    while (--repeat) bpAuxLow();
                    break;
                case 'A': //bpWline("-AUX hi");
                    repeat = getrepeat() + 1;
                    while (--repeat) bpAuxHigh();
                    break;
                case '@': //bpWline("-Aux read");
                    repeat = getrepeat() + 1;
                    while (--repeat) { //bpWstring(OUMSG_AUX_INPUT_READ);
                        BPMSG1095;
                        bpEchoState(bpAuxRead());
                        bpWBR;
                    }
                    break;
                case 'W': //bpWline("-PSU on");	//enable any active power supplies
                    if (bpConfig.busMode == HIZ) { //bpWmessage(MSG_ERROR_MODE);
                        BPMSG1088;
                    } else {
                        BP_VREG_ON();
                        ADCON(); // turn ADC ON
                        bpDelayMS(2); //wait for VREG to come up

                        if ((bpADC(BP_ADC_3V3) > V33L) && (bpADC(BP_ADC_5V0) > V5L)) { //voltages are correct
                            //bpWmessage(MSG_VREG_ON);
                            BPMSG1096;
                            bpBR;
                            //modeConfig.vregEN=1;
                            
                            //Engaging Clutch
                            //finishes the settup and conects the pins...
                            protos[bpConfig.busMode].protocol_setup_exc();
                            bpWline("Clutch engaged!!!");
                        } else {
                            BP_VREG_OFF();
                            bpWline("VREG too low, is there a short?");
                            BPMSG1097;
                            bpBR;
                        }
                        ADCOFF(); // turn ADC OFF
                    }
                    break;
                case 'w': //bpWline("-PSU off");	//disable the power supplies
                    if (bpConfig.busMode == HIZ) { //bpWmessage(MSG_ERROR_MODE);
                        BPMSG1088;
                    } else {
                        //disengaging Clutch
                        //cleansup the protocol and HiZs the pins
                        protos[bpConfig.busMode].protocol_cleanup();
                        bpWline("Clutch disengaged!!!");
                        
                        BP_VREG_OFF();
                        //bpWmessage(MSG_VREG_OFF);
                        BPMSG1097;
                        bpBR;
                        //modeConfig.vregEN=0;
                    }
                    break;
                case 'd': //bpWline("-read ADC");	//do an adc reading
                    //bpWstring(OUMSG_PS_ADC_VOLT_PROBE);
                    BPMSG1044;
                    bpADCprobe();
                    //bpWline(OUMSG_PS_ADC_VOLTS);`
                    BPMSG1045;
                    bpBR;
                    break;
                case 'D': //bpWline("-DVM mode");	//dumb voltmeter mode
                    bpADCCprobe();
                    break;
                case '&': //bpWline("-delay 1ms");
                    repeat = getrepeat();
                    //bpWstring(OUMSG_PS_DELAY);
                    BPMSG1099;
                    bpWintdec(repeat);
                    //bpWline(OUMSG_PS_DELAY_US);
                    BPMSG1100;
                    bpDelayUS(repeat);
                    break;
                case '%': repeat = getrepeat();
                    BPMSG1099;
                    bpWintdec(repeat);
                    BPMSG1212;
                    bpDelayMS(repeat);
                    break;
                case '+': //bpWline("-easter egg");
                    //easterEgg();
                    break;
#ifdef BP_USE_BASIC
                case 's': //bpWline("Listing:");
                    bpConfig.basic = 1;
                    break;
#endif
                case 'S': //servo control
                    if (bpConfig.busMode == HIZ) { //bpWmessage(MSG_ERROR_MODE);
                        BPMSG1088;
                    } else {
                        bpServo();
                    }
                    break;
                case '<': cmderror = 1;
                    temp = 1;

                    while (cmdbuf[((cmdstart + temp) & CMDLENMSK)] != 0x00) {
                        if (cmdbuf[((cmdstart + temp) & CMDLENMSK)] == '>') cmderror = 0; // clear error if we found a > before the command ends
                        temp++;
                    }
                    if (temp >= (USRMACROLEN + 3)) cmderror = 1; // too long (avoid overflows)
                    if (!cmderror) {
                        cmdstart = (cmdstart + 1) & CMDLENMSK;
                        temp = getint();
                        if (cmdbuf[((cmdstart) & CMDLENMSK)] == '=') // assignment
                        {
                            if ((temp > 0) && (temp <= USRMACROS)) {
                                cmdstart = (cmdstart + 1) & CMDLENMSK;
                                temp--;
                                for (repeat = 0; repeat < USRMACROLEN; repeat++) {
                                    usrmacros[temp][repeat] = 0;
                                }
                                repeat = 0;
                                while (cmdbuf[cmdstart] != '>') {
                                    usrmacros[temp][repeat] = cmdbuf[cmdstart];
                                    repeat++;
                                    cmdstart = (cmdstart + 1) & CMDLENMSK;
                                }
                            } else {
                                cmderror = 1;
                            }
                        } else {
                            if (temp == 0) {
                                for (repeat = 0; repeat < USRMACROS; repeat++) {
                                    bpWdec(repeat + 1);
                                    bpWstring(". <");
                                    bpWstring(usrmacros[repeat]);
                                    bpWline(">");
                                }
                            } else if ((temp > 0) && (temp <= USRMACROS)) { //bpWstring("execute : ");
                                //BPMSG1236;
                                //bpWdec(temp-1);
                                bpBR;
                                usrmacro = temp;
                            } else {
                                cmderror = 1;
                            }
                        }
                    }
                    break;
                    // command for subsys (i2c, UART, etc)
                case '(': //bpWline("-macro");
                    cmdstart = (cmdstart + 1) & CMDLENMSK;
                    sendw = getint();
                    consumewhitechars();
                    if (cmdbuf[((cmdstart) & CMDLENMSK)] == ')') { //cmdstart++;				// skip )
                        //cmdstart&=CMDLENMSK;
                        //bpWdec(sendw);
                        protos[bpConfig.busMode].protocol_macro(sendw);
                        bpBR;
                    } else {
                        cmderror = 1;
                    }
                    break;
                case 0x22: //bpWline("-send string");
                    cmderror = 1;
                    temp = 1;

                    while (cmdbuf[((cmdstart + temp) & CMDLENMSK)] != 0x00) {
                        if (cmdbuf[((cmdstart + temp) & CMDLENMSK)] == 0x22) cmderror = 0; // clear error if we found a " before the command ends
                        temp++;
                    }

                    if (!cmderror) {
                        BPMSG1101;
                        UART1TX(0x22);
                        while (cmdbuf[((++cmdstart) & CMDLENMSK)] != 0x22) {
                            cmdstart &= CMDLENMSK;
                            UART1TX(cmdbuf[cmdstart]);
                            sendw = cmdbuf[cmdstart];
                            if (modeConfig.lsbEN == 1) //adjust bitorder
                            {
                                sendw = bpRevByte(sendw);
                            }
                            protos[bpConfig.busMode].protocol_send(sendw);
                        }
                        cmdstart &= CMDLENMSK;
                        UART1TX(0x22);
                        bpBR;
                    }
                    break;
                case '[': //bpWline("-Start");
                    protos[bpConfig.busMode].protocol_start();
                    break;
                case '{': //bpWline("-StartR");
                    protos[bpConfig.busMode].protocol_startR();
                    break;
                case ']': //bpWline("-Stop");
                    protos[bpConfig.busMode].protocol_stop();
                    break;
                case '}': //bpWline("-StopR");
                    protos[bpConfig.busMode].protocol_stopR();
                    break;
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9': //bpWline("-Send");
                    //bpWmessage(MSG_WRITE);
                    BPMSG1101;
                    sendw = getint();
                    cmdstart--;
                    cmdstart &= CMDLENMSK;
                    repeat = getrepeat() + 1;
                    numbits = getnumbits();
                    if (numbits) {
                        modeConfig.numbits = numbits;
                        if (numbits > 8) modeConfig.int16 = 1;
                        else modeConfig.int16 = 0;
                    }
                    while (--repeat) {
                        bpWbyte(sendw);
                        if (((modeConfig.int16 == 0) && (modeConfig.numbits != 8)) || ((modeConfig.int16 == 1) && (modeConfig.numbits != 16))) {
                            UART1TX(';');
                            bpWdec(modeConfig.numbits);
                        }
                        if (modeConfig.lsbEN == 1) {//adjust bitorder
                            sendw = bpRevByte(sendw);
                        }
                        received = protos[bpConfig.busMode].protocol_send(sendw);
                        bpSP;
                        if (modeConfig.wwr) { //bpWmessage(MSG_READ);
                            BPMSG1102;
                            if (modeConfig.lsbEN == 1) {//adjust bitorder
                                received = bpRevByte(received);
                            }
                            bpWbyte(received);
                            bpSP;
                        }
                    }
                    bpBR;
                    break;
                case 'r': //bpWline("-Read");
                    //bpWmessage(MSG_READ);
                    BPMSG1102;
						  //newDmode = 0;
						  newDmode = changeReadDisplay();
                    repeat = getrepeat() + 1;
                    numbits = getnumbits();
                    if (numbits) {
                        modeConfig.numbits = numbits;
                        if (numbits > 8) modeConfig.int16 = 1;
                        else modeConfig.int16 = 0;
                    }
						  if(newDmode)
						  {
						  		oldDmode = bpConfig.displayMode;
								bpConfig.displayMode = newDmode-1;
						  }
                    while (--repeat) {
                        received = protos[bpConfig.busMode].protocol_read();
                        if (modeConfig.lsbEN == 1) {//adjust bitorder
                            received = bpRevByte(received);
                        }
                        bpWbyte(received);
                        if (((modeConfig.int16 == 0) && (modeConfig.numbits != 8)) || ((modeConfig.int16 == 1) && (modeConfig.numbits != 16))) {
                            UART1TX(';');
                            bpWdec(modeConfig.numbits);
                        }
                        bpSP;
                    }
						  if(newDmode)
						  {
								bpConfig.displayMode = oldDmode;
								newDmode=0;
						  }
                    bpBR;
                    break;
                case '/': //bpWline("-CLK hi");
                    //repeat=getrepeat()+1;
                    //while(--repeat)
                    //{	//bpWmessage(MSG_BIT_CLKH);
                    BPMSG1103;
                    protos[bpConfig.busMode].protocol_clkh();
                    //}
                    break;
                case '\\': //bpWline("-CLK lo");
                    //repeat=getrepeat()+1;
                    //while(--repeat)
                    //{	//bpWmessage(MSG_BIT_CLKL);
                    BPMSG1104;
                    protos[bpConfig.busMode].protocol_clkl();
                    //}
                    break;
                case '-': //bpWline("-DAT hi");
                    //repeat=getrepeat()+1;
                    //while(--repeat)
                    //{	//bpWmessage(MSG_BIT_DATH);
                    BPMSG1105;
                    protos[bpConfig.busMode].protocol_dath();
                    //}
                    break;
                case '_': //bpWline("-DAT lo");
                    //repeat=getrepeat()+1;
                    //while(--repeat)
                    //{	//bpWmessage(MSG_BIT_DATL);
                    BPMSG1106;
                    protos[bpConfig.busMode].protocol_datl();
                    //}
                    break;
                case '.': //bpWline("-DAT state read");
                    //repeat=getrepeat()+1;
                    BPMSG1098;
                    //while(--repeat)
                {
                    bpEchoState(protos[bpConfig.busMode].protocol_dats());
                    //bpWmessage(MSG_BIT_NOWINPUT);
                    //BPMSG1107;
                }
                    break;
                case '^': //bpWline("-CLK pulse");
                    repeat = getrepeat();
                    BPMSG1108;
                    bpWbyte(repeat);
                    repeat++;
                    while (--repeat) { //bpWmessage(MSG_BIT_CLK);
                        protos[bpConfig.busMode].protocol_clk();
                    }
                    bpBR;
                    break;
                case '!': //bpWline("-bit read");
                    repeat = getrepeat() + 1;
                    BPMSG1109;
                    while (--repeat) { //bpWmessage(MSG_BIT_READ);
                        bpEchoState(protos[bpConfig.busMode].protocol_bitr());
                        bpSP;
                    }
                    //bpWmessage(MSG_BIT_NOWINPUT);
                    BPMSG1107;
                    break;
                    // white char/delimeters
                case 0x00:
                case 0x0D: // not necessary but got random error msg at end, just to be sure
                case 0x0A: // same here
                case ' ':
                case ',': break; // no match so it is an error
                default: cmderror = 1;
            } //switch(c)
            cmdstart = (cmdstart + 1) & CMDLENMSK;

            //			bpWline("");
            //			bpWstring("cmdstart = ");
            //			bpWinthex(cmdstart);
            //			bpWstring(" cmdend = ");
            //			bpWinthex(cmdend);
            //			bpWline("");

            if (cmderror) { //bpWstring("Syntax error at char ");
                BPMSG1110;
                if (cmdstart > oldstart) // find error position :S
                {
                    bpWdec(cmdstart - oldstart);
                } else {
                    bpWdec((CMDBUFLEN + cmdstart) - oldstart);
                }
                cmderror = 0;
                stop = 1;
                bpBR;
            }

            if (cmdstart == cmdend) stop = 1; // reached end of user input??
        } //while(!stop)


        cmdstart = newstart;
        cmdend = newstart; // 'empty' cmdbuffer
        cmd = 0;
    } //while(1)
} //serviceuser(void)

int getint(void) // get int from user (accept decimal, hex (0x) or binairy (0b)
{
    int i;
    int number;

    i = 0;
    number = 0;

    if ((cmdbuf[((cmdstart + i) & CMDLENMSK)] >= 0x31) && (cmdbuf[((cmdstart + i) & CMDLENMSK)] <= 0x39)) // 1-9 is for sure decimal
    {
        number = cmdbuf[(cmdstart + i) & CMDLENMSK] - 0x30;
        i++;
        while ((cmdbuf[((cmdstart + i) & CMDLENMSK)] >= 0x30) && (cmdbuf[((cmdstart + i) & CMDLENMSK)] <= 0x39)) // consume all digits
        {
            number *= 10;
            number += cmdbuf[((cmdstart + i) & CMDLENMSK)] - 0x30;
            i++;
        }
    } else if (cmdbuf[((cmdstart + i) & CMDLENMSK)] == 0x30) // 0 could be anything
    {
        i++;
        if ((cmdbuf[((cmdstart + i) & CMDLENMSK)] == 'b') || (cmdbuf[((cmdstart + i) & CMDLENMSK)] == 'B')) {
            i++;
            while ((cmdbuf[((cmdstart + i) & CMDLENMSK)] == '1') || (cmdbuf[((cmdstart + i) & CMDLENMSK)] == '0')) {
                number <<= 1;
                number += cmdbuf[((cmdstart + i) & CMDLENMSK)] - 0x30;
                i++;
            }
        } else if ((cmdbuf[((cmdstart + i) & CMDLENMSK)] == 'x') || (cmdbuf[((cmdstart + i) & CMDLENMSK)] == 'X')) {
            i++;
            while (((cmdbuf[((cmdstart + i) & CMDLENMSK)] >= 0x30) && (cmdbuf[((cmdstart + i) & CMDLENMSK)] <= 0x39)) ||    \
				((cmdbuf[((cmdstart + i) & CMDLENMSK)] >= 'A') && (cmdbuf[((cmdstart + i) & CMDLENMSK)] <= 'F')) ||    \
				((cmdbuf[((cmdstart + i) & CMDLENMSK)] >= 'a') && (cmdbuf[((cmdstart + i) & CMDLENMSK)] <= 'f'))) {
                number <<= 4;
                if ((cmdbuf[(cmdstart + i) & CMDLENMSK] >= 0x30) && (cmdbuf[((cmdstart + i) & CMDLENMSK)] <= 0x39)) {
                    number += cmdbuf[((cmdstart + i) & CMDLENMSK)] - 0x30;
                } else {
                    cmdbuf[((cmdstart + i) & CMDLENMSK)] |= 0x20; // make it lowercase
                    number += cmdbuf[((cmdstart + i) & CMDLENMSK)] - 0x57; // 'a'-0x61+0x0a
                }
                i++;
            }
        } else if ((cmdbuf[((cmdstart + i) & CMDLENMSK)] >= 0x30) && (cmdbuf[((cmdstart + i) & CMDLENMSK)] <= 0x39)) {
            number = cmdbuf[((cmdstart + i) & CMDLENMSK)] - 0x30;
            while ((cmdbuf[((cmdstart + i) & CMDLENMSK)] >= 0x30) && (cmdbuf[((cmdstart + i) & CMDLENMSK)] <= 0x39)) // consume all digits
            {
                number *= 10;
                number += cmdbuf[((cmdstart + i) & CMDLENMSK)] - 0x30;
                i++;
            }
        }
    } else // how did we come here??
    {
        cmderror = 1;
        return 0;
    }

    cmdstart += i; // we used i chars
    cmdstart &= CMDLENMSK;
    return number;
} //getint(void)

int getrepeat(void) {
    int temp;

    if (cmdbuf[(cmdstart + 1) & CMDLENMSK] == ':') {
        cmdstart += 2;
        cmdstart &= CMDLENMSK;
        temp = getint();
        cmdstart--; // to allow [6:10] (start send 6 10 times stop)
        cmdstart &= CMDLENMSK;
        return temp;
    }
    return 1; // no repeat count=1
} //

int getnumbits(void) {
    int temp;

    if (cmdbuf[(cmdstart + 1) & CMDLENMSK] == ';') {
        cmdstart = (cmdstart + 2) & CMDLENMSK;
        temp = getint();
        cmdstart = (cmdstart - 1) & CMDLENMSK; // to allow [6:10] (start send 6 10 times stop)
        return temp;
    }
    return 0; // no numbits=0;
} //

unsigned char changeReadDisplay(void)
{
	if(cmdbuf[(cmdstart + 1) & CMDLENMSK] == 'x')
	{
		cmdstart = (cmdstart + 1) & CMDLENMSK;
		return 1;
	}
	if(cmdbuf[(cmdstart + 1) & CMDLENMSK] == 'd')
	{
		cmdstart = (cmdstart + 1) & CMDLENMSK;
		return 2;
	}
	if(cmdbuf[(cmdstart + 1) & CMDLENMSK] == 'b')
	{
		cmdstart = (cmdstart + 1) & CMDLENMSK;
		return 3;
	}
return 0;
}

void consumewhitechars(void) {
    while (cmdbuf[cmdstart] == 0x20) {
        cmdstart = (cmdstart + 1) & CMDLENMSK; // remove spaces
    }
} //

void changemode(void) {
    int i, busmode;

    busmode = 0;

    //	bpWline("--changemode()");

    cmdstart = (cmdstart + 1) & CMDLENMSK;

    consumewhitechars();

    //	bpWline("whitechars stripped");

    busmode = getint();

    //	bpWstring("numbers: busmode=");
    //	bpWinthex(busmode);
    //	bpWline("");

    if (!busmode) // no argument entered
    {
        for (i = 0; i < MAXPROTO; i++) {
            bpWdec(i + 1);
            bpWstring(". ");
            bpWline(protos[i].protocol_name);
        }
        //bpWline("x. exit(without change)");
        BPMSG1111;
        cmderror = 0; // error is set because no number found, but it is no error here:S eeeh confusing right?
        busmode = getnumber(1, 1, MAXPROTO, 1) - 1;
        //		bpWstring("busmode= ");
        //		bpWdec(busmode);
        if ((busmode == -2) || (busmode == -1)) { //bpWline("no mode change");
            BPMSG1112;
        } else {
            protos[bpConfig.busMode].protocol_cleanup();
            bpInit();
            bpConfig.busMode = busmode;
            protos[bpConfig.busMode].protocol_setup();
            bpWline("Clutch disengaged!!!");
            if (busmode) {
               BP_LEDMODE = 1; // mode led is on when proto >0
               bpWline("To finish setup, start up the power supplies with command 'W'\r\n");
            }
            //bpWmessage(MSG_READY);
            BPMSG1085;
        }
    } else // number entered
    {
        busmode--; // save a couple of programwords to do it here :D
        if (busmode < MAXPROTO) {
            protos[bpConfig.busMode].protocol_cleanup();
            bpInit();
            bpConfig.busMode = busmode;
            protos[bpConfig.busMode].protocol_setup();
            if (busmode) BP_LEDMODE = 1; // mode led is on when proto >0
            //bpWmessage(MSG_READY);
            BPMSG1085;
        } else { //bpWline("Nonexistent protocol!!");
            BPMSG1114;
        }
    }
    cmdstart = (cmdend - 1) & CMDLENMSK;
} //changemode(void)


#if defined(BP_ENABLE_HISTORY)

int cmdhistory(void) {
    int i, j, k;

    int historypos[CMDHISTORY];

    i = 1;
    j = (cmdstart - 1) & CMDLENMSK;

    while (j != cmdstart) // scroll through the whole buffer
    {
        if ((cmdbuf[j] == 0x00) && (cmdbuf[(j + 1) & CMDLENMSK] != 0x00)) // did we find an end? is it not empty?
        {
            bpWdec(i);
            bpWstring(". ");
            k = 1;
            while (cmdbuf[((j + k) & CMDLENMSK)]) {
                UART1TX(cmdbuf[((j + k) & CMDLENMSK)]); // print it
                k++;
            }
            historypos[i] = (j + 1) & CMDLENMSK;
            i++;
            if (i == CMDHISTORY) break;
            bpWline("");
        }
        j = (j - 1) & CMDLENMSK;
    }

    bpWline(" ");
    BPMSG1115;

    j = getnumber(0, 1, i, 1);

    if (j == -1 || !j) // x is -1, default is 0
    {
        bpWline("");
        return 1;
    } else {
        i = 0;
        while (cmdbuf[(historypos[j] + i) & CMDLENMSK]) // copy it to the end of the ringbuffer
        {
            cmdbuf[(cmdend + i) & CMDLENMSK] = cmdbuf[(historypos[j] + i) & CMDLENMSK];
            i++;
        }
        cmdstart = (cmdend - 1) & CMDLENMSK; // start will be increased before parsing in main loop
        cmdend = (cmdstart + i + 2) & CMDLENMSK;
        cmdbuf[(cmdend - 1) & CMDLENMSK] = 0x00;
    }
    return 0;
} //cmdhistory(void)
#endif


// gets number from input
// -1 = abort (x)
// -2 = input to much
// 0-max return
// x=1 exit is enabled (we don't want that in the mode changes ;)

int getnumber(int def, int min, int max, int x) //default, minimum, maximum, show exit option
{
    char c;
    char buf[6]; // max 4 digits;
    int i, j, stop, temp, neg;

again: // need to do it proper with whiles and ifs..

    i = 0;
    stop = 0;
    temp = 0;
    neg = 0;

    bpWstring("\r\n(");
    if (def < 0) {
        bpWstring("x");
    } else {
        bpWdec(def);
    }
    bpWstring(")>");

    while (!stop) {
        c = UART1RX();
        switch (c) {
            case 0x08: if (i) {
                    i--;
                    bpWstring("\x08 \x08");
                } else {
                    if (neg) {
                        neg = 0;
                        bpWstring("\x08 \x08");
                    } else {
                        UART1TX(BELL);
                    }
                }
                break;
            case 0x0A:
            case 0x0D: stop = 1;
                break;
            case '-': if (!i) // enable negative numbers
                {
                    UART1TX('-');
                    neg = 1;
                } else {
                    UART1TX(BELL);
                }
                break;
            case 'x': if (x) return -1; // user wants to quit :( only if we enable it :D
            default: if ((c >= 0x30) && (c <= 0x39)) // we got a digit
                {
                    if (i > 3) // 0-9999 should be enough??
                    {
                        UART1TX(BELL);
                        i = 4;
                    } else {
                        UART1TX(c);
                        buf[i] = c; // store user input
                        i++;
                    }
                } else // ignore input :)
                {
                    UART1TX(BELL);
                }

        }
    }
    bpBR;

    if (i == 0) {
        return def; // no user input so return default option
    } else {
        temp = 0;
        i--;
        j = i;

        for (; i >= 0; i--) {
            temp *= 10;
            temp += (buf[j - i] - 0x30);
        }

        if ((temp >= min) && (temp <= max)) {
            if (neg) {
                return -temp;
            } else {
                return temp;
            }
        } else { //bpWline("\r\nInvalid choice, try again\r\n");
            BPMSG1211;
            goto again;
        }
    }
    return temp; // we dont get here, but keep compiler happy
} //getnumber(int def, int min, int max, int x)


#if defined(BUSPIRATEV4)
// gets number from input
// -1 = abort (x)
// -2 = input to much
// 0-max return
// x=1 exit is enabled (we don't want that in the mode changes ;)

long getlong(long def, int min, long max, int x) //default, minimum, maximum, show exit option
{
    char c;
    char buf[12]; // max long = 2147483647 so 10
    int i, j, stop, neg;
    long temp;

again: // need to do it proper with whiles and ifs..

    i = 0;
    stop = 0;
    temp = 0;
    neg = 0;

    bpWstring("\r\n(");
    if (def < 0) {
        bpWstring("x");
    } else {
        bpWlongdec(def);
    }
    bpWstring(")>");

    while (!stop) {
        c = UART1RX();
        switch (c) {
            case 0x08: if (i) {
                    i--;
                    bpWstring("\x08 \x08");
                } else {
                    if (neg) {
                        neg = 0;
                        bpWstring("\x08 \x08");
                    } else {
                        UART1TX(BELL);
                    }
                }
                break;
            case 0x0A:
            case 0x0D: stop = 1;
                break;
            case '-': if (!i) // enable negative numbers
                {
                    UART1TX('-');
                    neg = 1;
                } else {
                    UART1TX(BELL);
                }
                break;
            case 'x': if (x) return -1; // user wants to quit :( only if we enable it :D
            default: if ((c >= 0x30) && (c <= 0x39)) // we got a digit
                {
                    if (i > 9) // 0-9999 should be enough??
                    {
                        UART1TX(BELL);
                        i = 10;
                    } else {
                        UART1TX(c);
                        buf[i] = c; // store user input
                        i++;
                    }
                } else // ignore input :)
                {
                    UART1TX(BELL);
                }

        }
    }
    bpBR;

    if (i == 0) {
        return def; // no user input so return default option
    } else {
        temp = 0;
        i--;
        j = i;

        for (; i >= 0; i--) {
            temp *= 10;
            temp += (buf[j - i] - 0x30);
        }

        if ((temp >= min) && (temp <= max)) {
            if (neg) {
                return -temp;
            } else {
                return temp;
            }
        } else { //bpWline("\r\nInvalid choice, try again\r\n");
            BPMSG1211;
            goto again;
        }
    }
    return temp; // we dont get here, but keep compiler happy
}
#endif




//print version info (used in menu and at startup in main.c)

void versionInfo(void) {
    unsigned int i;


#if defined (BUSPIRATEV2) //we can tell if it's v3a or v3b, show it here
    bpWstring(BP_VERSION_STRING);
    UART1TX('.');
    UART1TX(bpConfig.HWversion);
    if (bpConfig.dev_type == 0x44F) {//sandbox electronics clone with 44pin PIC24FJ64GA004
        bpWstring(" clone w/different PIC");
    }
    bpBR;
#else
    bpWline(BP_VERSION_STRING);
#endif

    bpWstring(BP_FIRMWARE_STRING);

#ifndef BP_MAIN
    UART1TX('[');
    for (i = 0; i < MAXPROTO; i++) {
        if (i) bpSP;
        bpWstring(protos[i].protocol_name);
    }
    UART1TX(']');
#endif

#ifndef BUSPIRATEV4
    //bpWstring(" Bootloader v");
    BPMSG1126;
    i = bpReadFlash(0x0000, BL_ADDR_VER);
    bpWdec(i >> 8);
    UART1TX('.');
    bpWdec(i);
#endif
    bpWline("");

    //bpWstring("DEVID:");
    BPMSG1117;
    bpWinthex(bpConfig.dev_type);

    //bpWstring(" REVID:");
    BPMSG1210;
    bpWinthex(bpConfig.dev_rev);
#ifdef BUSPIRATEV4
    bpWstring(" (24FJ256GB106 ");
    switch (bpConfig.dev_rev) {
        case PIC_REV_A3:
            bpWstring("A3");
            break;
        case PIC_REV_A5:
            bpWstring("A5");
            break;
        default:
            bpWstring("UNK");
            break;
    }
#else
    bpWstring(" (24FJ64GA00");
    if (bpConfig.dev_type == 0x44F) {//sandbox electronics clone with 44pin PIC24FJ64GA004
        bpWstring("4 ");
    } else {
        bpWstring("2 ");
    }

    switch (bpConfig.dev_rev) {
        case PIC_REV_A3:
            bpWstring("A3"); //also A4, but that's not in the wild and makes it confusing to users
            break;
        case PIC_REV_B4:
            bpWstring("B4");
            break;
        case PIC_REV_B5:
            bpWstring("B5");
            break;
        case PIC_REV_B8:
            bpWstring("B8");
            break;
        default:
            bpWstring("UNK");
            break;
    }
#endif

    bpWline(")");
    //bpWline("http://dangerousprototypes.com");
    BPMSG1118;
    i = 0;
} //versionInfo(void)

//display properties of the current bus mode (pullups, vreg, lsb, output type, etc)

void statusInfo(void) {

#ifdef BUSPIRATEV4
    bpWstring("CFG0: ");
    bpWinthex(bpReadFlash(CFG_ADDR_UPPER, CFG_ADDR_0));
    bpSP;
#endif

    //bpWstring("CFG1:");
    BPMSG1136;
    bpWinthex(bpReadFlash(CFG_ADDR_UPPER, CFG_ADDR_1));
    //bpWstring(" CFG2:");
    BPMSG1137;
    bpWinthex(bpReadFlash(CFG_ADDR_UPPER, CFG_ADDR_2));
    bpBR;

    //bpWline("*----------*");
    BPMSG1119;

    pinStates();

    //vreg status (was modeConfig.vregEN)
    if (BP_VREGEN == 1) BPMSG1096;
    else BPMSG1097; //bpWmessage(MSG_VREG_ON); else bpWmessage(MSG_VREG_OFF);
    UART1TX(',');
    bpSP;

#ifndef BUSPIRATEV1A
    //pullups available, enabled?
    //was modeConfig.pullupEN
    if (BP_PULLUP == 1) BPMSG1091;
    else BPMSG1089; //bpWmessage(MSG_OPT_PULLUP_ON); else bpWmessage(MSG_OPT_PULLUP_OFF);
    UART1TX(',');
    bpSP;
#endif

#ifdef BUSPIRATEV4
    if (BP_PUVSEL50_DIR == 0) bpWstring("Vpu=5V, ");
    if (BP_PUVSEL33_DIR == 0) bpWstring("Vpu=3V3, ");

#endif

    //open collector outputs?
    if (modeConfig.HiZ == 1) BPMSG1120;
    else BPMSG1121; // bpWmessage(MSG_STATUS_OUTPUT_HIZ); else bpWmessage(MSG_STATUS_OUTPUT_NORMAL);

    //voltage report
    //	measureSupplyVoltages();


    //bitorder toggle available, enabled
    if (modeConfig.lsbEN == 0) BPMSG1123;
    else BPMSG1124; //bpWmessage(MSG_OPT_BITORDER_LSB); else bpWmessage(MSG_OPT_BITORDER_MSB);
    UART1TX(',');
    bpSP;

    // show partial writes
    //bpWline("Number of bits read/write: ");
    BPMSG1252;
    bpWdec(modeConfig.numbits);
    bpBR;

    //AUX pin setting
#ifndef BUSPIRATEV4
    if (modeConfig.altAUX == 1) BPMSG1087;
    else BPMSG1086; //bpWmessage(MSG_OPT_AUXPIN_CS); else bpWmessage(MSG_OPT_AUXPIN_AUX);
#endif
#ifdef BUSPIRATEV4
    switch (modeConfig.altAUX) {
        case 0: BPMSG1087;
            break;
        case 1: BPMSG1086;
            break;
        case 2: BPMSG1263;
            break;
        case 3: BPMSG1264;
            break;
    }
#endif

    protos[bpConfig.busMode].protocol_settings();

    //bpWline("*----------*");
    BPMSG1119;
} //statusInfo(void)

void pinStates(void) { //bpWline("Pinstates:");
    BPMSG1226;
#if defined(BUSPIRATEV4)
    BPMSG1256; //bpWstring("12.(RD)\t11.(BR)\t10.(BLK)\t9.(WT)\t8.(GR)\t7.(PU)\t6.(BL)\t5.(GN)\t4.(YW)\t3.(OR)\t2.(RD)\1.(BR)");
#else
    BPMSG1233; //bpWstring("1.(BR)\t2.(RD)\t3.(OR)\t4.(YW)\t5.(GN)\t6.(BL)\t7.(PU)\t8.(GR)\t9.(WT)\t0.(BLK)");
#endif
#if defined(BUSPIRATEV4)
    BPMSG1257; //bpWstring("GND\t5.0V\t3.3V\tVPU\tADC\tAUX2\tAUX1\tAUX\t");
#elif defined(BUSPIRATEV25)
    BPMSG1235; //bpWstring("GND\tADC\t5.0V\t3.3V\tVPU\tAUX\t");
#else
    BPMSG1227; //bpWstring("GND\t3.3V\t5.0V\tADC\tVPU\tAUX\t");
#endif
    protos[bpConfig.busMode].protocol_pins();
    BPMSG1228; //bpWstring("P\tP\tP\tI\tI\t");
#if defined(BUSPIRATEV4)    
    pinDirection(AUX2);
    pinDirection(AUX1);
    pinDirection(AUX);
    pinDirection(CS);
    pinDirection(MISO);
    pinDirection(CLK);
    pinDirection(MOSI);
#else    
    pinDirection(AUX);
    pinDirection(CLK);
    pinDirection(MOSI);
    pinDirection(CS);
    pinDirection(MISO);
#endif
    bpBR;
    BPMSG1234; //bpWstring("GND\t");
    ADCON();


#if defined(BUSPIRATEV25)
    bpWvolts(bpADC(BP_ADC_PROBE));
    BPMSG1045;
    UART1TX('\t');
#elif defined(BUSPIRATEV4)
    bpWvolts(bpADC(BP_ADC_5V0));
    BPMSG1045;
    UART1TX('\t');
#else
    bpWvolts(bpADC(BP_ADC_3V3));
    BPMSG1045;
    UART1TX('\t');
#endif

#if defined(BUSPIRATEV4)
    bpWvolts(bpADC(BP_ADC_3V3));
    BPMSG1045;
    UART1TX('\t');
#else
    bpWvolts(bpADC(BP_ADC_5V0));
    BPMSG1045;
    UART1TX('\t');
#endif

#if defined(BUSPIRATEV25)
    bpWvolts(bpADC(BP_ADC_3V3));
    BPMSG1045;
    UART1TX('\t');
#elif defined(BUSPIRATEV4)
    bpWvolts(bpADC(BP_ADC_VPU));
    BPMSG1045;
    UART1TX('\t');
#else
    bpWvolts(bpADC(BP_ADC_PROBE));
    BPMSG1045;
    UART1TX('\t');
#endif

#if defined(BUSPIRATEV4)
    bpWvolts(bpADC(BP_ADC_PROBE));
    BPMSG1045;
    UART1TX('\t');
#else
    bpWvolts(bpADC(BP_ADC_VPU));
    BPMSG1045;
    UART1TX('\t');
#endif
    ADCOFF();
#if defined(BUSPIRATEV4)
    pinState(AUX2);
    pinState(AUX1);
    pinState(AUX);
    pinState(CS);
    pinState(MISO);
    pinState(CLK);
    pinState(MOSI);
#else
    pinState(AUX);
    pinState(CLK);
    pinState(MOSI);
    pinState(CS);
    pinState(MISO);
#endif
    bpBR;
} //pinStates(void)

void pinDirection(unsigned int pin) {

    if (IODIR & pin) {
        bpWstring("I\t");
    } else {
        bpWstring("O\t");
    }

} //

void pinState(unsigned int pin) {

    if (IOPOR & pin) {
        bpWstring("H\t");
    } else {
        bpWstring("L\t");
    }

} //

//user terminal number display mode dialog (eg HEX, DEC, BIN, RAW)

void setDisplayMode(void) {
    int mode;

    cmdstart = (cmdstart + 1) & CMDLENMSK;

    consumewhitechars();
    mode = getint();

    if ((mode > 0) && (mode <= 4)) {
        bpConfig.displayMode = mode - 1;
    } else {
        cmderror = 0;
        //bpWmessage(MSG_OPT_DISPLAYMODE); //show the display mode options message
        BPMSG1127;
        //	bpConfig.displayMode=(bpUserNumberPrompt(1, 4, 1)-1); //get, store user reply
        bpConfig.displayMode = getnumber(1, 1, 4, 0) - 1; //get, store user reply
    }
    //bpWmessage(MSG_OPT_DISPLAYMODESET);//show display mode update text
    BPMSG1128;
} //

//configure user terminal side UART baud rate

void setBaudRate(void) {
    unsigned char speed;
    unsigned char brg = 0;

    cmdstart = (cmdstart + 1) & CMDLENMSK;

    consumewhitechars();
    speed = getint();

    if ((speed > 0) && (speed <= 10)) {
        bpConfig.termSpeed = speed - 1;
    } else {
        cmderror = 0;
        //bpWmessage(MSG_OPT_UART_BAUD); //show stored dialog
        BPMSG1133;
        //	bpConfig.termSpeed=(bpUserNumberPrompt(1, 9, 9)-1);
        bpConfig.termSpeed = getnumber(9, 1, 10, 0) - 1;
    }

    if (bpConfig.termSpeed == 9) {
        consumewhitechars();
        brg = getint();

        if (brg == 0) {
            cmderror = 0;
            bpWline("Enter raw value for BRG");
            brg = getnumber(34, 0, 32767, 0);
        }
    }

    //bpWmessage(MSG_OPT_TERMBAUD_ADJUST); //show 'adjust and press space dialog'
    BPMSG1134;
    BPMSG1251;
    while (0 == UART1TXRdy()); //wait for TX to finish or reinit flushes part of prompt string from buffer

    if (bpConfig.termSpeed == 9) {
        UART1Speed(brg);
    }
    InitializeUART1();
    while (1) { //wait for space to prove valid baud rate switch
        //JTR Not required while (!UART1RXRdy());
        if (UART1RX() == ' ')break;
    }
} //

#ifdef BUSPIRATEV4

void setPullupVoltage(void) {
    int temp;


    //don't allow pullups on some modules. also: V0a limitation of 2 resistors
    if (bpConfig.busMode == HIZ) { //bpWmessage(MSG_ERROR_MODE);
        BPMSG1088;
        cmderror = 1; // raise error
        return;
    }
    if (modeConfig.HiZ == 0) { //bpWmessage(MSG_ERROR_NOTHIZPIN);
        BPMSG1209;
        cmderror = 1; // raise error
        return;
    }

    BP_3V3PU_OFF(); //disable any existing pullup
    bpDelayMS(2);
    ADCON();
    if (bpADC(BP_ADC_VPU) > 0x100) { //is there already an external voltage?
        bpWline("Warning: already a voltage on Vpullup pin"); // shouldn;t this be an error?
    }
    ADCOFF();

    cmdstart = (cmdstart + 1) & CMDLENMSK;
    consumewhitechars();

    temp = getint();
    if (cmderror) // I think the user wants a menu
    {
        cmderror = 0;

        //bpWline("Select Vpu source");
        //bpWline(" 1) None or external");
        //bpWline(" 2) Onboard 3V3 Vreg");
        //bpWline(" 3) Onboard 5V Vreg");
        BPMSG1271;

        temp = getnumber(1, 1, 3, 0);
    }
    switch (temp) {
        case 1: BP_3V3PU_OFF();

            BPMSG1272; //;0;" on-board pullup voltage "
            BPMSG1274; //1;"disabled"

            //bpWline("on-board pullup voltage disabled");
            break;
        case 2: BP_3V3PU_ON();
            BPMSG1173; //3.3v
            BPMSG1272; //;0;" on-board pullup voltage "
            BPMSG1273; //1;"enabled"
            //bpWline("3V3 on-board pullup voltage enabled");
            break;
        case 3: BP_5VPU_ON();
            BPMSG1171; //5v
            BPMSG1272; //;0;" on-board pullup voltage "
            BPMSG1273; //1;"enabled"
            //bpWline("5V on-board pullup voltage enabled");
            break;
        default:BP_3V3PU_OFF();
            BPMSG1272; //;0;" on-board pullup voltage "
            BPMSG1274; //1;"disabled"
            //bpWline("on-board pullup voltage disabled");
    }
}

#endif




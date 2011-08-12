#include "base.h"
#include "bitbang.h"
#include "busPirateCore.h"
#include "binIOhelpers.h"

extern struct _modeConfig modeConfig;
extern struct _bpConfig bpConfig;
void binrawversionString(void);
void PIC24NOP(void);
void PIC416Write(unsigned char cmd, unsigned char datl, unsigned char dath);
void PIC424Write(unsigned long cmd, unsigned char pn);
void PIC424Read(void);

#define R3WMOSI_TRIS 	BP_MOSI_DIR
#define R3WCLK_TRIS 	BP_CLK_DIR
#define R3WMISO_TRIS 	BP_MISO_DIR
#define R3WCS_TRIS 		BP_CS_DIR


#define R3WMOSI 		BP_MOSI
#define R3WCLK 			BP_CLK 
#define R3WMISO 		BP_MISO 
#define R3WCS 			BP_CS 

/*
 * 00000000 � Enter raw bitbang mode, reset to raw bitbang mode
 * 00000001 � Mode version string (RAW1)
 * 00000010 - I2C style start bit
 * 00000011 - I2C style stop bit
 * 00000100 - CS low (0) (respects hiz setting)
 * 00000101 - CS high (1)
 * 00000110 - Read byte
 * 00000111 - Read bit
 * 00001000 - Peek at input pin
 * 00001001 - Clock tick
 * 00001010 - Clock low
 * 00001011 - Clock high
 * 00001100 - Data low
 * 00001101 - Data high
 * 0001xxxx � Bulk transfer, send 1-16 bytes (0=1byte!)
 * 0010xxxx - Bulk clock ticks, send 1-16 ticks
 * 0011xxxx - Bulk bits, send 1-8 bits of the next byte (0=1bit!)
 * 0100wxyz � Configure peripherals, w=power, x=pullups, y=AUX, z=CS
 * 0101xxxx - Bulk read, read 1-16bytes (0=1byte!)
 * 0110000x � Set speed
 * 1000wxyz � Config, w=output type, x=3wire, y=lsb, z=n/a

 */

void binrawversionString(void) {
    bpWstring("RAW1");
}

void PIC24NOP(void) {
    //send four bit SIX command (write)
    bbWriteBit(0); //send bit
    bbWriteBit(0); //send bit
    bbWriteBit(0); //send bit
    bbWriteBit(0); //send bit

    //send data payload
    bbWriteByte(0x00); //send byte
    bbWriteByte(0x00); //send byte
    bbWriteByte(0x00); //send byte

}

enum {
    PIC614 = 0,
    PIC416,
    PIC424,
};

void binwire(void) {
    static unsigned char inByte, rawCommand, i, c, wires, picMode = PIC614;
    static unsigned int cmds, j;

    modeConfig.HiZ = 1; //yes, always hiz (bbio uses this setting, should be changed to a setup variable because stringing the modeconfig struct everyhwere is getting ugly!)
    modeConfig.lsbEN = 0; //just in case!
    modeConfig.speed = 1;
    modeConfig.numbits = 8;
    //startup in raw2wire mode
    wires = 2;
    //configure for raw3wire mode
    bbSetup(2, 0xff); //setup the bitbang library, must be done before calling bbCS below
    //setup pins (pins are input/low when we start)
    //MOSI output, low
    //clock output, low
    //MISO input
    //CS output, high
    R3WMOSI_TRIS = 0;
    R3WCLK_TRIS = 0;
    R3WMISO_TRIS = 1;
    bbCS(1); //takes care of custom HiZ settings too

    binrawversionString(); //reply ID string

    while (1) {


        inByte = UART1RX(); // /* JTR usb port; */; //grab it
        rawCommand = (inByte >> 4); //get command bits in seperate variable

        switch (rawCommand) {
            case 0://reset/setup/config commands
                switch (inByte) {
                    case 0://0, reset exit
                        //cleanup!!!!!!!!!!
                        return; //exit
                        break;
                    case 1://id reply string
                        binrawversionString(); //reply string
                        break;
                    case 2://start bit
                        bbI2Cstart();
                        UART1TX(1);
                        break;
                    case 3://stop bit
                        bbI2Cstop();
                        UART1TX(1);
                        break;
                    case 4: //cs low
                        bbCS(0);
                        UART1TX(1);
                        break;
                    case 5://cs high
                        bbCS(1);
                        UART1TX(1);
                        break;
                    case 6://read byte
                        if (wires == 2) {
                            i = bbReadByte();
                        } else {
                            i = bbReadWriteByte(0xff);
                        }
                        if (modeConfig.lsbEN == 1) {//adjust bitorder
                            i = bpRevByte(i);
                        }
                        UART1TX(i);
                        break;
                    case 7://read bit
                        UART1TX(bbReadBit());
                        break;
                    case 8://peek bit
                        UART1TX(bbMISO());
                        break;
                    case 9://clock tick
                        bbClockTicks(1);
                        UART1TX(1);
                        break;
                    case 10://clock low
                        bbCLK(0);
                        UART1TX(1);
                        break;
                    case 11://clock high
                        bbCLK(1);
                        UART1TX(1);
                        break;
                    case 12://data low
                        bbMOSI(0);
                        UART1TX(1);
                        break;
                    case 13://data high
                        bbMOSI(1);
                        UART1TX(1);
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
                    c = UART1RX(); // /* JTR usb port; */;
                    if (modeConfig.lsbEN == 1) {//adjust bitorder
                        c = bpRevByte(c);
                    }
                    if (wires == 2) {//2 wire, send 1
                        bbWriteByte(c); //send byte
                        UART1TX(1);
                    } else { //3 wire, return read byte
                        c = bbReadWriteByte(c); //send byte
                        if (modeConfig.lsbEN == 1) {//adjust bitorder
                            c = bpRevByte(c);
                        }
                        UART1TX(c);
                    }
                }

                break;

            case 0b0010://bulk clock ticks
                inByte &= (~0b11110000); //clear command portion
                inByte++; //increment by 1, 0=1byte
                bbClockTicks(inByte);
                UART1TX(1); //send 1/OK
                break;

            case 0b0011: //# 0011xxxx - Bulk bits, send 1-8 bits of the next byte (0=1bit!)
                inByte &= (~0b11110000); //clear command portion
                inByte++; //increment by 1, 0=1byte
                UART1TX(1); //send 1/OK

                rawCommand = UART1RX(); //  //get byte, reuse rawCommand variable
                for (i = 0; i < inByte; i++) {
                    if (rawCommand & 0b10000000) {//send 1
                        bbWriteBit(1); //send bit
                    } else { //send 0
                        bbWriteBit(0); //send bit
                    }
                    rawCommand = rawCommand << 1; //pop the MSB off
                }
                UART1TX(1);
                break;

            case 0b1010:// PIC commands

                switch (inByte) {
                    case 0b10100000:

                        picMode = UART1RX(); // /* JTR usb port; */; //get byte
                        UART1TX(1); //send 1/OK
                        break;
                    case 0b10100100: //write
                        switch (picMode) {
                            case PIC416:

                                //get the number of commands that will follow
                                cmds = UART1RX(); //  //get byte, reuse rawCommand variable
                                cmds = cmds * 3; //make sure an int
                                //get command byte, two data bytes
                                for (j = 0; j < cmds; j++) {

                                    bpConfig.terminalInput[j] = UART1RX(); // /* JTR usb port; */;
                                }

                                for (j = 0; j < cmds; j = j + 3) {
                                    PIC416Write(bpConfig.terminalInput[j], bpConfig.terminalInput[j + 1], bpConfig.terminalInput[j + 2]);
                                }

                                UART1TX(1); //send 1/OK
                                break;
                            case PIC424:
                                //get the number of commands that will follow
                                cmds = UART1RX(); // /* JTR usb port; */; //get byte, reuse rawCommand variable
                                cmds = cmds * 4; //make sure an int
                                //get three byte command, 1 byte pre-post NOP
                                for (j = 0; j < cmds; j++) {

                                    bpConfig.terminalInput[j] = UART1RX(); // /* JTR usb port; */;
                                }

                                for (j = 0; j < cmds; j = j + 4) {
                                    //do any pre instruction NOPs

                                    //send four bit SIX command (write)
                                    bbWriteBit(0); //send bit
                                    bbWriteBit(0); //send bit
                                    bbWriteBit(0); //send bit
                                    bbWriteBit(0); //send bit

                                    //send data payload
                                    bbWriteByte(bpConfig.terminalInput[j]); //send byte
                                    bbWriteByte(bpConfig.terminalInput[j + 1]); //send byte
                                    bbWriteByte(bpConfig.terminalInput[j + 2]); //send byte

                                    //do any post instruction NOPs
                                    bpConfig.terminalInput[j + 3] &= 0x0F;
                                    for (i = 0; i < bpConfig.terminalInput[j + 3]; i++) {
                                        PIC24NOP();
                                    }
                                }
                                UART1TX(1); //send 1/OK
                                break;
                            default:
                                UART1TX(0); //send 1/OK
                                break;
                        }
                        break;
                    case 0b10100101://write x bit command, read x bits and return in 2 bytes
                        switch (picMode) {
                            case PIC416:

                                //get the number of commands that will follow
                                cmds = UART1RX(); // //get byte, reuse rawCommand variable
                                //cmds=cmds; //make sure an int
                                //get teh command to send on each read....

                                rawCommand = UART1RX(); // /* JTR usb port; */;


                                for (j = 0; j < cmds; j++) {
                                    //write command
                                    c = rawCommand; //temporary varaible
                                    for (i = 0; i < 4; i++) {
                                        if (c & 0b1) {//send 1
                                            bbWriteBit(1); //send bit
                                        } else { //send 0
                                            bbWriteBit(0); //send bit
                                        }
                                        c = c >> 1; //pop the LSB off
                                    }
                                    bbReadByte(); //dummy byte, setup input
                                    UART1TX(bbReadByte());
                                }
                                break;
                            case PIC424:
                                //get the number of commands that will follow
                                cmds = UART1RX(); //  //get byte, reuse rawCommand variable
                                //cmds=cmds; //make sure an int
                                //get the command to send before the read...
                                //rawCommand=UART1RX(); //

                                /*
                                                                                                                                        pic[0]=UART1RX(); ////get byte, reuse rawCommand variable

                                pic[1] = UART1RX(); //; //get byte, reuse rawCommand variable
                                pic[2] = UART1RX(); //  //get byte, reuse rawCommand variable


                                for (j = 0; j < cmds; j++) {
                                    //write command
                                    //send four bit SIX command (write)
                                    bbWriteBit(0); //send bit
                                    bbWriteBit(0); //send bit
                                    bbWriteBit(0); //send bit
                                    bbWriteBit(0); //send bit

                                    //send data payload
                                    bbWriteByte(pic[0]); //send byte
                                    bbWriteByte(pic[1]); //send byte
                                    bbWriteByte(pic[2]); //send byte

                                    //do any post instruction NOPs
                                    //bpConfig.terminalInput[j+3]&=0x0F;
                                    //for(i=0; i<2; i++){
                                    PIC24NOP();
                                    PIC24NOP();
                                    //}

                                    //send four bit REGOUT command (read)
                                    bbWriteBit(1); //send bit
                                    bbWriteBit(0); //send bit
                                    bbWriteBit(0); //send bit
                                    bbWriteBit(0); //send bit

                                    //one byte output
                                    bbWriteByte(0x00); //send byte

                                    //read 2 bytes
                                    //return bytes
                                    UART1TX(bbReadByte());
                                    UART1TX(bbReadByte());

                                    //ALWAYS POST nop TWICE after a read
                                    PIC24NOP();
                                    PIC24NOP();
                                }
                                 */
                                for (j = 0; j < cmds; j++) {
                                    //write command
                                    PIC424Write(0xBA0B96, 2);
                                    PIC424Read();

                                    PIC424Write(0xBADBB6, 2);
                                    PIC424Write(0xBAD3D6, 2);
                                    PIC424Read();

                                    PIC424Write(0xBA0BB6, 2);
                                    PIC424Read();

                                }
                                break;
                        }
                        break;
                    default:
                        UART1TX(0x00); //send 0/Error
                        break;
                }

                break;

                //case 0b0101: //# 0101xxxx - Bulk read, read 1-16bytes (0=1byte!)

            case 0b0100: //configure peripherals w=power, x=pullups, y=AUX, z=CS
                binIOperipheralset(inByte);
                UART1TX(1); //send 1/OK
                break;

            case 0b0110://set speed
                inByte &= (~0b11111100); //clear command portion
                modeConfig.speed = inByte;
                bbSetup(wires, modeConfig.speed);
                bbCS(1); //takes care of custom HiZ settings too
                UART1TX(1);
                break;

            case 0b1000: //set config
                //wxyz //w=HiZ(0)/3.3v(1), x=3wireenable, y=lsb, z=n/a
                modeConfig.HiZ = 0;
                if ((inByte & 0b1000) == 0) modeConfig.HiZ = 1; //hiz output if this bit is 1

                wires = 2;
                if (inByte & 0b100) wires = 3; //3wire/2wire toggle

                modeConfig.lsbEN = 0;
                if (inByte & 0b10) modeConfig.lsbEN = 1; //lsb/msb, bit order

                //if(inByte&0b1) //bit unused

                bbSetup(wires, modeConfig.speed); //setup the bitbang library, must be done before calling bbCS below
                bbCS(1); //takes care of custom HiZ settings too
                UART1TX(1); //send 1/OK
                break;

            default:
                UART1TX(0x00); //send 0/Error
                break;
        }//command switch
    }//while loop

}

void PIC416Write(unsigned char cmd, unsigned char datl, unsigned char dath) {
    unsigned char i, delay;

    //use upper 2 bits of pic[0] to determine a delay, if any.
    delay = cmd >> 6;
    //needs to support 4 or 6 or etc modes
    //should steal from pic.c
    for (i = 0; i < 4; i++) {

        //hold data for write time
        if (i == 3 && (delay > 0)) {
            bbCLK(1);
            bpDelayMS(delay);
            bbCLK(0);
            continue;
        }

        if (cmd & 0b1) {//send 1
            bbWriteBit(1); //send bit
        } else { //send 0
            bbWriteBit(0); //send bit
        }
        cmd = cmd >> 1; //pop the LSB off
    }

    //needs to support 14 or 16 bit writes
    //should steal from pic.c
    bbWriteByte(datl); //send byte
    bbWriteByte(dath); //send byte

}

void PIC424Write(unsigned long cmd, unsigned char pn) {
    unsigned char i;
    //send four bit SIX command (write)
    bbWriteBit(0); //send bit
    bbWriteBit(0); //send bit
    bbWriteBit(0); //send bit
    bbWriteBit(0); //send bit

    //send data payload 0xBA0B96 0xBADBB6 0xBA0BB6
    bbWriteByte(bpRevByte(cmd)); //send byte
    bbWriteByte(bpRevByte(cmd >> 8)); //send byte
    bbWriteByte(bpRevByte(cmd >> 16)); //send byte

    //do any post instruction NOPs
    pn &= 0x0F;
    for (i = 0; i < pn; i++) {
        PIC24NOP();
    }
}

void PIC424Read(void) {
    //send four bit REGOUT command (read)
    bbWriteBit(1); //send bit
    bbWriteBit(0); //send bit
    bbWriteBit(0); //send bit
    bbWriteBit(0); //send bit

    //one byte output
    bbWriteByte(0x00); //send byte

    //read 2 bytes
    //return bytes
    UART1TX(bbReadByte());
    UART1TX(bbReadByte());

    //ALWAYS POST nop TWICE after a read
    PIC24NOP();
    PIC24NOP();
}

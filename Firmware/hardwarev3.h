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
//this profile is for the Bus Pirate v3 hardware from dangerousprototypes.com
#define BP_VERSION_STRING "Bus Pirate v3"// (Seeed Studio)"
#define BP_VERSION "v3"

#define NUM_OF_SUMP_CHANNELS 5

//set the pin to use for POST detection
#define BP_POST 	PORTBbits.RB0
#define BP_POST_DIR TRISBbits.TRISB0

//this configures the placement of the serial port for 
//the PC side terminal interface
//the other modules should be similarly abstracted, but since v0A and v1a share pins, it doesn't mater
#define BP_TERM_TX		U1TX_IO;
#define BP_TERM_TX_RP  	RPOR2bits.RP4R
#define BP_TERM_RX		RPINR18bits.U1RXR
#define BP_TERM_RX_RP	5

//this is a different type of pin identifier used by the bitbang library
#define AUX		0b10000000000 //RB10 on v1, 2, 2go, 3 (not v0)
#define MOSI 	0b1000000000  //RB9
#define CLK 	0b100000000    //RB8
#define MISO 	0b10000000 //RB7
#define CS 		0b1000000 //RB6
#define IOPOR 	PORTB
#define IOLAT	LATB
#define IODIR	TRISB
#define ALLIO  (MOSI+CLK+MISO+CS)

//these macros set the pin IO used in each module
#define BP_MOSI 		PORTBbits.RB9
#define BP_CLK 			PORTBbits.RB8
#define BP_MISO 		PORTBbits.RB7
#define BP_CS 			PORTBbits.RB6
#define BP_AUX0 			PORTBbits.RB10 //moved pin5 (AUX) to RB10
#define BP_LEDMODE		PORTAbits.RA1 //single MODE led on v2a
#define BP_VREGEN		PORTAbits.RA0 //single vreg enable pin on v2a
#define BP_PULLUP		PORTBbits.RB11 //Pull-up pin on V2a
#define BP_PGD			PORTBbits.RB0 //PGD pin on programming header

//these macros set the direction registers for each IO pin
#define BP_MOSI_DIR 	TRISBbits.TRISB9
#define BP_CLK_DIR 		TRISBbits.TRISB8
#define BP_MISO_DIR 	TRISBbits.TRISB7
#define BP_CS_DIR 		TRISBbits.TRISB6
#define BP_AUX0_DIR 		TRISBbits.TRISB10
#define BP_LEDMODE_DIR	TRISAbits.TRISA1
#define BP_VREGEN_DIR	TRISAbits.TRISA0
#define BP_PULLUP_DIR	TRISBbits.TRISB11
#define BP_PGD_DIR		TRISBbits.TRISB0

//Open drain/high impedance pin setup
#define BP_MOSI_ODC 	ODCBbits.ODB9
#define BP_CLK_ODC 		ODCBbits.ODB8
#define BP_MISO_ODC 	ODCBbits.ODB7
#define BP_CS_ODC 		ODCBbits.ODB6
#define BP_AUX0_ODC 		ODCBbits.ODB10

//Change notice assignment
#define BP_MOSI_CN		CNEN2bits.CN21IE
#define BP_CLK_CN		CNEN2bits.CN22IE

//ADC assignment
#define BP_ADC_PROBE 12
#define BP_ADC_3V3 10
#define BP_ADC_5V0 9
#define BP_ADC_VPU 11

//calculate the acceptable range of voltages for the power supplies
//these values will be compared to the ADC reading from the supply monitor
#define V5BASE 0x307 //(((5/2)/3.3)*1024))
#define V5H	V5BASE+0x50
#define V5L V5BASE-0x50

#define V33BASE 0x200 //(((3.3/2)/3.3)*1024))
#define V33H	V33BASE+0x50
#define V33L	V33BASE-0x50

#define ADCON() AD1CON1bits.ADON = 1 // turn ADC ON 
#define ADCOFF() AD1CON1bits.ADON = 0 // turn ADC OFF 

//ADC pin configuration
#define BP_ADC_PROBE_CFG AD1PCFGbits.PCFG12 //B12/AN12/ADC1/EXT 
#define BP_ADC_3V3_CFG AD1PCFGbits.PCFG10 //B14/AN10/ADC3/33V 
#define BP_ADC_5V0_CFG AD1PCFGbits.PCFG9 //B15/AN9/ADC4/50V
#define BP_ADC_VPU_CFG AD1PCFGbits.PCFG11 //B13/AN11/ADC2/Vextpullup

#define BP_ADC_PINSETUP() BP_ADC_PROBE_CFG=0;BP_ADC_3V3_CFG=0;BP_ADC_5V0_CFG=0;BP_ADC_VPU_CFG=0

//flow control connections for FTDI chip
//used in uart brige
#define FTDI_RTS_DIR TRISAbits.TRISA4
#define FTDI_RTS PORTAbits.RA4
#define FTDI_CTS_DIR TRISAbits.TRISA3
#define FTDI_CTS PORTAbits.RA3

//defines the PPS input/output for MISO and MOSI pins
#define BP_MISO_RPIN 7
#define BP_MISO_RPOUT RPOR3bits.RP7R
#define BP_MOSI_RPIN 9
#define BP_MOSI_RPOUT RPOR4bits.RP9R

//define the PPS input/output for the AUX pin
#define BP_AUX_RPIN 10	
#define BP_AUX_RPOUT RPOR5bits.RP10R
#define BP_CS_RPIN 6
#define BP_CS_RPOUT RPOR5bits.RP6R

//define the PPS input/output for CLK pin
#define BP_CLK_RPIN 8
#define BP_CLK_RPOUT RPOR4bits.RP8R

//pseudofunctions for pullup resistors
//for V2/3 we need B5/pullup to be output and gnd to turn OFF the pullups...
#define BP_PULLUP_ON() BP_PULLUP_DIR=1;BP_PULLUP=0
#define BP_PULLUP_OFF() BP_PULLUP=0;BP_PULLUP_DIR=0

//pseudofunctions for voltage regulator switch
#define BP_VREG_ON() BP_VREGEN_DIR=0; BP_VREGEN=1
#define BP_VREG_OFF() BP_VREGEN_DIR=0; BP_VREGEN=0

#define BP_MODELED_ON() LATA|=0b10
#define BP_MODELED_OFF()LATA&=(~0b10)

//define device ID and revision memory location
#define DEV_ADDR_UPPER	0x00FF
#define DEV_ADDR_TYPE	0x0000
#define DEV_ADDR_REV	0x0002
#define CFG_ADDR_UPPER 0x0000
#define BL_ADDR_VER		0xABFA //location of the bootloader version info
#define CFG_ADDR_1     0xABFC
#define CFG_ADDR_2     0xABFE

// Peripheral Pin Select Outputs
#define NULL_IO		0
#define C1OUT_IO	1
#define C2OUT_IO	2
#define U1TX_IO		3
#define U1RTS_IO	4
#define U2TX_IO		5
#define U2RTS_IO	6
#define SDO1_IO		7
#define SCK1OUT_IO	8
#define SS1OUT_IO	9
#define SDO2_IO		10
#define SCK2OUT_IO	11
#define SS2OUT_IO	12
#define OC1_IO		18
#define OC2_IO		19
#define OC3_IO		20
#define OC4_IO		21
#define OC5_IO		22

#define PIC_REV_A3	0x3003
#define PIC_REV_B4	0x3042
#define PIC_REV_B5	0x3043
#define PIC_REV_B8	0x3046


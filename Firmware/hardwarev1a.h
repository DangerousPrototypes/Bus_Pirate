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

//this hardware profile is for the second, surface mount Bus Pirate.
//http://hackaday.com/2009/01/22/how-to-bus-pirate-v1-improved-universal-serial-interface

#define BP_VERSION_STRING "Bus Pirate v1a"
#define BP_VERSION "v1a"

//this configures the placement of the serial port for 
//the PC side terminal interface
//the other modules should be similarly abstracted, but since v0A and v1a share pins, it doesn't mater
//when we move to a USB PIC, we'll need to do that....
#define BP_TERM_TX		U1TX_IO;
#define BP_TERM_TX_RP  	RPOR1bits.RP2R
#define BP_TERM_RX		RPINR18bits.U1RXR
#define BP_TERM_RX_RP	3

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
#define BP_AUX 			PORTBbits.RB10 //moved pin5 (AUX) to RB10
#define BP_PIN6			PORTBbits.RB11
#define BP_LEDMODE		PORTBbits.RB4
#define BP_LEDPWR		PORTAbits.RA3
#define BP_EN50V		PORTAbits.RA4
#define BP_EN33V		PORTBbits.RB5
#define BP_VREGEN		BP_EN33V //duplicate so it's compatiable wtih v2 code in procMenu.c
#define BP_PULLUP		BP_EN33V //duplicate so it's compatiable wtih v2 code in procMenu.c

//these macros set the direction registers for each IO pin
#define BP_MOSI_DIR 	TRISBbits.TRISB9
#define BP_CLK_DIR 		TRISBbits.TRISB8
#define BP_MISO_DIR 	TRISBbits.TRISB7
#define BP_CS_DIR 		TRISBbits.TRISB6
#define BP_AUX_DIR 		TRISBbits.TRISB10
#define BP_PIN6_DIR 	TRISBbits.TRISB11
#define BP_LEDMODE_DIR		TRISBbits.TRISB4
#define BP_LEDPWR_DIR		TRISAbits.TRISA3
#define BP_EN50V_DIR	TRISAbits.TRISA4
#define BP_EN33V_DIR	TRISBbits.TRISB5

//Open drain/high impedance pin setup
#define BP_MOSI_ODC 	ODCBbits.ODB9
#define BP_CLK_ODC 		ODCBbits.ODB8
#define BP_MISO_ODC 	ODCBbits.ODB7
#define BP_CS_ODC 		ODCBbits.ODB6
#define BP_AUX_ODC 		ODCBbits.ODB10

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

//define the PPS input/output for the AUX pin
#define BP_AUX_RPIN 10	
#define BP_AUX_RPOUT RPOR5bits.RP10R
#define BP_CS_RPIN 6
#define BP_CS_RPOUT RPOR5bits.RP6R

//pseudo function to reset pins to hizstate
//pseudofunctions for voltage regulator switch
#define BP_VREG_ON() BP_EN50V_DIR=0; BP_EN50V=1; BP_EN33V_DIR=0; BP_EN33V=1
#define BP_VREG_OFF() BP_EN50V_DIR=0; BP_EN50V=0; BP_EN33V_DIR=0; BP_EN33V=0

#define BP_AUX2_HI() BP_PIN6_DIR=1;BP_PIN6=0
#define BP_AUX2_LOW() BP_PIN6=0;BP_PIN6_DIR=0
#define BP_AUX2_IN() BP_PIN6_DIR=0;BP_PIN6=0

//define device ID and revision memory location
#define DEV_ADDR_UPPER	0x00FF
#define DEV_ADDR_TYPE	0x0000
#define DEV_ADDR_REV	0x0002
#define CFG_ADDR_UPPER 0x0000
#define BL_ADDR_VER	0xABFA //location of the bootloader version info
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


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
#define BP_VERSION_STRING "Bus Pirate v4"// (Seeed Studio)"
#define BP_VERSION "v4"

//this is a different type of pin identifier used by the bitbang library
#define MOSI 	0b10  
#define CLK 	0b100  
#define MISO 	0b1000 
#define CS 		0b10000 
#define AUX		0b100000
#define IOPOR 	PORTD
#define IOLAT	LATD
#define IODIR	TRISD
#define ALLIO  (MOSI+CLK+MISO+CS)

//these macros set the pin IO used in each module
#define BP_MOSI 		PORTDbits.RD1
#define BP_CLK 			PORTDbits.RD2
#define BP_MISO 		PORTDbits.RD3
#define BP_CS 			PORTDbits.RD4
#define BP_AUX 			PORTDbits.RD5 //moved pin5 (AUX) to RB10
#define BP_LEDMODE		PORTBbits.RB8 //single MODE led on v2a
#define BP_VREGEN		PORTBbits.RB9 //single vreg enable pin on v2a
#define BP_PULLUP		PORTEbits.RE4 //Pull-up pin on V2a
#define BP_PGD			PORTBbits.RB7 //PGD pin on programming header

//new in v4
#define BP_LEDUSB		PORTBbits.RB10
#define BP_BUTTON		PORTCbits.RC14
#define BP_AUX1 		PORTDbits.RD8 
#define BP_AUX2 		PORTDbits.RD0 
#define BP_EE_SDA 		PORTDbits.RD9 
#define BP_EE_SCL 		PORTDbits.RD10 
#define BP_EE_WP 		PORTDbits.RD11
#define BP_PUVSEL33		PORTFbits.RF4
#define BP_PUVSEL50		PORTFbits.RF5
/*
//these macros set the pin IO used in each module
//#define BP_MOSI 		PORTDbits.RD1
//#define BP_CLK 			PORTDbits.RD2
//#define BP_MISO 		PORTDbits.RD3
//#define BP_CS 			PORTDbits.RD4
#define BP_AUX_LAT 			LATDbits.LATD5 //moved pin5 (AUX) to RB10
//#define BP_LEDMODE		PORTBbits.RB8 //single MODE led on v2a
//#define BP_VREGEN		PORTBbits.RB9 //single vreg enable pin on v2a
//#define BP_PULLUP		PORTEbits.RE4 //Pull-up pin on V2a
//#define BP_PGD			PORTBbits.RB7 //PGD pin on programming header

//new in v4
//#define BP_LEDUSB		PORTBbits.RB10
//#define BP_BUTTON		PORTCbits.RC14
//#define BP_AUX1 		PORTFbits.RF5 
//#define BP_AUX2 		PORTFbits.RF4 
//#define BP_EE_SDA 		PORTDbits.RD9 
//#define BP_EE_SCL 		PORTDbits.RD10 
//#define BP_EE_WP 		PORTDbits.RD11
#define BP_PUVSEL33_LAT		LATBbits.LATB3
#define BP_PUVSEL50_LAT		LATBbits.LATB4
*/
//these macros set the direction registers for each IO pin
#define BP_MOSI_DIR 	TRISDbits.TRISD1
#define BP_CLK_DIR 		TRISDbits.TRISD2
#define BP_MISO_DIR 	TRISDbits.TRISD3
#define BP_CS_DIR 		TRISDbits.TRISD4
#define BP_AUX_DIR 		TRISDbits.TRISD5
#define BP_LEDMODE_DIR	TRISBbits.TRISB8
#define BP_VREGEN_DIR	TRISBbits.TRISB9
#define BP_PULLUP_DIR	TRISEbits.TRISE4
#define BP_PGD_DIR		TRISBbits.TRISB7

//new in v4
#define BP_LEDUSB_DIR	TRISBbits.TRISB10
#define BP_BUTTON_DIR	TRISCbits.TRISC14
#define BP_AUX1_DIR		TRISDbits.TRISD8
#define BP_AUX2_DIR		TRISDbits.TRISD0
#define BP_EE_SDA_DIR 	TRISDbits.TRISD9
#define BP_EE_SCL_DIR 	TRISDbits.TRISD10
#define BP_EE_WP_DIR 	TRISDbits.TRISD11
#define BP_PUVSEL33_DIR	TRISFbits.TRISF4
#define BP_PUVSEL50_DIR	TRISFbits.TRISF5


//secondary MISO/CLK/MOSI connection setup (input, low)
#define BPV4_HWI2CPINS_SETUP() TRISE|=0b11100000; LATE&=(~0b11100000)

#define BP_EEPROM_SETUP() BP_EE_SDA_DIR=1; BP_EE_SCL_DIR=1;BP_EE_WP_DIR=0;BP_EE_SCL=0; BP_EE_SDA=0; BP_EE_WP=1 //EEPROM pins to input, write protect on

#define BP_BUTTON_IF IFS1bits.CNIF
#define BP_BUTTON_SETUP() BP_BUTTON_DIR=1; CNPU1|=0b1; CNEN1|=0b1; IEC1bits.CNIE=0; BP_BUTTON_IF=0;

//pullup voltage enable/disable
//always disables the other pullup
#define BP_3V3PU_ON() BP_PUVSEL50_DIR=1; BP_PUVSEL33=0; BP_PUVSEL33_DIR=0
#define BP_3V3PU_OFF() BP_PUVSEL50_DIR=1; BP_PUVSEL33_DIR=1

#define BP_5VPU_ON() BP_PUVSEL33_DIR=1; BP_PUVSEL50=0; BP_PUVSEL50_DIR=0
#define BP_5VPU_OFF() BP_PUVSEL33_DIR=1; BP_PUVSEL50_DIR=1

//Open drain/high impedance pin setup
#define BP_MOSI_ODC 	ODCDbits.ODD1
#define BP_CLK_ODC 		ODCDbits.ODD2
#define BP_MISO_ODC 	ODCDbits.ODD3
#define BP_CS_ODC 		ODCDbits.ODD4
#define BP_AUX_ODC 		ODCDbits.ODD5

//Change notice assignment
#define BP_MOSI_CN		CNEN4bits.CN50IE
#define BP_CLK_CN		CNEN4bits.CN51IE

//ADC assignment
#define BP_ADC_PROBE 1
#define BP_ADC_3V3 0
#define BP_ADC_5V0 2
#define BP_ADC_VPU 5
#define BP_ADC_USB 15

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
#define BP_ADC_PROBE_CFG AD1PCFGbits.PCFG2 //AN1/ADC1/EXT 
#define BP_ADC_3V3_CFG AD1PCFGbits.PCFG1 //AN0/ADC3/33V 
#define BP_ADC_5V0_CFG AD1PCFGbits.PCFG0 //AN2/ADC4/50V
#define BP_ADC_VPU_CFG AD1PCFGbits.PCFG5 //AN5/ADC2/Vextpullup
#define BP_ADC_USB_CFG AD1PCFGbits.PCFG15 //AN15/ADC5/Vusb

#define BP_ADC_PINSETUP() BP_ADC_PROBE_CFG=0;BP_ADC_3V3_CFG=0;BP_ADC_5V0_CFG=0;BP_ADC_VPU_CFG=0;BP_ADC_USB_CFG=0

//error fixing defines (find with keyword #BPV4
#define OC5CON OC5CON1

//define the PPS input/output for the AUX pin
#define BP_AUX_RPIN 20	
#define BP_AUX_RPOUT RPOR10bits.RP20R
#define BP_CS_RPIN 25
#define BP_CS_RPOUT RPOR12bits.RP25R

//pseudofunctions for pullup resistors
//for V2/3 we need B5/pullup to be output and gnd to turn OFF the pullups...
#define BP_PULLUP_ON() BP_PULLUP_DIR=1;BP_PULLUP=0
#define BP_PULLUP_OFF() BP_PULLUP=0;BP_PULLUP_DIR=0

//pseudofunctions for voltage regulator switch
#define BP_VREG_ON() BP_VREGEN_DIR=0; BP_VREGEN=1
#define BP_VREG_OFF() BP_VREGEN_DIR=0; BP_VREGEN=0

//pseudofunctions for USB, MODE LEDs
#define BP_USBLED_ON() LATB&=(~0x400);TRISB&=(~0x400)
#define BP_USBLED_OFF() LATB&=(~0x400);TRISB|=0x400
#define BP_USBLED_T() LATB&=(~0x400);TRISB^=0x400
#define BP_MODELED_ON() LATB|=0x100
#define BP_MODELED_OFF()LATB&=(~0x100)

//define device ID and revision memory location
#define DEV_ADDR_UPPER	0x00FF
#define DEV_ADDR_TYPE	0x0000
#define DEV_ADDR_REV	0x0002
#define CFG_ADDR_UPPER 0x0001 //should be 0x0001, but it crashes...
#define BL_ADDR_VER		0xABFA //location of the bootloader version info
#define CFG_ADDR_0	0x57FA
#define CFG_ADDR_1     0x57FC
#define CFG_ADDR_2     0x57FE

//these are unneeded, but the stack complains if they're not defined
//#define USE_SELF_POWER_SENSE_IO
#define tris_self_power     TRISCbits.TRISC2    
#define self_power          1

//#define USE_USB_BUS_SENSE_IO
#define tris_usb_bus_sense  TRISCbits.TRISC2    
#define USB_BUS_SENSE       U1OTGSTATbits.SESVD 

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

//#define putUnsignedCharArrayUsbUsart(u8Array,Num)       putPARTARRAYUSBUSART((BYTE*)(u8Array),(Num))
////#define putUnsignedCharArrayUsbUsart(u8Array,Num) 	putUSBUSART((unsigned char*)(u8Array),(Num))
//#define getUnsignedCharArrayUsbUart(u8Array,Num)	getsUSBUSART(( unsigned char*)(u8Array),(Num))

#define PIC_REV_A3	0x0001
#define PIC_REV_A5	0x0003

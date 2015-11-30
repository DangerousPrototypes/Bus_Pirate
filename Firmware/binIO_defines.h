/*
 * This file is part of the Bus Pirate project (https://github.com/DangerousPrototypes/Bus_Pirate).
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
/* 
 * File:   binIO_defines.h
 * Author: Andrew Haines
 *
 * Created on November 28, 2015, 3:48 PM
 */

#ifndef BINIO_DEFINES_H
#define	BINIO_DEFINES_H

#define BP_OK			1
#define BP_NOT_OK		0

#define BP_MODE_BINARY          0b00000000 // Reset to raw BB mode, get raw BB version string
#define BP_MODE_SPI             0b00000001 // enter rawSPI mode  
#define BP_MODE_I2C             0b00000010 // enter raw I2C mode
#define BP_MODE_UART            0b00000011 // enter raw UART mode
#define BP_MODE_RAW_1WIRE       0b00000100 // enter raw 1-wire
#define BP_MODE_RAW_WIRE        0b00000101 // enter raw wire mode
#define BP_MODE_OPEN_OCD        0b00000110 // enter openOCD
#define BP_MODE_PIC_PROGRAM     0b00000111 // pic programming mode
#define BP_MODE_XSVF_PLAYER     0b00011000 // XSVF Player // BPv4 only. JM

#define BP_BINM_RESET_TO_TERM   0b00001111 // reset, return to user terminal
#define BP_BINM_SELF_TEST_SHORT 0b00010000 // short self test
#define BP_BINM_SELF_TEST_FULL  0b00010001 // full self test with jumpers
#define BP_BINM_SETUP_PWM       0b00010010 // setup PWM
#define BP_BINM_CLEAR_PWM       0b00010011 // clear PWM
#define BP_BINM_ADC_MEASURE     0b00010100 // ADC measurement

// Added JM  Only with BP4
#define BP_BINM_ADC_MEASURE_CONT 0b00010101 // ADC ....
#define BP_BINM_ADC_STOP_CONT    0b00010110 // ADC Stop
// End added JM

#define BP_BINM_SET_PINS        0b10000000 // SET PINS

// 010xxxxx  – Configure pins as input(1) or output(0): AUX|MOSI|CLK|MISO|CS
#define BP_BINM_CONFIGURE_PINS  0b01000000 // configure pins 

#define BP_PIN_VREG             0b1000000
#define BP_PIN_PULLUPS          0b0100000
#define BP_PIN_AUX0             0b0010000
#define BP_PIN_MOSI             0b0001000
#define BP_PIN_CLK              0b0000100
#define BP_PIN_MISO             0b0000010
#define BP_PIN_CS               0b0000001


#endif	/* BINIO_DEFINES_H */


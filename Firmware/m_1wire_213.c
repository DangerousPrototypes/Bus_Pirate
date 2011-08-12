//    1-wire bus communications routines
//    Copyright (C) 2000  Michael Pearce <mike (at) kiwacan.co.nz>
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//****************************************************************************
//                            1wire002.c
//                          Version 2.1.3
//                         25 October 2000
//
//  Version 2 of the Dallas 1-wire Bus communications routines for Hi-Tech C
//  This Time Sat down and worked out my way of doing it!!
//
//
//  Author: Michael Pearce
//
//  Started: 29 June 1998
//
//****************************************************************************
//                      Version Information
//****************************************************************************
// Version 2.1.3 - 25 October 2000
// Changed the Reset Error Code return so you can identify what error occured
//
//****************************************************************************
// Version 2.1.2 - 7 August 2000
// Changed some of the timings to see if can improve the data integrity.
//
//****************************************************************************
// Version 2.1.1 - 19 July 2000
// Removed Fixed Port settings and added Error message to indicate if
// ports have not been set up.
// Have done this to allow use on other processors without to much hassel
//
//****************************************************************************
// Version 2.1.0 - 24 July 1998
//    - Added ReadRom and Match Rom Commands - need definition to activate
//
//****************************************************************************
// Version 2.0.0 - 29 June 1998
//  Start of Project
//****************************************************************************

#include "base.h"

#ifdef BP_USE_1WIRE

#include "m_1wire_213.h"

//#define D_RiseSpace 5        // Rise Time + Minimum Space (3uS+1uS absolute min)
#define D_RiseSpace 10       // Rise Time + Minimum Space (3uS+1uS absolute min)

#ifndef SDA
 #error SDA Must Be Defined for 1-wire Bus I/O
#endif

#ifndef SDA_TRIS
 #error SDA_TRIS Must be defined for 1-wire Bus I/O
#endif

static unsigned char  D_Presence;
static unsigned char  D_ShortCircuit;
static unsigned char  D_Data;

//****************************************************************************
//D_Reset  -- Resets the 1-wire bus and checks for presence & short cct
//****************************************************************************
unsigned char OWReset(void){
	 char count=47;
	 //-- Reset the status bits
	 unsigned char D_Error=0;
	
	 //-- Ensure Correct port pin settings
	 SDA_TRIS=1;
	 SDA=0;
	
	 //-- Start the reset Pulse
	 SDA_TRIS=0;          //-- Pull Line Low to start reset pulse
	
	 for(;count>0;count--){
		bpDelayUS(10);      //-- 480uS Delay
	 }
	
	 SDA_TRIS=1;          //-- Release the line
	 bpDelayUS(60);       //-- Delay to about centre of presence pulse
	 D_Presence=SDA;  //-- Get Presence status 1=None 0=something there
	
	 for(count=41;count>0;count--){ //-- (480-60)uS delay till end
		bpDelayUS(10);
	 }
	
	 D_ShortCircuit=SDA;  //-- Check for short circuit on bus pin.
	
	 if(D_ShortCircuit==0){  //-- If short circuit then indicate an error
	  	D_Error=1;
	 }
	
	 if(D_Presence==1){      //-- If no device detected indicate an error
	  	D_Error+=2;
	 }
	
	 bpDelayUS(D_RiseSpace);    //-- Rise time + Min Space

	return D_Error;

}
//******************END OF D_Reset

//****************************************************************************
// D_Write
//****************************************************************************
void OWWriteByte(unsigned char Data){
	 char count=8;
	 for(;count>0;count--){
		  D_Data= Data & 0x01;    //-- Get LSB
		
		  //-- Write the bit to the port
		  SDA=0;
		  SDA_TRIS=0;               //-- Lower the port
		  bpDelayUS(5);             //-- Time slot start time
		  SDA_TRIS=D_Data;          //-- Output the data to the port
		  bpDelayUS(50);            //-- Finish Timeslot
		  bpDelayUS(5);
		
		  SDA_TRIS=1;               //-- Ensure Release of Port Pin
		
		  //-- Delay Between Bits
		  bpDelayUS(D_RiseSpace);             //-- Recovery time between Bits
		
		  //-- Prep Next Bit
		  Data=Data>>1;           //-- Shift next bit into place
	 }
	 bpDelayUS(D_RiseSpace);    //-- Rise time + Min Space
}
//******************END OF D_Write

//additional function that writes a single bit of val c
void OWWriteBit(unsigned char c){
	  //-- Write the bit to the port
	  SDA=0;
	  SDA_TRIS=0;               //-- Lower the port
	  bpDelayUS(5);             //-- Time slot start time
	  SDA_TRIS=c;          //-- Output the data to the port
	  bpDelayUS(50);            //-- Finish Timeslot
	  bpDelayUS(5);
	
	  SDA_TRIS=1;               //-- Ensure Release of Port Pin
	
	  //-- Delay Between Bits
	  bpDelayUS(D_RiseSpace);             //-- Recovery time between Bits
}

//****************************************************************************
// D_Read
//****************************************************************************
unsigned char OWReadByte(void){
	 char count=8;
	unsigned char data=0;
	 for(;count>0;count--){
		  //-- Write the bit to the port
		  SDA=0;
		  SDA_TRIS=0;               //-- Lower the port
			//  bpDelayUS(5);             //-- Time slot start time
		  bpDelayUS(3);             //-- Time slot start time
		  SDA_TRIS=1;               //-- Release port for reading
		  bpDelayUS(5);             //-- Get close to center of timeslot
		  //bpDelayUS(9);             //-- Get close to center of timeslot
		  D_Data=SDA;           //-- Read the data bit in
		  bpDelayUS(52);            //-- Finish the timeslot
		
		
		
		  //-- Put data into the byte
		  data = data >> 1;        //-- Shift previous bits across
		  if(D_Data==1) data +=0x80; //-- If data was high then add High bit to data
	
		  //-- Delay Between Bits
		  bpDelayUS(D_RiseSpace);             //-- Recovery time between Bits
	 }
	 bpDelayUS(D_RiseSpace);    //-- Rise time + Min Space
	 return(data);
}
//******************END OF D_Read

//additional function to read a single bit from the bus
unsigned char OWReadBit(void){
	 //-- Write the bit to the port
	 SDA=0;
	 SDA_TRIS=0;               //-- Lower the port
	//  bpDelayUS(5);             //-- Time slot start time
	 bpDelayUS(3);             //-- Time slot start time
	 SDA_TRIS=1;               //-- Release port for reading
	 bpDelayUS(5);             //-- Get close to center of timeslot
	 //bpDelayUS(9);             //-- Get close to center of timeslot
	 D_Data=SDA;           //-- Read the data bit in
	 bpDelayUS(52);            //-- Finish the timeslot		
	
	 //-- Delay Between Bits
	 bpDelayUS(D_RiseSpace);             //-- Recovery time between Bits
	
	return D_Data;
}

//**************************************************************************
//D_MatchRom - Matches the ROM Code to device
//**************************************************************************
#ifdef _D_MATCHROM_
void D_MatchRom(const char address[]){
	 char count;
	 D_Write(0x55);          //-- Match Rom
	                         //-- Send the Address ROM Code.
	 for(count=0;count<8;count++) D_Write(address[count]);
}
#endif
//****************** END OF D_SendAddress

//**************************************************************************
//D_ReadRom - Reads the ROM Code from a device
//**************************************************************************
#ifdef _D_READROM_
void D_ReadRom(char *address){
	 char count;
	 D_Write(0x33);          //-- Read Rom
	                         //-- Read in the ROM code from the Device
	 for(count=0;count<8;count++){
		address[count]=D_Read();
		address++
	 }
}
#endif
//****************** END OF D_ReadRom

//**************************************************************************
//
//**************************************************************************

//**************************************************************
//                      checksum.c
//                        V0.1.1
//                     25 October 2000
//
// - David Haywood 30AUG99
//
// - Michael Pearce - Modified For PIC12C6xx Processor use 28 August 2000
//    For use with HiTech - PIC C.
//
//**************************************************************
// V0.1.1 -25 October 2000
// Customised the Function names for the Heater2 Project
//**************************************************************
// V0.1.0 - 28 August 2000
// Mods done by Mike Pearce to make it work on a PIC12C67x
//**************************************************************


// MIKE,
//
// The CHECKSUM function calculates the CRC for incoming bytes stored in the
// array BYTE.  The returned CRC value can then be compared with the value
// given by the device.
//
// The other parameter required is the size (length) of the array BYTE.  This
// can be calculated using the "sizeof" function when CHECKSUM is called.
// eg. CHECKSUM(BYTE, sizeof(BYTE))
//
// The read_temp_sensor function (listed below CHECKSUM) gives an example of
// how CHECKSUM can be used to check the integrity of a reading from a DS1820
//
// - Originally from DALLAS SEMICONDUCTORS Application Note 27
//  Understanding & Using Cyclic Redundancy Checks with Dallas iButton Products
//
// - David Haywood 30AUG99
//
// - Michael Pearce - Modified For PIC12C6xx Processor use 28 August 2000
//    For use with HiTech - PIC C.
//

 // DECLARE AND SET VARIABLES
const unsigned char BYTE_ARRAY [256] =
 {
  0, 94, 188, 226, 97, 63, 221, 131, 194, 156, 126, 32, 163, 253, 31, 65,
  157, 195, 33, 127, 252, 162, 64, 30, 95, 1, 227, 189, 62, 96, 130, 220,
  35, 125, 159, 193, 66, 28, 254, 160, 225, 191, 93, 3, 128, 222, 60, 98,
  190, 224, 2, 92, 223, 129, 99, 61, 124, 34, 192, 158, 29, 67, 161, 255,
  70, 24, 250, 164, 39, 121, 155, 197, 132, 218, 56, 102, 229, 187, 89, 7,
  219, 133, 103, 57, 186, 228, 6, 88, 25, 71, 165, 251, 120, 38, 196, 154,
  101, 59, 217, 135, 4, 90, 184, 230, 167, 249, 27, 69, 198, 152, 122, 36,
  248, 166, 68, 26, 153, 199, 37, 123, 58, 100, 134, 216, 91, 5, 231, 185,
  140, 210, 48, 110, 237, 179, 81, 15, 78, 16, 242, 172, 47, 113, 147, 205,
  17, 79, 173, 243, 112, 46, 204, 146, 211, 141, 111, 49, 178, 236, 14, 80,
  175, 241, 19, 77, 206, 144, 114, 44, 109, 51, 209, 143, 12, 82, 176, 238,
  50, 108, 142, 208, 83, 13, 239, 177, 240, 174, 76, 18, 145, 207, 45, 115,
  202, 148, 118, 40, 171, 245, 23, 73, 8, 86, 180, 234, 105, 55, 213, 139,
  87, 9, 235, 181, 54, 104, 138, 212, 149, 203, 41, 119, 244, 170, 72, 22,
  233, 183, 85, 11, 136, 214, 52, 106, 43, 117, 151, 201, 74, 20, 246, 168,
  116, 42, 200, 150, 21, 75, 169, 247, 182, 232, 10, 84, 215, 137, 107, 53
 };
 // lookup array for CHECKSUM calcs


// Calculates DOW CRC value for a stream of bytes
// unsigned char CHECKSUM (unsigned char *BYTE, int size_of_BYTE)
unsigned char PROBE_CHECKSUM (unsigned char *BYTE, int size_of_BYTE){
	 // DECLARE VARIABLES
	 int BYTE_loop;
	 unsigned char CRC;
	
	 // FUNCTION BODY;
	 CRC = 0;
	 for (BYTE_loop = 0; BYTE_loop < size_of_BYTE; BYTE_loop++){
	  	CRC = BYTE_ARRAY[CRC ^ BYTE[BYTE_loop]]; // look up array for appropriate calc
	 }
	 return (CRC);
}

//****************** END OF
#endif

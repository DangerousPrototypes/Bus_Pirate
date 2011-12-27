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
//#include "uart2io.h"

//PPS Settings - Adjust in the correct hardware file.
#define UARTRX_PIN BP_MISO_RPIN
#define UARTTX_PIN BP_MOSI_RPOUT

// Old settings remain intact
//	#if defined (BUSPIRATEV3)
//		#define UARTRX_PIN 7
//		#define UARTTX_PIN RPOR4bits.RP9R
//	#elif defined (BUSPIRATEV4)
//		#define UARTRX_PIN 22
//		#define UARTTX_PIN RPOR12bits.RP24R
//	#endif

#define UARTTX_ODC BP_MOSI_ODC

void UART2Setup(unsigned int brg, unsigned char ODCoutput, unsigned char rxp, unsigned char dbp, unsigned char sb ){
	//use open drain control register to 
	//enable Hi-Z mode on hardware module outputs
	//inputs are already HiZ
	//if(ODCoutput==1){
		UARTTX_ODC=ODCoutput;//1;
	//}

	//assign UART2 with PPS
	RPINR19bits.U2RXR=UARTRX_PIN; //Inputs U2RX
	UARTTX_PIN=U2TX_IO;	// Outputs U2TX

    U2BRG = brg; 			//assign baud rate
    U2MODE = 0;
    U2MODEbits.BRGH = 1;
	U2MODEbits.RXINV=rxp; //rx polarity
	U2MODEbits.PDSEL=dbp; //databits and parity

	//U2MODEbits.PDSEL1=1;//dbp; //databits and parity
	U2MODEbits.STSEL=sb; //stop bits
    U2STA = 0;

}

void UART2Enable(void){
    U2MODEbits.UARTEN = 1;
    U2STAbits.UTXEN = 1;
    IFS1bits.U2RXIF = 0;
}

void UART2Disable(void){
    U2MODEbits.UARTEN = 0;
    U2STAbits.UTXEN = 0;
	RPINR19bits.U2RXR=0b11111;	//B7 U2RX
	
	//PPS
	UARTTX_PIN=0;
//		#if defined(BUSPIRATEV3)
//			RPOR3bits.RP6R=0; 			//B6 U2TX
//		#elif defined(BUSPIRATEV4)
//	        RPOR12bits.RP24R=0;      
//		#endif
	UARTTX_ODC=0;
}

void UART2TX(unsigned int c){
	while(U2STAbits.UTXBF == 1); //if buffer is full, wait
    U2TXREG = c;
}

unsigned int UART2RX(void){
    while(U2STAbits.URXDA == 0);
	return U2RXREG;
}

unsigned char UART2RXRdy(void){
    return U2STAbits.URXDA;
}

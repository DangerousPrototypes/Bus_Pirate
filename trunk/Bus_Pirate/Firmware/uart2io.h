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
void UART2Setup(unsigned int brg, unsigned char ODCoutput, unsigned char rxp, unsigned char dbpl, unsigned char sb );
void UART2Enable(void);
void UART2Disable(void);
void UART2TX(unsigned int c);
unsigned char UART2RXRdy(void);
unsigned int UART2RX(void);

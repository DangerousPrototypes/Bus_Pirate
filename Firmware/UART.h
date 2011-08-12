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
void uartProcess(void);
void binUART(void);

unsigned UARTwrite(unsigned int c);
unsigned int UARTread(void);
void UARTsetup(void);
void UARTcleanup(void);
void UARTmacro(unsigned int c);
void UARTstart(void);
void UARTstop(void);
unsigned int UARTperiodic(void);
void UARTpins(void);
void UARTsettings(void);


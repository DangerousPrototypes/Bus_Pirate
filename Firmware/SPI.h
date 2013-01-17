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
void spiProcess(void);
void binSPI(void);

void SPIstart(void);
void SPIstartr(void);
void SPIstop(void);
unsigned int SPIread(void);
unsigned int SPIwrite(unsigned int c);
void SPIsetup(void);
void SPIsetup_exc(void);
void SPIcleanup(void);
void SPImacro(unsigned int macro);
void SPIpins(void);
void spiSetup(unsigned char spiSpeed);
void spiDisable(void);
unsigned char spiWriteByte(unsigned char c);
void SPIsettings(void);


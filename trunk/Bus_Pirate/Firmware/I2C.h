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

void i2cProcess(void);
void binI2C(void);


unsigned int I2Cread(void);
unsigned int I2Cwrite(unsigned int c);
void I2Cstart(void);
void I2Cstop(void);
void I2Csetup(void);
void I2Ccleanup(void);
void I2Cmacro(unsigned int c);
void I2Cpins(void);
void I2Csettings(void);


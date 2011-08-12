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
void bpFreq(void);
void bpPWM(void);

void updatePWM(void);
void bpAuxHiZ(void);
void bpAuxHigh(void);
void bpAuxLow(void);
unsigned int bpAuxRead(void);

void bpServo(void);

extern int PWMfreq;
extern int PWMduty;


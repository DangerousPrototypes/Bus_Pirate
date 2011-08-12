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

//print text in UImessages[s] array (member s) to the user terminal
//message text defined in /translations/*-*.h
void bpWmessage(unsigned char s);

//print the help info in help[] to the user terminal
//message text defined in /translations/*-*.h
void printHelp(void);

void bpMsg(int offset, int length);
void bpMsgBR(int offset, int length);

int agree(void); //are you sure?



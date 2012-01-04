/*
 * This file is part of the Bus Pirate project (http://code.google.com/p/the-bus-pirate/).
 *
 * Written and maintained by the Bus Pirate project and http://dangerousprototypes.com
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
 *
 *  Reference on the wiki:
 *  http://dangerousprototypes.com/docs/Bus_Pirate_PIC_programming_adapter
 *  http://dangerousprototypes.com/docs/Bitbang
 *
 *  video instruction : http://www.whereisian.com/files/j-hvp-self-test.swf
 *
 *   Forum links :  http://dangerousprototypes.com/forum/index.php?topic=1046.0
 */

// under codebase, You always need to add the other files (includes) to compile properly
//
/*
Doc ref: http://dangerousprototypes.com/docs/MMA7455L_breakout_board_manufacturing_tests#Notes
The automated test app uses the Bus Pirate to test the connection to the MMA7455L:
Enter BBIO mode
Enter SPI mode
Enable power, normal pin outputs
Set CS high
Set CS low
Send 0x1A
Read one byte
Set CS high
Verify read=0x1D
Exit SPI mode (to disable power and pins)
Repeat from 2
*/

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <unistd.h>
#include <string.h>
#include <windef.h>

#include "..\framework\buspirate.h"
#include "..\framework\serial.h"


int modem =FALSE;   //set this to TRUE of testing a MODEM
int verbose = 0;
//int disable_comport = 0;   //1 to say yes, disable comport, any value to enable port default is 0 meaning port is enable.
int dumphandle;     // use by dump file when using the -d dumfile.txt parameter
char *dumpfile;

int print_usage(char * appname)
{
	//print usage
	printf("\n\n");

	printf("-------------------------------------------------------------------------\n");
	printf("\n");
	printf(" Usage:              \n");
	printf("   %s  -p device \n ",appname);
	printf("\n");
	printf("   Example Usage:   %s COM1 -R \n",appname);
	printf("\n");
	printf("           Where: -p device is port e.g.  COM1  \n");
	printf("                  -s Speed is port Speed  default is 115200 \n");
	printf("\n");

	printf("\n");

	printf("-------------------------------------------------------------------------\n");


	return 0;
}



int main(int argc, char** argv)
{
	int opt;
    char buffer[256] = {0};
	int fd;
	int res,c;

	int flag=0,firsttime=0;
	char *param_port = NULL;
	char *param_speed = NULL;
   	char ret,i;

	printf("-------------------------------------------------------------------------------------\n");
	printf("\n");
	printf(" Bus Pirate-MMA7455L breakout board manufacturing SELF TEST utility v0.1 (CC-0)\n");
	printf(" http://www.dangerousprototypes.com\n");
	printf("\n");
	printf("-------------------------------------------------------------------------------------\n");

	if (argc <= 1){
		printf(" Help Menu\n");
		print_usage(argv[0]);
		exit(-1);
	}

	while ((opt = getopt(argc, argv, "ms:p:")) != -1) {
       // printf("%c  \n",opt);
		switch (opt) {

			case 'p':  // device   eg. com1 com12 etc
				if ( param_port != NULL){
					printf(" Device/PORT error!\n");
					exit(-1);
				}
				param_port = strdup(optarg);
				break;
            case 's':
				if (param_speed != NULL) {
					printf(" Speed should be set: eg  115200 \n");
					exit(-1);
				}
				param_speed = strdup(optarg);

				break;

			default:
				printf(" Invalid argument %c", opt);
				print_usage(argv[0]);
				//exit(-1);
				break;
		}
	}

	if (param_port==NULL){
			printf(" No serial port set\n");
			print_usage(argv[0]);
			exit(-1);
		}

	if (param_speed==NULL)
		param_speed=strdup("115200");


	printf("\n Parameters used: Device = %s,  Speed = %s\n\n",param_port,param_speed);
	flag=0;
	//
	// Loop and repeat test as needed for manufacturing
	//

	printf(" Press Esc to exit, any other key to start the self-test \n\n");
	while(1){

		//pause for space, or ESC to exit

		if (flag==1){
			printf("\n--------------------- Starting a new Test-------------------------\n");
		}
		while(1){
			Sleep(1);
			if (flag==1){
				flag=0;   //has flag been set to just go testing?
				break;    // proceed with test
			}
			if(kbhit()){
				c = getch();
				if(c == 27){
					printf("\n Esc key hit, stopping...\n");
					printf(" (Bye for now!)\n");
					exit(-1);
				}
				else {//make space only
					printf("\n Starting test! \n");
					break;
				}
			}
		}



//
// Open serial port
//

		printf(" Opening Bus Pirate on %s at %sbps...\n", param_port, param_speed);
		fd = serial_open(param_port);
		if (fd < 0) {
			fprintf(stderr, " Error opening serial port\n");
			return -1;
		}
        //Set Speed of port
		serial_setup(fd,(speed_t) param_speed);

		printf(" Configuring Bus Pirate ....\n");

		//go into binary bitbang mode
		printf(" Going into Binary Bitbang mode..\n");
		if(BP_EnableBinary(fd)!=BBIO){
			printf(" Buspirate cannot switch to binary bitbang mode :( \n");
			return -1;
		}
		else
			printf(" ..ok\n\n");

		printf(" Going into SPI mode..");

		if(BP_EnableMode(fd, SPI)!=SPI){
			printf(" Buspirate cannot switch to SPI mode :( \n");
			return -1;
		}
		else
			printf(" ..ok\n\n");
		//0100wxyz - Configure peripherals w=power, x=pull-ups, y=AUX, z=CS
		i=0x40;
		i|=0x08;//power on
		i|=0x01; //CS high
		printf(" Power on...\n");
		BP_WriteToPirate(fd,&i);

		//1000wxyz - SPI config, w=HiZ/3.3v, x=CKP idle, y=CKE edge, z=SMP sample
		// 10001110
		// 3. Enable power, normal pin outputs
		i=0x80;
		i|=0x02;
		//i|=0x04;
		// w=hiZ/3.3v
		i|=0x08;
		printf(" Enabling  power, normal pin outputs..\n");
		BP_WriteToPirate(fd,&i);

		//
		//Start self-test

		// Set CS high
		i=0x03;
		printf(" CS High : 0x03....\n");
		BP_WriteToPirate(fd,&i);

		// Set CS low
		i=0x02;
		printf(" CS Low : 0x02....\n\n");
		BP_WriteToPirate(fd,&i);

		//  Send 0x1A
		printf(" Sending  0x1A....\n");
		buffer[0]=0x11;//number of bytes
		buffer[1]=0x1a; //command to MMA7455
		buffer[2]=0xff; //read one byte
		serial_write(fd, buffer, 3);
		Sleep(1);
		// Read one byte
		res = serial_read(fd, buffer, 3);
		if (res!=0) {
			printf(" Got reply:  ");
			printf("0X%02X ",buffer[2]);
			printf(" \n\n");
			if (buffer[2] !=0x1D)
				printf(" Self test status with MMA7455L breakout board: !!!! FAILED! !!!! \n\n");
			else
				printf(" Self test status with MMA7455L breakout board: **** PASS! *****\n\n");
		}else{
			printf("FAIL! Got No Reply\n");
		}

		// Set CS high
		i=0x03;
		printf(" CS High : 0x03....\n");
		BP_WriteToPirate(fd,&i);

		printf(" Exiting SPI mode/reseting Buspirate..\n");
		i=0x00;
		serial_write(fd, &i, 1);
		Sleep(1);
		res = serial_read(fd, buffer, sizeof(buffer));

		//  pause after the result

		if (firsttime==0){    // run here once and don't say again the next time
			printf(" Press any key to continue...\n");
			firsttime=1;
			while(1){
				Sleep(1);
				if(kbhit()){
				   c = getch();
					break;
				}
			}
		}

		res = serial_write( fd, "\x00", 1);  //reset buspirate

		//close port so they can attach the next Bus Pirate
		serial_close(fd);


		//TODO: Loop back to pause after this
		printf("\n Connect another MMA7455L breakout board to the bus pirate \n  and press any key to start the self-test again \n");
		printf(" Or hit ESC key to stop and end the test.\n");

		while(1){
			Sleep(1);
			if(kbhit()){
			   c = getch();
			   if(c == 27){
					printf("\n Esc key hit, stopping...\n");
					printf(" (Bye for now!)\n");
					exit(-1);
				}else {//make space only
					flag=1;  //flag to tell the other loop to bypass another keypress

					break;
				}
			}
		}
	}

	#define FREE(x) if(x) free(x);
	FREE(param_port);
	FREE(param_speed);
    return 0;
}

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
	char buffer[256] = {0},  ADCraw[2]={0};
	int fd;
	int res,c,adc;
	float voltage ;
	int flag=0,firsttime=0;
	char *param_port = NULL;
	char *param_speed = NULL;

	printf("-------------------------------------------------------\n");
	printf("\n");
	printf(" Bus Pirate HVP Adapter SELF TEST utility v0.1 (CC-0)\n");
	printf(" http://www.dangerousprototypes.com\n");
	printf("\n");
	printf("-------------------------------------------------------\n");

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
			case 'm':    //modem debugging for testing
                   modem =TRUE;   // enable modem mode
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

		if (modem==TRUE){    // connected to modem for testing response	{

				  serial_write( fd, "ATI7\x0D\0",5 );
				  Sleep(1);
				  res= serial_read(fd, buffer, sizeof(buffer));
				  printf("\n %s\n",buffer);

		}
		else{
			printf(" Configuring Bus Pirate HVP Adapter...\n");
			//go into binary bitbang mode
			printf(" Going into Binary Bitbang mode..");
			if(BP_EnableBinary(fd)!=BBIO){
				printf(" Buspirate cannot switch to binary bitbang mode :( \n");
				return -1;
			}
			printf("ok\n");
			//
			//Start self-test
			// 010xxxxx - Configure pins as input(1) or output(0): AUX|MOSI|CLK|MISO|CS
			// should configured as output 01000000 = 0x40

			printf(" configuring pins as output...\n");
            printf(" sending 01000000...");
			BP_WriteToPirate(fd,"\x40");
			printf("OK\n");
		   //1xxxxxxx - Set on (1) or off (0): POWER|PULLUP|AUX|MOSI|CLK|MISO|CS
		   //11000000 - on :  poweronly = 0xC0
            printf(" Sending Command to power on : 11000000....");
			BP_WriteToPirate(fd,"\xC0");
			printf("OK\n");
		}

        //
        // Done with setup ,start with testing
        // 00010100 - Take voltage probe measurement (returns 2 bytes)
        // 00010100 = 0x14

		printf(" Voltage Probe measurement...,sending 00010100 \n");
		serial_write( fd, "\x14",1 );
		res=0;
		while(res==0){
            Sleep(1); //how long wait for a reply?
            res = serial_read(fd, ADCraw, 2);
        }
        //for debugging without hardware using only modem
		if (modem==TRUE){
		    res=2;
            ADCraw[0]=0x4D;
            ADCraw[1]=0x9C;
		}
        // comment the above if with hardware
        if (res!=2){
           printf(" Expecting 2 bytes returned but got %i instead\n",res);
           printf(" Voltage Measurement: !!!!FAIL!!!! \n");
        }
        else
        {
           // Take a measurement from the Bus Pirate voltage probe.
           //  Returns a 2 byte ADC reading, high 8bits come first.
           //  To determine the actual voltage measurement:
           //  (ADC/1024)*3.3volts/(49/10)
            adc = ADCraw[0] << 8;
            adc += ADCraw[1];
			voltage = (adc/1024.0) * 3.3/(49.0/10.0);

			printf(" ADC Reading: %2.0f Volts\n",voltage);
			if(voltage > 12.0 ){
                printf(" Voltage Measurement: ****PASS**** \n");
            }else{
                printf(" Voltage Measurement: !!!!FAIL!!!! \n");
            }

        }
    	//power off
	    //1xxxxxxx - Set on (1) or off (0): POWER|PULLUP|AUX|MOSI|CLK|MISO|CS
		   //10000000 - off :  power only = 0x80
           printf( " powering off.....\n");
		   BP_WriteToPirate(fd,"\x80");

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

		res = serial_write( fd, "\x0F", 1);  //exit BBIO

		//close port so they can attach the next Bus Pirate
		serial_close(fd);


		//TODO: Loop back to pause after this
		printf("\n Connect another Bus Pirate HVP adapter and press any key to start the self-test again \n");
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

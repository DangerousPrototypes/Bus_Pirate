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
		printf("   %s  -d device \n ",appname);
		printf("\n");
		printf("   Example Usage:   %s COM1 \n",appname);
		printf("\n");
		printf("           Where: -d device is port e.g.  COM1  \n");
		printf("                  -s Speed is port Speed  default is 115200 \n");
		printf("\n");

        printf("\n");

	    printf("-------------------------------------------------------------------------\n");


		return 0;
	}



int main(int argc, char** argv)
{
  int opt;
  char buffer[256] = {0}, i;
  int fd;
  int res,c;
  int flag=0,firsttime=0;
  char *param_port = NULL;
  char *param_speed = NULL;

    printf("-------------------------------------------------------\n");
    printf("\n");
    printf(" Bus Pirate LCD adapter test utility v0.1 (CC-0)\n");
    printf(" http://dangerousprototypes.com\n");
    printf("\n");
    printf("-------------------------------------------------------\n");
    // printf("\n");

if (argc <= 1)  {

	    printf(" Help Menu\n");
		print_usage(argv[0]);
		exit(-1);
	}

while ((opt = getopt(argc, argv, "ms:d:")) != -1) {
       // printf("%c  \n",opt);
		switch (opt) {

			case 'd':  // device   eg. com1 com12 etc
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
   // printf(" Press escape to exit \n");
  //  printf("\n");



    //param_port=strdup("COM3");
    //Set default if NULL
    if (param_port==NULL){
        printf(" No serial port set\n");
		print_usage(argv[0]);
		exit(-1);
    }

    if (param_speed==NULL)
          param_speed=strdup("115200");


    printf("\n Parameters used: Device = %s,  Speed = %s\n\n",param_port,param_speed);


    //
    // Open serial port
    //

	printf(" Opening Bus Pirate on %s at %sbps...\n", param_port, param_speed);
	fd = serial_open(param_port);
	if (fd < 0) {
		fprintf(stderr, " Error opening serial port\n");
		return -1;
	}

    //
	// Enter binary mode, then enter a protocol mode
	//
	serial_setup(fd,(speed_t) param_speed);


    fprintf(stderr, " Configuring Bus Pirate...\n");
    //BP_EnableMode(fd, SPI); //enter BBIO then SPI
    if(BP_EnableBinary(fd)!=BBIO){
        fprintf(stderr, " Buspirate cannot switch to binary mode :( \n");
        return -1;
    }

    fprintf(stderr, " Switch to SPI...\n");
    if(BP_EnableMode(fd , SPI)!=SPI){
        fprintf(stderr, " Buspirate cannot switch to SPI mode :( \n");
        fprintf(stderr, " Exiting...\n");
        return -1;
    }

    flag=0;
    //
    // Loop and repeat test as needed for manufacturing
    //
    //TO DO: add loop here
     printf(" Press Esc to exit, any other key to start the test \n\n");
    while(1){

     //pause for space, or ESC to exit

     if (flag==1)
     {
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
            }else {//make space only
                break;
            }
        }
    }



    //
	//Start self-test
	//
        //printf(" Starting test...\n");

        //fprintf(stderr, " SPI power on...\n");

        i=0x48;//power on, CS low, etc.
        if (BP_WriteToPirate(fd, &i))
             printf("WARNING.. Not Good\n");


        //fprintf(stderr, " SPI pin setup...\n");
    //configure according to user settings
    //1000wxyz - SPI config, w=HiZ/3.3v, x=CKP idle, y=CKE edge, z=SMP sample
        i=0x80;
        i|=0x0A;
        if (BP_WriteToPirate(fd, &i))
             printf("WARNING.. Not Good\n");

        printf(" LED and all outputs should blink!!!\n ");


        printf(" Press any key to continue...\n");
        firsttime=1;
        while(1){
            i=0x10;//send data
            if (BP_WriteToPirate(fd, &i))
                 printf("WARNING.. Not Good\n");

             i=0x00;//data
             BP_WriteToPirateNoCheck(fd, &i);

            i=0x03;//cs High
            if (BP_WriteToPirate(fd, &i))
                 printf("WARNING.. Not Good\n");

            i=0x02;//cs low
            if (BP_WriteToPirate(fd, &i))
                 printf("WARNING.. Not Good\n");


             i=0x10;//send data
            if (BP_WriteToPirate(fd, &i))
                 printf("WARNING.. Not Good\n");

             i=0xFF;//data
             BP_WriteToPirateNoCheck(fd, &i);

            i=0x03;//cs High
            if (BP_WriteToPirate(fd, &i))
                 printf("WARNING.. Not Good\n");

            i=0x02;//cs low
            if (BP_WriteToPirate(fd, &i))
                 printf("WARNING.. Not Good\n");

            Sleep(1);
            if(kbhit()){
               c = getch();
                break;
            }
        }

        //fprintf(stderr, " SPI power off...\n");

        i=0x40;//power on, CS low, etc.
        if (BP_WriteToPirate(fd, &i))
             printf("WARNING.. Not Good\n");


        //fprintf(stderr, " SPI pin hiz...\n");
    //configure according to user settings
    //1000wxyz - SPI config, w=HiZ/3.3v, x=CKP idle, y=CKE edge, z=SMP sample
        i=0x80;
        i|=0x02;
        if (BP_WriteToPirate(fd, &i))
             printf("WARNING.. Not Good\n");

        //TODO: Loop back to pause after this
        printf("\n Connect another adapter and press any key to start the self-test again \n");
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

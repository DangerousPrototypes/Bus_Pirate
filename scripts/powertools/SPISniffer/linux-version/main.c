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
#include <unistd.h>
#include <string.h>

#ifdef WIN32
#include <conio.h>
#include <windef.h>
#else
//#include <curses.h>
#endif

#include "buspirate.h"
#include "serial.h"

int modem =FALSE;   //set this to TRUE of testing a MODEM
int verbose = 0;
int disable_comport = 0;   //1 to say yes, disable comport, any value to enable port default is 0 meaning port is enable.
int dumphandle;     // use by dump file when using the -d dumfile.txt parameter
char *dumpfile;

#define SPI 0x01

int print_usage(char * appname)
	{
		//print usage
		printf("\n\n");

	  printf("-------------------------------------------------------\n");
		printf("\n");
        printf(" Usage:              \n");
		printf("   %s  -d device -e 1 -p 0 \n ",appname);
		printf("\n");
		printf("   Example Usage:   %s COM1 -s Speed -e 1 -p 0 \n",appname);
		printf("\n");
		printf("           Where: -d device is port e.g.  COM1  \n");
		printf("                  -s Speed is port Speed  default is 115200 \n");
		printf("                  -e ClockEdge is 0 or 1  default is 1 \n");
		printf("                  -p Polarity  is 0 or 1  default is 0 \n");
		printf("                  -r RawData is 0 or 1  default is 0 \n");
		printf("\n");

        printf("\n");

       printf("-------------------------------------------------------\n");


		return 0;
	}



int main(int argc, char** argv)
{
int opt;
  char buffer[256] = {0}, i;
  int fd;
  int res,c;
  int new_state,state=0;

  char *param_port = NULL;
  char *param_speed = NULL;
  char *param_polarity=NULL;
  char *param_clockedge=NULL;
  char *param_rawdata=NULL;

//  int clock_edge;
// int polarity;
 //default speed
 // param_port=strdup("COM3");    //default for debugging


    printf("-------------------------------------------------------\n");
    printf("\n");
    printf(" Bus Pirate binary mode SPI SNIFFER utility v0.2 (CC-0)\n");
    printf(" http://dangerousprototypes.com\n");
    printf("\n");
    printf("-------------------------------------------------------\n");
   printf("\n\n\n");

//p == port
// p=polarity
// e=clock edge

if (argc <= 1)  {
	    printf("ERROR: Invalid argument(s).\n\n");
	    printf("Help Menu\n");
		print_usage(argv[0]);
		exit(-1);
	}

while ((opt = getopt(argc, argv, "ms:p:e:d:r:")) != -1) {
       // printf("%c  \n",opt);
		switch (opt) {

			case 'd':  // device   eg. com1 com12 etc
				if ( param_port != NULL){
					printf("Device/PORT error!\n");
					exit(-1);
				}
				param_port = strdup(optarg);
				break;
			case 'e':      // clock edge
 				if (param_clockedge != NULL) {
					printf("Clock Edge should be 0 or 1\n");
					exit(-1);
				}
				param_clockedge = strdup(optarg);

				break;
			case 'p':
				if (param_polarity != NULL) {
					printf("Polarity must be 0 or 1\n");
					exit(-1);
				}
				param_polarity = strdup(optarg);

				break;
			case 's':
				if (param_speed != NULL) {
					printf("Speed should be set: eg  115200 \n");
					exit(-1);
				}
				param_speed = strdup(optarg);

				break;
			case 'r':      // raw data
 				if (param_rawdata != NULL) {
					printf("Raw Data should be 0 or 1\n");
					exit(-1);
				}
				param_rawdata = strdup(optarg);

				break;
			case 'm':    //modem debugging for testing
                   modem =TRUE;   // enable modem mode
				break;

			default:
				printf("Invalid argument %c", opt);
				print_usage(argv[0]);
				//exit(-1);
				break;
		}
	}
    printf(" Press escape to exit \n");
    printf("\n");



    //param_port=strdup("COM3");
    //Set default if NULL
    if (param_port==NULL){
        printf("No serial port set\n");
		print_usage(argv[0]);
		exit(-1);
    }

    if (param_clockedge==NULL)
          param_clockedge=strdup("1");

    if (param_polarity==NULL)
          param_polarity=strdup("0");

    if (param_speed==NULL)
          param_speed=strdup("115200");

    if (param_rawdata==NULL)
          param_rawdata=strdup("0");


    printf("\n  Parameters used: Device = %s,  Speed = %s, Clock Edge= %s, Polarity= %s\n\n",param_port,param_speed,param_clockedge,param_polarity);


    //
    // Open serial port
    //

	printf(" Opening Bus Pirate on %s at %sbps...\n", param_port, param_speed);
	fd = serial_open(param_port);
	if (fd < 0) {
		fprintf(stderr, "Error opening serial port\n");
		return -1;
	}

    //
	// Enter binary mode, then enter a protocol mode
	//
	serial_setup(fd,(speed_t) atoi(param_speed));

	printf(" Starting SPI sniffer...\n");


	if (modem==TRUE){    // connected to modem for testing response	{

              serial_write( fd, "ATI3\x0D\0",5 );
	          usleep(1);
	          res= serial_read(fd, buffer, sizeof(buffer));
	          printf("\n %s\n",buffer);

              serial_write( fd, "ATI4\x0D\0",5 );
	          usleep(1);
	          res= serial_read(fd, buffer, sizeof(buffer));
	          printf("\n %s\n",buffer);
              serial_write( fd, "ATI7\x0D\0",5 );
	          usleep(1);
	          res= serial_read(fd, buffer, sizeof(buffer));
	          printf("\n %s\n",buffer);


	}
	else
	{


          fprintf(stderr, " Configuring Bus Pirate...\n");
   	      BP_EnableMode(fd, SPI); //enter BBIO then SPI
    //
	//Start sniffer
	//

    //configure according to user settings
    //1000wxyz - SPI config, w=HiZ/3.3v, x=CKP idle, y=CKE edge, z=SMP sample
            i=0x80;
            if(strncmp(param_clockedge, "1", 1)==0)
                i|=0x02;
            if(strncmp(param_polarity, "1", 1)==0)
                i|=0x04;

            BP_WriteToPirate(fd, &i);

    //start the sniffer
             serial_write( fd, "\x0E", 1);

    //
    // Done with setup
    //
	}


	printf(" (OK) Happy sniffing! Press ESC to stop.\n");

    //
    // Loop and print input from the serial port
    //
	 while(1){

        usleep(1);
        res= serial_read(fd, buffer, 100);

        if(res>0){
            for(c=0; c<res; c++){
            if(strncmp(param_rawdata, "1", 1)==0) printf("%02X ", (uint8_t)buffer[c]);
	    else {
		switch(state) {
			default:
			case 0:	// waiting CS active
				if (buffer[c]==0x5B) {
					printf("[");
					new_state=1;
		  		} else {
					printf("Sync\n");
					new_state=0;
				}
				break;
			case 1:	// check for data or CS inactive
				if (buffer[c]==0x5C) {
					new_state=2;
				} else if (buffer[c]==0x5D) {
					printf("]\n");
					new_state=0;
				} else {
					printf("Sync\n");
					new_state=0;
				}
				break;
			case 2:	// MPI
				printf("0x%02X(", (uint8_t) buffer[c]);
				new_state=3;
				break;
			case 3:	// MPO
				printf("0x%02X)", (uint8_t) buffer[c]);
				new_state=1;
				break;
		}
		state=new_state;
	    }
            }
        }

#ifdef WIN32
        if(kbhit()){
           c = getch();

           if(c == 27){
                printf("\n Esc key hit, stopping...\n");
                printf(" Clean up Bus Pirate...\n");
                buffer[0]=0x00;//exit sniffer
                buffer[1]=0x00;//exit spi
                buffer[2]=0x0f;//exit BBIO
                res = serial_write( fd, buffer, 3);
                printf(" (Bye for now!)\n");
                break;
            }
        }
#endif

    }    //hit enter to stop

#define FREE(x) if(x) free(x);

	FREE(param_port);
	FREE(param_speed);
    return 0;
}

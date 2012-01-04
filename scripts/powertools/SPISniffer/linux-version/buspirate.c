#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "serial.h"
#include "buspirate.h"

//static struct BP_t pBP;
//static uint8_t BP_reversebyte(uint8_t c);
//static char bpbuf[4096];
//static int bpbufcnt;
extern int disable_comport;
extern int dumphandle;
extern int verbose;
extern int modem;
//low lever send command, get reply function

uint32_t BP_WriteToPirate(int fd, char * val) {
	int res = -1;
	char ret = 0;


    serial_write(fd, val, 1);

    if (disable_comport != 1) {  //if comport is enable, we need a response from the port

          res = serial_read(fd, &ret, 1);

	      if( ret != '\x01') {
	          if (modem==TRUE){
	             printf("Modem responded with %i byte and with a value of 0X%X\n",res,ret);

	          } else {
		          printf("ERROR");
		          return -1;
	          }
	        }
        }
	return 0;
}

void BP_EnableMode(int fd, char bbmode)
{
	int ret;
	char tmp[100] = { [0 ... 20] = 0x00 };
	int done = 0;
	//int cmd_sent = 0;
	int tries=0;

	printf(" Entering binary mode...\n");

	if (fd==-1)   //added because the fd has already returned null
	{
	    printf("Port does not exist!");
	    return;

	}
	while (!done) {
		tmp[0]=0x00;
		//printf("Sending 0X%02X to port\n",tmp[0]);
		serial_write(fd, tmp, 1);
		tries++;
		//printf("tries: %i Ret %i\n",tries,ret);
		usleep(1);
		ret = serial_read(fd, tmp, 5);
		//printf("read returned %i:%s\n", ret,tmp);
		if (modem==TRUE)
		{
            printf("\nModem Responded = %i\n",ret);
            done=1;
		}
		else {
		     if (ret != 5 && tries>20) {
			         fprintf(stderr, "Buspirate did not respond correctly :( %i \n", ret );
			         exit(-1);
		      }    else if (strncmp(tmp, "BBIO1", 5) == 0) {
			       done=1;
		           }
		}

		if (tries>25){
		printf("Buspirate:Too many tries in serial read! -exiting \n - chip not detected, or not readable/writable\n");
		exit(-1);
		}
	}

	done=0;
	tmp[0] = bbmode;
	//printf("Sending 0X%02X to port\n",tmp[0]);
	serial_write(fd, tmp, 1);
	tries++;
	 usleep(1);
	ret = serial_read(fd, tmp, 4);
	if (modem==TRUE)
		{
            printf("Modem Responded = %i with value %#X\n",ret,tmp[0]);
		}
        else {
	         if ( (ret==4) && (strncmp(tmp, "SPI1", 4) == 0)) {

	         } else{
	        	fprintf(stderr, "Buspirate did not respond correctly :( %i \n", ret );
	        	exit(-1);
	        }
        }
}

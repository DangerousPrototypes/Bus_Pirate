/* Author: James Stephenson
 * This program runs under windows and uses the bus pirate to read/write a file in 
 * ascii hex to/from an I2C eeprom such as the 24FC64, note the address used is 2 bytes.
 * Page write isn't used since chips don't have a standard write buffer size.
 * Released under the WTF license.
 *
 * Hex file should contain space seperated hex values and no line breaks, ie: 
 * 0xFF 0xAA 0x BB
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

int main(int argc, char **argv){
	FILE *inoutpfile;
    DCB my_dcb;
	HANDLE hComm;
    int bytes_written, bytes_received, retval, memaddr = 0, loop, failcount, datalen = 0;
    unsigned char receive_buf[10], write_buf[9], curcode[5], curbincode, chipaddr, flags = 0;
    
    if(argc < 4 || argc > 7){
	    printf("pc_serial [com?] [chip addr 0-8] [R/W] [file] (read len) (mem addr)\n");
		return 1;		
    }

	if(argv[2][0] > '8' || argv[2][0] < '0'){
		printf("Chip address out of range\n");
		return 2;
	}
	chipaddr = 0xA0 | ((argv[2][0] - '0') << 1);

	if(argc >= 6){
		if(!sscanf(argv[5], "%d", &datalen)){
			printf("Data length invalid\n");
			return 3;
		}
	}

	if(argc == 7){
		if(!sscanf(argv[6], "%d", &memaddr)){
			printf("Memory address invalid\n");
			return 3;
		}
	}

	if(argv[3][0] != 'R' && argv[3][0] != 'W'){
		printf("Must specify read or write mode\n");
		return 4;
	}

	if(argv[3][0] == 'R' && !datalen){
		printf("Must specify read length\n");
		return 4;
	}

	if(argv[3][0] == 'R')
		inoutpfile = fopen(argv[4], "wb");
	else
		inoutpfile = fopen(argv[4], "rb");
	if(!inoutpfile){
		printf("Error opening file: %s\n", argv[4]);
		return 2;
	}
    
    hComm = CreateFile(argv[1], GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if(hComm == INVALID_HANDLE_VALUE){
        printf("Error Opening Serial Port\n");
        return 3;
    }
    
	my_dcb.DCBlength = sizeof(my_dcb);
	GetCommState(hComm, &my_dcb);
	my_dcb.BaudRate = CBR_115200;
	my_dcb.ByteSize = 8;
	my_dcb.StopBits = ONESTOPBIT;
	my_dcb.Parity = NOPARITY;

	if(!SetCommState(hComm, &my_dcb)){
		printf("Error setting up serial port\n");
		return 4;
	}

	WriteFile(hComm, "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\x02\x4C", 22, &bytes_written, 0);
	ReadFile(hComm, receive_buf, 10, &bytes_received, 0);
	receive_buf[8] = 0;
	if(memcmp(receive_buf, "BBIO1I2C", 8)){
		printf("Error setting up RAW I2C mode: %s\n", receive_buf);
		return 5;
	}
	sleep(500);

	if(argv[3][0] == 'R'){
		memcpy(write_buf, "\x02\x12\xAA\xBB\xCC\x03", 6);
		write_buf[2] = chipaddr;
		write_buf[3] = memaddr >> 8;
		write_buf[4] = memaddr & 0xFF;
		WriteFile(hComm, write_buf, 6, &bytes_written, 0);
		ReadFile(hComm, receive_buf, 6, &bytes_received, 0);
		memcpy(write_buf, "\x02\x10\xAA", 3);
		write_buf[2] = chipaddr | 1;
		WriteFile(hComm, write_buf, 3, &bytes_written, 0);
		ReadFile(hComm, receive_buf, 3, &bytes_received, 0);
		for(; datalen; datalen--){
			WriteFile(hComm, "\x04\x06", 2, &bytes_written, 0);
			ReadFile(hComm, receive_buf, 2, &bytes_received, 0);
			printf("%02X ", receive_buf[0]);
			fprintf(inoutpfile, "%02X ", receive_buf[0]);
		}
		WriteFile(hComm, "\x04\x07\x03\x00\x0F", 5, &bytes_written, 0);
	}else{
		failcount = 0;
		memcpy(write_buf, "\x02\x13\xAA\xBB\xCC\xDD\x03", 7);
		write_buf[2] = chipaddr;
		while(fread(curcode, 5, 1, inoutpfile)){
			if(!sscanf(curcode, "%X", &curbincode)){
				printf("Error processing code in file: %s", curcode);
				return 6;
			}
			write_buf[3] = memaddr >> 8;
			write_buf[4] = memaddr & 0xFF;
			write_buf[5] = curbincode;
			do{
				failcount++;
		    	WriteFile(hComm, write_buf, 7, &bytes_written, 0);
				ReadFile(hComm, receive_buf, 7, &bytes_received, 0);
			}while(receive_buf[2] || receive_buf[3] || receive_buf[4] || receive_buf[5]);
			printf("%02X ", curbincode);
			failcount--;
			memaddr++;
		}
		printf("\n%d bytes written with %d retries\n", memaddr+1, failcount);
		WriteFile(hComm, "\x00\x0F", 2, &bytes_written, 0);
	}
	
	fclose(inoutpfile);
    return 0;
}

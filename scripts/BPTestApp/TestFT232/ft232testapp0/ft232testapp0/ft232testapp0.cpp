// ft232testapp0.cpp : Defines the entry point for the console application.
//
//See  D@XXX programmers reference
// Objective:   Find devices connected to ports and enumerate them


#include "stdafx.h"
#include <windows.h>
#pragma comment(lib, "FTD2XX.lib")
#include "FTD2XX.h"
#include <stdlib.h>
#include <conio.h>	

FT_STATUS ftStatus;			//Status defined in D2XX to indicate operation result
FT_HANDLE ftHandle;			//Handle of FT2232H device port 
char *devices[]={"FT_DEVICE_232BM",
	"FT_DEVICE_232AM",
	"FT_DEVICE_100AX",
	"FT_DEVICE_UNKNOWN",
	"FT_DEVICE_2232C",
	"FT_DEVICE_232R",
	"FT_DEVICE_2232H",
	"FT_DEVICE_4232H",
	NULL,
};
DWORD i=0;
FT_DEVICE_LIST_INFO_NODE *devInfo;
FT_PROGRAM_DATA ftData;
UCHAR old_cbus1_state;
void get_info(void)
{
	
	UCHAR Mask = 0xff;
	UCHAR Mode = 1;     // Set asynchronous bit-bang mode
	LONG lComPortNumber;
	DWORD numDevs;
	
	//UCHAR BitMode;
	

	char ManufacturerBuf[32];
	char ManufacturerIdBuf[16];
	char DescriptionBuf[64];
	char SerialNumberBuf[16];
	ftData.Signature1 = 0x00000000; 
	ftData.Signature2 = 0xffffffff; 
	ftData.Version = 0x00000002; 

	ftData.Manufacturer = ManufacturerBuf;
	ftData.ManufacturerId = ManufacturerIdBuf;
	ftData.Description = DescriptionBuf;
	ftData.SerialNumber = SerialNumberBuf;

	ftStatus = FT_CreateDeviceInfoList(&numDevs);
	if (ftStatus == FT_OK)
	{ 
		printf("Number of devices is %d\n",numDevs);
	}
	if (numDevs > 0)
	{
		// allocate storage for list based on numDevs
		devInfo = (FT_DEVICE_LIST_INFO_NODE*)malloc(sizeof(FT_DEVICE_LIST_INFO_NODE)*numDevs);  // get the device information list
		ftStatus = FT_GetDeviceInfoList(devInfo,&numDevs); 
		if (ftStatus == FT_OK)
		{
			for (i = 0; i < numDevs; i++)
			{ 
				printf("Dev %d:\n",i);
				printf(" Flags=0x%x\n",devInfo[i].Flags);
				printf(" Type=%s\n",devices[devInfo[i].Type]); 
				printf(" ID=0x%x\n",devInfo[i].ID);
				printf(" LocId=0x%x\n",devInfo[i].LocId);
				printf(" SerialNumber=%s\n",devInfo[i].SerialNumber);
				printf(" Description=%s\n",devInfo[i].Description);
				// printf(" ftHandle=0x%x\n",devInfo[i].ftHandle);			

			}
		}

		i=0;  //open first device
		ftStatus = FT_Open(i,&ftHandle);

		if (ftStatus != FT_OK)
		{
			printf("Can't open %s device! \n",devInfo[i].Description);
		}
		else
		{     
			printf("Successfully open  %s device! \n",devInfo[i].Description);	
			ftStatus = FT_GetComPortNumber(ftHandle,&lComPortNumber);
			if (ftStatus == FT_OK)
			{ 
				if (lComPortNumber == -1)
				{ 
					printf(" NO com port Assigned!\n");
				}
				else
				{ 
					// COM port assigned with number held in lComPortNumber
					printf(" Current assigned COM Port: %d \n",lComPortNumber);
				} 
			}
			else
			{
				printf(" Failed to get the COM Port!\n");
			}

			ftStatus = FT_EE_Read(ftHandle, &ftData);
			if (ftStatus == FT_OK)
			{ 
				// FT_EE_Read OK, data is available in ftData
				printf(" EEPROM READ OK\n");
				printf("Signature1 = 0x%04x\n", ftData.Signature1);
				printf("Signature2 = 0x%04x\n", ftData.Signature2);
				printf("Version = 0x%04x\n", ftData.Version);
				printf("VendorID = 0x%04x\n", ftData.VendorId);
				printf("ProductID = 0x%04x\n", ftData.ProductId);
				printf("Manufacturer = %s\n", ftData.Manufacturer);
				printf("ManufacturerID = %s\n", ftData.ManufacturerId);
				printf("Description = %s\n", ftData.Description);
				printf("SerialNumber = %s\n", ftData.SerialNumber);
				printf("MaxPower = %d\n", ftData.MaxPower);
				printf("PnP = %x\n", ftData.PnP);
				printf("SelfPowered = %x\n", ftData.SelfPowered);
				printf("RemoteWakeup = %x\n", ftData.RemoteWakeup);
				printf("Use Ext Osc = %x\n", ftData.UseExtOsc);
				printf("High Drives = %x\n", ftData.HighDriveIOs);
				printf("Endpoint Size = %x\n", ftData.EndpointSize);
				printf("Pull Down Enabled = %x\n", ftData.PullDownEnableR);
				printf("Serial Number Enabled = %x\n", ftData.SerNumEnableR);
				printf("Invert TXD = %x\n", ftData.InvertTXD);
				printf("Invert RXD = %x\n", ftData.InvertRXD);
				printf("Invert RTS = %x\n", ftData.InvertRTS);
				printf("Invert CTS = %x\n", ftData.InvertCTS);
				printf("Invert DTR = %x\n", ftData.InvertDTR);
				printf("Invert DSR = %x\n", ftData.InvertDSR);
				printf("Invert DCD = %x\n", ftData.InvertDCD);
				printf("Invert RI = %x\n", ftData.InvertRI);
				printf("CBUS0 =  0X%02X\n", ftData.Cbus0);
				printf("CBUS1 =  0X%02X\n", ftData.Cbus1);
				printf("CBUS2 =  0X%02X\n", ftData.Cbus2);
				printf("CBUS3 =  0X%02X\n", ftData.Cbus3);
				printf("CBUS4 =  0X%02X\n", ftData.Cbus4);



			} 
			else
			{ 
				// FT_EE_Read FAILED! 
				printf(" EEPROM READ FAILED\n");

			}
			FT_Close(ftHandle);	
		}      
	}
	else
	{
		printf("No FT232 Device found! \n");
	}
}

void readCBuSbits(void)
{
	ftStatus = FT_EE_Read(ftHandle, &ftData);
	if (ftStatus == FT_OK) {
		
		printf(" CBUS0 =  0X%02X\n", ftData.Cbus0);
		printf(" CBUS1 =  0X%02X\n", ftData.Cbus1);
		printf(" CBUS2 =  0X%02X\n", ftData.Cbus2);
		printf(" CBUS3 =  0X%02X\n", ftData.Cbus3);
		printf(" CBUS4 =  0X%02X\n", ftData.Cbus4);
	}
	else
	{
		printf("Failed Reading status o Cbus pins! \n");
	}
}

void setCBUSbits(UCHAR cbus1)
{
	ftStatus = FT_Open(i,&ftHandle);
	ftStatus = FT_EE_Read(ftHandle, &ftData);
	if (ftStatus == FT_OK) {
			
		old_cbus1_state=ftData.Cbus1;  // save state of cbus1
	//	ftData.Cbus0=0;
		ftData.Cbus1 = cbus1;    //  0x0A will set to I/O mode, otherwise restore from old_cbus1_state
	//	ftData.Cbus2=1;
	//	ftData.Cbus3=2;
	//	ftData.Cbus4=1;

		ftStatus = FT_EE_Program(ftHandle, &ftData);
		if (ftStatus == FT_OK)
		{
			ftStatus = FT_CyclePort(ftHandle);
			if (ftStatus == FT_OK) 
			{ 
				// Port has been cycled.
				// Close the handle.
				ftStatus = FT_Close(ftHandle);
				if (ftStatus==FT_OK) {

					printf("Device Close OK! \n");
					ftStatus=FT_Rescan();
					Sleep(5000);

				}
				else
				{
					printf("Device Close Failed! \n");
				}
			}
			else
			{ 
				// FT_CyclePort FAILED!
				printf("CyclePort Failed!\n");
			}
		}
		else
		{ 
			// FT_EE_Program FAILED!
			printf("FT_EE_Program FAILED!\n");
		}
	}
}
void writebits(UCHAR Mask)
{
	DWORD devIndex = 0;
	DWORD numDevs;

	UCHAR ucMask =0x0;
	ftStatus = FT_CreateDeviceInfoList(&numDevs);
	if (ftStatus == FT_OK)
	{ 
		// printf("Number of devices is %d\n",numDevs);
	}
	if (numDevs > 0)
	{
		// allocate storage for list based on numDevs
		devInfo = (FT_DEVICE_LIST_INFO_NODE*)malloc(sizeof(FT_DEVICE_LIST_INFO_NODE)*numDevs);  // get the device information list
		ftStatus = FT_GetDeviceInfoList(devInfo,&numDevs); 
		if (ftStatus == FT_OK)
		{
			/*
			for (i = 0; i < numDevs; i++)
			{ 
				printf("Dev %d:\n",i);
				printf(" Flags=0x%x\n",devInfo[i].Flags);
				printf(" Type=%s\n",devices[devInfo[i].Type]); 
				printf(" ID=0x%x\n",devInfo[i].ID);
				printf(" LocId=0x%x\n",devInfo[i].LocId);
				printf(" SerialNumber=%s\n",devInfo[i].SerialNumber);
				printf(" Description=%s\n",devInfo[i].Description);
				printf(" ftHandle=0x%x\n",devInfo[i].ftHandle);			

			}
			*/
		}
		else
		{
			
		}

		i=0;
		ftStatus = FT_Open(i,&ftHandle);

		if (ftStatus == FT_OK)
		{ 
			// printf("open OK\n");
		}
		else
		{
			printf("Open failed! \n");
		}
	
			ucMask = Mask;
		
	// Set CBUS pins states
		ftStatus = FT_SetBitMode(ftHandle,ucMask,0x20);

	// Check to see if write successful
		if (ftStatus == FT_OK) {
			printf("CBUS Write Succesful: 0x%02X\n",ucMask);
		}
		else
		{
			printf("CBUS write failed!\n");
		}
	// Close device handle
	ftStatus = FT_Close(ftHandle);
	}
}

void readBits(void)
{
	byte ucMask;
		
	// Get CBUS pins states
	ftStatus = FT_GetBitMode(ftHandle,&ucMask);

	// Check to see if read successful
	if (ftStatus == FT_OK) {
		printf("read bits value: 0x%02X \n",ucMask);
		
	}
	else
	{
		printf("readbits failed\n");
	}
	

	// Close device handle
	ftStatus = FT_Close(ftHandle);
}

int _tmain(int argc, _TCHAR* argv[])
{
			
	get_info();
	//configure CBUS for bitbang mode
	
	printf("Setting cbus1 to I/O mode..\n");
	
	setCBUSbits(0x0A);  // will use only cbus1 rxled, 0x0A set to Bitbang I/O mode
	
	FT_SetBitMode(ftHandle,0,0);  // try resetting

	//Still having trouble  response is always FAILED
	
	for (int x=0;x<10;x++)   //blink rxlex 10x
	{
		writebits( 0x22);  //  0010 0010 
		//Sleep(1000);
		readBits();
		writebits( 0x20);  // 0010 0000 
		//Sleep(1000);
		readBits();
	}
	printf("Restoring Cbus1....\n");
	setCBUSbits(old_cbus1_state); // restore it back
	printf("Press any key to exit.... \n");
	while(!_kbhit());

	return 0;
}



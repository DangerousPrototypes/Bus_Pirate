#include "base.h"
extern struct _modeConfig modeConfig; //holds persistant bus pirate settings (see base.h)

void binIOperipheralset(unsigned char inByte){

	if(inByte&0b1000){
		BP_VREG_ON();//power on
	}else{
		BP_VREG_OFF();//power off
	}
	
	#ifndef BUSPIRATEV1A
	if(inByte&0b100){
		BP_PULLUP_ON();//pullups on
	}else{
		BP_PULLUP_OFF();
	}
	#endif
	#if defined(BUSPIRATEV1A)
	if(inByte&0b100){
		BP_AUX2_HI();//AUX2 control
	}else{
		BP_AUX2_LOW();
	}
	#endif

	//AUX pin, high/low only
	if(inByte&0b10){
		BP_AUX_DIR=0;//aux output
		BP_AUX=1;//aux high
	}else{
		BP_AUX_DIR=0;//aux output
		BP_AUX=0;//aux low
	}

	//CS pin, follows HiZ setting
	if(inByte&0b1){
		if(modeConfig.HiZ==1){
			IODIR|=CS; //CS iput in open drain mode
		}else{	
			IOLAT|=CS; //CS high
			IODIR&=(~CS); //CS output
		}
	}else{
		IOLAT&=(~CS); //BP_CS=0;
		IODIR&=(~CS); //CS output
	}

	//UART1TX(1);//send 1/OK		
}


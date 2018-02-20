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

#include "base.h"
#include "selftest.h"
#include "busPirateCore.h"

void bpTest(unsigned char p, unsigned char d);
void bpBusPinsTest(unsigned char d);
void bpADCPinTest(unsigned char a, unsigned int lval, unsigned int hval);

static unsigned char errors;
extern struct _bpConfig bpConfig;


//self test, showProgress=1 displays the test results in the terminal, set to 0 for silent mode
//errors are counted in the global errors variable
//returns number of errors
unsigned char selfTest(unsigned char showProgress, unsigned char jumperTest){
//toggle display of test results with show Progress variable
	errors=0;
	if(!showProgress) bpConfig.quiet=1;
	
//instructions (skip pause if no display output)
	if(showProgress && jumperTest){
		
		
		BPMSG1163;//"Disconnect any devices\r\nConnect (Vpu to +5V) and (ADC to +3.3V)"
		BPMSG1251; //"Press a key to start"
		//JTR Not required while(!UART1RXRdy()); //wait for key
		UART1RX();//discard byte
	}

	//bpPOSTWline("Ctrl");
	BPMSG1164;
	BP_AUX0=1;
	BP_AUX0_DIR=0;
	//bpPOSTWstring("AUX");
	BPMSG1165;
	bpTest(BP_AUX0,1);
	BP_AUX0=0;
	BP_AUX0_DIR=1;
				
	BP_LEDMODE=1;
	BP_LEDMODE_DIR=0;	
	//bpPOSTWstring("MODE LED");
	BPMSG1166;
	bpTest(BP_LEDMODE,1);
	BP_LEDMODE=0;
	
	//v3/v4 the external pullup resistor on the 4066 will pull the pin high when EXTPU is on
	//v5 pin drives the 74HCT4066 directly
	BP_EXTPU_ON();
	BPMSG1167; //EXTPUSEL H
	bpTest(BP_PUVSELEXT,1);
	BP_PULLUP_OFF();
	BPMSG1168; //EXTPUSEL L
	bpTest(BP_PUVSELEXT,0);

#if defined(BUSPIRATEV5)
	//test the 3.3volt and 5volt pullup select pins on v5
	BP_3V3PU_ON();
	BPMSG1173; //3.3V
	BPMSG5000; //PUSEL
	bpTest(BP_PUVSEL33,1);

	BP_5VPU_ON();
	BPMSG1171; //"5V"
	BPMSG5000; //PUSEL
	bpTest(BP_PUVSEL50,1);
#endif
	
	BP_VREG_ON();
	bpDelayMS(2);//in silent mode there's not enought delay for the power supplied to come on
	//bpPOSTWstring("VREG");
	BPMSG1169;
	bpTest(BP_VREGEN,1);

#if defined (BUSPIRATEV4)
	BPMSG1265; //bpWline("EEPROM");
	BPMSG1266; //bpWstring("SCL");
	bpTest(BP_EE_SCL, 1);
	BPMSG1267; //bpWstring("SDA");
	bpTest(BP_EE_SDA, 1);
	BPMSG1268; //bpWstring("WP");
	bpTest(BP_EE_WP, 1);
	BPMSG1269; //bpWstring("ACK");
    bpTest(eetest(), 0);
#endif

	//ADC check
	BPMSG1170; //ADC & VREG
	ADCON(); // turn ADC ON

#if defined (BUSPIRATEV4)
	BPMSG1270; //"Vusb"
	bpADCPinTest(BP_ADC_USB,V5L, V5H);

	BPMSG1171; //VREG
	bpADCPinTest(BP_ADC_5V0,V5L, V5H);

	//enable 5v0 pullup and test
	BP_5VPU_ON();
	BPMSG1171; //5V VPU
	bpWstring(" ");
	BPMSG1172; //VPU 
	bpDelayMS(2);
	bpADCPinTest(BP_ADC_VPU,V5L, V5H);
	BP_PULLUP_OFF();

	//ADC test (3.3 volts)
	if(jumperTest){
		//ADC is connected to 3.3 volts
		//bpPOSTWstring("ADC");
		BPMSG1174;
		bpADCPinTest(BP_ADC_PROBE,V33L, V33H);
	}	

	//bpPOSTWstring("3.3V");
	BPMSG1173;
	bpADCPinTest(BP_ADC_3V3,V33L, V33H);

	//enable 3v3 pullup and test
	BP_3V3PU_ON();
	BPMSG1173; //bpWstring("3V3 VPU");
	bpWstring(" ");
	BPMSG1172; //VPU 
	
	bpDelayMS(2);
	bpADCPinTest(BP_ADC_VPU,V33L, V33H);
	BP_PULLUP_OFF();

#elif defined (BUSPIRATEV2)
	//v3 & v5 ADC and VREG check
	//test that the 5v VREG ADC reads 5
	BPMSG1171; //5V
	bpADCPinTest(BP_ADC_5V0,V5L, V5H);
	
	if(jumperTest){
		//test that the VPU pin ADC, attached to 5V with jumper, reads 5volts
		BPMSG1172; //VPU
		bpADCPinTest(BP_ADC_VPU,V5L, V5H);
	}

	//test that the 3.3v VREG ADC reads 3.3
	BPMSG1173;//3.3V
	bpADCPinTest(BP_ADC_3V3,V33L, V33H);

	if(jumperTest){
		//test that the ADC pin ADC, attached to 3.3V with jumper, reads 3.3volts
		BPMSG1174; //ADC
		bpADCPinTest(BP_ADC_PROBE,V33L, V33H);
	}
	#if defined (BUSPIRATEV5)
		BPMSG5000;
		bpBR;
		//TODO: v5 additional pullup options test (requires additional ADC connection to VPU net..
		BP_3V3PU_ON();
		bpDelayMS(2);
		BPMSG1173;//3.3V
		bpADCPinTest(BP_ADC_VPUN,V33L, V33H);	

		BP_5VPU_ON();
		bpDelayMS(2);
		BPMSG1171;//5V
		bpADCPinTest(BP_ADC_VPUN,V5L, V5H);
		BP_PULLUP_OFF();

	#endif
#endif

	ADCOFF(); // turn ADC OFF 

//*************
//
//  Test bus pins three ways, also tests on-board pullup resistors:
//	1. normal/high, 2. open collector ground, 3. open collector high.
//
//***************

	//BUS HIGH
	//pullup off, pins=output & high, read input, high?
	//bpPOSTWline("Bus high");
	BPMSG1175;
	IODIR&= ~(ALLIO);//output
	IOLAT|=ALLIO; //high	
	bpDelayMS(100);
	bpBusPinsTest(1);

	//BUS HI-Z + LOW
	//pullup on, pins=output & low, read input, low?
	IOLAT&= ~(ALLIO); //low
	#if defined	(BUSPIRATEV4) || defined(BUSPIRATEV5)
		BPMSG1176; //"Bus Hi-Z 0"	
		BP_3V3PU_ON();
		bpDelayMS(100);
		bpBusPinsTest(0);
	#elif defined(BUSPIRATEV3)
		if(jumperTest){
			BPMSG1176; //"Bus Hi-Z 0"
			BP_EXTPU_ON();
			bpDelayMS(100);
			bpBusPinsTest(0);
		}
	#endif


	//BUG HI-Z + HIGH
	//pullup on, pins=input & low, read input, high?
	#if defined	(BUSPIRATEV4) || defined(BUSPIRATEV5)
		BPMSG1177; //"Bus Hi-Z 1"
		IODIR|=ALLIO;//output
		bpDelayMS(100);
		bpBusPinsTest(1);
		//BP_PULLUP_OFF(); //will be done by bpInt at end of test
	#elif defined(BUSPIRATEV3)
		if(jumperTest){
			BPMSG1177; //"Bus Hi-Z 1"
			IODIR|=ALLIO;//output
			bpDelayMS(100);
			bpBusPinsTest(1);
		}
	#endif

//instructions (skip pause if no display output)
	if(showProgress && jumperTest){
		BP_VREG_ON();
		BP_MODELED_ON();
		//bpPOSTWline("MODE and VREG LEDs should be on! Any key exits.");
		#if defined (BUSPIRATEV4)
			BP_USBLED_ON();
		#endif
		BPMSG1178;
		BPMSG1250;
		//JTR Not required while(!UART1RXRdy());
		UART1RX();
		//this is not needed, handled in bpInit() below!
		/*#ifdef BUSPIRATEV4
			BP_USBLED_OFF();
		#endif
		BP_MODELED_OFF();
		BP_VREG_OFF();*/
	}

	bpInit();//clean up

	BPMSG1179;
	bpWdec(errors);
	BPMSG1180;
	bpConfig.quiet=0;


	return errors;

}

void bpADCPinTest(unsigned char a, unsigned int lval, unsigned int hval){
	unsigned int b;
	UART1TX('(');
	b=bpADC(a);
	bpWvolts(b);
	UART1TX(')');
	bpTest(((b>lval)&&(b<hval)),1);
}

//test that all bus pins are direction d
void bpBusPinsTest(unsigned char d){
	//bpPOSTWstring("MOSI");
	BPMSG1181;
	bpTest(BP_MOSI,d);
	//bpPOSTWstring("CLK");
	BPMSG1182;
	bpTest(BP_CLK,d);
	//bpPOSTWstring("MISO");
	BPMSG1183;
	bpTest(BP_MISO,d);
	//bpPOSTWstring("CS");
	BPMSG1184;
	bpTest(BP_CS,d);
}

//tests pin p for direction d
void bpTest(unsigned char p, unsigned char d){
	if(p==d){
		//bpPOSTWline(" OK");
		BPMSG1185;
	}else{
		//bpPOSTWline(" FAIL");
		BPMSG1186;
		errors++;
	}
}

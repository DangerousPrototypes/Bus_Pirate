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

#include "smps.h"
#include "base.h"
#include "procMenu.h"

unsigned int PWM_dutycycle, V_out, reading;
	
void smpsStart(unsigned int V) {	
	V_out = V;
	PWM_dutycycle = 128-(64000/V_out);			// Duty cycle in 7 bits
	V_out = V_out*45/58;						// Change to 1024 bit ADC reading to make comparison faster
	
	// Assign pin with PPS
	BP_AUX1_RPOUT = OC5_IO;

	// ADC settings
	AD1CHS = BP_ADC_PROBE;						// Set channel to ADC pin
	IFS0bits.AD1IF = 0;							// Clear ADC interrupt
	IEC0bits.AD1IE = 1;							// Enable ADC interrupt
	AD1CON1bits.ASAM = 1; 						// Enable auto sample
	
	// PWM settings
	OC5R = PWM_dutycycle;
	OC5RS = 0x7F;								// Set PWM period to 125 KHZ
	OC5CON2 = 0x1F;
	OC5CON = 0x1C06;
	
	AD1CON1bits.ADON = 1;						// turn ADC ON
}

void smpsStop(void) {
	AD1CON1bits.ADON = 0;						// Turn ADC OFF
	IEC0bits.AD1IE = 0;							// Disable ADC interrupt
	AD1CON1bits.ASAM = 0; 						// Disable auto sample
	OC5R = 0;
	OC5RS = 0;
	OC5CON2 = 0;
	OC5CON = 0;
	BP_AUX1_RPOUT = 0;
}

void smpsADC(void) {
	UART1TX((reading >> 8));
	UART1TX(reading);
}

void __attribute__((interrupt, no_auto_psv)) _ADC1Interrupt() {
	IFS0bits.AD1IF = 0;							// Clear ADC interrupt
	reading = ADC1BUF0;							// For checking out ADC results in binmode

	if(ADC1BUF0 > V_out) {						// if output voltage is higher than requested voltage
		OC5R = 0x00;						// turn PWM off temporarily
	} else {
		OC5R = PWM_dutycycle;					// otherwise continue normally
	}
}

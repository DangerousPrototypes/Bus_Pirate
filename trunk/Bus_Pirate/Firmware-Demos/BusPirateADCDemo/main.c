#include <p24fxxxx.h>

_CONFIG1(    JTAGEN_OFF &     //JTAG port is disabled
            GCP_OFF &         //GSP Memory Code Protection OFF
            GWRP_OFF &        //GCC Flash Write Protection OFF
            COE_OFF &         //
            FWDTEN_OFF &      //Watchdog Timer OFF
            ICS_PGx1)         //debug over PGD1 and PGC1
   
_CONFIG2(    FNOSC_FRCPLL &   //Internal FRC with PLL
            OSCIOFNC_ON &     //RA3 is clk out (fosc/2)
            POSCMOD_NONE &    //Primary oscillator disabled
            I2C1SEL_PRI)      //Use default SCL1/SDA1 pins

#define VREG33_DIR TRISAbits.TRISA0
#define VREG33_EN LATAbits.LATA0
#define MODE_LED_DIR TRISAbits.TRISA1
#define MODE_LED LATAbits.LATA1


#pragma code

int main(void)
{

    CLKDIVbits.RCDIV0=0;     //clock divider to 0
    AD1PCFG = 0xFFFF;        // Default all pins to digital
    OSCCONbits.SOSCEN=0;     //Disables the secondary oscilator
   
    MODE_LED_DIR = 0;        //sets the Mode LED pin RA1 as output
    MODE_LED = 0;            //turns LED off
    VREG33_DIR =0;           //sets teh VREG pin RA0 as output
    VREG33_EN = 1;           //turns on the voltage regulator

    unsigned long int i,voltage;      
//////////////////////////////////////////////////////////////
///////////////ADC config//////////////////////////////////
    AD1PCFGbits.PCFG12=0;          //configure RB12 as analog
    AD1CON1bits.SSRC  = 0b111;     // SSRC<3:0> = 111 implies internal
                                   // counter ends sampling and starts
                                   // converting.
    AD1CON3 = 0x1F02;              // Sample time = 31Tad,
                                   // Tad = 2 Tcy
    AD1CHS =12;                    //ADC channel select 12
    AD1CON1bits.ADON =1;           // turn ADC on
///FOREVER LOOP///////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
    while(1)
    {
        //this just gives us a little delay between measurements
        i =0xFFFFF;                 //sets i to 1048575
        while (i--);                //delay function

        //start a measurement with the ADC
        AD1CON1bits.DONE=0;         //resets DONE bit
        AD1CON1bits.SAMP=1;         //start sample
        
        while(AD1CON1bits.DONE==0); //wait for conversion to finish

        //get the measurement and use it to control the LED
        voltage = ADC1BUF0;         //get the voltage measurement
        if (voltage > 0x1D1) MODE_LED = 1; //enable LED if measurement is > 3volts
        else MODE_LED = 0;           //disable LED if less than 3volts
   }
}
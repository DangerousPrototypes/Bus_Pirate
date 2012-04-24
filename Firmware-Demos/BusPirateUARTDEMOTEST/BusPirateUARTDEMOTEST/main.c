#include <p24fxxxx.h>
_CONFIG2(FNOSC_FRCPLL & OSCIOFNC_ON &POSCMOD_NONE & I2C1SEL_PRI)        // Internal FRC OSC = 8MHz
_CONFIG1(JTAGEN_OFF & GCP_OFF & GWRP_OFF & COE_OFF & FWDTEN_OFF & ICS_PGx1) //turn off junk we don't need
 
 
#pragma code
 
int main(void){
 
  CLKDIVbits.RCDIV0=0; //clock divider to 0
  AD1PCFG = 0xFFFF;    //Default all pins to digital
  OSCCONbits.SOSCEN=0;   
 
  //Initialize UART1
  RPINR18bits.U1RXR = 5; // UART1 RX assign to RP5
  RPOR2bits.RP4R = 3; //UART1 TX assign to RP4
 
  U1BRG = 34;
  U1MODE = 0;
  U1MODEbits.BRGH = 1;
  U1STA = 0;
  U1MODEbits.UARTEN = 1;
  U1STAbits.UTXEN = 1;
  IFS0bits.U1RXIF = 0;
  
  char c;
  //Main Loop
    while(1){
         //wait for byte from USB to serial converter
         while(U1STAbits.URXDA == 0); 
         c=U1RXREG;//get byte
 
         //wait for empty spot in transmit buffer
         while(U1STAbits.UTXBF == 1); 
         //echo received character back to USB to serial converter
         U1TXREG = c; 
    }
}
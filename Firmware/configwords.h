/*
*
*       License: creative commons - attribution, share-alike 
*       Copyright Ian Lesnet and friends 2011
*       http://dangerousprototypes.com
*
*/

#ifndef CONFIG_H
#define CONFIG_H

// JTR v0.1a
// JTR v0.1b change capitals of file name. Renamed BUSPIRATEV4 and all other wardware to be UPPERCASE

#include "cdc_config.h"
#if defined(BUSPIRATEV4)
        #if defined(__PIC24FJ64GB106__) || defined(__PIC24FJ128GB106__) || defined(__PIC24FJ192GB106__) || defined(__PIC24FJ256GB106__) || defined(__PIC24FJ64GB108__) || defined(__PIC24FJ128GB108__) || defined(__PIC24FJ192GB108__) || defined(__PIC24FJ256GB108__) || defined(__PIC24FJ64GB110__) || defined(__PIC24FJ128GB110__) || defined(__PIC24FJ192GB110__) || defined(__PIC24FJ256GB110__) 
        _CONFIG1( JTAGEN_OFF & GCP_OFF & GWRP_OFF & COE_OFF & FWDTEN_OFF & ICS_PGx2) 
        _CONFIG2( IESO_OFF & FCKSM_CSDCMD & OSCIOFNC_ON & POSCMOD_HS & FNOSC_PRIPLL & PLLDIV_DIV3 & IOL1WAY_ON & PLL_96MHZ_ON)
        #else
        #error "ERROR wrong processor defined or no processor defined."
        #endif

#elif defined(JTR_PIC24FJXXXDA)
    _CONFIG1(FWDTEN_OFF & ICS_PGx2 & GWRP_OFF & GCP_OFF & JTAGEN_OFF)
    _CONFIG2(POSCMOD_HS & IOL1WAY_ON & OSCIOFNC_ON & FCKSM_CSDCMD & FNOSC_PRIPLL & PLL96MHZ_ON & PLLDIV_DIV2 & IESO_OFF)

#elif defined(JTR_PIC24FJXXGB2XX)
    _CONFIG1(FWDTEN_OFF & ICS_PGx2 & GWRP_OFF & GCP_OFF & JTAGEN_OFF)
    _CONFIG2(POSCMOD_HS & IOL1WAY_ON & OSCIOFNC_ON & FCKSM_CSDCMD & FNOSC_PRIPLL & PLL96MHZ_ON & PLLDIV_DIV2 & IESO_OFF)

#elif defined(IRTOY)
        #if defined(__18F2450) || defined(__18F2455) || defined(__18F2458) || defined(__18F2550) || defined(__18F2553)|| defined(__18F4450) || defined(__18F4455) || defined(__18F4458) || defined(__18F4550) || defined(__18F4553)
        
                #pragma config PLLDIV   = 5         
                #pragma config CPUDIV   = OSC1_PLL2   
                #pragma config USBDIV   = 2         
                #pragma config FOSC     = HSPLL_HS
                #pragma config FCMEN    = OFF
                #pragma config IESO     = OFF
                #pragma config PWRT     = OFF
                #pragma config BOR      = ON
                #pragma config BORV     = 3
                #pragma config VREGEN   = ON      
                #pragma config WDT      = OFF
                #pragma config WDTPS    = 32768
                #pragma config MCLRE    = ON
                #pragma config LPT1OSC  = OFF
                #pragma config PBADEN   = OFF
                #pragma config STVREN   = ON
                #pragma config LVP      = OFF
                #pragma config XINST    = OFF       
                #pragma config CP0      = OFF
                #pragma config CP1      = OFF
                #pragma config CPB      = OFF
                #pragma config WRT0     = OFF
                #pragma config WRT1     = OFF
                #pragma config WRTB     = OFF   
                #pragma config WRTC     = OFF
                #pragma config EBTR0    = OFF
                #pragma config EBTR1    = OFF
                #pragma config EBTRB    = OFF
        #endif

        #if defined(__18F2550) || defined(__18F2553)||  defined(__18F4550) || defined(__18F4553)

                #pragma config CP3      = OFF
                #pragma config WRT3     = OFF
                #pragma config EBTR3    = OFF

        #endif   
        
        #if defined(__18F2455) || defined(__18F2458) || defined(__18F2550) || defined(__18F2553)|| defined(__18F4455) || defined(__18F4458) || defined(__18F4550) || defined(__18F4553)
                #pragma config CCP2MX   = ON
                #pragma config CP2      = OFF
                #pragma config CPD      = OFF
                #pragma config WRT2     = OFF
                #pragma config WRTD     = OFF
                #pragma config EBTR2    = OFF

        #else   //JTR meaning then the 18F2450 and 18F4450
                #pragma config BBSIZ    = BB1K 
        #endif

        
        #if defined(__18F4450) || defined(__18F4455) || defined(__18F4458) || defined(__18F4550) || defined(__18F4553)
               #pragma config ICPRT    = OFF    //JTR 44-pin packages only
        #endif    

// JTR end of IRTOY config words        

#elif defined(IANS18FJ)  //__18F24J50

     #pragma config WDTEN = OFF       

     #pragma config PLLDIV = 5           //Divide by 5 (20 MHz oscillator input) 
     #pragma config STVREN = ON         
     #pragma config XINST = OFF          
     #pragma config CPUDIV = OSC1        
     #pragma config CP0 = OFF            
     #pragma config OSC = HSPLL          
     #pragma config T1DIG = ON           
     #pragma config LPT1OSC = OFF        
     #pragma config FCMEN = OFF          
     #pragma config IESO = OFF           
     #pragma config WDTPS = 32768        
     #pragma config DSWDTOSC = INTOSCREF 
     #pragma config RTCOSC = T1OSCREF    
     #pragma config DSBOREN = OFF        
     #pragma config DSWDTEN = OFF       
     #pragma config DSWDTPS = 8192       
     #pragma config IOL1WAY = OFF        
     #pragma config MSSP7B_EN = MSK7     
     #pragma config WPFP = PAGE_1        
     #pragma config WPEND = PAGE_0       
     #pragma config WPCFG = OFF          
     #pragma config WPDIS = OFF       

#elif defined(EA14K) || defined(LPC14K) || defined(__18F14K50)

// configword 0x300000
        #pragma config CPUDIV = NOCLKDIV
        #pragma config USBDIV = OFF
        #pragma config FOSC = HS
        #pragma config PLLEN = ON
        #pragma config PCLKEN = ON
        #pragma config FCMEN = OFF
        #pragma config IESO = OFF

// configword 0x300002
        #pragma config PWRTEN = ON
        #pragma config BOREN = OFF
        #pragma config BORV = 30

// configword 0x300003
        #pragma config WDTEN = OFF
        #pragma config WDTPS = 32768

// configword 0x300005
        #pragma config HFOFST = OFF
        #pragma config MCLRE = ON   

// configword 0x300006
        #pragma config STVREN = ON
        #pragma config LVP = OFF
        #pragma config XINST = OFF

// configword 0x300008
        #pragma config CP0 = OFF 
        #pragma config CP1 = OFF 

// configword 0x300009
        #pragma config CPB = OFF 
        #pragma config CPD = OFF

// configword 0x30000a 
        #pragma config WRT0 = OFF 
        #pragma config WRT1 = OFF

// configword 0x30000b 
        #pragma config WRTB = OFF 
        #pragma config WRTC = OFF  
        #pragma config WRTD = OFF

// configword 0x30000c 
        #pragma config EBTR0 = OFF 
        #pragma config EBTR1 = OFF 

// configword 0x30000d
        #pragma config EBTRB = OFF
        
#elif defined(JTR_18FXXJ53)

     #pragma config WDTEN = OFF          //WDT disabled (enabled by SWDTEN bit)
     #pragma config PLLDIV = 1           //Divide by 1 (4 MHz oscillator input)
     #pragma config STVREN = ON          //stack overflow/underflow reset enabled
     #pragma config XINST = OFF          //Extended instruction set disabled
     #pragma config CPUDIV = OSC1        //No CPU system clock divide
     #pragma config CP0 = OFF            //Program memory is not code-protected
     #pragma config OSC = HSPLL          //HS oscillator, PLL enabled, HSPLL used by USB
     #pragma config FCMEN = OFF          //Fail-Safe Clock Monitor disabled
     #pragma config IESO = OFF           //Two-Speed Start-up disabled
     #prabma config SOSCSEL = LOW        // Low power timer1
     #pragma config WDTPS = 32768        //1:32768
     #pragma config DSWDTOSC = INTOSCREF //DSWDT uses INTOSC/INTRC as clock
     #pragma config RTCOSC = T1OSCREF    //RTCC uses T1OSC/T1CKI as clock
     #pragma config DSBOREN = OFF        //Zero-Power BOR disabled in Deep Sleep
     #pragma config DSWDTEN = OFF        //Disabled
     #pragma config DSWDTPS = 8192       //1:8,192 (8.5 seconds)
     #pragma config IOL1WAY = OFF        //IOLOCK bit can be set and cleared
     #pragma config MSSP7B_EN = MSK7     //7 Bit address masking
     #pragma config WPFP = PAGE_1        //Write Protect Program Flash Page 0
     #pragma config WPEND = PAGE_0       //Start protection at page 0
     #pragma config WPCFG = OFF          //Write/Erase last page protect Disabled
     #pragma config WPDIS = OFF          //WPFP[5:0], WPEND, and WPCFG bits ignored  

#endif
#endif


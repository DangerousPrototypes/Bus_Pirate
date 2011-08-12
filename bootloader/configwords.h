/*
 *
 *       License: creative commons - attribution, share-alike
 *       Copyright Ian Lesnet and friends 2011
 *       http://dangerousprototypes.com
 *
 */

#ifndef CONFIG_H
#define CONFIG_H

#include "boot_config.h"

#if defined(__PIC24FJ64GB106__) || defined(__PIC24FJ128GB106__) || defined(__PIC24FJ192GB106__) || defined(__PIC24FJ256GB106__) || defined(__PIC24FJ64GB108__) || defined(__PIC24FJ128GB108__) || defined(__PIC24FJ192GB108__) || defined(__PIC24FJ256GB108__) || defined(__PIC24FJ64GB110__) || defined(__PIC24FJ128GB110__) || defined(__PIC24FJ192GB110__) || defined(__PIC24FJ256GB110__) 
_CONFIG1(JTAGEN_OFF & GCP_OFF & GWRP_OFF & COE_OFF & FWDTEN_OFF & ICS_PGx2)
_CONFIG2(IESO_OFF & FCKSM_CSDCMD & OSCIOFNC_ON & POSCMOD_HS & FNOSC_PRIPLL & PLLDIV_DIV3 & IOL1WAY_ON & PLL_96MHZ_ON)

#elif defined(JTR_PIC24FJXXXDA)
_CONFIG1(FWDTEN_OFF & ICS_PGx2 & GWRP_OFF & GCP_OFF & JTAGEN_OFF)
_CONFIG2(POSCMOD_HS & IOL1WAY_ON & OSCIOFNC_ON & FCKSM_CSDCMD & FNOSC_PRIPLL & PLL96MHZ_ON & PLLDIV_DIV2 & IESO_OFF)

#elif defined(JTR_PIC24FJXXGB2XX)
_CONFIG1(FWDTEN_OFF & ICS_PGx2 & GWRP_OFF & GCP_OFF & JTAGEN_OFF)
_CONFIG2(POSCMOD_HS & IOL1WAY_ON & OSCIOFNC_ON & FCKSM_CSDCMD & FNOSC_PRIPLL & PLL96MHZ_ON & PLLDIV_DIV2 & IESO_OFF)

#else
#error "ERROR wrong processor defined or no processor defined."
#endif

#endif



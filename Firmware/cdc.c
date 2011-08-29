/*
This work is licensed under the Creative Commons Attribution 3.0 Unported License.
To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
or send a letter to
        Creative Commons,
        171 Second Street,
        Suite 300,
        San Francisco,
        California,
        94105,
        USA.
 */

// JTR V0.1a
// JTR v0.1b
// JTR v0.1c	// tidy up added code line code handler to set the SPBRG based on line coding.

#include "globals.h"

#ifdef BUSPIRATEV4
//#include "base.h"
#include <string.h>
void USBDeviceTasks(void);

enum stopbits {
    one = 0, oneandahalf = 1, two = 2
};

enum parity {
    none = 0, odd = 1, even = 2, mark = 3, space = 4
};
const char parity_str[] = {'N', 'O', 'E', 'M', 'S'};

struct cdc_LineCodeing {
    unsigned long int dwDTERate;
    enum stopbits bCharFormat;
    enum parity bParityType;
    BYTE bDataBits;
} linecodeing;

/*
struct _cdc_ControlLineState {
    int DTR : 1;
    int RTS : 1;
    int unused1 : 6;
    BYTE unused2;
}; // cls;
 */

#pragma udata usb_data
BYTE cdc_acm_in_buffer[CDC_NOTICE_BUFFER_SIZE]; //JTR NEWLY defined NOTICE BUFFER SIZE and increased from 8 to 10 bytes in usb_config.h

#pragma udata usb_data2
BYTE cdc_In_buffer[CDC_BUFFER_SIZE];
BYTE cdc_Out_buffer[CDC_BUFFER_SIZE];

#pragma udata usb_data3
BYTE cdc_In_bufferA[64];
BYTE cdc_In_bufferB[64];
BYTE cdc_Out_bufferA[64];
BYTE cdc_Out_bufferB[64];

#pragma udata

static struct _irtoyusbbuffer {
    BYTE inBuf[CDC_BUFFER_SIZE]; //JTR2
    unsigned char cnt;
    unsigned char rdptr;
    // unsigned char inbufflag;
} usbbuf;
struct _cdc_ControlLineState cls;
BYTE cdc_In_len; // total cdc In length
BYTE cdc_Out_len; // total cdc out length
BYTE IsInBufferA;
BYTE IsOutBufferA;
BYTE *InPtr;
BYTE *OutPtr;
BYTE LineStateUpdated = 0;

//unsigned int SOFCOUNT;
//BYTE *data, *data_end;
BDentry *Outbdp, *Inbdp;
BYTE CDCFunctionError;

volatile BYTE cdc_trf_state; // JTR don't see that it is really volatile in current context may be in future.

void initCDC(void) {

    // JTR The function usb_init() is now called from main.c prior to anything else belonging to the CDC CLASS
    // If we have multiple functions we want the USB initialization to be in only one consistant place.
    // The sort of things we would do in InitCDC would be to setup I/O pins and the HARDWARE UART so it
    // is not transmitting junk between a RESET and the device being enumerated. Hardware CTS/RTS
    // would also be setup here if being used.

    linecodeing.dwDTERate = 115200;
    linecodeing.bCharFormat = one;
    linecodeing.bParityType = none;
    linecodeing.bDataBits = 8;
    cls.DTR = 0;
    cls.RTS = 0;
    usb_register_class_setup_handler(cdc_setup);
    //data = data_end = 0;
}

void user_configured_init(void) {
    // JTR NEW FUNCTION
    // After the device is enumerated and configured then we set up non EP0 endpoints.
    // We only enable the endpoints we are using, not all of them.
    // Prior to this they are held in a disarmed state.

    // This function belongs to the current USB function and IS NOT generic. This is CLASS specific
    // and will vary from implementation to implementation.

    usb_unset_in_handler(1);
    usb_unset_in_handler(2);
    usb_unset_out_handler(2);

    //USB_UEP[1] = USB_EP_IN;
    //USB_UEP[2] = USB_EP_INOUT;

    USB_UEP1 = USB_EP_IN;
    USB_UEP2 = USB_EP_INOUT;

    /* Configure buffer descriptors */
#if USB_PP_BUF_MODE == 0
    // JTR Setup CDC LINE_NOTICE EP (Interrupt IN)
    usb_bdt[USB_CALC_BD(1, USB_DIR_IN, USB_PP_EVEN)].BDCNT = 0;
    usb_bdt[USB_CALC_BD(1, USB_DIR_IN, USB_PP_EVEN)].BDADDR = cdc_acm_in_buffer;
    usb_bdt[USB_CALC_BD(1, USB_DIR_IN, USB_PP_EVEN)].BDSTAT = DTS + DTSEN; // Set DTS => First packet inverts, ie. is Data0

    usb_bdt[USB_CALC_BD(2, USB_DIR_OUT, USB_PP_EVEN)].BDCNT = CDC_BUFFER_SIZE; // JTR N/A endpoints[i].buffer_size;
    usb_bdt[USB_CALC_BD(2, USB_DIR_OUT, USB_PP_EVEN)].BDADDR = cdc_Out_buffer;
    usb_bdt[USB_CALC_BD(2, USB_DIR_OUT, USB_PP_EVEN)].BDSTAT = UOWN + DTSEN;

    usb_bdt[USB_CALC_BD(2, USB_DIR_IN, USB_PP_EVEN)].BDCNT = 0;
    usb_bdt[USB_CALC_BD(2, USB_DIR_IN, USB_PP_EVEN)].BDADDR = cdc_In_buffer;
    usb_bdt[USB_CALC_BD(2, USB_DIR_IN, USB_PP_EVEN)].BDSTAT = DTS + DTSEN; // Set DTS => First packet inverts, ie. is Data0

#else
    // TODO: Implement Ping-Pong buffering setup.
#error "PP Mode not implemented yet"
#endif

    usb_register_class_setup_handler(cdc_setup);
    cdc_trf_state = 0;
    Outbdp = &usb_bdt[USB_CALC_BD(2, USB_DIR_OUT, USB_PP_EVEN)];
    Inbdp = &usb_bdt[USB_CALC_BD(2, USB_DIR_IN, USB_PP_EVEN)];
}

void cdc_setup(void) {
    BYTE *packet;
    size_t reply_len;
    packet = bdp->BDADDR;
    switch (packet[USB_bmRequestType] & (USB_bmRequestType_TypeMask | USB_bmRequestType_RecipientMask)) {
        case (USB_bmRequestType_Class | USB_bmRequestType_Interface):
            switch (packet[USB_bRequest]) {

                    //JTR This is just a dummy, nothing defined to do for CDC ACM
                case CDC_SEND_ENCAPSULATED_COMMAND:
                    usb_ack_dat1(rbdp, 0);
                    break;

                    //JTR This is just a dummy, nothing defined to do for CDC ACM
                case CDC_GET_ENCAPSULATED_RESPONSE:
                    //usb_ack_zero(rbdp);
                    usb_ack_dat1(rbdp, 0);
                    break;

                case CDC_SET_COMM_FEATURE: // Optional
                case CDC_GET_COMM_FEATURE: // Optional
                case CDC_CLEAR_COMM_FEATURE: // Optional
                    usb_RequestError(); // Not advertised in ACM functional descriptor
                    break;

                case CDC_SET_LINE_CODING: // Optional, strongly recomended
                    usb_set_out_handler(0, cdc_set_line_coding_data); // Register out handler function
                    break;

                case CDC_GET_LINE_CODING: // Optional, strongly recomended
                    // JTR reply length (7) is always going to be less than minimum EP0 size (8)

                    reply_len = *((unsigned int *) &packet[USB_wLength]);
                    if (sizeof (struct cdc_LineCodeing) < reply_len) {
                        reply_len = sizeof (struct cdc_LineCodeing);
                    }
                    memcpy(rbdp->BDADDR, (const void *) &linecodeing, reply_len);
                    usb_ack_dat1(rbdp, reply_len); // JTR common addition for STD and CLASS ACK

                    usb_set_in_handler(0, cdc_get_line_coding); // JTR why bother? There is nothing more to do.

                    break;

                case CDC_SET_CONTROL_LINE_STATE: // Optional
                    cls = *((struct _cdc_ControlLineState *) &packet[USB_wValue]);
                    usb_set_in_handler(0, cdc_set_control_line_state_status); // JTR why bother?
                    usb_ack_dat1(rbdp, 0); // JTR common addition for STD and CLASS ACK
                    LineStateUpdated = 1;


                    break;

                case CDC_SEND_BREAK: // Optional
                default:
                    usb_RequestError();
            }
            break;
        default:
            usb_RequestError();
    }
}

void cdc_get_line_coding(void) {
    usb_unset_in_handler(0); // Unregister IN handler;
}

void cdc_set_line_coding_data(void) { // JTR handling an OUT token In the CDC stack this is the only function that handles an OUT data stage.
    unsigned long dwBaud;

    memcpy(&linecodeing, (const void *) bdp->BDADDR, sizeof (struct cdc_LineCodeing));

    // JTR2 added 8th June 2011 not tested

    //  if (linecodeing.dwDTERate > 115200) {
    //      usb_RequestError();
    //  } else {



    dwBaud = (CLOCK_FREQ / 4) / linecodeing.dwDTERate - 1;

    UART_BAUD_setup(dwBaud);
    //  }

    usb_unset_out_handler(0); // Unregister OUT handler; JTR serious bug fix in macro!
    usb_set_in_handler(0, cdc_set_line_coding_status); // JTR why bother?
    usb_ack_dat1(rbdp, 0); // JTR common addition for STD and CLASS ACK

    // JTR This part of the USB-CDC stack is worth highlighting
    // This is the only place that we have an OUT DATA packet on
    // EP0. At this point it has been completed. This stack unlike
    // the microchip stack does not have a common IN or OUT data
    // packet complete tail and therefore it is the responsibility
    // of each section to ensure that EP0 is set-up correctly for
    // the next setup packet.


    //  Force EP0 OUT to the DAT0 state
    //  after we have all our data packets.
    bdp->BDCNT = USB_EP0_BUFFER_SIZE;
    bdp->BDSTAT = UOWN | DTSEN;
}

void cdc_set_line_coding_status(void) {
    usb_unset_in_handler(0);
}

void cdc_set_control_line_state_status(void) {
    usb_unset_in_handler(0);
}


// JTR keep this fragment as it may be useful later if reworked a little
//void cdc_acm_in( void ) {

// JTR LINE_NOTIFICATION EP has been increased to ten bytes.
// This is because if it were ever to be used it is more likely
// that it will be for the Serial State Notification which has
// two bytes of data to include in the packet.

// Also we will not actually come to this code if the LINE_NOTIFICATION
// endpoint is not already armed and we do not arm this end point
// until we have something to send. Therefore we have a chicken and egg
// deadlock and this function is of no value the way it is currently coded.
// There is no IN token if the endpoint is not armed!
// No IN handler is required nor a state machine as there is no need for a ZLP

/*
if (0) { // Response Available Notification
        // Not TODO: Probably never implement this, we're not a modem.
        // Is this correct placement of the response notification?
        bdp->BDADDR[USB_bmRequestType]	= USB_bmRequestType_D2H | USB_bmRequestType_Class | USB_bmRequestType_Interface;
        bdp->BDADDR[USB_bRequest]		= CDC_RESPONSE_AVAILABLE;
        bdp->BDADDR[USB_wValue]			= 0;
        bdp->BDADDR[USB_wValueHigh]		= 0;
        bdp->BDADDR[USB_wIndex]			= 0;
        bdp->BDADDR[USB_wIndexHigh]		= 0;
        bdp->BDADDR[USB_wLength]		= 0;
        bdp->BDADDR[USB_wLengthHigh]	= 0;
        // JTR past below bdp->BDCNT = 8;
        usb_ack_dat1(bdp, 8);
} else if (0) {	// Network Connection Notification
} else if (0) {	// Serial State Notification
}
} */

/* END OF CDC CLASS REQUESTS HANDLING

 Below are the CDC USBUART functions */

//void sof_counter_handler() {
//    if (SOFCOUNT == 0) return;
//    SOFCOUNT--;
//}

/**********************************************************************************
  Function:
        BYTE WaitOutReady()

  // JTR2 addition
  Summary:
    It is a semi-blocking function. It waits for the cdc Out Buffer to
    become available before returning. However if the USB interrupt is
    not enabled it call the fast USB housekeeper to ensure that the USB
    stack is always serviced in the meantime. If in polling mode and a
    setup packet or bus reset happens it returns FALSE (00) to alert the calling
    function that the transfer may have been interfered with.


 /*****************************************************************************/
BYTE WaitOutReady() // JTR2 added reduced overhead
{
    BYTE i = 0;
    while ((Outbdp->BDSTAT & UOWN)) {
        if (!TestUsbInterruptEnabled()) {
            if (0 != FAST_usb_handler()) // JTR2 Pop non-EP0 (USB IN) tranfers from the FIFO and also give SETUP PACKETs a chance.
                i = 0;
        }
    }//end WaitOutReady
    return i;
}

/**********************************************************************************/

/**********************************************************************************
  Function:
        BYTE WaitInReady()

  // JTR2 addition
  Summary:
    As this CDC stack writes directly into USB RAM this function checks for
    for its availability.
    It is a semi-blocking function. It waits for the cdc In Buffer to
    become available before returning. However if the USB interrupt is
    not enabled it call the fast USB housekeeper to ensure that the USB
    stack is always serviced in the meantime. If in polling mode and a
    setup packet or bus reset happens it returns FALSE (00) to alert the calling
    function that the transfer may have been interfered with.


 /*****************************************************************************/
BYTE WaitInReady() // JTR2 added reduced overhead
{
    BYTE i = 0;
    while ((Inbdp->BDSTAT & UOWN)) {
        if (!TestUsbInterruptEnabled()) {
            if (0 != FAST_usb_handler()) // JTR2 Pop non-EP0 (USB IN) tranfers from the FIFO and also give SETUP PACKETs a chance.
                i = 0;
        }
    }
    return i;
}//end WaitInReady

/**********************************************************************************
  Function:
        BYTE putPARTARRAYUSBUSART()

  // JTR2 addition
  Summary:
    Fast, low overhead CDC IN packet does not track or send ZLP.
    It is a semi-blocking function. It waits for the cdc In Buffer to
    become available before returning. However if the USB interrupt is
    not enabled it call the fast USB housekeeper to ensure that the USB
    stack is always serviced in the meantime. If in polling mode and a
    setup packet or bus reset happens it returns 0xFF to alert the calling
    function that the transfer may have been interfered with.

    Note that the data must reside in USB RAM.


 /*****************************************************************************/
// JTR2 Does NOT send ZLP after each packet only one at the end is required.

BYTE putPARTARRAYUSBUSART(BYTE *data, BYTE length) // JTR2 added reduced overhead
{
    while ((Inbdp->BDSTAT & UOWN)) {
        if (!TestUsbInterruptEnabled()) {
            FAST_usb_handler();

            /*
                        if (0 != FAST_usb_handler()) { // JTR2 Pop non-EP0 (USB IN) tranfers from the FIFO and also give SETUP PACKETs a chance.
                            //return 0xff;
                        }
             */
        }
    }
    Inbdp->BDADDR = data;
    Inbdp->BDCNT = length;
    Inbdp->BDSTAT = ((Inbdp->BDSTAT ^ DTS) & DTS) | UOWN | DTSEN;

    return length;
}//end putUSBUSARTFAST

/**********************************************************************************
  Function:
        BYTE putFULLARRAYUSBUSART()

  // JTR2 addition
  Summary:
    Fast, low overhead CDC In packet does not track or send ZLP.
    It is a non-blocking function. It does not wait for the cdc In Buffer to
    become available. Instead it returns '0' to notify the caller that
    data has not been sent. Otherwise it return the endpoint size.


 /*****************************************************************************/
BYTE putFULLARRAYUSBUSART() // JTR2 added reduced overhead
{
    if ((Inbdp->BDSTAT & UOWN)) {
        return 0;
    } else {
        Inbdp->BDCNT = CDC_BUFFER_SIZE;
        Inbdp->BDSTAT = ((Inbdp->BDSTAT ^ DTS) & DTS) | UOWN | DTSEN;
    }
    return CDC_BUFFER_SIZE;
}//end putUSBUSARTFAST

/**********************************************************************************/
BYTE getCDC_Out_ArmNext(void) {

    CDCFunctionError = 0;

    /*
        if (WaitOutReady()) {
     */
    WaitOutReady();

    if ((IsOutBufferA & 1)) {
        OutPtr = &cdc_Out_bufferA[0];
        Outbdp->BDADDR = &cdc_Out_bufferB[0];
    } else {
        OutPtr = &cdc_Out_bufferB[0];
        Outbdp->BDADDR = &cdc_Out_bufferA[0];
    }
    IsOutBufferA ^= 0xFF;
    cdc_Out_len = Outbdp->BDCNT;
    Outbdp->BDCNT = CDC_BUFFER_SIZE;
    Outbdp->BDSTAT = ((Outbdp->BDSTAT ^ DTS) & DTS) | UOWN | DTSEN;

    /*
        } else {
            cdc_Out_len = 0;
            CDCFunctionError = 1;
        }
     */
    return cdc_Out_len;
}//end getCDC_Out_ArmNext

BYTE SendCDC_In_ArmNext(BYTE count) {

    CDCFunctionError = 0;
    /*
        if (WaitInReady()) {
     */
    WaitInReady();
    if (IsInBufferA) {
        Inbdp->BDADDR = cdc_In_bufferA;
        InPtr = cdc_In_bufferB;
    } else {
        Inbdp->BDADDR = cdc_In_bufferB;
        InPtr = cdc_In_bufferA;
    }
    Inbdp->BDCNT = count;
    Inbdp->BDSTAT = ((Inbdp->BDSTAT ^ DTS) & DTS) | UOWN | DTSEN;
    IsInBufferA ^= 0xFF;
    /*
        } else {
            CDCFunctionError = 1;
        }
     */
    return CDCFunctionError;
}



void ArmCDCInDB(void) {
    WaitInReady();
    //Inbdp->BDSTAT &= ~UOWN; // JTR3 immediately reclaim In buffer
    Inbdp->BDADDR = &cdc_In_bufferA[0];
    Inbdp->BDCNT = CDC_BUFFER_SIZE;
    //Inbdp->BDSTAT |= UOWN;
    IsInBufferA = 0xFF;
    InPtr = cdc_In_bufferA;
}

void DisArmCDCInDB(void) {
    WaitInReady();
    //Inbdp->BDSTAT &= ~UOWN; // JTR3 immediately reclaim In buffer
    Inbdp->BDADDR = &cdc_In_buffer[0];
    Inbdp->BDCNT = CDC_BUFFER_SIZE;
    //Inbdp->BDSTAT |= UOWN;
}

void ArmCDCOutDB(void) {
	BYTE i;
	IsOutBufferA = 0xFF;
	OutPtr = cdc_Out_bufferA;
	i = Outbdp->BDSTAT;
	i &= DTS;
	Outbdp->BDSTAT = i;
	Outbdp->BDADDR = &cdc_Out_bufferA[0];
	i |= (UOWN | DTSEN);
	Outbdp->BDSTAT = i;
		
	/*
	Outbdp->BDSTAT &= ~UOWN; // JTR3 immediately reclaim out buffer
	Outbdp->BDADDR = &cdc_Out_bufferA[0];
	Outbdp->BDCNT = CDC_BUFFER_SIZE;
	Outbdp->BDSTAT |= UOWN;
	*/
}

void DisArmCDCOutDB(void) {
	BYTE i;
	i = Outbdp->BDSTAT;
	i &= DTS;
	Outbdp->BDSTAT = i;
	Outbdp->BDADDR = &cdc_Out_buffer[0];
	i |= (UOWN | DTSEN);
	Outbdp->BDSTAT = i;
	
	/*
	// WaitOutReady();
	Outbdp->BDSTAT &= ~UOWN; // JTR3 immediately reclaim out buffer
	Outbdp->BDADDR = &cdc_Out_buffer[0];
	//Outbdp->BDCNT = CDC_BUFFER_SIZE;
	Outbdp->BDSTAT |= UOWN;
	*/
}

void SendZLP(void) {
    WaitInReady();
    Inbdp->BDCNT = 0;
    Inbdp->BDSTAT = ((Inbdp->BDSTAT ^ DTS) & DTS) | UOWN | DTSEN;
}

/**********************************************************************************/
BYTE getOutReady(void) {

    return !(Outbdp->BDSTAT & UOWN); // Do we have a packet from host?
}

/**********************************************************************************/
BYTE getInReady(void) {

    return !(Inbdp->BDSTAT & UOWN); // Is the CDC In buffer ready?
}

/************************************************************************/

BYTE getsUSBUSART(BYTE *buffer, BYTE len) {
    cdc_Out_len = 0;
    if (0 == (Outbdp->BDSTAT & UOWN)) // Do we have a packet from host?
    {

        // Adjust the expected number of BYTEs to equal
        // the actual number of BYTEs received.

        if (len > Outbdp->BDCNT)
            len = Outbdp->BDCNT;

        // Copy data from dual-ram buffer to user's buffer

        for (cdc_Out_len = 0; cdc_Out_len < len; cdc_Out_len++)
            buffer[cdc_Out_len] = cdc_Out_buffer[cdc_Out_len];

        // Prepare dual-ram buffer for next OUT transaction

        if (!TestUsbInterruptEnabled()) {
            FAST_usb_handler(); // JTR2 Pop non-EP0 (USB IN) tranfers from the FIFO and also give SETUP PACKETs a chance.
        }
        Outbdp->BDCNT = CDC_BUFFER_SIZE;
        Outbdp->BDSTAT = ((Outbdp->BDSTAT ^ DTS) & DTS) | UOWN | DTSEN;
    }//end if

    return cdc_Out_len;

}//end getsUSBUSART

unsigned char usbbufservice(void) {

    if (usbbuf.cnt == 0) {//if the buffer is empty, get more data
        usbbuf.cnt = getsUSBUSART(usbbuf.inBuf, CDC_BUFFER_SIZE); //JTR2
        usbbuf.rdptr = 0;
    }
	if (usbbuf.cnt)return 1;
	return 0;
}

//puts a byte from the buffer in the byte, returns 1 if byte

unsigned char usbbufgetbyte(unsigned char* c) {
    if (usbbuf.cnt > 0) {
        *c = usbbuf.inBuf[usbbuf.rdptr];
        usbbuf.cnt--;
        usbbuf.rdptr++;
        return 1;
    }
    return 0;
}

//puts a byte from the buffer in the byte, returns 1 if byte

unsigned char PEEKusbbufgetbyte(unsigned char* c) {
    if (usbbuf.cnt > 0) {
        *c = usbbuf.inBuf[usbbuf.rdptr];
        return 1;
    }
    return 0;
}

void usbbufflush(void) {
    usbbuf.cnt = 0;
    usbbuf.rdptr = 0;
}
#endif


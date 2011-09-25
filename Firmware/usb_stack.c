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

Influence and inspiration taken from http://pe.ece.olin.edu/ece/projects.html
 */

// JTR v0.1a

#include "globals.h"    // Compiler STD etc. Not function related.

#ifdef BUSPIRATEV4
#include "cdc_config.h"
#include "usb_stack.h"

// JTR removed all references to USB_MAX_BUFFER_SIZE as it was being applied to all endpoints.
// It is EP0 that must be checked so we use USB_EP0_BUFFER_SIZE

// JTR moved this block here from usb_stack heeder so the EP0 size can be defined once (and only once)
// in the cdc_config header. This will universally trap errors and avoid the stack being conpiled
// with different sizes defined in different places.
#if USB_EP0_BUFFER_SIZE == 8u
#elif USB_EP0_BUFFER_SIZE == 16u
#elif USB_EP0_BUFFER_SIZE == 32u
#elif USB_EP0_BUFFER_SIZE == 64u
#else
#error "USB_EP0_BUFFER_SIZE needs to be 8, 16, 32 or 64 bytes"
#endif


ROMPTR const unsigned char *usb_device_descriptor;
ROMPTR const unsigned char *usb_config_descriptor;
ROMPTR const unsigned char *usb_string_descriptor;
int usb_num_string_descriptors;

usb_handler_t sof_handler;
usb_handler_t class_setup_handler, vendor_setup_handler;


usb_ep_t endpoints[MAX_CHIP_EP]; // JTR change. MAX_CHIP_EP is the number of hardware endpoints on the silicon. See picusb.h

/* Allocate buffers for buffer description table and the actual buffers */
// CvD: linkscript puts it in the right memory location


// JTR comments. This below goes part way to ridding us of the need for a linker script (PIC18).
// It floats the EP0 buffers to occupy space immediately behind the buffer descriptors.
// However there are problems. It only works for the EP0 buffers because they are the only
// buffers defined in this block. It is not possible to do arithmatic within a #pragma
// therefore there seems to be no way to cause EPs in other blocks to float.
// All the buffers would need to be defined here and this would break the universal nature
// of this module. As such efforts to create a custom linker script free stack have been
// put on hold for now but can be revisited at any stage.
// Please see http://dangerousprototypes.com/forum/viewtopic.php?f=39&t=1651&start=120#p17401

#if defined(PIC_18F)
#pragma udata usb_bdt
BDentry usb_bdt[2 + 2 * MAX_EPNUM_USED]; // JTR changed index from 32 to variable
#pragma udata usb_data
//* Only claim buffer for ep 0 */
#if USB_PP_BUF_MODE == 0
unsigned char usb_ep0_out_buf[USB_EP0_BUFFER_SIZE];
unsigned char usb_ep0_in_buf[USB_EP0_BUFFER_SIZE];
#else
#error "Ping pong buffer not implemented yet!"
#endif

#elif defined(PIC_24F)
#pragma udata usb_bdt
BDentry usb_bdt[2 + 2 * MAX_EPNUM_USED] __attribute__((aligned(512))); // JTR changed index from 32 to variable TODO: Dynamic allocation reflecting number of used endpoints. (How to do counting in preprocessor?)
#if USB_PP_BUF_MODE == 0
unsigned char usb_ep0_out_buf[USB_EP0_BUFFER_SIZE];
unsigned char usb_ep0_in_buf[USB_EP0_BUFFER_SIZE];
#else
#error "Ping pong buffer not implemented yet!"
#endif
#endif

#pragma udata
unsigned int usb_device_status;
unsigned int usb_current_cfg;
volatile unsigned char usb_device_state;
unsigned char usb_addr_pending;
BYTE trn_status; // Global since it is needed everywere
BDentry *bdp, *rbdp; // Dito

ROMPTR const unsigned char *usb_rom_ptr;
size_t usb_rom_len;

/* Forward Reference Prototypes */
void usb_handle_error(void);
void usb_handle_reset(void);
void usb_handle_transaction(void);
void usb_handle_setup(void);
void usb_handle_out(void);
void usb_handle_in(void);
void usb_handle_StandardDeviceRequest(BDentry*);
void usb_handle_StandardInterfaceRequest(BDentry*);
void usb_handle_StandardEndpointRequest(BDentry*);
//void usb_handle_ClassRequest( void );
//void usb_handle_VendorRequest( void );
void usb_RequestError(void);

void usb_ack_dat1(BDentry *rbdp, int bdcnt); // JTR added
void usb_set_address(void);
void usb_send_rom(void);
void user_configured_init(void); //JTR added.Non EP0 endpoints are arm after USB device enters CONFIGURED STATE. This is a CALLBACK function.
BYTE FAST_usb_handler(void); // JTR2 added fast USB service. Pops non EP0 transfer flags from FIFO. Sevices RESET and SETUP
/*DOM-IGNORE-BEGIN*/

/*DOM-IGNORE-END*/

void usb_init(ROMPTR const unsigned char *device_descriptor,
        ROMPTR const unsigned char *config_descriptor,
        ROMPTR const unsigned char *string_descriptor,
        int num_string_descriptors) {
    int i;


    usb_device_descriptor = device_descriptor;
    usb_config_descriptor = config_descriptor;
    usb_string_descriptor = string_descriptor;
    usb_num_string_descriptors = num_string_descriptors;

    SetUsbAddress(0); // JTR added here. Probably not required though

    ResetPPbuffers();

    DisableUsbInterrupts();
    DisableAllUsbInterrupts();
    //	EnableAllUsbErrorInterrupts();				// Enable all errors to set flag in UIR

    ClearAllUsbErrorInterruptFlags();
    ClearAllUsbInterruptFlags();

    ConfigureUsbHardware();

    sof_handler = NULL;
    class_setup_handler = NULL;
    vendor_setup_handler = NULL;

    for (i = 0; i < MAX_CHIP_EP; i++) {

        //		endpoints[i].type = 0;              // JTR Not in use
        //		endpoints[i].buffer_size = 0;
        //		endpoints[i].out_buffer = NULL;
        //		endpoints[i].in_buffer = NULL;      //  ""  ""
        endpoints[i].out_handler = NULL;
        endpoints[i].in_handler = NULL;
    }
    // JTR not in use. Only IN and OUT handlers registered below.
    //usb_register_endpoint(0, USB_EP_CONTROL, USB_EP0_BUFFER_SIZE, usb_ep0_out_buf, usb_ep0_in_buf, NULL, NULL);		// Register ep0 - no handlers

    usb_unset_in_handler(0);
    usb_unset_out_handler(0);

    // JTR All UEPx SPRs are hard coded. I cannot see much point of tables or indexing for these.
    // On the PIC16C765 such indexing was required for the STALL bit but now the STALL feature
    // is in the Buffer descriptor table not in the UEPx SPRs.
    // See changes to "USB_REQUEST_GET_STATUS"

    //	USB_UEP[0] = endpoints[0].type;

    USB_UEP0 = USB_EP_CONTROL;
    /* Configure endpoints TODO: Only ep0 ? */
    // JTR Right! code for other end points snipped...At this point we are only setting up EP0

#ifdef USB_SELF_POWERED

    // JTR TODO this isn't actually 100% correct. "usb_device_status" is a runtime variable
    // In the case of a bus powered device it will always be 0x000 but in the case of
    // self powered with bus powered option it becames variable to the current powered
    // State. This is a minor thing and for now but it may need to be addressed if there is
    // any hardware that is dual powered.

    usb_device_status = 0x0001;
#else
    usb_device_status = 0x0000;
#endif
    usb_device_state = DETACHED_STATE; // JTR added flag byte for enumeration state
    usb_current_cfg = 0; // JTR formally usb_configured
    usb_addr_pending = 0x00;

#if USB_PP_BUF_MODE == 0
    usb_bdt[USB_CALC_BD(0, USB_DIR_OUT, USB_PP_EVEN)].BDCNT = USB_EP0_BUFFER_SIZE; // JTR endpoints[0].buffer_size; same thing done more obviously
    usb_bdt[USB_CALC_BD(0, USB_DIR_OUT, USB_PP_EVEN)].BDADDR = usb_ep0_out_buf; //endpoints[0].out_buffer;
    usb_bdt[USB_CALC_BD(0, USB_DIR_OUT, USB_PP_EVEN)].BDSTAT = UOWN + DTSEN;
    usb_bdt[USB_CALC_BD(0, USB_DIR_IN, USB_PP_EVEN)].BDCNT = 0;
    usb_bdt[USB_CALC_BD(0, USB_DIR_IN, USB_PP_EVEN)].BDADDR = usb_ep0_in_buf; //endpoints[0].in_buffer;
    usb_bdt[USB_CALC_BD(0, USB_DIR_IN, USB_PP_EVEN)].BDSTAT = DTS; // Set DTS => First packet inverts, ie. is Data0
#else
    // TODO: Implement Ping-Pong buffering setup.
#error "PP Mode not implemented yet"
#endif
}

void usb_start(void) {
    EnableUsb(); // Enable USB-hardware
    usb_device_state = ATTACHED_STATE;
    while (SingleEndedZeroIsSet()); // Busywait for initial power-up
    usb_device_state = DEFAULT_STATE; //JTR2

    // JTR USB_ACTIV should be enabled in the SUSPEND mode handler.
    // JTR debugging	EnableUsbInterrupt(USB_STALL + USB_IDLE + USB_TRN + USB_ACTIV + USB_SOF + USB_UERR + USB_URST);

    	//EnableUsbInterrupt(USB_TRN + USB_SOF + USB_UERR + USB_URST);
    	//EnableUsbInterrupts();
    // JTR PIC18 global and peripherial enable.
    // TODO Add conditional compile here for priority modes etc.
    //	EnableUSBHighInterrupts();
}

void USBDeviceTasks() {
    usb_handler();
}


void usb_handler(void) {
	if (USB_RESET_FLAG) {
		usb_handle_reset();
		ClearUsbInterruptFlag(USB_URST);
	} //else {
	if (USB_ERROR_FLAG) {
		ClearAllUsbErrorInterruptFlags();
		ClearUsbInterruptFlag(USB_UERR);
	}
	// process all pending transactions // JTR What is the point of this loop when the USB is running on interrupts?
	// No, this causes all sorts of problems if USB interrupts are enabled.
	/*while */ 
	if (USB_TRANSACTION_FLAG) {
		usb_handle_transaction();
		ClearUsbInterruptFlag(USB_TRN); // JTR Missing! This is why Ian was only getting one interrupt??
	} // Side effect: advance USTAT Fifo
	
	/* N/A
	if (USB_RESUME_FLAG) {
	// Activity - unsuspend
	//WakeupUsb();
	ClearUsbInterruptFlag(USB_RESUM);
	}
	*/
	/* N/A
	if (USB_IDLE_FLAG) {
	// Idle - suspend
	//SuspendUsb();
	//usb_low_power_request();
	ClearUsbInterruptFlag(USB_IDLE);
	}
	*/
	/* N/A
	if (USB_STALL_FLAG) {
	// Stall detected
	// * Not sure when this interrupt fires
	// * as the host never should send out a stall.
	// * Perhaps as a respons to our own stalls?
	// * For now just ignore it.
	ClearUsbInterruptFlag(USB_STALL);
	}
	*/
	
	if (USB_SOF_FLAG) {
		// Start-of-frame
		if (sof_handler) sof_handler();
		ClearUsbInterruptFlag(USB_SOF);
	}

}

#if 0
void usb_handler(void) {
    if (USB_ERROR_FLAG) {
        usb_handle_error();
        ClearUsbInterruptFlag(USB_UERR);
    } else if (USB_RESET_FLAG) {
        usb_handle_reset();
        ClearUsbInterruptFlag(USB_URST);
    } else if (USB_ACTIVITY_FLAG) {
        /* Activity - unsuspend */
        WakeupUsb();
        ClearUsbInterruptFlag(USB_ACTIV);
    } else if (USB_IDLE_FLAG) {
        /* Idle - suspend */
        //SuspendUsb();
        //usb_low_power_request();
        ClearUsbInterruptFlag(USB_IDLE);
    } else if (USB_STALL_FLAG) {
        /* Stall detected
         * Not sure when this interrupt fires
         * as the host never should send out a stall.
         * Perhaps as a respons to our own stalls?
         * For now just ignore it. */
        ClearUsbInterruptFlag(USB_STALL);
    } else if (USB_SOF_FLAG) {
        /* Start-of-frame */
        //		if (sof_handler) sof_handler();
        {
            ClearUsbInterruptFlag(USB_SOF);
        }

    } else {
        //		process all pending transactions  // JTR  What is the point of this loop when the USB is running on interrupts?
        while (USB_TRANSACTION_FLAG) {
            usb_handle_transaction();
            ClearUsbInterruptFlag(USB_TRN); // JTR Missing! This is why Ian was only getting one interrupt??
        } // Side effect: advance USTAT Fifo
    }
}
#endif
void usb_handle_error(void) {
    /* No errorhandler for now, just clear offending flag*/
    ClearAllUsbErrorInterruptFlags();
}

void usb_handle_reset(void) {
    int i;

    // JTR see note about this in usb_init()
#ifdef USB_SELF_POWERED
    usb_device_status = 0x0001;
#else
    usb_device_status = 0x0000;
#endif

    do {
        ClearUsbInterruptFlag(USB_TRN); // JTR corrected Must poll TRN Flag and clear, then wait 6 cycles. for next flag set.
        usb_current_cfg = 0;
        usb_device_state = DEFAULT_STATE; // This BLOCK creates the requied 6 cycle delay for TRNF to reassert.
        usb_addr_pending = 0x00;
    } while (USB_TRANSACTION_FLAG);


    //        for (i=0; i < MAX_CHIP_EP; i++ )      // JTR this loop seems to work proving that the USB_UEP ptr is now correct.
    //        {
    //            USB_UEP[i] = 0;                   // JTR Char on PIC18 and int ptr on PIC24
    //        }
    USB_UEP0 = 0; //  Disable all endpoints
    USB_UEP1 = 0;
    USB_UEP2 = 0;
    USB_UEP3 = 0;
    USB_UEP4 = 0;
    USB_UEP5 = 0;
    USB_UEP6 = 0;
    USB_UEP7 = 0;
#if MAX_CHIP_EP > 8 // JTR alteration
    USB_UEP8 = 0;
    USB_UEP9 = 0;
    USB_UEP10 = 0;
    USB_UEP11 = 0;
    USB_UEP12 = 0;
    USB_UEP13 = 0;
    USB_UEP14 = 0;
    USB_UEP15 = 0;

#endif
    SetUsbAddress(0); // After reset we don't have an address
    ClearAllUsbInterruptFlags();
    ClearAllUsbErrorInterruptFlags();

    // JTR added. Clear all BD STAT registers so that they are owned by the CPU and it is safe to change them.

    for (i = 0; i < (2 + 2 * MAX_EPNUM_USED); i += 4) {
        usb_bdt[i].BDSTAT = 0;
    }


    USB_UEP0 = USB_EP_CONTROL;

    /* Configure buffer descriptors */

#if USB_PP_BUF_MODE == 0
    usb_bdt[USB_CALC_BD(0, USB_DIR_OUT, USB_PP_EVEN)].BDCNT = USB_EP0_BUFFER_SIZE; //endpoints[0].buffer_size;
    usb_bdt[USB_CALC_BD(0, USB_DIR_OUT, USB_PP_EVEN)].BDADDR = usb_ep0_out_buf; //endpoints[0].out_buffer;
    usb_bdt[USB_CALC_BD(0, USB_DIR_OUT, USB_PP_EVEN)].BDSTAT = UOWN + DTSEN;
    usb_bdt[USB_CALC_BD(0, USB_DIR_IN, USB_PP_EVEN)].BDCNT = 0;
    usb_bdt[USB_CALC_BD(0, USB_DIR_IN, USB_PP_EVEN)].BDADDR = usb_ep0_in_buf; //endpoints[0].in_buffer;
    usb_bdt[USB_CALC_BD(0, USB_DIR_IN, USB_PP_EVEN)].BDSTAT = DTS; // Set DTS => First packet inverts, ie. is Data0

#else
    // TODO: Implement Ping-Pong buffering setup.
#error "PP Mode not implemented yet"
#endif

    EnableAllUsbErrorInterrupts();
    EnablePacketTransfer();
}

void usb_handle_transaction(void) {

    trn_status = GetUsbTransaction();
    bdp = &usb_bdt[USB_USTAT2BD(trn_status)];
    // JTR PIC24 fixup Added DIRBIT as it is different for PIC24F ( and different again for the PIC24E/dsPIC33E)
    // the USB_USTAT2BD macro is also altered to shift the result the correct number of places.

    rbdp = &usb_bdt[USB_USTAT2BD(trn_status | DIRBIT)]; // All replies in IN direction

    switch (bdp->BDSTAT & USB_TOKEN_Mask) {
        case USB_TOKEN_SETUP:
            usb_handle_setup();
            break;
        case USB_TOKEN_OUT:
            usb_handle_out();
            break;
        case USB_TOKEN_IN:
            usb_handle_in();
            break;
            //default:
            /* Default case of unknown TOKEN - discard */
    }
}

void usb_handle_setup(void) {
    rbdp->BDSTAT = DTSEN; // Reclaim reply buffer

    EnablePacketTransfer();
    switch (bdp->BDADDR[USB_bmRequestType] & USB_bmRequestType_TypeMask) {
        case USB_bmRequestType_Standard:
            switch (bdp->BDADDR[USB_bmRequestType] & USB_bmRequestType_RecipientMask) {
                case USB_bmRequestType_Device:
                    usb_handle_StandardDeviceRequest(bdp);
                    break;
                case USB_bmRequestType_Interface:
                    usb_handle_StandardInterfaceRequest(bdp);
                    break;
                case USB_bmRequestType_Endpoint:
                    usb_handle_StandardEndpointRequest(bdp);
                    break;
                default:
                    usb_RequestError();
            }
            break;
        case USB_bmRequestType_Class:
            if (class_setup_handler) class_setup_handler();
            break;
        case USB_bmRequestType_Vendor:
            if (vendor_setup_handler) class_setup_handler();
            break;
        default:
            usb_RequestError();
    }
    /* Prepare endpoint for new reception */

    bdp->BDCNT = USB_EP0_BUFFER_SIZE; // Size  of EP0, should always be ep0? (JTR in practice YES)

    // JTR, is the next OUT transfer to be a setup packet (DAT0) or a DATA packet (DAT1)?
    // note that this is not an entirely robust way of doing things. See the microchip stack for
    // further comments and a better system as this does not account for errors and retries.

    // JTR Breakdown of what is happening here FYI.

    //bdp->BDSTAT = (!(bdp->BDADDR[USB_bmRequestType] & USB_bmRequestType_PhaseMask) &&
    // JTR meaning. If transfer was a CONTROL OUT then B7 (USB_bmRequestType_PhaseMask) of bmRequestType is '0'

    // &&

    //bdp->BDADDR[USB_wLength] || bdp->BDADDR[USB_wLengthHigh]))? UOWN + DTS + DTSEN : UOWN + DTSEN;
    // If there is a CONTROL OUT DATA PACKET to follow then its count in (int) wLength will be != 0
    // When both conditions are true then set for DAT1 (UOWN + DTS + DTSEN)
    // else set for DAT0 (UOWN + DTSEN)

    // See USB 2.0 spec 8.5.3

    bdp->BDSTAT = (!(bdp->BDADDR[USB_bmRequestType] & USB_bmRequestType_PhaseMask) &&
            (bdp->BDADDR[USB_wLength] || bdp->BDADDR[USB_wLengthHigh])) ? UOWN + DTS + DTSEN : UOWN + DTSEN;

    // JTR Note. For rbdp after a setup packet, the standard or class request handler will force the correct DTS state
    // JTR Note. that CONTROL IN and OUT DATA packet transfers do not come back here and there is no
    // univesal way and place of setting up EP0 after these DATA transfers in this stack.


    EnablePacketTransfer();
}

// JTR comment on the standard device requests handler.
// This does not address that some requests need to be
// handled differently depending on what USB defined
// "STATE" of enumeration we are at. In fact this stack
// does not even track any of the different states.
// except that I have added basic tracking to determine
// if the device is configured or not. This is a minimum.
// Later the stack can be improved.

void usb_handle_StandardDeviceRequest(BDentry *bdp) {
    unsigned char *packet = bdp->BDADDR;
    int i;

    switch (packet[USB_bRequest]) {
        case USB_REQUEST_GET_STATUS:
            rbdp->BDADDR[0] = usb_device_status & 0xFF;
            rbdp->BDADDR[1] = usb_device_status >> 8;

            // JTR I added usb_ack_dat1() simply so that the handling
            // of STANDARD and CLASS requests (in cdc.c) were consistant
            // rather than have the same thing done in two different ways.

            usb_ack_dat1(rbdp, 2); // JTR common addition for STD and CLASS ACK
            //		rbdp->BDCNT = 2;
            //		rbdp->BDSTAT = UOWN + DTS + DTSEN;
            break;
        case USB_REQUEST_CLEAR_FEATURE:
            if (0x01u == packet[USB_wValue]) { // TODO: Remove magic (REMOTE_WAKEUP_FEATURE)
                usb_device_status &= ~0x0002;
                usb_ack_dat1(rbdp, 0); // JTR common addition for STD and CLASS ACK
                //			rbdp->BDCNT = 0;
                //			rbdp->BDSTAT = UOWN + DTS + DTSEN;
            } else
                usb_RequestError();
            break;
        case USB_REQUEST_SET_FEATURE:
            if (0x01u == packet[USB_wValue]) { // TODO: Remove magic (REMOTE_WAKEUP_FEATURE)
                usb_device_status |= 0x0002;
                usb_ack_dat1(rbdp, 0); // JTR common addition for STD and CLASS ACK
                //			rbdp->BDCNT = 0;
                //			rbdp->BDSTAT = UOWN + DTS + DTSEN;
            } else
                usb_RequestError();
            break;
        case USB_REQUEST_SET_ADDRESS:
            if (0x00u == packet[USB_wValueHigh] && 0x7Fu >= packet[USB_wValue]) {
                usb_addr_pending = packet[USB_wValue];
                usb_ack_dat1(rbdp, 0); // JTR common addition for STD and CLASS ACK
                //			rbdp->BDCNT = 0;
                //			rbdp->BDSTAT = UOWN + DTS + DTSEN;
                usb_set_in_handler(0, usb_set_address);
            } else
                usb_RequestError();
            break;


        case USB_REQUEST_GET_DESCRIPTOR:
            switch (packet[USB_bDescriptorType]) {
                case USB_DEVICE_DESCRIPTOR_TYPE:
                    usb_rom_ptr = usb_device_descriptor;
                    usb_rom_len = usb_device_descriptor[0];
                    if ((0 == packet[USB_wLengthHigh] && packet[USB_wLength] < usb_rom_ptr[0]))
                        usb_rom_len = packet[USB_wLength];
                    break;
                case USB_CONFIGURATION_DESCRIPTOR_TYPE:
                    if (packet[USB_bDescriptorIndex] >= usb_device_descriptor[17]) // TODO: remove magic
                        usb_RequestError();
                    usb_rom_ptr = usb_config_descriptor;
                    usb_rom_len = usb_rom_ptr[2] + usb_rom_ptr[3] * 256;
                    for (i = 0; i < packet[USB_bDescriptorIndex]; i++) { // Implicit linked list traversal until requested configuration
                        usb_rom_ptr += usb_rom_len;
                        usb_rom_len = usb_rom_ptr[2] + usb_rom_ptr[3] * 256;
                    }
                    if ((packet[USB_wLengthHigh] < usb_rom_ptr[3]) ||
                            (packet[USB_wLengthHigh] == usb_rom_ptr[3] && packet[USB_wLength] < usb_rom_ptr[2]))
                        usb_rom_len = packet[USB_wLength] + packet[USB_wLengthHigh] * 256;
                    break;
                case USB_STRING_DESCRIPTOR_TYPE:
                    // TODO: Handle language request. For now return standard language.
                    if (packet[USB_bDescriptorIndex] >= usb_num_string_descriptors)
                        usb_RequestError();
                    usb_rom_ptr = usb_string_descriptor;
                    usb_rom_len = usb_rom_ptr[0];
                    for (i = 0; i < packet[USB_bDescriptorIndex]; i++) { // Implicit linked list traversal until requested configuration
                        usb_rom_ptr += usb_rom_len;
                        usb_rom_len = usb_rom_ptr[0];
                    }
                    if ((0 == packet[USB_wLengthHigh] && packet[USB_wLength] < usb_rom_ptr[0]))
                        usb_rom_len = packet[USB_wLength];
                    break;
                case USB_INTERFACE_DESCRIPTOR_TYPE:
                case USB_ENDPOINT_DESCRIPTOR_TYPE:
                default:
                    usb_RequestError();
            }
            usb_send_rom(); // Send first part of packet right away
            usb_set_in_handler(0, usb_send_rom);
            break;
        case USB_REQUEST_GET_CONFIGURATION:
            rbdp->BDADDR[0] = usb_current_cfg;
            usb_ack_dat1(rbdp, 1); // JTR common addition for STD and CLASS ACK
            break;

        case USB_REQUEST_SET_CONFIGURATION:
            if (USB_NUM_CONFIGURATIONS >= packet[USB_wValue]) {
                // TODO: Support multiple configurations
                /* Configure endpoints (USB_UEPn - registers) */
                usb_current_cfg = packet[USB_wValue];
                if (usb_current_cfg != 0) {

                    // JTR user_configured_init major addition. This is a CALLBACK to the USER when the device is enumerated.
                    // This is when we setup non EP0 endpoints.
                    // TODO: This really could be a function pointer

                    usb_device_state = CONFIGURED_STATE;
                    user_configured_init();
                } else {
                    usb_device_state = ADDRESS_STATE;
                }

                usb_ack_dat1(rbdp, 0); // JTR common addition for STD and CLASS ACK

            } else
                usb_RequestError();
            break;

        case USB_REQUEST_SET_DESCRIPTOR:
        default:
            usb_RequestError();
    }
}

void usb_handle_StandardInterfaceRequest(BDentry *bdp) {
    unsigned char *packet = bdp->BDADDR;

    switch (packet[USB_bRequest]) {
        case USB_REQUEST_GET_STATUS:
            rbdp->BDADDR[0] = 0x00;
            rbdp->BDADDR[1] = 0x00;
            usb_ack_dat1(rbdp, 2); // JTR common addition for STD and CLASS ACK
            break;
        case USB_REQUEST_GET_INTERFACE:
            if (USB_NUM_INTERFACES > packet[USB_bInterface]) {
                // TODO: Implement alternative interfaces, or move responsibility to class/vendor functions.
                rbdp->BDADDR[0] = 0;
                usb_ack_dat1(rbdp, 1); // JTR common addition for STD and CLASS ACK
            } else
                usb_RequestError();
            break;
        case USB_REQUEST_SET_INTERFACE:
            if (USB_NUM_INTERFACES > packet[USB_bInterface] && 0u == packet[USB_wValue]) {
                // TODO: Implement alternative interfaces...
                usb_ack_dat1(rbdp, 0); // JTR common addition for STD and CLASS ACK
            } else
                usb_RequestError();
            break;
        case USB_REQUEST_CLEAR_FEATURE: // JTR N/A for interface
        case USB_REQUEST_SET_FEATURE: // This is correct and finished code.
        default:
            usb_RequestError();
    }
}

void usb_handle_StandardEndpointRequest(BDentry *bdp) {
    unsigned char *packet;
    unsigned char epnum;
    unsigned char dir;
    BDentry *epbd;

    packet = bdp->BDADDR;

    switch (packet[USB_bRequest]) {
        case USB_REQUEST_GET_STATUS:

            // JTR this code block is not correct. It is a throw back to the 16C765
            // stack where the STALL status was B0 in the UEPx. As it is the EPSTALL
            // bit is NOT B0 on the PIC24 so this code was not correct for this family.
            // Instead we will use the STALL bits in the BD table.

            // JTR PIC24 fixup (albeit not a fatal one)

            //		epnum = packet[USB_wIndex] & 0x0F;
            //		dir = packet[USB_wIndex] >> 7;
            //		rbdp->BDADDR[0] = USB_UEP[epnum] & USB_UEP_EPSTALL;
            //		rbdp->BDADDR[1] = 0x00;
            //		rbdp->BDCNT = 2;
            //		rbdp->BDSTAT = UOWN + DTS + DTSEN;
            //		break;


            // JTR new USB_REQUEST_GET_STATUS block

            rbdp->BDADDR[0] = 0x00; // Assume no stall
            rbdp->BDADDR[1] = 0x00; // Same for stall or not
            epnum = packet[USB_wIndex] & 0x0F;
            dir = packet[USB_wIndex] >> 7;
            epbd = &usb_bdt[USB_CALC_BD(epnum, dir, USB_PP_EVEN)];
            if (epbd->BDSTAT &= ~BSTALL)
                rbdp->BDADDR[0] = 0x01; // EVEN BD is stall flag set?
            //epbd = &usb_bdt[USB_CALC_BD(epnum, dir, USB_PP_ODD)];
            //if (epbd->BDSTAT &= ~BSTALL)
            //    rbdp->BDADDR[0] = 0x01; // ODD BD is stall flag set?
            usb_ack_dat1(rbdp, 2); // JTR common addition for STD and CLASS ACK
            break;

        case USB_REQUEST_CLEAR_FEATURE:
            // JTR This block was not complete. The CLEAR ENDPOINT FEATURE
            // must also reset the data toggling. This is a real PAIN
            // when ping-pong is enabled. It was not until version 2.8
            // of their stack that microchip got it right.

            // As it is this really is an application event and there
            // should be a call back and protocol for handling the
            // possible lost of a data packet.

            // TODO: ping-ping support.

            epnum = packet[USB_wIndex] & 0x0F;
            dir = packet[USB_wIndex] >> 7;
            epbd = &usb_bdt[USB_CALC_BD(epnum, dir, USB_PP_EVEN)];
            epbd->BDSTAT &= ~BSTALL;
            if (dir) epbd->BDSTAT |= DTS; // JTR added IN EP set DTS as it will be toggled to zero next transfer
            if (0 == dir) epbd->BDSTAT &= ~DTS; // JTR added

            // JTR this pointless ATM. If ping-pong is enabled then you need to track PPBI
            // and set up ODD and EVEN BDs in respect to this. See complicated system in
            // microchip stack >= 2.8

            //		epbd = &usb_bdt[USB_CALC_BD(epnum, dir, USB_PP_ODD)];
            //		epbd->BDSTAT &= ~BSTALL;
            //		if (dir) epbd->BDSTAT |= DTS;		// JTR added
            //		if (0 == dir) epbd->BDSTAT &= ~DTS;	// JTR added


            usb_ack_dat1(rbdp, 0); // JTR common addition for STD and CLASS ACK
            break;


        case USB_REQUEST_SET_FEATURE:
            epnum = packet[USB_wIndex] & 0x0F;
            dir = packet[USB_wIndex] >> 7;
            epbd = &usb_bdt[USB_CALC_BD(epnum, dir, USB_PP_EVEN)];
            epbd->BDSTAT |= BSTALL;
            //epbd = &usb_bdt[USB_CALC_BD(epnum, dir, USB_PP_ODD)];
            //epbd->BDSTAT |= BSTALL;
            usb_ack_dat1(rbdp, 0); // JTR common addition for STD and CLASS ACK
            break;
        case USB_REQUEST_SYNCH_FRAME:
        default:
            usb_RequestError();
    }
}

/*
void usb_handle_ClassRequest( void ) {
        class_setup(); // TODO: Refactor
}

void usb_handle_VendorRequest( void ) {
}
//
 */
//

// JTR2 addition.
// This is a minimal USB stack handler. It only services
// USB RESET and CONTROL transfers on EP0. Otherwise it
// simply clears the TRF flag to advance the FIFO leaving
// user apps to control the non-EP0 transfers.
// The purpose of this function is to allow for faster,
// lower overhead of APP level USB transfers. It can be
// thought of as a basic housekeeper and only designed for
// use when there is a specific need for fast USB transfers
// at crittical times. The main loop of the user APP still
// should call USBDeviceTasks() for full USB stack service.

// If no action other than clearing the TRF flag to advance
// the FIFO then the return value is 0 otherwise if something
// intervened it return 0xFF to alert the user app to abort
// the current transactions.

BYTE FAST_usb_handler(void) {
  //  if (TestGlobalUsbInterruptFlag()) {
        if (USB_RESET_FLAG) {
            usb_handle_reset();
            ClearUsbInterruptFlag(USB_URST);
            return 0xFF;
        }
        if (USB_TRANSACTION_FLAG) {
            trn_status = GetUsbTransaction();
            if (USB_STAT2EP(trn_status)) {
                usb_handle_transaction();
                ClearUsbInterruptFlag(USB_TRN); // non-EP0 only
                return 0x0;
            } else {
                ClearUsbInterruptFlag(USB_TRN); // non-EP0 only
                return 0;
            }
       // }
    //   ClearGlobalUsbInterruptFlag();
    }
	return 0;
}

void usb_handle_in(void) {
    if (endpoints[USB_STAT2EP(trn_status)].in_handler) {
        endpoints[USB_STAT2EP(trn_status)].in_handler();
    }
}

void usb_handle_out(void) {
    //rbdp->BDSTAT &= ~UOWN;            // Reclaim reply buffer
    if (endpoints[USB_STAT2EP(trn_status)].out_handler) {
        endpoints[USB_STAT2EP(trn_status)].out_handler();
    }
}

void usb_register_sof_handler(usb_handler_t handler) {
    sof_handler = handler;
}

void usb_register_class_setup_handler(usb_handler_t handler) {
    class_setup_handler = handler;
}

void usb_register_vendor_setup_handler(usb_handler_t handler) {
    vendor_setup_handler = handler;
}

// JTR removed this. Cannot see the point of it at this time.
//void usb_register_endpoint(	unsigned int ep, usb_uep_t type, unsigned int buf_size,
//							unsigned char *out_buf, unsigned char *in_buf,
//							usb_handler_t out_handler, usb_handler_t in_handler ) {
// JTR we are not using these. Nowhere in the stack are they actually useful
//	endpoints[ep].type		= type;
//	endpoints[ep].buffer_size	= buf_size;
//	endpoints[ep].out_buffer	= out_buf;
//	endpoints[ep].in_buffer		= in_buf;

//    endpoints[ep].out_handler	= out_handler;
//	endpoints[ep].in_handler = in_handler;
//}

void usb_set_in_handler(int ep, usb_handler_t in_handler) {
    endpoints[ep].in_handler = in_handler;
}

void usb_set_out_handler(int ep, usb_handler_t out_handler) {
    endpoints[ep].out_handler = out_handler;
}


// JTR not actually using these two helper functions below
// but they are still useful and could be put back to use
// as they aught to result in smaller code. For now I am
// using the usb_ack_dat1() as it more correctly forces a
// DAT1 transfer and passes a varible for the transfer count.

void usb_ack(BDentry * bd) {
    bd->BDSTAT = ((bd->BDSTAT ^ DTS) & DTS) | UOWN | DTSEN; // TODO: Accomodate for >=256 byte buffers
}

void usb_ack_zero(BDentry * bd) {
    bd->BDCNT = 0;
    bd->BDSTAT = ((bd->BDSTAT ^ DTS) & DTS) | UOWN | DTSEN;

}

// JTR New added helper function use extensively by
// the standard and class request handlers. All status
// IN packets are DAT1 as is the first DATA packet of
// a IN transfer. Currently with this CDC stack the
// only IN DATA transfers that are > 8 bytes is the
// descriptor transfer and these are transfered in
// usb_send_rom()

void usb_ack_dat1(BDentry *rbdp, int bdcnt) {
    rbdp->BDCNT = (bdcnt & 0xFF);
    rbdp->BDSTAT = (DTS | UOWN | DTSEN); // | ((bdcnt & 0x300) >> 8));
}

void usb_RequestError(void) {

    usb_bdt[USB_CALC_BD(0, USB_DIR_OUT, USB_PP_EVEN)].BDCNT = USB_EP0_BUFFER_SIZE;
    //usb_bdt[USB_CALC_BD(0, USB_DIR_OUT, USB_PP_ODD)].BDCNT = USB_EP0_BUFFER_SIZE;

    usb_bdt[USB_CALC_BD(0, USB_DIR_IN, USB_PP_EVEN)].BDSTAT = UOWN + BSTALL;
    usb_bdt[USB_CALC_BD(0, USB_DIR_OUT, USB_PP_EVEN)].BDSTAT = UOWN + BSTALL;
    //usb_bdt[USB_CALC_BD(0, USB_DIR_IN, USB_PP_ODD)].BDSTAT = UOWN + BSTALL;
    //usb_bdt[USB_CALC_BD(0, USB_DIR_OUT, USB_PP_ODD)].BDSTAT = UOWN + BSTALL;

    // JTR TODO: Should also kill the IN and OUT handlers?

}

void usb_set_address(void) {
    if (0x00u == usb_addr_pending) {
        usb_device_state = DEFAULT_STATE;
    } else {
        usb_device_state = ADDRESS_STATE;
    }
    SetUsbAddress(usb_addr_pending);
    usb_addr_pending = 0xFF;
    usb_unset_in_handler(0); // Unregister handler
}

void usb_send_rom(void) {

    unsigned int i;
    size_t packet_len;
    if (usb_rom_len) {
        packet_len = (usb_rom_len < USB_EP0_BUFFER_SIZE) ? usb_rom_len : USB_EP0_BUFFER_SIZE; // JTR changed from MAX_BUFFER_SIZE

        for (i = 0; i < packet_len; i++) {
            rbdp->BDADDR[i] = usb_rom_ptr[i];
        }
    } else {
        packet_len = 0;
        usb_unset_in_handler(0);
    }

    rbdp->BDCNT = (unsigned char) packet_len;
    rbdp->BDSTAT = ((rbdp->BDSTAT ^ DTS) & DTS) | UOWN | DTSEN; // Packet length always less then 256 on endpoint 0

    usb_rom_ptr += packet_len;
    usb_rom_len -= packet_len;
}

#endif

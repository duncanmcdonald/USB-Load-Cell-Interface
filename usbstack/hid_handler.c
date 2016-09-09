/** @file hid_handler.c
*
* Copyright (c) 2013 Solid-Motion
*
* @brief Handles USB class specific HID requests
*
*/

#include "stdafx.h"
#include "usb_stack.h"
#include "usb_stack_std.h"
#include "hid_handler.h"

U16 wInterface;

U8 process_user_set_configuration_request(U8 descriptorType) //this function used
	{
	Usb_ack_receive_setup();
	Usb_send_control_in();

	while(!Is_usb_receive_out());
	Usb_ack_receive_out();
	Usb_send_control_in();
   
	return 1;
	}

U8 process_user_get_interface_request() //this function used
	{
	U8 dummy;
	dummy = Usb_read_byte();
	dummy = Usb_read_byte();
	LSB(wInterface)=Usb_read_byte();
	MSB(wInterface)=Usb_read_byte();
   
	Usb_ack_receive_setup();
   
	Usb_send_control_in();                       // send a ZLP for STATUS phase
	while(!Is_usb_in_ready());
	return 1;
	}
	
	U8 set_report_REPORT_OUT_CMD_DEVICE_RESET(void)
{
	usb_stack_read_control_write(0, 0); // Complete sequence for Control Write Transfer; params: global_teds.teds_data, pageNumber, length
	usb_stack_ack_control_write(1); // Partial sequence for Control Read Transfer;  I don't understand why this sequence is needed, but it is.
									// Calling usb_stack_ack_control_write(U8 success) should be preceded by Usb_ack_receive_setup()
									
	// Enable the watchdog and force a timeout to reset the AVR. 
	wdt_enable(WDTO_250MS); //avr/wdt.h
	
	return 1;
}

/*The application must provide this entry point.  It is for non-standard (e.g Class)
 * control request processing.  Return true if handled.  False otherwise.
 */
U8 usb_stack_user_process_request(U8 requestType, U8 request) 
	{
	U16 wValue, wIndex, wLength;
	U8 requestCategory;
   
	LSB(wValue)=Usb_read_byte();
	MSB(wValue)=Usb_read_byte();
   
	LSB(wIndex)=Usb_read_byte();
	MSB(wIndex)=Usb_read_byte();
   
	LSB(wLength)=Usb_read_byte();
	MSB(wLength)=Usb_read_byte();
   
	requestCategory=(requestType>>5) & 0x3; //Filter for the Type portion of bmRequestType
   
	if (requestCategory==0x1) //0x01 = Class; does this section of code ever get hit?
		{
		// Class specific (HID) request
		switch (request)
			{
			case 0x1:// HID Get Report 
				if (usb_stack_hid_user_get_report(MSB(wValue), LSB(wValue), wLength)) 
					{
					return 1;   // Handled
					}
			return 0;   // Not handled
			break;
         
			case 0x9:// HID Set_Report
				
				if (usb_stack_hid_user_set_report(MSB(wValue), LSB(wValue), wLength)) 
					{
					return 1;	// Handled
					}
			return 0;   // Not handled
			break;
			case 0x0A:// HID Set_IDLE
				
				//if (MSB(wValue) == 0) // Duration is zero  so we can ignore it
					{
					return 1;	// Handled
					}
			break;
			
			}
		} else if (requestCategory==0x0) // Standard USB request. 
										 // See Device Class Definition for Human Interface Devices (HID) 5.1
		{

		switch (request) 
			{
			case SET_CONFIGURATION: // See Device Class Definition for Human Interface Devices (HID) 5.1
				return process_user_set_configuration_request(MSB(wValue));  // set the configuration; this function used
			break;
         
			case  GET_INTERFACE: // See Device Class Definition for Human Interface Devices (HID) 5.1
								// The Class field of the Interface Descriptor defines the device as a HID class device
				return process_user_get_interface_request();	// get the interface; this function used
			break;
			default:
			return 0; // Not handled, e.g. SYNCH FRAME
			break;
			}
		}
   
	return 0;
	}



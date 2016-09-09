//! @file usb_stack.c
//!
//! @brief Low level stuff for USB stack service
//!
//! Copyright (c) 2013 Solid-Motion

#include "stdafx.h"
#include "usb_stack.h"
#include "usb_stack_std.h"

bit usb_stack_connected;

/* global event flag used for interfacing with ISR originated
 * events.  See macros at the top of usb_stack.h
 */
volatile U16 usb_stack_event;

static void usb_stack_start_device();

/*Initialize the mcu usb controller in device mode
 */
void usb_stack_init_device() 
	{
	// Detach USB
	UDCON=1;
   
	//usbstack_debug_write_string("UINIT");
	//usbstack_debug_write_hex(UDCON);
	//usbstack_debug_write_nl();

	// Comment out to disable regulator
	Usb_enable_regulator();
   
	// Setup usb as device
	Usb_force_device_mode();
   
	// Enable the controller
   
	Disable_interrupt();
	Usb_disable();
	Usb_enable();
	Usb_select_device();
	Usb_enable_vbus_interrupt();
	Enable_interrupt();
   
	//usbstack_debug_write_string("UE");
	//usbstack_debug_write_hex(UDCON);
	//usbstack_debug_write_hex(USBCON);
	//usbstack_debug_write_nl();
   
	//LOG('*');
	}

/*Main service function.  Call repeatedly.
 */
void usb_stack_service()
	{
	if (Is_usb_vbus_high() && usb_stack_connected==FALSE)
		{
		// ?? Missed an interrupt.  See isr for mirror code
		usb_stack_connected=TRUE;
		usb_stack_action_vbus_on();
		usb_stack_start_device();
      
		//usbstack_debug_write_string("CT!");
		}
		//! @brief Low level initialization of USB; Wake up device, reset endpoint o, etc.
   
		// Handle reset event from ISR
	if (usb_stack_is_usb_event(EVT_USB_RESET)) 
		{
		//usbstack_debug_write_string("R!\n");

		usb_stack_ack_event(EVT_USB_RESET);
		Usb_reset_endpoint(0);
      
		/** Reset resets endpoint 0 - it must be reconfigured **/
		Usb_select_device();
		Usb_select_endpoint(0);
		if (!Is_usb_endpoint_enabled())
			{
			//usbstack_debug_write_string("UEE!\n");
			usb_configure_endpoint(EP_CONTROL,    \
                                TYPE_CONTROL,  \
                                DIRECTION_OUT, \
                                SIZE_64,       \
                                ONE_BANK,      \
                                NYET_DISABLED);         
			}
		return;
		}
   
	// Handle device enumeration process
	Usb_select_endpoint(EP_CONTROL);
	if (Is_usb_receive_setup()) 
		{
		usb_stack_std_process_request();  
		//status_byte='$';
		}
	}

/*Starts the device after vbus has been received
 */
void usb_stack_start_device()
	{
	Pll_start_auto();
	Wait_pll_ready();
	Usb_unfreeze_clock();
	Usb_enable_suspend_interrupt();
	Usb_enable_reset_interrupt();
	usb_init_device();   // setup EP0 - part of mcu driver despite naming convention diff
	Usb_attach();
	}

/*The USB interrupt.  This is multiplexed to signal
* a variety of events.
*/
ISR(USB_GEN_vect)
	{
	//status_byte='u';
	//LOG('U');
	// VBUS state detection
	if (Is_usb_vbus_transition() && Is_usb_vbus_interrupt_enabled()) 
		{
		Usb_ack_vbus_transition();
		if (Is_usb_vbus_high())
			{
			// Connect
			usb_stack_connected=TRUE;
			usb_stack_action_vbus_on();
         
			usb_stack_send_event(EVT_USB_POWERED);
			Usb_enable_reset_interrupt();
         
			usb_stack_start_device();
         
			Usb_attach();
			//usbstack_debug_write_string("CT\n");
			//status_byte='L';
			} else 
			{
			// Disconnect
			usb_stack_action_vbus_off();
         
			usb_stack_connected=FALSE;
         
			usb_stack_send_event(EVT_USB_UNPOWERED);
			//usbstack_debug_write_string("DT\n");
			//status_byte='M';
			}
		}
   
		// Device start of frame
		if (Is_usb_sof() && Is_sof_interrupt_enabled())
		{
		Usb_ack_sof();
		usb_stack_action_sof();
		//status_byte='O';
		//PORTC=1<<PC1;  // set "Activity" State LED
		}
   
		// Device suspense event
		if (Is_usb_suspend() && Is_suspend_interrupt_enabled()) 
			{
			Usb_ack_suspend();
			Usb_enable_wake_up_interrupt();
			Usb_ack_wake_up();
			Usb_freeze_clock();
			usb_stack_send_event(EVT_USB_SUSPEND);
      
			usb_stack_action_suspend();
			//PORTC=1<<PC0;  // set Suspend State LED
			//status_byte='S';
      
			}
   
		// Device wake up event
		if (Is_usb_wake_up() && Is_swake_up_interrupt_enabled()) 
			{
			Usb_unfreeze_clock();
			Usb_ack_wake_up();
			Usb_disable_wake_up_interrupt();
      
			usb_stack_action_wake_up();
			usb_stack_send_event(EVT_USB_WAKE_UP);
			//status_byte='W';
			}
   
		// Resume state bus detection
		if (Is_usb_resume() && Is_resume_interrupt_enabled())
			{
			Usb_disable_wake_up_interrupt();
			Usb_ack_resume();
			Usb_disable_resume_interrupt();
      
			usb_stack_action_resume();
			usb_stack_send_event(EVT_USB_RESUME);
			//status_byte='R';
			}
   
		// Detect usb bus reset
		if (Is_usb_reset() && Is_reset_interrupt_enabled())
			{
			Usb_ack_reset();
      
			usb_stack_action_reset();
			usb_stack_send_event(EVT_USB_RESET);
			//status_byte='!';
			//usbstack_debug_write_string("IR!\n");
			}
	}



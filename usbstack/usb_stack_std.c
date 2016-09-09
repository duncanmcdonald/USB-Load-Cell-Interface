//! @file usb_stack_std.c
//!
//! @brief Chap 9 USB commands
//!
//! Copyright (c) 2013 Solid-Motion

#include "stdafx.h"
#include "serial_debug.h"
#include "usb_stack.h"
#include "usb_stack_std.h"
#include "usb_settings_i.c"
//#include "usb_drv.h"

// Variables
U8 usb_stack_configuration_number;

static U8 endpoint_status[NB_ENDPOINT];

// Chapter 9 standard request processing
void usb_stack_std_process_request() 
	{
	U8 request;
	U8 requestType;
	bit handled=FALSE;
   
	requestType = Usb_read_byte();
	request = Usb_read_byte();
   
	usbstack_debug_write_string("STD ");
	usbstack_debug_write_hex(request); //Field = bRequest
	usbstack_debug_write_hex(requestType); //Field = bmRequestType
	
	switch (request) 
		{
		case GET_DESCRIPTOR: //defined in usb_stack.h
		if (requestType == REQUEST_DEVICE_STATUS || requestType==REQUEST_INTERFACE_STATUS) //device-to-host standard device or interface request
			{
			usb_stack_std_get_descriptor(requestType);
			handled = TRUE;
			}
			else
			handled = FALSE;
		break;
      
		case GET_CONFIGURATION:
		if (requestType == REQUEST_DEVICE_STATUS)  //defined in usb_stack.h
			{
			usb_stack_std_get_configuration();
			handled = TRUE;
			}
		break;
      
		case SET_ADDRESS:
		if (requestType == ZERO_TYPE)
			{
			usb_stack_std_set_address();
			//PORTC=1<<PC3;  // set Address State LED
			handled=TRUE;
			}
		break;
      
		case SET_CONFIGURATION:
		if (requestType == ZERO_TYPE)
			{
			usb_stack_std_set_configuration();
			//PORTC=1<<PC2;  // set Configured State LED
			handled=TRUE;
			}
		break;
      
		case CLEAR_FEATURE:
		if (requestType < ENDPOINT_TYPE)
			{
			usb_stack_std_clear_feature(requestType);
			handled = TRUE;
			}
		break;
      
		case SET_FEATURE:
		if (requestType < ENDPOINT_TYPE)
			{
			usb_stack_std_set_feature(requestType);
			handled = TRUE;
			}
		break;
      
		case GET_STATUS:
		if (requestType == REQUEST_DEVICE_STATUS || requestType == REQUEST_INTERFACE_STATUS)
			{
			usb_stack_std_get_status(requestType);
			handled = TRUE;
			}
		break;
      
		case GET_INTERFACE:
		if (requestType == REQUEST_INTERFACE_STATUS)
			{
			usb_stack_std_get_interface();
			handled = TRUE;
			}
			else if(requestType == H2D_CLASS_INTERFACE_REQUEST || requestType == D2H_CLASS_INTERFACE_REQUEST) //if requestType = 0x21 or )xA0 it is really a SET_IDLE class request, not a GET INTERFACE request
			{
			Usb_ack_receive_setup();
			Usb_send_control_in();// Clear Transmitter (IN) Ready Interrupt Flag
			handled = TRUE;
			}
			else
			handled = FALSE;
		break;
      
		case SET_INTERFACE:
		if (requestType == INTERFACE_TYPE)
			{
			usb_stack_std_set_interface();
			handled = TRUE;
			}
		break;
      
		}
   
	if (!handled)
		{
		if (!usb_stack_user_process_request(requestType, request)) //check if it is a class request
			{
			//unknown request; not a standard request
			//any request that is not a standard request (class, vendor) but is known will be handled in hid_handler.c.
			//If it is not known it will be handled here & a STALL returned.
			Usb_enable_stall_handshake();
			Usb_ack_receive_setup();
			usbstack_debug_write('X');
			} else 
		{
		usbstack_debug_write('U'); //valid request, but not implemented. i.e. SET DESCRIPTOR, SYNCH FRAME
		}
		} else 
		{
		usbstack_debug_write('S'); //valid request, handled
		}
   
	usbstack_debug_write_nl();
	}

void usb_stack_std_get_configuration()
	{
	Usb_ack_receive_setup();
   usbstack_debug_write_string("_1_");
	Usb_write_byte(usb_stack_configuration_number);
	Usb_ack_in_ready();
   
	while (!Is_usb_receive_out());
   
	Usb_ack_receive_out();
	}

void usb_stack_std_set_address()
	{
	U8 addr = Usb_read_byte();
   
	usbstack_debug_write_string("ADDR");
	usbstack_debug_write_hex(addr);
	//usbstack_debug_write_nl();
   
   
	Usb_configure_address(addr);
	Usb_ack_receive_setup(); // Acks receive SETUP; SIE command
   
	Usb_send_control_in(); // Clear Transmitter (IN) Ready Interrupt Flag
	while (!Is_usb_in_ready());
   
	Usb_enable_address();
	}

void usb_stack_std_set_configuration() 
	{
	U8 number;
	number = Usb_read_byte();
	if (TRUE) 
		{    
		// Valid configuration
		Usb_ack_receive_setup();
		usb_stack_configuration_number = number;
		} else 
		{
		// Invalid configuration number
		Usb_enable_stall_handshake();
		Usb_ack_receive_setup();
		return;
		}
   
	Usb_send_control_in(); // Clear Transmitter (IN) Ready Interrupt Flag
   
	usb_stack_user_endpoint_init(number);
   
	usb_stack_action_set_configuration();
	}
	

void usb_stack_std_clear_feature(U8 requestType)
	{
	U8 value;
	U8 index;
	U8 dummy;
   
	if (requestType == ZERO_TYPE)
		{
		Usb_enable_stall_handshake();
		Usb_ack_receive_setup();
		usbstack_debug_write_string("_4_");
		return;
		}
   
	if (requestType ==  INTERFACE_TYPE) 
		{
		Usb_enable_stall_handshake();
		Usb_ack_receive_setup();
		return;
		}
   
	if (requestType == ENDPOINT_TYPE) 
		{
		value = Usb_read_byte();
		dummy = Usb_read_byte();
		if (value == FEATURE_ENDPOINT_HALT) 
			{
			index = (Usb_read_byte()&MSK_EP_DIR);
         
			Usb_select_endpoint(index);
			if (Is_usb_endpoint_enabled())
				{
				if (index != EP_CONTROL)
					{
					Usb_disable_stall_handshake();
					Usb_reset_endpoint(index);
					Usb_reset_data_toggle();
					}
				Usb_select_endpoint(EP_CONTROL);
				endpoint_status[index]=0x00; //FEATURE_ENDPOINT_HALT??
				Usb_ack_receive_setup();
				Usb_send_control_in(); // Clear Transmitter (IN) Ready Interrupt Flag
				} else
				{
				Usb_select_endpoint(EP_CONTROL);
				Usb_enable_stall_handshake();
				Usb_ack_receive_setup();
				return;
				}
			} else
			{
			Usb_enable_stall_handshake();
			Usb_ack_receive_setup();
			return;
			}
		}
	}

void usb_stack_std_set_feature(U8 requestType)
	{
	//SET FEATURE, Device not supported
	U8 value;
	U8 index;
	U8 dummy;
   
	if (requestType == INTERFACE_TYPE)
		{
		Usb_enable_stall_handshake();
		Usb_ack_receive_setup();
		return;
		} else if (requestType == ENDPOINT_TYPE) 
		{
		value = Usb_read_byte();
		dummy = Usb_read_byte();
      
		if (value == FEATURE_ENDPOINT_HALT) 
			{
			index = (Usb_read_byte() & MSK_EP_DIR);
         
			if (index == EP_CONTROL)
				{
				Usb_enable_stall_handshake();
				Usb_ack_receive_setup();
				return;
				}
			Usb_select_endpoint(index);
			if (Is_usb_endpoint_enabled())
				{
				Usb_enable_stall_handshake();
				Usb_select_endpoint(EP_CONTROL);
				endpoint_status[index]=0x01; //FEATURE_DEVICE_REMOTE_WAKEUP??
				Usb_ack_receive_setup();
				Usb_send_control_in(); // Clear Transmitter (IN) Ready Interrupt Flag
				} else 
				{
				Usb_select_endpoint(EP_CONTROL);
				Usb_enable_stall_handshake();
				Usb_ack_receive_setup();
				return;
				}
			} else 
			{
			Usb_enable_stall_handshake();
			Usb_ack_receive_setup();
			return;
			}
		}
	}

void usb_stack_std_get_status(U8 requestType) 
	{
	U8 index;
	U8 dummy;
   
	dummy = Usb_read_byte();
	dummy = Usb_read_byte();
	index = Usb_read_byte();
   
	switch (requestType)
		{
		case REQUEST_DEVICE_STATUS:
		Usb_ack_receive_setup();
		Usb_write_byte(0);
		break;
      
		case REQUEST_INTERFACE_STATUS:
		Usb_ack_receive_setup();
		Usb_write_byte(0);
		break;
      
		case REQUEST_ENDPOINT_STATUS:
		Usb_ack_receive_setup();
		index = index & MSK_EP_DIR;
		Usb_write_byte(endpoint_status[index]);
		break;
      
		default:
		Usb_enable_stall_handshake();
		Usb_ack_receive_setup();
		return;
		}
   
	Usb_write_byte(0x00);
	Usb_send_control_in(); // Clear Transmitter (IN) Ready Interrupt Flag
   
	while (!Is_usb_receive_out());
	Usb_ack_receive_out();
	}

void usb_stack_std_get_interface()
	{
	Usb_enable_stall_handshake();
	Usb_ack_receive_setup();
	}

void usb_stack_std_set_interface()
	{
	Usb_ack_receive_setup();// Acks receive SETUP; SIE command
	Usb_send_control_in(); // Clear Transmitter (IN) Ready Interrupt Flag
	while (!Is_usb_in_ready());
	}

void usb_stack_std_get_descriptor(U8 requestType)
	{
	data_buffer_t buffer;
   
	U16 length;
	U16 offset;
	U8 descriptortype;
	U8 stringtype;
	U8 dummy;
	bit zlp;
	U8 cur_cnt;
   
	stringtype = Usb_read_byte(); //Field = wValue lower byte; descriptor index
	descriptortype = Usb_read_byte();  //Field = wValue upper byte; descriptor type
   
	usbstack_debug_write_string("SGD ");
	usbstack_debug_write_hex(stringtype);
	usbstack_debug_write_hex(descriptortype);
    
	buffer.type = 0;
	buffer.length = 0;
	usb_stack_user_populate_descriptor(buffer, requestType, descriptortype, stringtype);
   
	// No descriptor found?
	if (buffer.type == 0)
		{
		Usb_enable_stall_handshake();
		Usb_ack_receive_setup();
		return;
		}
   
	dummy = Usb_read_byte(); // wIndex field not needed
	dummy = Usb_read_byte();
   
	LSB(length) = Usb_read_byte();
	MSB(length) = Usb_read_byte();
	Usb_ack_receive_setup(); // ack setup stage
   
	if (length > buffer.length)
		{
		if ((buffer.length % EP_CONTROL_LENGTH) == 0) zlp = TRUE; // Can send the whole thing
		else zlp = FALSE;
		} else 
		{
		buffer.length = length; // Send only requested amount
		zlp = TRUE;
		}
   
	offset = 0;
	while (buffer.length != 0 && !Is_usb_receive_out()) 
		{
		while (!Is_usb_read_control_enabled());
      
		cur_cnt = 0;
		while (buffer.length != 0)
			{
			if (cur_cnt == EP_CONTROL_LENGTH) break; // New packet
			cur_cnt += 1;
         
			Usb_write_byte(data_buffer_read_byte(buffer, offset));
			offset += 1;
			buffer.length -= 1;
			}
      
		Usb_send_control_in(); // ack data stage; Clear Transmitter (IN) Ready Interrupt Flag
		}
   
	if (Is_usb_receive_out()) 
		{ 
		Usb_ack_receive_out(); 
		return; 
		}   // Abort from host
	if (zlp == TRUE)
		{
		while (!Is_usb_read_control_enabled());
		Usb_send_control_in(); // Clear Transmitter (IN) Ready Interrupt Flag
		}
   
	while (!Is_usb_receive_out()); // wait to receive OUT data
	Usb_ack_receive_out(); // ack status transaction
	}	


U8 usb_stack_read_control_write(U8 *buffer, U8 bLength) 
	{
	// Complete sequence for Control Write Transfer
	if (bLength != 0)
		{
		U8 i, dummy; 
   
		Usb_ack_receive_setup(); //Setup transaction, handshake stage ACK
		//Usb_send_control_in();  
   
		while (!Is_usb_receive_out()); // test if OUT received
	
		dummy = Usb_read_byte(); // Get rid of page number
	
		for (i = 0; i < bLength; i++) // Start index at page offset
			{
			buffer[i] = Usb_read_byte();
			}
		
			Usb_ack_receive_out(); // OUT data transaction, handshake stage ACK
			Usb_send_control_in(); // Status transaction, data ZLP
		
			return 1;
		}
		else  // Sequence if this is CMD Reset TEDS; doesn't actually do anything but a presence pulse & detection could go here
		{
			Usb_ack_receive_setup(); //Setup transaction, handshake stage ACK
			//Usb_send_control_in();
			while (!Is_usb_receive_out()); // test if OUT received
			Usb_read_byte();
			Usb_read_byte();
			Usb_ack_receive_out(); // OUT data transaction, handshake stage ACK
			Usb_send_control_in(); // Status transaction, data ZLP
		}
		return 1;
	}
		

void usb_stack_ack_control_write(U8 success)
	{
	// Partial sequence for Control Read Transfer
	// Calling usb_stack_ack_control_write(U8 success) should be preceded by Usb_ack_receive_setup()

	if (!success) Usb_enable_stall_handshake();

	Usb_send_control_in(); // IN data transaction, data stage, done sending data
	Usb_ack_receive_out(); // Status transaction, handshake stage ACK
	while (!Is_usb_in_ready());
	}
//! @}

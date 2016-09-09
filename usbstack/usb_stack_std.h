//! @file usb_stack_std.h
//!
//! @brief Chap 9 USB commands
//!
//! Copyright (c) 2013 Solid-Motion
 
#ifndef __USB_STACK_STD__H__
#define __USB_STACK_STD__H__
//! @brief Macro to determine if device has been enumerated (i.e. a configuration has been set)
//!
#define usb_stack_is_enumerated() (usb_stack_configuration_number!=0)

#endif

#define EP_CONTROL_LENGTH 64

//! @defgroup std_request USB request processing
//! @brief Simplified USB stack suitable for a boot loader.
//! @{
//!
//! Called by usb_stack_service to handle set-up requests on the control endpoint.
//! It handles as much as it can and then defers to user handlers.
//!
//! This module reads the SETUP request sent to the default control endpoint
//! and calls the appropriate function. When exiting the usb_read_request
//! function, the device is ready to manage the next request.
//! If the received request is not supported or not a USB standard request, the function
//! will call a custom decoding function in the usb_specific_request module.
//!
//! @note list of supported requests:
//!- GET_DESCRIPTOR
//!- GET_CONFIGURATION
//!- SET_ADDRESS
//!- SET_CONFIGURATION
//!- CLEAR_FEATURE
//!- SET_FEATURE
//!- GET_STATUS
//!- GET INTERFACE
//!- SET INTERFACE
//!
//! SYNCH FRAME and SET DESCRIPTOR not supported
//!
//!

//! @brief GET DESCRIPTOR
//! @param requestType GET DESCRIPTOR requestType is always 0x80, device-to-host
//!
void usb_stack_std_get_descriptor(U8 requestType);

//! @brief GET CONFIGURATION
//!
//! Sends the Configuration Number to the host that was set by SET CONFIGURATION.
//! Only executed if requestType = 0x80 (device-to-host; standard request; device). 
//!
void usb_stack_std_get_configuration();

//! @brief SET CONFIGURATION
//! Set the Configuration Number received from the host.
//!
void usb_stack_std_set_configuration();

//! @brief SET ADDRESS
//! Set the Device address received from the host.
//!
void usb_stack_std_set_address();

//! @brief CLEAR FEATURE
//! @param requestType CLEAR FEATURE requestType is either 0x00 (device), or 0x01 (interface).
//! @todo Filter valid configurations in usb_stack_std_set_configuration
//! See Table 9-6 for a definition of which feature selector values are defined for which recipients.
//!
void usb_stack_std_clear_feature(U8 requestType);

//! @brief SET FEATURE
//! @param requestType SET FEATURE requestType is either 0x00 (device), 0x01 (interface), or 0x02 (endpoint).
//!
//! See Table 9-6 for a definition of which feature selector values are defined for which recipients.
//!- Device is not currently supported
//!- Interface returns a STALL
//!- Endpoint at least partially supported
//!
void usb_stack_std_set_feature(U8 requestType);

//! @brief GET STATUS
//! @param requestType GET STATUS requestType can be either 0x00 (device), 0x01 (interface), or 0x02 (endpoint).
//!
//!- Request device status or interface status just returns an ACK
//!- Request endpoint status returns the endpoint status
//!
void usb_stack_std_get_status(U8 requestType);

//! @brief GET INTERFACE
//!
//! Supposed to return the selected alternate setting for the specified interface.
//! Doesn't do anything here, just returns an ACK.
void usb_stack_std_get_interface();

//! @brief SET INTERFACE
//! Supposed to allow the host to select an alternate setting for the specified interface.
//! Doesn't do anything here, just returns an ACK.
//!
void usb_stack_std_set_interface();

//! @brief Entry point for standard USB requests
//!
//! If not a standard request, the request will be passed off to usb_stack_user_process_request
//! @note list of supported requests:
//!- GET_DESCRIPTOR
//!- GET_CONFIGURATION
//!- SET_ADDRESS
//!- SET_CONFIGURATION
//!- CLEAR_FEATURE
//!- SET_FEATURE
//!- GET_STATUS
//!- GET INTERFACE
//!- SET INTERFACE
//!
//! SET DESCRIPTOR and SYNCH FRAME not supported
//!
void usb_stack_std_process_request();


//! @brief  Non-standard (i.e. Class specific) control request processing.
//! @param requestType bmRequestType = 0x01 (Class request)
//! @param request bmRequest = the class request
//! @note list of supported Class specific (HID) requests:
//!- HID Get Report
//!- HID Set_Report
//!- Two standard USB requests can show up in a class request and must be specially handled
//!  - SET CONFIGURATION
//!  - GET INTERFACE
//!
//! See Device Class Definition for Human Interface Devices (HID) 5.1
//!
U8 usb_stack_user_process_request(U8 requestType, U8 request);


//! @brief Control write transfer
//!
//! Reads the data stage of a control write transfer
//! into the given buffer.  Will read length bytes.
//! @param buffer holds the data stage of a control write transfer
//! @param page memory page number
//! @param bLength read length in bytes
//!
U8 usb_stack_read_control_write(U8 *buffer, U8 bLength);

//! @brief Acks a control write transfer.
//!
//! Call after usb_stack_read_control_write().  Ack if success is TRUE, otherwise STALL.
//! @param success 
//! @warning TJ could find no example of how to do this correctly, but what he implemented seems to work.
//!
void usb_stack_ack_control_write(U8 success);

//! @brief If !0, then this is the enumerated configuration number
//!
extern U8 usb_stack_configuration_number;



//! @}



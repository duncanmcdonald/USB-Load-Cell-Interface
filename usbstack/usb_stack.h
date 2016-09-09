//! @file usb_stack.h
//!
//! Copyright (c) 2013 Solid-Motion
//!
//! @brief Simplified usb stack suitable for a boot loader
//!
//! @defgroup std_request USB request processing
//! @{
//!
#ifndef __USB_STACK__H__
#define __USB_STACK__H__

#ifdef USB_DEBUG
#define usbstack_debug_write debug_write
#define usbstack_debug_write_nl debug_write_nl
#define usbstack_debug_write_string debug_write_string
#define usbstack_debug_write_hex debug_write_hex
#else
#define usbstack_debug_write(...)
#define usbstack_debug_write_nl(...)
#define usbstack_debug_write_string(...)
#define usbstack_debug_write_hex(...)
#endif

//! @brief Initializes the usb stack in device mode
//!
void usb_stack_init_device();

//! @brief Called in a loop to service the usb stack
//!
void usb_stack_service();
//! @}

extern volatile U16 usb_stack_event;

/** USB Event management.  Called from ISR **/
#ifndef usb_stack_action_vbus_on
#define usb_stack_action_vbus_on()
#endif

#ifndef usb_stack_action_vbus_off
#define usb_stack_action_vbus_off()
#endif

#ifndef usb_stack_action_sof
#define usb_stack_action_sof()
#endif

#ifndef usb_stack_action_suspend
#define usb_stack_action_suspend()
#endif

#ifndef usb_stack_action_wake_up
#define usb_stack_action_wake_up()
#endif

#ifndef usb_stack_action_resume
#define usb_stack_action_resume()
#endif

#ifndef usb_stack_action_reset
#define usb_stack_action_reset()
#endif

#ifndef usb_stack_action_set_configuration
#define usb_stack_action_set_configuration()
#endif

/** -- The following chopped down from Atmel usbtask.h header file -- **/
#define usb_stack_send_event(x)               (usb_stack_event |= (1<<x))
#define usb_stack_ack_event(x)                (usb_stack_event &= ~(1<<x))
#define usb_stack_clear_all_event()           (usb_stack_event = 0)
#define usb_stack_is_usb_event(x)                 ((usb_stack_event & (1<<x)) ? TRUE : FALSE)
#define usb_stack_is_not_usb_event(x)             ((usb_stack_event & (1<<x)) ? FALSE: TRUE)
#define usb_stack_is_host_emergency_exit()        (usb_stack_is_usb_event(EVT_HOST_DISCONNECTION) || usb_stack_is_usb_event(EVT_USB_DEVICE_FUNCTION))

#define EVT_USB_POWERED               1         // USB plugged
#define EVT_USB_UNPOWERED             2         // USB unplugged
#define EVT_USB_DEVICE_FUNCTION       3         // USB in device
#define EVT_USB_HOST_FUNCTION         4         // USB in host
#define EVT_USB_SUSPEND               5         // USB suspend
#define EVT_USB_WAKE_UP               6         // USB wake up
#define EVT_USB_RESUME                7         // USB resume
#define EVT_USB_RESET                 8         // USB reset
#define EVT_HOST_SOF                  9         // Host start of frame sent
#define EVT_HOST_HWUP                 10        // Host wakeup detected
#define EVT_HOST_DISCONNECTION        11        // The target device is disconnected

//standard device requests; table 9-3
#define GET_STATUS                     0x00
#define GET_DEVICE                     0x01
#define CLEAR_FEATURE                  0x01           
#define GET_STRING                     0x03
#define SET_FEATURE                    0x03           
#define SET_ADDRESS                    0x05
#define GET_DESCRIPTOR                 0x06
#define SET_DESCRIPTOR                 0x07
#define GET_CONFIGURATION              0x08
#define SET_CONFIGURATION              0x09
#define GET_INTERFACE                  0x0A
#define SET_INTERFACE                  0x0B
#define SYNCH_FRAME                    0x0C

//two of the table 9-5 descriptor types (there are 8 altogether)
#define GET_DEVICE_DESCRIPTOR             1
#define GET_CONFIGURATION_DESCRIPTOR      4

//some of the request types from table 9-2
#define REQUEST_DEVICE_STATUS          0x80 //device-to-host; standard request; device
#define REQUEST_INTERFACE_STATUS       0x81 //device-to-host; standard request; interface
#define D2H_CLASS_INTERFACE_REQUEST    0xA1 //device-to-host; class request; interface
#define REQUEST_ENDPOINT_STATUS        0x82 //device-to-host; standard request; endpoint
#define H2D_CLASS_INTERFACE_REQUEST    0x21 //host-to-device; class request; interface
#define ZERO_TYPE                      0x00 //host-to-device; standard request; device
#define INTERFACE_TYPE                 0x01 //host-to-device; standard request; interface
#define ENDPOINT_TYPE                  0x02 //host-to-device; standard request; endpoint

//descriptor types from table 9-5
//used by GET DESCRIPTOR and SET DESCRIPTOR
#define DEVICE_DESCRIPTOR                     0x01
#define CONFIGURATION_DESCRIPTOR              0x02
#define STRING_DESCRIPTOR                     0x03
#define INTERFACE_DESCRIPTOR                  0x04
#define ENDPOINT_DESCRIPTOR                   0x05
#define DEVICE_QUALIFIER_DESCRIPTOR           0x06
#define OTHER_SPEED_CONFIGURATION_DESCRIPTOR  0x07



//standard feature selectors from table 9-6
//used by CLEAR FEATURE and SET FEATURE
#define FEATURE_DEVICE_REMOTE_WAKEUP   0x01
#define FEATURE_ENDPOINT_HALT          0x00
//TEST MODE is 0x02

//test mode selectors from table 9-7. Used by SET FEATURE, TEST MODE
#define TEST_J                         0x01
#define TEST_K                         0x02
#define TEST_SEO_NAK                   0x03
#define TEST_PACKET                    0x04
#define TEST_FORCE_ENABLE              0x05

//device states
#define ATTACHED                          0 //only comes into play for a self powered device
#define POWERED                           1
#define DEFAULT                           2
#define ADDRESSED                         3
#define CONFIGURED                        4
#define SUSPENDED                         5

#define USB_CONFIG_ATTRIBUTES_RESERVED    0x80
#define USB_CONFIG_BUSPOWERED            (USB_CONFIG_ATTRIBUTES_RESERVED | 0x00)
#define USB_CONFIG_SELFPOWERED           (USB_CONFIG_ATTRIBUTES_RESERVED | 0x40)
#define USB_CONFIG_REMOTEWAKEUP          (USB_CONFIG_ATTRIBUTES_RESERVED | 0x20)

// Device Status
#define BUS_POWERED                       0
#define SELF_POWERED                      1
/** -- End excerpt from usb_task.h -- **/

#endif

//! @}

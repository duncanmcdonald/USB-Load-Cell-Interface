//! @file conf_usb.h
//!
//! @brief Atmel file. USB regulator and device mode configuration
//!
/*** This file came from an Atmel example and is internally included
 *** by the Atmel support code.
 ***/
#ifndef _CONF_USB_H_
#define _CONF_USB_H_

#define USB_HOST_FEATURE            DISABLED
#define USB_DEVICE_FEATURE          ENABLED

   // _________________ USB REGULATOR CONFIGURATION _______________________
   //

#ifndef USE_USB_PADS_REGULATOR
   #define USE_USB_PADS_REGULATOR   ENABLE      // Possible values ENABLE or DISABLE
#endif


// _________________ DEVICE MODE CONFIGURATION __________________________


#define NB_ENDPOINTS          3  //  number of endpoints in the application including control endpoint
#define EP_HID_IN             1
#define EP_HID_OUT            2

//! The USB device speed mode (default DISABLE to get full speed connection)
#define USB_LOW_SPEED_DEVICE  DISABLE


   // write here the action to associate to each USB event
   // be carefull not to waste time in order not disturbing the functions
#define Usb_sof_action()         sof_action();
#define Usb_wake_up_action()
#define Usb_resume_action()
#define Usb_suspend_action()
#define Usb_reset_action()
#define Usb_vbus_on_action()
#define Usb_vbus_off_action()
#define Usb_set_configuration_action()


extern void sof_action(void);




#endif // _CONF_USB_H_

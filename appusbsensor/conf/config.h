//! @file config.h
//!
//! @brief Atmel file. May not be needed
//!

/*** This file originally came from an Atmel example.  It is internally included
 *** by a number of Atmel support libraries.  Most of it is probably irrelevent.
 ***/
#ifndef _CONFIG_H_
#define _CONFIG_H_

#define  STK525   0
#define  USBKEY   1

//! Enable or not the ADC usage
#undef USE_ADC
//! To include proper target hardware definitions, select
//! target board (USBKEY or STK525)
#define TARGET_BOARD STK525

#if (TARGET_BOARD==USBKEY)
   //! @warning for #define USBKEY_HAS_321_DF, only first prototypes versions have AT45DB321C memories
   //! should be undefined for std series
   #define USBKEY_HAS_321_DF
   #include "lib_board\usb_key\usb_key.h"
#elif (TARGET_BOARD==STK525)
   #include "lib_board\stk_525\stk_525.h"
#else
   #error TARGET_BOARD must be defined somewhere
#endif


//! CPU core frequency in kHz
#define FOSC 8000


#define LOG_BYTE(b) send_status_byte(b);
void send_status_now(unsigned char status);
#define LOG(c) send_status_now(c)
extern unsigned char status_byte;

#endif // _CONFIG_H_


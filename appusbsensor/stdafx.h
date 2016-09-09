//! @file stdafx.h
//!
//! @brief Atmel file. Debug turned on/off here
//!
//!

#ifndef __STDAFX__H__
#define __STDAFX__H__

#define USB_DEBUG 0

#include "config.h"

#include "lib_mcu/compiler.h" //!< Compiler definitions
#include "lib_mcu/wdt/wdt_drv.h"
#include "lib_mcu/power/power_drv.h"
#include "lib_mcu/usb/usb_drv.h"
#include "lib_mcu/pll/pll_drv.h"

#include <avr/io.h>
#include <avr/eeprom.h>

#include "memaccess.h"

#include "globals.h"

#endif


//! @file globals.h
//!
//! Copyright (c) 2013 Solid-Motion
//!
//! @brief Defines various globals
//!
//! This is included as part of stdafx.h so it is always available.

//! @brief Handle the USB event set_configuration
#define usb_stack_action_set_configuration() appusbsensor_set_configuration()

//! @brief Called when the device is configured.
//! Starts the poweron ticks timer. This function is called from the usb stack
void appusbsensor_set_configuration();



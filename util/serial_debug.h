//! @file serial_debug.h
//!
//! Copyright (c) 2013 Solid-Motion
//!
//! @brief Serial port functions for debug messages
//!

#ifndef __SERIAL_DEBUG_H__
#define __SERIAL_DEBUG_H__

#define DEBUG_BUFFER_SIZE 255
#define DEBUG_UBRRVAL 51

extern U8 debug_buffer[];
extern U8 debug_buffer_head;
extern U8 debug_buffertail;

void debug_init();
void debug_service();
void debug_service_all();
void debug_write(U8 byte);
void debug_write_string(char* str);
void debug_write_hex(U8 byte);
void debug_write_nl();

#endif


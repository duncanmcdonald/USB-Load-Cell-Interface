//! @file serial_debug.c
//!
//! Copyright (c) 2013 Solid-Motion
//!
//! @brief Serial port functions for debug messages
//!
#include "stdafx.h"
#include "serial_debug.h"

U8 debug_buffer[DEBUG_BUFFER_SIZE];
U8 debug_buffer_head;
U8 debug_buffertail;

inline void debug_init() {
   // Initialize the UART
   UBRR1L=DEBUG_UBRRVAL;
   UBRR1H=DEBUG_UBRRVAL >> 8;
   
   UCSR1B=(1<<RXEN1)|(1<<TXEN1);
   UCSR1C=(1<<USBS1)|(3<<UCSZ10);
}

inline void debug_service() {
   U8 b;
   U8 sreg;
   
   if (!(UCSR1A & (1<<UDRE1))) {
      // Not ready to write
      return;
   }
   
   if (debug_buffertail!=debug_buffer_head) {
      sreg=SREG;
      cli();
      
      b=debug_buffer[debug_buffertail];
      debug_buffer[debug_buffertail]=0;
      UDR1=b;
      debug_buffertail+=1;
      if (debug_buffertail==DEBUG_BUFFER_SIZE) debug_buffertail=0;
      
      SREG=sreg;
   }
}

inline void debug_service_all() {
   while (debug_buffer_head!=debug_buffertail) {
      debug_service();
   }
}

inline void debug_write(U8 byte) {
   debug_buffer[debug_buffer_head]=byte;
   debug_buffer_head+=1;
   if (debug_buffer_head==DEBUG_BUFFER_SIZE) debug_buffer_head=0;
}

inline void debug_write_string(char* str) {
   for (;;) {
      char c=*str;
      if (!c) break;
      if (c=='\n') debug_write('\r');
      debug_write(c);
      str++;
   }
}

inline void debug_write_nl() {
   debug_write('\r');
   debug_write('\n');
}

inline void debug_write_hex(U8 byte) {
   U8 nibble;
   
   debug_write(' ');
   debug_write('0');
   debug_write('x');
   
   nibble=(byte>>4);
   debug_write(nibble>9 ? nibble - 10 + 'a' : nibble + '0');
   
   nibble=byte & 0xf;
   debug_write(nibble>9 ? nibble - 10 + 'a' : nibble + '0');
   
   debug_write(' ');
}


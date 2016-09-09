//! @file long_delay.h
//!
//! @brief Create a longer delays than _delay_us (AVR libc) will allow
//!
//! Copyright (c) 2013 Solid-Motion


#include <util/delay.h>

static inline void long_delay_us(double __us) __attribute__((always_inline));

/**
 * Uses a 16bit counter to do a us delay loop.
 * This allows delays longer than 48us (at 16MHz)
 * which is all that _delay_us will do
 */
void long_delay_us(double _us) {
   U16 __ticks;
   double __tmp= ((F_CPU)/4e6) * _us;
   if (__tmp < 1.0) 
      __ticks = 1;
   else if (__tmp > 65535)
      __ticks = 0;
   else
      __ticks = (U16)__tmp;
   _delay_loop_2(__ticks);
}


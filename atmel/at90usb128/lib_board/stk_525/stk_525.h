//! @file stk_525.h
//!
//! Copyright (c) 2004 Atmel.
//!
//! Please read file license.txt for copyright notice.
//!
//! @brief Atmel file. Low level macros and definition for stk525 board
//!
//! Not sure why this is needed but comile fails without it.
//! @version 1.14 at90usb128-demo-template-1_0_5
//!
//! @todo
//! @bug What is actually used in this file?

#ifndef STK_525_H
#define STK_525_H

//_____ I N C L U D E S ____________________________________________________
#include "config.h"


//_____ M A C R O S ________________________________________________________
#define Leds_init()     (DDRD |= 0xF0)
#define Leds_on()       (PORTD |= 0xF0)
#define Leds_off()      (PORTD &= 0x0F)
#define Led0_on()       (PORTD |= 0x10)
#define Led1_on()       (PORTD |= 0x20)
#define Led2_on()       (PORTD |= 0x40)
#define Led3_on()       (PORTD |= 0x80)
#define Led0_off()      (PORTD &= 0xEF)
#define Led1_off()      (PORTD &= 0xDF)
#define Led2_off()      (PORTD &= 0xBF)
#define Led3_off()      (PORTD &= 0x7F)
#define Led0_toggle()   (PIND |= 0x10)
#define Led1_toggle()   (PIND |= 0x20)
#define Led2_toggle()   (PIND |= 0x40)
#define Led3_toggle()   (PIND |= 0x80)
#define Leds_set_val(c) (Leds_off(),PORTD |= (c<<4)&0xF0)
#define Leds_get_val()  (PORTD>>4)
#define Is_led0_on()    (PIND & 0x10 ? TRUE : FALSE)

#define Joy_init()      (DDRB &= 0x1F, PORTB |= 0xE0, DDRE &= 0xE7, PORTE |= 0x30)
#define Is_joy_up()     ((PINB & 0x80) ?  FALSE : TRUE)
#define Is_joy_not_up()       ((PINB & 0x80) ?  TRUE  : FALSE)
#define Is_joy_left()   ((PINB & 0x40) ?  FALSE : TRUE)
#define Is_joy_not_left()     ((PINB & 0x40) ?  TRUE : FALSE)
#define Is_joy_select() ((PINB & 0x20) ?  FALSE : TRUE)
#define Is_joy_not_select()   ((PINB & 0x20) ?  TRUE : FALSE)
#define Is_joy_right()  ((PINE & 0x10) ?  FALSE : TRUE)
#define Is_joy_not_right()    ((PINE & 0x10) ?  TRUE : FALSE)
#define Is_joy_down()   ((PINE & 0x20) ?  FALSE : TRUE)
#define Is_joy_not_down()     ((PINE & 0x20) ?  TRUE : FALSE)

#define Hwb_button_init()      (DDRE &= 0xFB, PORTE |= 0x04)
#define Is_hwb()               ((PINE & 0x04) ?  FALSE : TRUE)
#define Is_not_hwb()           ((PINE & 0x04) ?  TRUE : FALSE)


#define ADC_POT_CH   0x01
#define ADC_MIC_CH   0x02
#define ADC_TEMP_CH  0x00

#ifdef USE_ADC       //!< this define is set in config.h file

   U16 Get_adc_mic_val(void);

   U16 Get_adc_temp_val(void);

   U16 Get_adc_pot_val(void);

   S16  Read_temperature(void);

#endif  //!USE_ADC


#define DF_4_MB   // Type of DF implemented on STK525.


#define DF_SELECT_MEM        0
#define DF_UNSELECT_MEM      1
#define DF_NB_MEM            1

#ifndef AVRGCC
   // PORTB_Bit0=1,DDRB_Bit0=0: SPI Slave Select Input. The data direction of the
   //    NOT(SS) is INPUT and the pull-up resistor is activated.
   // DDRB_Bit1=1: the SCK signal direction is OUTPUT.
   // DDRB_Bit2=1: the SI signal direction is OUTPUT.
   // DDRB_Bit3=0: the SO signal direction is INPUT.
   // PORTB_Bit4=1,DDRB_Bit4=1: CS driven high
   //    and both CS are OUTPUT.

   #define Init_df_stk525()   (DDRB_Bit4=1,PORTB_Bit3=1,PORTB_Bit4=1,PORTB_Bit0=1)
   #define DF_CS_PIN          PORTB_Bit4
   #define DF_CS0             DF_CS_PIN
   #define DF_CS1             DF_CS0
   #define DF_CS2             DF_CS0
   #define DF_CS3             DF_CS0
   #define Df_select_df(i, val) (DF_CS##i = val)
   #define Df_desel_all()       (DF_CS0=DF_UNSELECT_MEM)     // set CS# dataflash memories
#else //! for AVRGCC
   #define Init_df_stk525()   (DDRB |=0x10,PORTB|=0x10,PORTB|=0x01,DDRB &= 0xFE,\
   							   DDRB|=0x01,DDRB|=0x04,DDRB|=0x04)
   #define DF_CS_PORT         PORTB
   #define DF_CS0             4
   #define DF_CS1             DF_CS0
   #define DF_CS2             DF_CS0
   #define DF_CS3             DF_CS0
   #define Df_desel_all()     (DF_CS_PORT|=0x10) // set CS# dataflash memories (unselect).
#endif
#define df_init_spi()      Init_df_stk525()

#endif  // STK_525_H

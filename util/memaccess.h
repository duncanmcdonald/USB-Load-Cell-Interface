//! @file memaccess.h
//!
//! @brief Memory thing called by stdafx.h
//!
//! Copyright (c) 2013 Solid-Motion

#ifndef __MEMACCESS__H__
#define __MEMACCESS__H__

#define DBTYPE_NONE 0
#define DBTYPE_SRAM 1
#define DBTYPE_LPM 2
#define DBTYPE_EEPROM 4
typedef struct {
   U8 type;
   U16 length;
   union {
      U8* pointer;
      PGM_P lpm;
      U8* eepromaddr;
   } pointer;
} data_buffer_t;

#ifdef PRG_HIMEM
#define __CUSTOM_LPM(address) __ELPM((U32)(((U32)(int)0x10000)|((U32)(int)address)))
#else
#define __CUSTOM_LPM(address) __LPM((U16)address)
#endif

#define data_buffer_read_byte(data_buffer, offset) \
   (data_buffer.type==DBTYPE_SRAM ?   \
      data_buffer.pointer.pointer[offset] :  \
      (data_buffer.type==DBTYPE_LPM ? \
         __CUSTOM_LPM(data_buffer.pointer.lpm+offset) :   \
            (data_buffer.type==DBTYPE_EEPROM ? \
               eeprom_read_byte(data_buffer.pointer.eepromaddr) : \
               0)))

#endif


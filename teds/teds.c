/** @file teds.c
*
* Copyright (c) 2013 Solid-Motion
*
* @defgroup teds_management TEDS management
*
* @brief Read/write functions for the TEDS memory
*
* @brief TEDS write commands. DS2433 4kb 1-Wire EEPROM
* Commands:
	ROM Commands
        - READ ROM [33H]
        - MATCH ROM [55H]
        - SKIP ROM [CCH] used
        - SEARCH ROM [F0H] used
        - OVERDRIVE SKIP ROM [3CH]
        - OVERDRIVE MATCH ROM [69H]
	MEMORY Commands
        - WRITE SCRATCHPAD COMMAND [0FH]  
		     - After issuing the write scratchpad command, the master must provide the 2-byte target address,
			 - followed by the data to be written to the scratchpad. 
			 - The memory address range of the DS2433 is 0000H to 01FFH
        - READ SCRATCHPAD COMMAND [AAH]
        - COPY SCRATCHPAD [55H]
        - READ MEMORY [F0H]  Read Memory; needs two byte memory start address, range 0x0000 to 0x01FF
            - Array Target Address
            - Sets scratchpad offset
@{
*/

/********************************* TEDS Management ************************************/

#include <lib_mcu/compiler.h> // core types, etc
#include <avr/io.h>
#include <util/delay.h>
#include <long_delay.h>
#include <serial_debug.h>
#include "teds.h"

/*** Teds Port Config ***/
/** Data direction register **/
#define TEDS_DDR DDRC
#define TEDS_PIN PINC
#define TEDS_PORT PORTC
#define TEDS_BIT 0

/** Port state changes **/
// Activate teds port as input and enable pullup
inline void teds_port_active_input()
	{
	TEDS_DDR &= ~_BV(TEDS_BIT);   // Clear DDR - input
	TEDS_PORT |= _BV(TEDS_BIT);   // Set port - enable pullup
	}

// Assert the teds port to logic 1
inline void teds_port_assert()
	{
	TEDS_PORT |= _BV(TEDS_BIT);   // Set output state to 1 (enable pullup if in input)
	TEDS_DDR  |= _BV(TEDS_BIT);   // Enable output
	}

// Enable a hard logic 0
inline void teds_port_deassert()
	{
	TEDS_PORT &= ~_BV(TEDS_BIT);  // Logic 0
	TEDS_DDR  |= _BV(TEDS_BIT);   // Enable output
	}

// High impedance input
inline void teds_port_passive_input()
	{
	TEDS_DDR  &= ~_BV(TEDS_BIT);   // Enable input
	TEDS_PORT &= ~_BV(TEDS_BIT);   // Disable pullup
	}

// Sample the value
inline U8 teds_port_sample()
	{
	return (TEDS_PIN & _BV(TEDS_BIT));
	}

void teds_init()
	{
	teds_port_passive_input();
	}

/** @brief Initiate a teds reset and detect cycle.  Return !0 if detect pulse received.
*   Teds detect sequence:
*   1. deassert
*   2. wait tRSTL (>=480us)
*   3. active input (pullup engaged)
*   ---  after 15-60us ---
*   4. device transmits presence pulse (pulls low) for 60-240us
*   If detect logic 0 during presence pulse stage, then device is connected  	
*/
U8 teds_detect()
	{
	U8 detected = 0;
   
	teds_port_deassert();
	//_delay_ms(1);   // Spec says >= 480us.  nothing less than 1ms worked for me
	long_delay_us(480);
   
	teds_port_passive_input();
	long_delay_us(66);    // 30us delay to allow things to stabilize
   
	detected =! teds_port_sample();
   
	long_delay_us(414);
   
	return detected;
	}

U8 teds_io(U8 bit_out)
	{
	teds_port_deassert();
	long_delay_us(1);
   
	if (bit_out) teds_port_passive_input();
	long_delay_us(14);
   
	if (teds_port_sample() == 0) bit_out = 0;
	long_delay_us(45);
   
	teds_port_passive_input();
   
	return bit_out;
	}

U8 teds_write_byte(U8 value)
	{
	U8 counter, j;

	for (counter = 0; counter < 8; counter++)
		{
		j = teds_io(value & 1);
		value = value >> 1;
		if (j) value |= 0x80;
		}
		
   return value;
	}
 
U8 teds_read_byte()
	{
	return teds_write_byte(0xff);
	}

//! @brief Read the ROM section of TEDS memory 
U8 teds_read_rom33(U8 *buffer)
	{
	
	U8 i;
	if (!teds_detect()) return 0;
	
	Disable_interrupt();
   
	teds_write_byte(0x33); //READ ROM
	for (i = 0; i < 8; i++)
		{
		buffer[i] = teds_read_byte();
		}
		
   	Enable_interrupt();
	return 8;
	}

/** @brief Read TEDS memory 
* The sequence is:
*   1. TEDS detect; reset pulse followed by a presence pulse
*	2. Disable_interrupts
*	3. Write byte: 0xCC, SKIP ROM since there is only one. For multiple ROMS this won't be OK(will need to use MATCH ROM).
*	4. Write byte: 0xF0, READ MEMORY; needs two byte memory start address, range 0x0000 to 0x01FF
*	5. Write byte: 0x00, Array Target Address; TA1
*	6. Write byte: 0x00, Sets scratchpad offset; TA2
*   7. Read *length* bytes from the TEDS memory
*/
U8 teds_read_memory(U8 *buffer, U8 length)
	{
	U8 i;
   
	if (!teds_detect()) return 0;
	Disable_interrupt();
	teds_write_byte(0xcc); //SKIP ROM since there is only one. For multiple ROMS this won't be OK(will need to use MATCH ROM).
	teds_write_byte(0xf0); //READ MEMORY; needs two byte memory start address, range 0x0000 to 0x01FF
	teds_write_byte(0x00); //Array Target Address; TA1
	teds_write_byte(0x00); //Sets scratchpad offset; TA2
   
	for (i = 0; i < length; i++)
		{
		buffer[i] = teds_read_byte();
		}
    
	Enable_interrupt();
	
	return length;
	}
	
	//if first byte of global_teds.teds_state.teds_rom33 
	//  -is 0x23 (DS2433) then length is 16 (pages)
	//  -is 0x2D (DS2431) then length is 4 (pages)
	
/** @brief Write TEDS memory 
* The sequence is:
*   1. TEDS detect; reset pulse followed by a presence pulse
*	2. Disable_interrupts
*	3. Write byte: 0xCC, SKIP ROM since there is only one. For multiple ROMS this won't be OK (will need to use MATCH ROM).
*	4. Write byte: 0x0F, WRITE SCRATCHPAD, needs two byte memory start address, range 0x0000 to 0x01FF
*	5. Write byte: 0x00, TA1; Array Target Address; lower byte = 0x00, 0x20, 0x40, 0x60; page 0,1,2,3
*	6. Write byte: 0x00, Sets scratchpad offset; TA2
*   7. Write 32 bytes, *length* times to the TEDS memory
*/
U8 teds_write_memory(U8 *buffer, U8 pageNumber, U8 bLength)
	{
	U8 j;
	U8 scratchpad_buffer[32];
   
	if (!teds_detect()) return 0;
	Disable_interrupt();
	
	teds_write_byte(0xcc); //SKIP ROM since there is only one. For multiple ROMS this won't be OK (will need to use MATCH ROM).
	teds_write_byte(0x0f); 	//WRITE SCRATCHPAD
							//needs two byte memory start address, range 0x0000 to 0x01FF
	teds_write_byte(pageNumber * 0x20); //TA1; Array Target Address; lower byte = 0x00, 0x20, 0x40, 0x60; page 0,1,2,3
	teds_write_byte(0x00); //TA2; Sets scratchpad offset; upper byte. Always 0x00
	
	for (j = 0; j < bLength; j++) // Start index at page offset
		{
			teds_write_byte(buffer[j]); //write 32 bytes
		}
			
	if (!teds_detect()) return 0;
	teds_write_byte(0xcc); //SKIP ROM
		
	teds_write_byte(0xaa); //READ SCRATCHPAD
	for (j = 0; j < 3; j++) 
		{
			scratchpad_buffer[j] = teds_read_byte(); // read scratchpad into scratchpad buffer; really only need the first three bytes
		}
			
	if (!teds_detect()) return 0;
	teds_write_byte(0xcc); //SKIP ROM
	
	teds_write_byte(0x55); //COPY SCRATCHPAD
	for (j = 0; j < 3; j++) //Write first 3 bytes of the SCRATCHPAD, the verification code
		{
			teds_write_byte(scratchpad_buffer[j]); //write TA1, TA2, E/S verification code
		}
		
	Enable_interrupt();
	return scratchpad_buffer[3]; // Return teds memory E/S byte
 }
//! @}
		


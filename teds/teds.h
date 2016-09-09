//! @file teds.h
//!
//! Copyright (c) 2013 Solid-Motion
//!
//! @brief Read/write functions for the TEDS memory. See "TEDS utility library" module
//!
//! @defgroup teds_utility TEDS utility library
//! @{
//! @brief Functions for initializing, reading, and writing the TEDS memory

//! @brief Activate TEDS port as input and enable pull-up
void teds_init();

//! @brief Initiate a teds reset and detect cycle.  Return !0 if detect pulse received.
/*! Sequence:
		1. de-assert
		2. wait tRSTL (>=480us)
		3. active input (pull-up engaged)
		---  after 15-60us ---
		4. device transmits presence pulse (pulls low) for 60-240us
		If detect logic 0 during presence pulse stage, then device is connected
!*/
U8 teds_detect();

//! @brief This function is commented out
U8 teds_read_app(U8 *buffer);

//! @brief Read the TEDS ROM.
//! Read the 8-byte ROM portion of the TEDS memory
//! @param buffer Temp buffer for the contents of memory read
U8 teds_read_rom33(U8 *buffer);

//! @brief This function is commented out
U8 teds_read_status(U8 *statusPtr);

//! @brief Read the TEDS memory.
//! Read the non-ROM portion of the TEDS memory
//! @param buffer Temp buffer for the contents of memory read
//! @param length Size of the memory minus one
U8 teds_read_memory(U8 *buffer, U8 length);

//! @brief Write the TEDS memory.
//! Write the non-ROM portion of the TEDS memory
//! @param buffer Contents of the memory write
//! @param pageNumber Memory page number
//! @param length Size of a memory page
U8 teds_write_memory(U8 *buffer, U8 pageNumber, U8 length);

//! @brief Read a byte from the TEDS memory
U8 teds_read_byte();

//! @brief Write a byte to the TEDS memory
//! @param value The byte to be written to the memory
U8 teds_write_byte(U8 value);

//! @}




//! @file messages.h
//!
//! Copyright (c) 2013 Solid-Motion
//!

//! @defgroup messages HID report structures
//! @brief Defines structures for all inbound and outbound messages
//!
//! It is intended to be included by the
//! device and host programs wishing to exchange messages
//! with the device.
//! It uses fundamental data types:
//!   U8
//!   U16
//!   U32
//!
//! @{
 
#pragma once

/** DEVICE STATES **/

//! @brief DEVICE_STATE_IDLE 
//!
//! The device starts in the idle state. Any operation is allowable from this state.
//!
#define DEVICE_STATE_IDLE 0

//! @brief DEVICE_STATE_READINGTEDS 
//!
//! When a command is received to read TEDS
//! data, the device transitions to this state.  No commands are possible
//! until the device comes out of this state.
//!
#define DEVICE_STATE_READINGTEDS 1

//! @brief DEVICE_STATE_ACQUIRING
//!
//! When the device is actively acquiring data, the
//! only allowable operation is to stop acquiring.  In this state, the device
//! is sending IN reports to the host at a constant interval.
//!
#define DEVICE_STATE_ACQUIRING 2

/**** Report IDs ****/

//! @brief Returns a report_feature_device_state_t structure to the host.
//!
//! This communicates basic TEDS device state and is available from any state.
//!
#define REPORT_FEATURE_QUERY_DEVICE_STATE 1

//! @brief Column definition char for report_feature_device_state_t
typedef struct {
   char dummy;
} report_feature_device_state_t;

//! @brief Returns a report_feature_teds_state_t structure to the host.
//!
//! This gives TEDS overview information including connection state, size, etc.
//! This report can be requested from any device state.
//!
#define REPORT_FEATURE_QUERYTEDS_STATE 2

//! @brief 1x6 char array report_feature_teds_state_t
//!
typedef struct {
   U8 teds_ready;
   U8 teds_sensor_connected;
   U8 teds_data_available;
   U8 teds_app[8];
   U8 teds_rom33[8];
   U8 teds_status;
} report_feature_teds_state_t;

//! @defgroup HID_report HID memory reports
//! @brief Defines structures reading TEDS memory
//!
//! Request pages of TEDS data from the device (if the TEDS_STATE is such
//! that TEDS data has been read).  Each page is 32 bytes long and is
//! addressed with a different report id (0, 1, 2, 3, 4).
//! Each of these returns a  REPORT_FEATURE_QUERYTEDS_PAGE_X structure
//!
#define REPORT_FEATURE_QUERYTEDS_PAGE_0 10
#define REPORT_FEATURE_QUERYTEDS_PAGE_1 11
#define REPORT_FEATURE_QUERYTEDS_PAGE_2 12
#define REPORT_FEATURE_QUERYTEDS_PAGE_3 13


//! @}

//! @brief struct that holds a 32-byte chunk of he TEDS memory
//!
//! U8 32-byte array report_feature_teds_page_t
//!
typedef struct {
   U8 page[32];
} report_feature_teds_page_t;

//! @brief Command to reset/re-read TEDS data.  
//!
//! This command can only be issued
//! from the IDLE device state.  It results in the device temporarily
//! transitioning to the DEVICE_STATE_READINGTEDS state.  The host should
//! query the teds state after sending this command until the TEDS state
//! is TEDS_STATE_READY.
//!
#define REPORT_OUT_CMD_RESET_TEDS 30

#define REPORT_OUT_CMD_DEVICE_RESET 50 

//! @brief Column definition char for report_out_cmd_reset_teds_t
//!
typedef struct {
   char dummy;
} report_out_cmd_reset_teds_t;

//! @brief Column definition char for report_out_cmd_device_reset_t
//!
typedef struct {
   char dummy;
} report_out_cmd_device_reset_t;

//! @}
//! @brief Command to write TEDS data.  
//!
//!
#define REPORT_OUT_WRITETEDS_0 10
#define REPORT_OUT_WRITETEDS_1 20
#define REPORT_OUT_WRITETEDS_2 30
#define REPORT_OUT_WRITETEDS_3 40 

//! @defgroup HID_report HID memory reports
//! @brief Defines for reports
//!
#define REPORTTYPE_INPUT 1
#define REPORTTYPE_OUTPUT 2
#define REPORTTYPE_FEATURE 3
#define REPORT_IDTEDS_STATE 2


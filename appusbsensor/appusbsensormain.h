/** @file appusbsensormain.h
*
* Copyright (c) 2016 Solid-Motion
*
* Use of this program is subject to Solid-Motion End User License Agreement.
* Please read file license.txt for copyright notice.
*
* @brief Solid-Motion file. appusbsensormain header file.
*
* @todo
* @bug
*/

//_____ Structures ______________________________________________

//! @brief Struct for TEDS data, pending read flag, report_feature_teds_state_t, and teds_state structs
typedef struct
	{
	U8 pending_teds_read;
	report_feature_teds_state_t teds_state;
	U8 teds_data[128];
	} global_teds;
	
//_____ Function Declarations ______________________________________

/**
* @brief Called by the main event loop.  If TEDS is not ready or the connect
* state changed, then re-read.
* @param[in]       none
* @return          none
*/
void serviceteds_state(void);

/** @brief TEDS output report
* @li [0] Report ID = 0x02 Output
* @li [1] TEDS ready
* @li [2] TEDS sensor connected
* @li [3] TEDS data available
* @param[in] length of TEDS report
* @return return 1 
*/
U8 get_report_FEATURE_QUERYTEDS_STATE(U8 length);

/** @brief These are the HID report ID's with the TEDS memory data
* @param[in] pageNumber of TEDS report
* @param[in] length of TEDS report
* @return return 1 
*/
U8 get_report_FEATURE_QUERYTEDS_PAGE(U8 pageNumber, U8 length);

/** @brief Write 32 byte pages to TEDS memory one page at a time.
* @li Data sent from device includes the pageNumber
* @li Collect 0-32 in array
* @li Write 1-32 to TEDS
* @param[in] pageNumber of TEDS report
* @param[in] length of TEDS report
* @return 1 
*/
U8 set_report_FEATURE_QUERYTEDS_PAGE(U8 pageNumber, U8 length);	

/** @brief  Reset TEDS	
* @li Data sent from device includes the pageNumber
* @li Collect 0-32 in array
* @li Write 1-32 to TEDS
* @param[in] none
* @return 1 
*/
U8 set_report_cmd_reset(void);		




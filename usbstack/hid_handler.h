/** @file hid_handler.h
*
* Copyright (c) 2013 Solid-Motion
*/

#ifndef __HID_HANDLER_H__
#define __HID_HANDLER_H__

/** @defgroup hid_request HID TEDS report processing
* @brief HID TEDS report processing
* @todo
* - Are SET CONFIGURATION & GET INTERFACE needed?
* - Implement SET IDLE (right now returns STALL instead of ACK)
* - Why does report ID 1 show up twice when USB host issues GET REPORT DESCRIPTOR?
* 
* List of supported Class specific (HID) requests:
* - GET REPORT
* - get_report_FEATURE_QUERYTEDS_STATE. Read TEDS status
* - get_report_FEATURE_QUERYTEDS_PAGE. Read TEDS memory
* - SET REPORT
* - set_report_OUT
* - set_report_FEATURE_QUERYTEDS_PAGE. Write TEDS memory
* - Two standard USB requests can show up in a class request and must be specially handled
* - SET CONFIGURATION
* - GET INTERFACE
*
*See Device Class Definition for Human Interface Devices (HID) 5.1
*@{ 
*/


/** 
* @brief Class specific (HID) request Set Report
*
* Report id is:
* @li 0x30 Reset TEDS
* @li 0x10-0x13 Write memory page to TEDS
* @li 0x50 Reset device. Used after loading firmware to reset device to bootloader mode
* HID Set_Report. Called when an OUT report is received from the host.
* @param reportType 
* @param reportId
* @param length 
* @return  0 if not processed. 1 if successful.
*/
U8 usb_stack_hid_user_set_report(U8 reportType, U8 reportId, U8 length);

/** 
*@brief Class specific (HID) request Get Report
* 
* HID Get_Report. Called when an IN report is requested to be sent to the host.  Return 0 if not processed. !0 if successful.
* @param reportType 
* @param reportId 
* @param length 
*/
U8 usb_stack_hid_user_get_report(U8 reportType, U8 reportId, U8 length);

//void usb_stack_hid_set_report();

U8 set_report_REPORT_OUT_CMD_DEVICE_RESET(void);

/** 
@} 
*/
#endif


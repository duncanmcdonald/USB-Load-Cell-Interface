//! @file hid_macros.h
//!
//! Copyright (c) 2013 Solid-Motion
//!
//! @brief Convenience HID macros for defining reports
//!

#ifndef __HID_MACROS__H__
#define __HID_MACROS__H__

/**
 * Convenience hid macros for defining reports  
 **/
#define HID_REPORT_HEADER() \
    0x06, 0x00, 0xff,              /* USAGE_PAGE (Vendor Defined Page 1) */ \
    0x09, 0x01,                    /* USAGE (Vendor Usage 1) */ \
    0xa1, 0x01                     /* COLLECTION (Application) */
    
#define HID_REPORT_TRAILER() \
    0xc0                           /* END_COLLECTION */

#define HID_GENERIC_INPUT_REPORT(reportId, nbytes) \
    0x85, reportId,                /*   REPORT_ID (1) */ \
	0x09, 0x01,                    /*   USAGE (Vendor Usage 1) */ \
    0x15, 0x00,                    /*   LOGICAL_MINIMUM (0) */ \
    0x26, 0xff, 0x00,              /*   LOGICAL_MAXIMUM (255) */  \
    0x75, 0x08,                    /*   REPORT_SIZE (8) */  \
    0x95, nbytes,                  /*   REPORT_COUNT (63) */   \
    0x81, 0x00                     /*   INPUT (Data,Ary,Abs) */

#define HID_GENERIC_OUTPUT_REPORT(reportId, nbytes) \
    0x85, reportId,                /*   REPORT_ID (1) */ \
    0x09, 0x01,                    /*   USAGE (Vendor Usage 1) */ \
    0x15, 0x00,                    /*   LOGICAL_MINIMUM (0) */ \
    0x26, 0xff, 0x00,              /*   LOGICAL_MAXIMUM (255) */  \
    0x75, 0x08,                    /*   REPORT_SIZE (8) */  \
    0x95, nbytes,                  /*   REPORT_COUNT (63) */   \
    0x91, 0x00                     /*   OUTPUT (Data,Ary,Abs) */

#define HID_GENERIC_FEATURE_REPORT(reportId, nbytes) \
    0x85, reportId,                /*   REPORT_ID (1) */ \
    0x09, 0x01,                    /*   USAGE (Vendor Usage 1) */ \
    0x15, 0x00,                    /*   LOGICAL_MINIMUM (0) */ \
    0x26, 0xff, 0x00,              /*   LOGICAL_MAXIMUM (255) */  \
    0x75, 0x08,                    /*   REPORT_SIZE (8) */  \
    0x95, nbytes,                  /*   REPORT_COUNT (63) */   \
    0xb1, 0x00                     /*   FEATURE (Data,Ary,Abs) */

#endif


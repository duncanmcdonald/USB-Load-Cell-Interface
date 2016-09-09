//! @file usb_settings_i.c
//!
//! Copyright (c) 2013 Solid-Motion
//!
//! @brief HID report descriptors

#include "usb_settings.h"
#include <hid_macros.h>
#include "messages.h"

/** HID Report Descriptor **/
code code U8 usb_hid_report_descriptor[] = {
   HID_REPORT_HEADER(),
   
   /** Feature reports **/
   HID_GENERIC_FEATURE_REPORT(REPORT_FEATURE_QUERY_DEVICE_STATE,
      sizeof(report_feature_device_state_t)),
   HID_GENERIC_FEATURE_REPORT(REPORT_FEATURE_QUERYTEDS_STATE,
      sizeof(report_feature_teds_state_t)),
   HID_GENERIC_FEATURE_REPORT(REPORT_FEATURE_QUERYTEDS_PAGE_0,
      sizeof(report_feature_teds_page_t)),
   HID_GENERIC_FEATURE_REPORT(REPORT_FEATURE_QUERYTEDS_PAGE_1,
      sizeof(report_feature_teds_page_t)),
   HID_GENERIC_FEATURE_REPORT(REPORT_FEATURE_QUERYTEDS_PAGE_2,
      sizeof(report_feature_teds_page_t)),
   HID_GENERIC_FEATURE_REPORT(REPORT_FEATURE_QUERYTEDS_PAGE_3,
      sizeof(report_feature_teds_page_t)),
	  
	    /** IN Reports **/
	HID_GENERIC_INPUT_REPORT(1,63),   //HID_GENERIC_INPUT_REPORT(reportId, nbytes)
   
   /** OUT Reports **/
   HID_GENERIC_OUTPUT_REPORT(REPORT_OUT_CMD_RESET_TEDS,
      sizeof(report_out_cmd_reset_teds_t)),
   HID_GENERIC_OUTPUT_REPORT(REPORT_OUT_CMD_DEVICE_RESET,
      sizeof(report_out_cmd_device_reset_t)),
   
   HID_REPORT_TRAILER()
};


/** Definitions **/
// usb_user_device_descriptor
code S_usb_device_descriptor usb_dev_desc =
{
  sizeof(usb_dev_desc)
, DEVICE_DESCRIPTOR
, Usb_write_word_enum_struc(USB_SPECIFICATION)
, DEVICE_CLASS
, DEVICE_SUB_CLASS
, DEVICE_PROTOCOL
, EP_CONTROL_LENGTH
, Usb_write_word_enum_struc(VENDOR_ID)
, Usb_write_word_enum_struc(PRODUCT_ID)
, Usb_write_word_enum_struc(RELEASE_NUMBER)
, MAN_INDEX
, PROD_INDEX
, SN_INDEX
, NB_CONFIGURATION
};

// usb_user_configuration_descriptor FS
code S_usb_user_configuration_descriptor usb_conf_desc = {
 { sizeof(S_usb_configuration_descriptor)
 , CONFIGURATION_DESCRIPTOR
 , Usb_write_word_enum_struc(
       sizeof(S_usb_configuration_descriptor)
       +sizeof(S_usb_interface_descriptor)
       +sizeof(S_usb_endpoint_descriptor)
       //+sizeof(S_usb_endpoint_descriptor)
       +sizeof(S_usb_hid_descriptor))
 , NB_INTERFACE
 , CONF_NB
 , CONF_INDEX
 , CONF_ATTRIBUTES
 , MAX_POWER
 }
 ,
 { sizeof(S_usb_interface_descriptor)
 , INTERFACE_DESCRIPTOR
 , INTERFACE_NB
 , ALTERNATE
 , NB_ENDPOINT
 , INTERFACE_CLASS
 , INTERFACE_SUB_CLASS
 , INTERFACE_PROTOCOL
 , INTERFACE_INDEX
 }
 ,
 { 
    sizeof(S_usb_hid_descriptor),
    HID_DESCRIPTOR,
    HID_BDC,
    HID_COUNTRY_CODE,
    HID_CLASS_DESC_NB,  // Number of subordinate report and physical descriptors
    HID_DESCRIPTORTYPE,
    //Usb_write_word_enum_struc(sizeof
    sizeof(usb_hid_report_descriptor)
 }
 //{9, HID, 0x0111, 8, 1, REPORT, SIZE_OF_REPORT }
 ,
 { sizeof(S_usb_endpoint_descriptor)
 , ENDPOINT_DESCRIPTOR
 , ENDPOINT_NB_1
 , EP_ATTRIBUTES_1
 , Usb_write_word_enum_struc(EP_SIZE_1)
 , EP_INTERVAL_1
 }
 
 /*,
 { sizeof(S_usb_endpoint_descriptor)
 , ENDPOINT_DESCRIPTOR
 , ENDPOINT_NB_2
 , EP_ATTRIBUTES_2
 , Usb_write_word_enum_struc(EP_SIZE_2)
 , EP_INTERVAL_2
 }
 */
};



                                      // usb_user_manufacturer_string_descriptor
code S_usb_manufacturer_string_descriptor usb_user_manufacturer_string_descriptor = {
  sizeof(usb_user_manufacturer_string_descriptor)
, STRING_DESCRIPTOR
, USB_MANUFACTURER_NAME
};


                                      // usb_user_product_string_descriptor

code S_usb_product_string_descriptor usb_user_product_string_descriptor = {
  sizeof(usb_user_product_string_descriptor)
, STRING_DESCRIPTOR
, USB_PRODUCT_NAME
};


                                      // usb_user_serial_number

code S_usb_serial_number usb_user_serial_number = {
  sizeof(usb_user_serial_number)
, STRING_DESCRIPTOR
, USB_SERIAL_NUMBER
};


                                      // usb_user_language_id

code S_usb_language_id usb_user_language_id = {
  sizeof(usb_user_language_id)
, STRING_DESCRIPTOR
, Usb_write_word_enum_struc(LANGUAGE_ID)
};


static inline void user_populate_descriptor(data_buffer_t* data_buffer, U8 requesttype, U8 descriptortype, U8 stringtype) {
   data_buffer->type=DBTYPE_LPM;
   
   if (requesttype==REQUEST_DEVICE_STATUS) {
      // Handle device level descriptor requests
      switch (descriptortype) {
      case DEVICE_DESCRIPTOR:
         data_buffer->length=sizeof(usb_dev_desc);
         data_buffer->pointer.lpm=(PGM_P)&usb_dev_desc;
         return;
      case CONFIGURATION_DESCRIPTOR:
         data_buffer->length=sizeof(usb_conf_desc);
         data_buffer->pointer.lpm=(PGM_P)&usb_conf_desc;
         return;
      }
      
      // Other descriptor?
      if (descriptortype==STRING_DESCRIPTOR) {
         switch (stringtype) {
         case LANG_ID:
            data_buffer->length=sizeof(usb_user_language_id);
            data_buffer->pointer.lpm=(PGM_P)&usb_user_language_id;
            return;
         case MAN_INDEX:
            data_buffer->length=sizeof(usb_user_manufacturer_string_descriptor);
            data_buffer->pointer.lpm=(PGM_P)&usb_user_manufacturer_string_descriptor;
            return;
         case PROD_INDEX:
            data_buffer->length=sizeof(usb_user_product_string_descriptor);
            data_buffer->pointer.lpm=(PGM_P)&usb_user_product_string_descriptor;
            return;
         case SN_INDEX:
            data_buffer->length=sizeof(usb_user_serial_number);
            data_buffer->pointer.lpm=(PGM_P)&usb_user_serial_number;
            return;
         }
      }
   } else if (requesttype==REQUEST_INTERFACE_STATUS) {
      // Handle requests for interface descriptors
      switch (descriptortype) {
      case REPORT:
         data_buffer->length = sizeof(usb_hid_report_descriptor);
         data_buffer->pointer.lpm=(PGM_P)&(usb_hid_report_descriptor[0]);
         return;
         
      case HID:
         data_buffer->length = sizeof(usb_conf_desc.hid);
         data_buffer->pointer.lpm = (PGM_P)&(usb_conf_desc.hid.bLength);
         return;
      }
   }
   
   data_buffer->type=0;
}

#define usb_stack_user_populate_descriptor(data_buffer, requesttype, descriptortype, stringtype) \
   user_populate_descriptor(&data_buffer, requesttype, descriptortype, stringtype)


void usb_stack_user_endpoint_init(U8 conf_nbr) {
  usb_configure_endpoint(EP_HID_IN,
                         TYPE_INTERRUPT,
                         DIRECTION_IN,
                         SIZE_64,
                         ONE_BANK,
                         NYET_ENABLED);

  /*
  usb_configure_endpoint(EP_HID_OUT,
                         TYPE_INTERRUPT,
                         DIRECTION_OUT,
                         SIZE_64,
                         ONE_BANK,
                         NYET_ENABLED);
  */
}


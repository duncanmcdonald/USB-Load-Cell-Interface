//! @file usb_settings.h
//!
//! Copyright (c) 2013 Solid-Motion
//!
//! @brief USB descriptors

#ifndef __USB_SETTINGS_H__
#define __USB_SETTINGS_H__

#define Usb_unicode(a)			((U16)(a))

#define EP_HID_IN 1
#define EP_HID_OUT 2

//! @defgroup usb_descriptors USB descriptors
//! @brief Chap 9, Table 9-5 USB descriptor types
//!
//!- Device descriptor
//!- Configuration descriptors
//!- String descriptors
//!- Interface descriptors
//!- Endpoint descriptors
//!- Device qualifier descriptor
//!- HID specific descriptors
//! @{

#define USB_SPECIFICATION     0x0200
//! each configuration has its own class
#define DEVICE_CLASS          0   
//! each configuration has its own sub-class   
#define DEVICE_SUB_CLASS      0  
//! each configuration has its own protocol    
#define DEVICE_PROTOCOL       0      
#define EP_CONTROL_LENGTH     64
//! Solid motion vendor id
#define VENDOR_ID             0x1D07 
#define PRODUCT_ID            0x0010
#define RELEASE_NUMBER        0x1000
#define MAN_INDEX             0x01
#define PROD_INDEX            0x02	
#define SN_INDEX              0x03
#define NB_CONFIGURATION      1

//! The number of interface for this configuration
#define NB_INTERFACE       1 
//! Number of this configuration    
#define CONF_NB            1     
#define CONF_INDEX         0
#define CONF_ATTRIBUTES    USB_CONFIG_BUSPOWERED
//! 500 mA max, units of 2mA
#define MAX_POWER          250    

// USB Interface descriptor gen
//! The number of this interface
#define INTERFACE_NB        0        
//! The alt settting nb of this interface
#define ALTERNATE           0        

//! The number of endpoints this this interface have
#define NB_ENDPOINT         1       
//! HID Class 
#define INTERFACE_CLASS     0x03  
//! Generic HID   
#define INTERFACE_SUB_CLASS 0x00  
//! Protocol   
#define INTERFACE_PROTOCOL  0x00     
#define INTERFACE_INDEX     0

// USB Endpoint 1 descriptor FS
#define ENDPOINT_NB_1       (EP_HID_IN | 0x80)
//! BULK = 0x02, INTERUPT = 0x03
#define EP_ATTRIBUTES_1     0x03              
#define EP_IN_LENGTH        64
#define EP_SIZE_1           EP_IN_LENGTH
//! Interrupt polling interval from host
#define EP_INTERVAL_1       20 				 

// USB Endpoint 2 descriptor FS
#define ENDPOINT_NB_2       (EP_HID_OUT | 0x00)
//! BULK = 0x02, INTERUPT = 0x03
#define EP_ATTRIBUTES_2     0x03              
#define EP_OUT_LENGTH       64
#define EP_SIZE_2           EP_OUT_LENGTH
//! Interrupt polling interval from host
#define EP_INTERVAL_2       20 				 

#define LENGTH_OF_REPORT_IN      0x3F
#define LENGTH_OF_REPORT_OUT     0x3F

#define DEVICE_STATUS         0x00
//! TBD
#define INTERFACE_STATUS      0x00 

#define LANG_ID               0x00

/* HID specific */
#define HID                   0x21
#define REPORT                0x22
#define SET_REPORT			  0x02
#define SET_HID_REPORT        0x03


#define HID_DESCRIPTOR        0x21
#define HID_BDC               0x1001
#define HID_COUNTRY_CODE      0x00
#define HID_CLASS_DESC_NB     0x01
#define HID_DESCRIPTORTYPE   0x22


#define USB_MN_LENGTH         12
#define USB_MANUFACTURER_NAME \
{ Usb_unicode('S') \
, Usb_unicode('o') \
, Usb_unicode('l') \
, Usb_unicode('i') \
, Usb_unicode('d') \
, Usb_unicode('-') \
, Usb_unicode('M') \
, Usb_unicode('o') \
, Usb_unicode('t') \
, Usb_unicode('i') \
, Usb_unicode('o') \
, Usb_unicode('n') \
}

#define USB_PN_LENGTH         0x21
#define USB_PRODUCT_NAME \
{ Usb_unicode('S') \
, Usb_unicode('o') \
, Usb_unicode('l') \
, Usb_unicode('i') \
, Usb_unicode('d') \
, Usb_unicode('-') \
, Usb_unicode('M') \
, Usb_unicode('o') \
, Usb_unicode('t') \
, Usb_unicode('i') \
, Usb_unicode('o') \
, Usb_unicode('n') \
, Usb_unicode(' ') \
 ,Usb_unicode('U') \
 ,Usb_unicode('S') \
 ,Usb_unicode('B') \
 ,Usb_unicode(' ') \
 ,Usb_unicode('S') \
 ,Usb_unicode('m') \
 ,Usb_unicode('a') \
 ,Usb_unicode('r') \
 ,Usb_unicode('t') \
 ,Usb_unicode(' ') \
 ,Usb_unicode('S') \
 ,Usb_unicode('e') \
 ,Usb_unicode('n') \
 ,Usb_unicode('s') \
 ,Usb_unicode('o') \
 ,Usb_unicode('r') \
 ,Usb_unicode(' ') \
 ,Usb_unicode('A') \
 ,Usb_unicode('D') \
 ,Usb_unicode('C') \
}

#define USB_SN_LENGTH         0x03
//! Serial Number should be at least 12 characters long
#define USB_SERIAL_NUMBER \
{ Usb_unicode('1') \
 ,Usb_unicode('7') \
 ,Usb_unicode('8') \
}

#define LANGUAGE_ID           0x0409


//! Usb Device Descriptor
typedef struct {
   U8      bLength;              //!< Size of this descriptor in bytes
   U8      bDescriptorType;      //!< DEVICE descriptor type
   U16     bscUSB;               //!< Binay Coded Decimal Spec. release
   U8      bDeviceClass;         //!< Class code assigned by the USB
   U8      bDeviceSubClass;      //!< Sub-class code assigned by the USB
   U8      bDeviceProtocol;      //!< Protocol code assigned by the USB
   U8      bMaxPacketSize0;      //!< Max packet size for EP0
   U16     idVendor;             //!< Vendor ID. ATMEL = 0x03EB
   U16     idProduct;            //!< Product ID assigned by the manufacturer
   U16     bcdDevice;            //!< Device release number
   U8      iManufacturer;        //!< Index of manu. string descriptor
   U8      iProduct;             //!< Index of prod. string descriptor
   U8      iSerialNumber;        //!< Index of S.N.  string descriptor
   U8      bNumConfigurations;   //!< Number of possible configurations
}  S_usb_device_descriptor;


//! Usb Configuration Descriptor
typedef struct {
   U8      bLength;              //!< size of this descriptor in bytes
   U8      bDescriptorType;      //!< CONFIGURATION descriptor type
   U16     wTotalLength;         //!< total length of data returned
   U8      bNumInterfaces;       //!< number of interfaces for this conf.
   U8      bConfigurationValue;  //!< value for SetConfiguration resquest
   U8      iConfiguration;       //!< index of string descriptor
   U8      bmAttibutes;          //!< Configuration characteristics
   U8      MaxPower;             //!< maximum power consumption
}  S_usb_configuration_descriptor;


//! Usb Interface Descriptor
typedef struct {
   U8      bLength;               //!< size of this descriptor in bytes
   U8      bDescriptorType;       //!< INTERFACE descriptor type
   U8      bInterfaceNumber;      //!< Number of interface
   U8      bAlternateSetting;     //!< value to select alternate setting
   U8      bNumEndpoints;         //!< Number of EP except EP 0
   U8      bInterfaceClass;       //!< Class code assigned by the USB
   U8      bInterfaceSubClass;    //!< Sub-class code assigned by the USB
   U8      bInterfaceProtocol;    //!< Protocol code assigned by the USB
   U8      iInterface;            //!< Index of string descriptor
}  S_usb_interface_descriptor;


//! Usb Endpoint Descriptor
typedef struct {
   U8      bLength;               //!< Size of this descriptor in bytes
   U8      bDescriptorType;       //!< ENDPOINT descriptor type
   U8      bEndpointAddress;      //!< Address of the endpoint
   U8      bmAttributes;          //!< Endpoint's attributes
   U16     wMaxPacketSize;        //!< Maximum packet size for this EP
   U8      bInterval;             //!< Interval for polling EP in ms
} S_usb_endpoint_descriptor;


//! Usb Device Qualifier Descriptor
typedef struct {
   U8      bLength;               //!< Size of this descriptor in bytes
   U8      bDescriptorType;       //!< Device Qualifier descriptor type
   U16     bscUSB;                //!< Binary Coded Decimal Spec. release
   U8      bDeviceClass;          //!< Class code assigned by the USB
   U8      bDeviceSubClass;       //!< Sub-class code assigned by the USB
   U8      bDeviceProtocol;       //!< Protocol code assigned by the USB
   U8      bMaxPacketSize0;       //!< Max packet size for EP0
   U8      bNumConfigurations;    //!< Number of possible configurations
   U8      bReserved;             //!< Reserved for future use, must be zero
}  S_usb_device_qualifier_descriptor;


//! Usb Language Descriptor
typedef struct {
   U8      bLength;               //!< size of this descriptor in bytes
   U8      bDescriptorType;       //!< STRING descriptor type
   U16     wlangid;               //!< language id
}  S_usb_language_id;


//! Manufacturer string descriptor
typedef struct {
   U8  bLength;               //! size of this descriptor in bytes
   U8  bDescriptorType;       //! STRING descriptor type
   U16 wstring[USB_MN_LENGTH];//! unicode characters
} S_usb_manufacturer_string_descriptor;


//! Product string descriptor
typedef struct {
   U8  bLength;               //! size of this descriptor in bytes
   U8  bDescriptorType;       //! STRING descriptor type
   U16 wstring[USB_PN_LENGTH];//! unicode characters
} S_usb_product_string_descriptor;


//! Serial number descriptor
typedef struct {
   U8  bLength;               //! size of this descriptor in bytes
   U8  bDescriptorType;       //! STRING descriptor type
   U16 wstring[USB_SN_LENGTH];//! unicode characters
} S_usb_serial_number;


//! HID descriptor
typedef struct {
  U8  bLength;               //! Size of this descriptor in bytes */
  U8  bDescriptorType;       //! HID descriptor type */
  U16 bscHID;                //! Binay Coded Decimal Spec. release */
  U8  bCountryCode;          //! Hardware target country */
  U8  bNumDescriptors;       //! Number of HID class descriptors to follow */
  U8  bRDescriptorType;      //! Report descriptor type */
  U16 wDescriptorLength;     //! Total length of Report descriptor */
} S_usb_hid_descriptor;

//! Configuration descriptor
typedef struct {
  S_usb_configuration_descriptor cfg;
  S_usb_interface_descriptor     ifc;
  S_usb_hid_descriptor           hid;
  S_usb_endpoint_descriptor      ep1;
  S_usb_endpoint_descriptor      ep2;
} S_usb_user_configuration_descriptor;

//! @}
#endif


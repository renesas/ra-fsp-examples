/***********************************************************************************************************************
* File Name    : rm_usbx_prndis_descriptor.c
* Description  : Contains data structures and macros used in the initialize USBX RNDIS class
**********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#include "r_usb_basic.h"
#include "r_usb_basic_api.h"
#include "r_usb_basic_cfg.h"

/******************************************************************************
 Macro definitions
 ******************************************************************************/
/* bcdUSB */
#define USB_BCDNUM                 (0x0110U)
/* DCP max packet size */
#define USB_DCPMAXP                (0x40U)
/* Vendor ID */
#define USB_VENDORID               (0x045bU)
/* Product ID */
#define USB_PRODUCTID              (0x1127U)
/* Release Number */
#define USB_RELEASE                (0x0100U)
/* Configuration number */
#define USB_CONFIGNUM              (0x01U)
/* Interface Association Descriptor Type */
#define USB_IAD_TYPE               (0x0bU)

/* Descriptor type  Define */
#define USB_DT_DEVICE              (0x01U)   ///< Device Descriptor
#define USB_DT_CONFIGURATION       (0x02U)   ///< Configuration Descriptor
#define USB_DT_STRING              (0x03U)   ///< String Descriptor
#define USB_DT_INTERFACE           (0x04U)   ///< Interface Descriptor
#define USB_DT_ENDPOINT            (0x05U)   ///< Endpoint Descriptor
#define USB_DT_DEVICE_QUALIFIER    (0x06U)   ///< Device Qualifier Descriptor
#define USB_DT_OTHER_SPEED_CONF    (0x07U)   ///< Other Speed Configuration Descriptor
#define USB_DT_INTERFACE_POWER     (0x08U)   ///< Interface Power Descriptor
#define USB_DT_OTGDESCRIPTOR       (0x09U)   ///< OTG Descriptor
#define USB_DT_HUBDESCRIPTOR       (0x29U)   ///< HUB descriptor

/* Descriptor length */
#define USB_PCDC_QD_LEN            (0x0aU)
#define USB_PCDC_CD1_LEN           (0x4bU)
#define STRING_DESCRIPTOR0_LEN     (4U)
#define STRING_DESCRIPTOR1_LEN     (16U)
#define STRING_DESCRIPTOR2_LEN     (44U)
#define STRING_DESCRIPTOR3_LEN     (46U)
#define STRING_DESCRIPTOR4_LEN     (22U)
#define STRING_DESCRIPTOR5_LEN     (18U)
#define STRING_DESCRIPTOR6_LEN     (28U)

/* Descriptor data Mask */
#define USB_UCHAR_MAX              (0xffU)
#define USB_W_TOTAL_LENGTH_MASK    (256U)
#define USB_W_MAX_PACKET_SIZE_MASK (64U)

/* Communications Class Subclass Codes */
#define USB_PCDC_CLASS_SUBCLASS_CODE_ABS_CTR_MDL        (0x02U)
/* Class-Specific Configuration Descriptors */
#define USB_PCDC_CS_INTERFACE                           (0x24U)

/* bDescriptor SubType in Communications Class Functional Descriptors */
/* Header Functional Descriptor */
#define USB_PCDC_DT_SUBTYPE_HEADER_FUNC                 (0x00U)
/* Call Management Functional Descriptor. */
#define USB_PCDC_DT_SUBTYPE_CALL_MANAGE_FUNC            (0x01U)
/* Abstract Control Management Functional Descriptor. */
#define USB_PCDC_DT_SUBTYPE_ABSTRACT_CTR_MANAGE_FUNC    (0x02U)
/* Union Functional Descriptor */
#define USB_PCDC_DT_SUBTYPE_UNION_FUNC                  (0x06U)

/******************************************************************************
 Private global variables and functions
 ******************************************************************************/

/******************************************************************************
 Exported global variables
 ******************************************************************************/

/******************************************************************************
 Exported global functions (to be accessed by other files)
 ******************************************************************************/
/* Standard Device Descriptor */
#define DEVICE_FRAME_LENGTH_FULL_SPEED (93U)
uint8_t g_device_framework_full_speed[DEVICE_FRAME_LENGTH_FULL_SPEED] =
{
    USB_DD_BLENGTH,                                             /*  0:bLength */
    USB_DT_DEVICE,                                              /*  1:bDescriptorType */
    (USB_BCDNUM & (uint8_t) USB_UCHAR_MAX),                     /*  2:bcdUSB_lo */
    ((uint8_t) (USB_BCDNUM >> 8) & (uint8_t) USB_UCHAR_MAX),    /*  3:bcdUSB_hi */
#ifdef EXPRESS_LOGIC_ORIGINAL
    USB_IFCLS_CDCC,
    0x00,
    0x00,
#else
    0xEF, /* 4 bDeviceClass    : Device Class */
    0x02, /* 5 bDeviceSubClass : Common Class(0x02) */
    0x01, /* 6 bDeviceProtocol :Interface Association Descriptor(IAD) */
#endif
    (uint8_t) USB_DCPMAXP,                                      /*  7:bMAXPacketSize(for DCP) */
    (USB_VENDORID & (uint8_t) USB_UCHAR_MAX),                   /*  8:idVendor_lo */
    ((uint8_t) (USB_VENDORID >> 8) & (uint8_t) USB_UCHAR_MAX),  /*  9:idVendor_hi */
    ((uint16_t) USB_PRODUCTID & (uint8_t) USB_UCHAR_MAX),       /* 10:idProduct_lo */
    ((uint8_t) (USB_PRODUCTID >> 8) & (uint8_t) USB_UCHAR_MAX), /* 11:idProduct_hi */
    (USB_RELEASE & (uint8_t) USB_UCHAR_MAX),                    /* 12:bcdDevice_lo */
    ((uint8_t) (USB_RELEASE >> 8) & (uint8_t) USB_UCHAR_MAX),   /* 13:bcdDevice_hi */
    0x01,                                                       /* 14:iManufacturer */
    0x02,                                                       /* 15:iProduct */
    0x03,                                                       /* 16:iSerialNumber */
    USB_CONFIGNUM,                                              /* 17:bNumConfigurations */

    USB_CD_BLENGTH,                                             /*  0:bLength */
    USB_SOFT_CHANGE,                                            /*  1:bDescriptorType */
    USB_PCDC_CD1_LEN % USB_W_TOTAL_LENGTH_MASK,                 /*  2:wTotalLength(L) */
    USB_PCDC_CD1_LEN / USB_W_TOTAL_LENGTH_MASK,                 /*  3:wTotalLength(H) */
    0x02,                                                       /*  4:bNumInterfaces */
    0x01,                                                       /*  5:bConfigurationValue */
    0x00,                                                       /*  6:iConfiguration */
    USB_CF_RESERVED | USB_CF_SELFP,                             /*  7:bmAttributes */
    (10 / 2),                                                   /*  8:MAXPower (2mA unit) */

    /* Interface Association Descriptor (IAD) */
    0x08,                                                       /*  0:bLength */
    USB_IAD_TYPE,                                               /*  1:bDescriptorType */
    0x00,                                                       /*  2:bFirstInterface */
    0x02,                                                       /*  3:bInterfaceCount */
    USB_IFCLS_CDCC,                                             /*  4:bFunctionClass  */
#ifdef EXPRESS_LOGIC_ORIGINAL
    USB_PCDC_CLASS_SUBCLASS_CODE_ABS_CTR_MDL,                   /* 5:bFunctionSubClass */
#else
    0x00,                                                       /* 5:bFunctionSubClass */
#endif
    0x00,                                                       /*  6:bFunctionProtocol */
    0x00,                                                       /*  7:iFunction */

    /* Interface Descriptor */
    USB_ID_BLENGTH,                                             /*  0:bLength */
    USB_DT_INTERFACE,                                           /*  1:bDescriptor */
    0x00,                                                       /*  2:bInterfaceNumber */
    0x00,                                                       /*  3:bAlternateSetting */
    0x01,                                                       /*  4:bNumEndpoints */
    USB_IFCLS_CDCC,                                             /*  5:bInterfaceClass */
#ifdef EXPRESS_LOGIC_ORIGINAL
    USB_PCDC_CLASS_SUBCLASS_CODE_ABS_CTR_MDL
#else
    0x02,                                                       /*  5:bInterfaceClass */
#endif
    0x00,                                                       /*  7:bInterfaceProtocol */
    0x00,                                                       /*  8:iInterface */

    /* Header Functional Descriptor */
    0x05,                                                       /*  0:bLength */
    USB_PCDC_CS_INTERFACE,                                      /*  1:bDescriptorType */
    USB_PCDC_DT_SUBTYPE_HEADER_FUNC,                            /*  2:bDescriptorSubtype */
    (USB_BCDNUM & (uint8_t) USB_UCHAR_MAX),                     /*  2:bcdUSB_lo */
    ((uint8_t) (USB_BCDNUM >> 8) & (uint8_t) USB_UCHAR_MAX),    /*  3:bcdUSB_hi */

    /* ACM Functional Descriptor */
    0x04,                                                       /*  0:bLength */
    USB_PCDC_CS_INTERFACE,                                      /*  1:bDescriptorType */
    USB_PCDC_DT_SUBTYPE_ABSTRACT_CTR_MANAGE_FUNC,               /*  2:bDescriptorSubtype */
    0x00,                                                       /*  3:bmCapabilities */

    /* Union Functional Descriptor */
    0x05,                                                       /*  0:bLength */
    USB_PCDC_CS_INTERFACE,                                      /*  1:bDescriptorType */
    USB_PCDC_DT_SUBTYPE_UNION_FUNC,                             /*  2:bDescriptorSubtype */
    0x00,                                                       /*  3:bMasterInterface */
    0x01,                                                       /*  4:bSlaveInterface0 */

    /* Call Management Functional Descriptor */
    0x05,                                                       /*  0:bLength */
    USB_PCDC_CS_INTERFACE,                                      /*  1:bDescriptorType */
    USB_PCDC_DT_SUBTYPE_CALL_MANAGE_FUNC,                       /*  2:bDescriptorSubtype */
    /* D1:1-Device can send/receive call management
     information over a Data Class interface. */
    /* D0:1-Device handles call management itself. */
    0x00,                                                       /*  3:bmCapabilities */
    0x01,                                                       /*  4:bDataInterface */

    /* Endpoint Descriptor 0 */
    0x07,                                                       /*  0:bLength */
    USB_DT_ENDPOINT,                                            /*  1:bDescriptorType */
    USB_EP_IN | USB_EP3,                                        /*  2:bEndpointAddress */
    USB_EP_INT,                                                 /*  3:bmAttribute */
    0x08,                                                       /*  4:wMAXPacketSize_lo */
    0x00,                                                       /*  5:wMAXPacketSize_hi */
    0xFF,                                                       /*  6:bInterval */

    /* Data Class Interface Descriptor Requirement */
    USB_ID_BLENGTH,                                             /*  0:bLength */
    USB_DT_INTERFACE,                                           /*  1:bDescriptor */
    0x01,                                                       /*  2:bInterfaceNumber */
    0x00,                                                       /*  3:bAlternateSetting */
    0x02,                                                       /*  4:bNumEndpoints */
    USB_IFCLS_CDCD,                                             /*  5:bInterfaceClass */
    0x00,                                                       /*  6:bInterfaceSubClass */
    0x00,                                                       /*  7:bInterfaceProtocol */
    0x00,                                                       /*  8:iInterface */

    /* Endpoint Descriptor 0 */
    USB_ED_BLENGTH,             /*  0:bLength */
    USB_DT_ENDPOINT,            /*  1:bDescriptorType */
    USB_EP_IN | USB_EP1,       /*  2:bEndpointAddress */
    USB_EP_BULK,                /*  3:bmAttribute */
    USB_W_MAX_PACKET_SIZE_MASK, /*  4:wMAXPacketSize_lo */
    0,                          /*  5:wMAXPacketSize_hi */
    0x00,                          /*  6:bInterval */

    /* Endpoint Descriptor 1 */
    USB_ED_BLENGTH,             /*  0:bLength */
    USB_DT_ENDPOINT,            /*  1:bDescriptorType */
    USB_EP_OUT | USB_EP2,        /*  2:bEndpointAddress */
    USB_EP_BULK,                /*  3:bmAttribute */
    USB_W_MAX_PACKET_SIZE_MASK, /*  4:wMAXPacketSize_lo */
    0,                          /*  5:wMAXPacketSize_hi */
    0x00,                          /*  6:bInterval */
};
#define DEVICE_FRAME_LENGTH_HI_SPEED (93U)
uint8_t g_device_framework_hi_speed[DEVICE_FRAME_LENGTH_HI_SPEED] =
{
    USB_DD_BLENGTH,                                     /*  0:bLength */
    USB_DT_DEVICE,                                      /*  1:bDescriptorType */
    (USB_BCDNUM & (uint8_t) USB_UCHAR_MAX),                     /*  2:bcdUSB_lo */
    ((uint8_t) (USB_BCDNUM >> 8) & (uint8_t) USB_UCHAR_MAX),    /*  3:bcdUSB_hi */
#ifdef EXPRESS_LOGIC_ORIGINAL
    USB_IFCLS_CDCC,
    0x00,
    0x01,
#else
    0xEF, /* 4 bDeviceClass    : Device Class */
    0x02, /* 5 bDeviceSubClass : Common Class(0x02) */
    0x01, /* 6 bDeviceProtocol :Interface Association Descriptor(IAD) */
#endif
    (uint8_t) USB_DCPMAXP,                              /*  7:bMAXPacketSize(for DCP) */
    (USB_VENDORID & (uint8_t) USB_UCHAR_MAX),                   /*  8:idVendor_lo */
    ((uint8_t) (USB_VENDORID >> 8) & (uint8_t) USB_UCHAR_MAX),  /*  9:idVendor_hi */
    ((uint16_t) USB_PRODUCTID & (uint8_t) USB_UCHAR_MAX),       /* 10:idProduct_lo */
    ((uint8_t) (USB_PRODUCTID >> 8) & (uint8_t) USB_UCHAR_MAX), /* 11:idProduct_hi */
    (USB_RELEASE & (uint8_t) USB_UCHAR_MAX),                    /* 12:bcdDevice_lo */
    ((uint8_t) (USB_RELEASE >> 8) & (uint8_t) USB_UCHAR_MAX),   /* 13:bcdDevice_hi */
    0x01,                                                  /* 14:iManufacturer */
    0x02,                                                  /* 15:iProduct */
    0x03,                                                  /* 16:iSerialNumber */
    USB_CONFIGNUM,                                      /* 17:bNumConfigurations */

    USB_CD_BLENGTH,                 /*  0:bLength */
    USB_SOFT_CHANGE,                /*  1:bDescriptorType */
    USB_PCDC_CD1_LEN % USB_W_TOTAL_LENGTH_MASK, /*  2:wTotalLength(L) */
    USB_PCDC_CD1_LEN / USB_W_TOTAL_LENGTH_MASK, /*  3:wTotalLength(H) */
    2,                              /*  4:bNumInterfaces */
    1,                              /*  5:bConfigurationValue */
    0,                              /*  6:iConfiguration */
    USB_CF_RESERVED | USB_CF_SELFP, /*  7:bmAttributes */
    (10 / 2),                       /*  8:MAXPower (2mA unit) */

    /* Interface Association Descriptor (IAD) */
    0x08,                           /*  0:bLength */
    USB_IAD_TYPE,                   /*  1:bDescriptorType */
    0x00,                           /*  2:bFirstInterface */
    0x02,                           /*  3:bInterfaceCount */
    USB_IFCLS_CDCC,                 /*  4:bFunctionClass  */
#ifdef EXPRESS_LOGIC_ORIGINAL
    USB_PCDC_CLASS_SUBCLASS_CODE_ABS_CTR_MDL,                   /* 5:bFunctionSubClass */
#else
    0x00,                                                       /* 5:bFunctionSubClass */
#endif
    0x00,                           /*  6:bFunctionProtocol */
    0x00,                           /*  7:iFunction */

    /* Interface Descriptor */
    9,                              /*  0:bLength */
    USB_DT_INTERFACE,               /*  1:bDescriptor */
    0x00,                              /*  2:bInterfaceNumber */
    0x00,                              /*  3:bAlternateSetting */
    0x01,                              /*  4:bNumEndpoints */
    USB_IFCLS_CDCC,                 /*  5:bInterfaceClass */
    USB_PCDC_CLASS_SUBCLASS_CODE_ABS_CTR_MDL,   /*  6:bInterfaceSubClass */
    0x00,                              /*  7:bInterfaceProtocol */
    0x00,                              /*  8:iInterface */

    /* Communications Class Functional Descriptorss */
    5, /*  0:bLength */
    USB_PCDC_CS_INTERFACE,      /*  1:bDescriptorType */
    USB_PCDC_DT_SUBTYPE_HEADER_FUNC,    /*  2:bDescriptorSubtype */
    (USB_BCDNUM & (uint8_t) USB_UCHAR_MAX),                     /*  2:bcdUSB_lo */
    ((uint8_t) (USB_BCDNUM >> 8) & (uint8_t) USB_UCHAR_MAX),    /*  3:bcdUSB_hi */

    /* Communications Class Functional Descriptorss */
    4,                          /*  0:bLength */
    USB_PCDC_CS_INTERFACE,      /*  1:bDescriptorType */
    USB_PCDC_DT_SUBTYPE_ABSTRACT_CTR_MANAGE_FUNC,   /*  2:bDescriptorSubtype */
    2,                          /*  3:bmCapabilities */

    /* Communications Class Functional Descriptorss */
    5,                          /*  0:bLength */
    USB_PCDC_CS_INTERFACE,      /*  1:bDescriptorType */
    USB_PCDC_DT_SUBTYPE_UNION_FUNC, /*  2:bDescriptorSubtype */
    0,                          /*  3:bMasterInterface */
    1,                          /*  4:bSlaveInterface0 */

    /* Communications Class Functional Descriptorss */
    5,                          /*  0:bLength */
    USB_PCDC_CS_INTERFACE,      /*  1:bDescriptorType */
    USB_PCDC_DT_SUBTYPE_CALL_MANAGE_FUNC,   /*  2:bDescriptorSubtype */
    /* D1:1-Device can send/receive call management
     information over a Data Class interface. */
    /* D0:1-Device handles call management itself. */
    0x00,                          /*  3:bmCapabilities */
    0x01,                          /*  4:bDataInterface */

    /* Endpoint Descriptor 0 */
    7,                          /*  0:bLength */
    USB_DT_ENDPOINT,            /*  1:bDescriptorType */
    USB_EP_IN | USB_EP3,        /*  2:bEndpointAddress */
    USB_EP_INT,                 /*  3:bmAttribute */
    16,                         /*  4:wMAXPacketSize_lo */
    0,                          /*  5:wMAXPacketSize_hi */
    0x10,                       /*  6:bInterval */

    /* Interface Descriptor */
    9,                              /*  0:bLength */
    USB_DT_INTERFACE,               /*  1:bDescriptor */
    0x01,                              /*  2:bInterfaceNumber */
    0x00,                              /*  3:bAlternateSetting */
    0x02,                              /*  4:bNumEndpoints */
    USB_IFCLS_CDCD,                 /*  5:bInterfaceClass */
    0x00,                              /*  6:bInterfaceSubClass */
    0x00,                              /*  7:bInterfaceProtocol */
    0x00,                              /*  8:iInterface */

    /* Endpoint Descriptor 0 */
    7,                          /*  0:bLength */
    USB_DT_ENDPOINT,            /*  1:bDescriptorType */
    USB_EP_IN | USB_EP1,        /*  2:bEndpointAddress */
    USB_EP_BULK,                /*  3:bmAttribute */
    0,                          /*  4:wMAXPacketSize_lo */
    2,                          /*  5:wMAXPacketSize_hi */
    0,                          /*  6:bInterval */

    /* Endpoint Descriptor 1 */
    7,                          /*  0:bLength */
    USB_DT_ENDPOINT,            /*  1:bDescriptorType */
    USB_EP_OUT | USB_EP2,       /*  2:bEndpointAddress */
    USB_EP_BULK,                /*  3:bmAttribute */
    0,                          /*  4:wMAXPacketSize_lo */
    2,                          /*  5:wMAXPacketSize_hi */
    0,                          /*  6:bInterval */
};

#define STRING_FRAMEWORK_LENGTH                 (61U)
uint8_t g_string_framework[STRING_FRAMEWORK_LENGTH] =
{
    /* Manufacturer string descriptor:"Express Logic" */
     0x09, 0x04, 0x01, 0x0d,
    'E',
    'x',
    'p',
    'r',
    'e',
    's',
    's',
    ' ',
    'L',
    'o',
    'g',
    'i',
    'c',

    /* Product string descriptor: "EL RNDIS  Device" */
    0x09, 0x04, 0x02, 0x10,
    'E',
    'L',
    ' ',
    'R',
    'N',
    'D',
    'I',
    'S',
    ' ',
    ' ',
    'D',
    'e',
    'v',
    'i',
    'c',
    'e',

    /* Serial Number string descriptor: "0001" */
    0x09, 0x04, 0x03, 0x04,
    0x30, 0x30, 0x30, 0x31,

    /* MAC Address string descriptor: "001E5841B879" */
    0x09, 0x04, 0x04, 0x0C,
    0x30, 0x30, 0x31, 0x45, 0x35, 0x38,
    0x34, 0x31, 0x42, 0x38, 0x37, 0x38,
};
#define LANGUAGE_ID_FRAME_WORK_LENGTH           (2U)
uint8_t g_language_id_framework[LANGUAGE_ID_FRAME_WORK_LENGTH] =
{
    0x09,
    0x04
};


/***********************************************************************************************************************
 * File Name    : usb_pcdc_stdout.c
 * Description  : USB PCDC API Source file
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
 ***********************************************************************************************************************/

#include <main_thread.h>
#include "user_cfg.h"


int _write(int, char *, int);

usb_callback_t g_usb_cb;
static void g_usb_class_req_handler(usb_setup_t *);

extern usb_instance_ctrl_t g_basic0_ctrl;

static usb_pcdc_linecoding_t g_usb_lc;
static usb_status_t g_usb_status;
static bool g_usb_dtr;


int _write(int file, char * p_src, int len)
{
    fsp_err_t status = FSP_ERR_INTERNAL;

    if (true == g_usb_dtr)
    {
        uint32_t front_len = 4 - ((uint32_t) p_src & 0x3);

        if (4 > front_len)
        {
            char data[4];
            memcpy(data, p_src, front_len);
            _write(file, data, (signed) front_len);

            status = R_USB_Write(&g_basic0_ctrl, (uint8_t *) p_src + front_len,
                                 (uint32_t) len - front_len, USB_CLASS_PCDC);
        }

        else
        {
            status = R_USB_Write(&g_basic0_ctrl, (uint8_t *) p_src, (uint32_t) len,
                                 USB_CLASS_PCDC);
        }
    }

    if (FSP_SUCCESS == status)
    {
        xSemaphoreTake(g_usb_ready_sema, portMAX_DELAY);
    }

    return len; // not critical

    FSP_PARAMETER_NOT_USED(file);
}


void g_usb_cb(usb_event_info_t * p_ev, usb_hdl_t handle, usb_onoff_t state)
{
    switch (p_ev->event)
    {
        case USB_STATUS_REQUEST:
        {
            g_usb_class_req_handler(&p_ev->setup);
        }
        break;

        case USB_STATUS_WRITE_COMPLETE:
        {
            xSemaphoreGive(g_usb_ready_sema);
        }
        break;

        case USB_STATUS_CONFIGURED:
        {
            g_usb_status = USB_STATUS_CONFIGURED;
        }
        break;

        case USB_STATUS_REQUEST_COMPLETE:
        {
            if (USB_STATUS_CONFIGURED == g_usb_status)
            {
                g_usb_status = USB_STATUS_REQUEST_COMPLETE;
            }
        }
        break;

        case USB_STATUS_DETACH:
        {
            g_usb_status = USB_STATUS_DETACH;
        }
        break;

        default:
        {
            ;
        }
    }

    FSP_PARAMETER_NOT_USED(state);
    FSP_PARAMETER_NOT_USED(handle);
}


static void g_usb_class_req_handler(usb_setup_t * p_req)
{
    switch (p_req->request_type & USB_BREQUEST)
    {
        case USB_PCDC_SET_LINE_CODING:
        {
            R_USB_PeriControlDataGet(&g_basic0_ctrl, (void *) &g_usb_lc,
                    sizeof(g_usb_lc) - sizeof(g_usb_lc.rsv));
        }
        break;

        case USB_PCDC_GET_LINE_CODING:
        {
            R_USB_PeriControlDataSet(&g_basic0_ctrl, (void *) &g_usb_lc,
                    sizeof(g_usb_lc) - sizeof(g_usb_lc.rsv));
        }
        break;

        case USB_PCDC_SET_CONTROL_LINE_STATE:
        {
            g_usb_dtr = (p_req->request_value >> 0) & 0x01 ? true : false;
            R_USB_PeriControlStatusSet(&g_basic0_ctrl, USB_SETUP_STATUS_ACK);
        }
        break;

        default:
        {
            ;
        }
    }
}


/* bcdUSB */
#define USB_BCDNUM                      (0x0200U)
/* Release Number */
#define USB_RELEASE                     (0x0200U)
/* DCP max packet size */
#define USB_DCPMAXP                     (64U)
/* Configuration number */
#define USB_CONFIGNUM                   (1U)
/* Vendor ID */
#define USB_VENDORID                    (0x045BU)
/* Product ID */
#define USB_PRODUCTID                   (0x5310U)

/* Class-Specific Configuration Descriptors */
#define USB_PCDC_CS_INTERFACE                               (0x24U)

/* bDescriptor SubType in Communications Class Functional Descriptors */
/* Header Functional Descriptor */
#define USB_PCDC_DT_SUBTYPE_HEADER_FUNC                     (0x00U)
/* Call Management Functional Descriptor. */
#define USB_PCDC_DT_SUBTYPE_CALL_MANAGE_FUNC                (0x01U)
/* Abstract Control Management Functional Descriptor. */
#define USB_PCDC_DT_SUBTYPE_ABSTRACT_CTR_MANAGE_FUNC        (0x02U)
/* Union Functional Descriptor */
#define USB_PCDC_DT_SUBTYPE_UNION_FUNC                      (0x06U)

/* Communications Class Subclass Codes */
#define USB_PCDC_CLASS_SUBCLASS_CODE_ABS_CTR_MDL            (0x02U)


/* USB Class Definitions for Communications Devices Specification
 release number in binary-coded decimal. */
#define     USB_PCDC_BCD_CDC          (0x0110U)

/* Descriptor length */
#define USB_PCDC_QD_LEN               (10U)
#define USB_PCDC_CD1_LEN              (67U)
#define STRING_DESCRIPTOR0_LEN        (4U)
#define STRING_DESCRIPTOR1_LEN        (16U)
#define STRING_DESCRIPTOR2_LEN        (44U)
#define STRING_DESCRIPTOR3_LEN        (46U)
#define STRING_DESCRIPTOR4_LEN        (22U)
#define STRING_DESCRIPTOR5_LEN        (18U)
#define STRING_DESCRIPTOR6_LEN        (28U)

/* Descriptor data Mask */
#define USB_UCHAR_MAX                 (0xffU)
#define USB_W_TOTAL_LENGTH_MASK       (256U)
#define USB_W_MAX_PACKET_SIZE_MASK    (64U)
#define USB_PCDC_BCD_CDC_MASK         (256U)

/* Standard Device Descriptor */
static uint8_t g_apl_device[USB_DD_BLENGTH + ( USB_DD_BLENGTH % 2)] =
{
    USB_DD_BLENGTH,                                     /*  0:bLength */
    USB_DT_DEVICE,                                      /*  1:bDescriptorType */
    (USB_BCDNUM & (uint8_t) USB_UCHAR_MAX),                     /*  2:bcdUSB_lo */
    ((uint8_t) (USB_BCDNUM >> 8) & (uint8_t) USB_UCHAR_MAX),    /*  3:bcdUSB_hi */
    USB_IFCLS_CDCC,                                     /*  4:bDeviceClass */
    0,                                                  /*  5:bDeviceSubClass */
    0,                                                  /*  6:bDeviceProtocol */
    (uint8_t) USB_DCPMAXP,                              /*  7:bMAXPacketSize(for DCP) */
    (USB_VENDORID & (uint8_t) USB_UCHAR_MAX),                   /*  8:idVendor_lo */
    ((uint8_t) (USB_VENDORID >> 8) & (uint8_t) USB_UCHAR_MAX),  /*  9:idVendor_hi */
    ((uint16_t) USB_PRODUCTID & (uint8_t) USB_UCHAR_MAX),       /* 10:idProduct_lo */
    ((uint8_t) (USB_PRODUCTID >> 8) & (uint8_t) USB_UCHAR_MAX), /* 11:idProduct_hi */
    (USB_RELEASE & (uint8_t) USB_UCHAR_MAX),                    /* 12:bcdDevice_lo */
    ((uint8_t) (USB_RELEASE >> 8) & (uint8_t) USB_UCHAR_MAX),   /* 13:bcdDevice_hi */
    1,                                                  /* 14:iManufacturer */
    2,                                                  /* 15:iProduct */
    6,                                                  /* 16:iSerialNumber */
    USB_CONFIGNUM /* 17:bNumConfigurations */
};

/************************************************************
 *  Device Qualifier Descriptor   *
 ************************************************************/
static uint8_t g_apl_qualifier_descriptor[USB_PCDC_QD_LEN + ( USB_PCDC_QD_LEN % 2)] =
{
    USB_PCDC_QD_LEN,                                    /*  0:bLength */
    USB_DT_DEVICE_QUALIFIER,                            /*  1:bDescriptorType */
    (USB_BCDNUM & (uint8_t) USB_UCHAR_MAX),                  /*  2:bcdUSB_lo */
    ((uint8_t) (USB_BCDNUM >> 8) & (uint8_t) USB_UCHAR_MAX), /*  3:bcdUSB_hi */
    0,                                                  /*  4:bDeviceClass */
    0,                                                  /*  5:bDeviceSubClass */
    0,                                                  /*  6:bDeviceProtocol */
    (uint8_t) USB_DCPMAXP,                              /*  7:bMAXPacketSize(for DCP) */
    USB_CONFIGNUM,                                      /*  8:bNumConfigurations */
    0 /*  9:bReserved */
};

/************************************************************
 *  Configuration Or Other_Speed_Configuration Descriptor   *
 ************************************************************/
/* For Full-Speed */
static uint8_t g_apl_configuration[USB_PCDC_CD1_LEN + ( USB_PCDC_CD1_LEN % 2)] =
{
    USB_CD_BLENGTH,                 /*  0:bLength */
    USB_SOFT_CHANGE,                /*  1:bDescriptorType */
    USB_PCDC_CD1_LEN % USB_W_TOTAL_LENGTH_MASK, /*  2:wTotalLength(L) */
    USB_PCDC_CD1_LEN / USB_W_TOTAL_LENGTH_MASK, /*  3:wTotalLength(H) */
    2,                              /*  4:bNumInterfaces */
    1,                              /*  5:bConfigurationValue */
    0,                              /*  6:iConfiguration */
    USB_CF_RESERVED | USB_CF_SELFP, /*  7:bmAttributes */
    (10 / 2),                       /*  8:MAXPower (2mA unit) */

    /* Interface Descriptor */
    USB_ID_BLENGTH,                 /*  0:bLength */
    USB_DT_INTERFACE,               /*  1:bDescriptor */
    0,                              /*  2:bInterfaceNumber */
    0,                              /*  3:bAlternateSetting */
    1,                              /*  4:bNumEndpoints */
    USB_IFCLS_CDCC,                 /*  5:bInterfaceClass */
    USB_PCDC_CLASS_SUBCLASS_CODE_ABS_CTR_MDL,   /*  6:bInterfaceSubClass */
    1,                              /*  7:bInterfaceProtocol */
    0,                              /*  8:iInterface */

        /* Communications Class Functional Descriptorss */
        5,                          /*  0:bLength */
        USB_PCDC_CS_INTERFACE,      /*  1:bDescriptorType */
        USB_PCDC_DT_SUBTYPE_HEADER_FUNC,    /*  2:bDescriptorSubtype */
        USB_PCDC_BCD_CDC % USB_W_TOTAL_LENGTH_MASK,     /*  3:bcdCDC_lo */
        USB_PCDC_BCD_CDC / USB_W_TOTAL_LENGTH_MASK,     /*  4:bcdCDC_hi */

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
        3,                          /*  3:bmCapabilities */
        1,                          /*  4:bDataInterface */

        /* Endpoint Descriptor 0 */
        7,                          /*  0:bLength */
        USB_DT_ENDPOINT,            /*  1:bDescriptorType */
        USB_EP_IN | USB_EP3,        /*  2:bEndpointAddress */
        USB_EP_INT,                 /*  3:bmAttribute */
        16,                         /*  4:wMAXPacketSize_lo */
        0,                          /*  5:wMAXPacketSize_hi */
        0x10,                       /*  6:bInterval */

    /* Interface Descriptor */
    USB_ID_BLENGTH,                 /*  0:bLength */
    USB_DT_INTERFACE,               /*  1:bDescriptor */
    1,                              /*  2:bInterfaceNumber */
    0,                              /*  3:bAlternateSetting */
    2,                              /*  4:bNumEndpoints */
    USB_IFCLS_CDCD,                 /*  5:bInterfaceClass */
    0,                              /*  6:bInterfaceSubClass */
    0,                              /*  7:bInterfaceProtocol */
    0,                              /*  8:iInterface */

        /* Endpoint Descriptor 0 */
        USB_ED_BLENGTH,             /*  0:bLength */
        USB_DT_ENDPOINT,            /*  1:bDescriptorType */
        USB_EP_IN | USB_EP1,        /*  2:bEndpointAddress */
        USB_EP_BULK,                /*  3:bmAttribute */
        USB_W_MAX_PACKET_SIZE_MASK, /*  4:wMAXPacketSize_lo */
        0,                          /*  5:wMAXPacketSize_hi */
        0,                          /*  6:bInterval */

        /* Endpoint Descriptor 1 */
        USB_ED_BLENGTH,             /*  0:bLength */
        USB_DT_ENDPOINT,            /*  1:bDescriptorType */
        USB_EP_OUT | USB_EP2,       /*  2:bEndpointAddress */
        USB_EP_BULK,                /*  3:bmAttribute */
        USB_W_MAX_PACKET_SIZE_MASK, /*  4:wMAXPacketSize_lo */
        0,                          /*  5:wMAXPacketSize_hi */
        0,                          /*  6:bInterval */
};

/* For High-Speed */
static uint8_t g_apl_hs_configuration[USB_PCDC_CD1_LEN + ( USB_PCDC_CD1_LEN % 2)] =
{
    9,                              /*  0:bLength */
    USB_SOFT_CHANGE,                /*  1:bDescriptorType */
    USB_PCDC_CD1_LEN % USB_W_TOTAL_LENGTH_MASK,         /*  2:wTotalLength(L) */
    USB_PCDC_CD1_LEN / USB_W_TOTAL_LENGTH_MASK,         /*  3:wTotalLength(H) */
    2,                              /*  4:bNumInterfaces */
    1,                              /*  5:bConfigurationValue */
    0,                              /*  6:iConfiguration */
    USB_CF_RESERVED | USB_CF_SELFP, /*  7:bmAttributes */
    (10 / 2),                       /*  8:MAXPower (2mA unit) */

    /* Interface Descriptor */
    9,                              /*  0:bLength */
    USB_DT_INTERFACE,               /*  1:bDescriptor */
    0,                              /*  2:bInterfaceNumber */
    0,                              /*  3:bAlternateSetting */
    1,                              /*  4:bNumEndpoints */
    USB_IFCLS_CDCC,                 /*  5:bInterfaceClass */
    USB_PCDC_CLASS_SUBCLASS_CODE_ABS_CTR_MDL,   /*  6:bInterfaceSubClass */
    1,                              /*  7:bInterfaceProtocol */
    0,                              /*  8:iInterface */

        /* Communications Class Functional Descriptorss */
        5, /*  0:bLength */
        USB_PCDC_CS_INTERFACE,      /*  1:bDescriptorType */
        USB_PCDC_DT_SUBTYPE_HEADER_FUNC,    /*  2:bDescriptorSubtype */
        USB_PCDC_BCD_CDC % USB_PCDC_BCD_CDC_MASK,     /*  3:bcdCDC_lo */
        USB_PCDC_BCD_CDC / USB_PCDC_BCD_CDC_MASK,     /*  4:bcdCDC_hi */

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
        3,                          /*  3:bmCapabilities */
        1,                          /*  4:bDataInterface */

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
    1,                              /*  2:bInterfaceNumber */
    0,                              /*  3:bAlternateSetting */
    2,                              /*  4:bNumEndpoints */
    USB_IFCLS_CDCD,                 /*  5:bInterfaceClass */
    0,                              /*  6:bInterfaceSubClass */
    0,                              /*  7:bInterfaceProtocol */
    0,                              /*  8:iInterface */

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

/*************************************
 *    String Descriptor              *
 *************************************/
/* UNICODE 0x0409 English (United States) */
static uint8_t g_cdc_string_descriptor0[STRING_DESCRIPTOR0_LEN + ( STRING_DESCRIPTOR0_LEN % 2)] =
{
    STRING_DESCRIPTOR0_LEN,     /*  0:bLength */
    USB_DT_STRING,              /*  1:bDescriptorType */
    0x09, 0x04                  /*  2:wLANGID[0] */
};

/* iManufacturer */
static uint8_t g_cdc_string_descriptor1[STRING_DESCRIPTOR1_LEN + ( STRING_DESCRIPTOR1_LEN % 2)] =
{
    STRING_DESCRIPTOR1_LEN,     /*  0:bLength */
    USB_DT_STRING,              /*  1:bDescriptorType */
    'R', 0x00,                  /*  2:wLANGID[0] */
    'E', 0x00,
    'N', 0x00,
    'E', 0x00,
    'S', 0x00,
    'A', 0x00,
    'S', 0x00,
};

/* iProduct */
static uint8_t g_cdc_string_descriptor2[STRING_DESCRIPTOR2_LEN + ( STRING_DESCRIPTOR2_LEN % 2)] =
{
    STRING_DESCRIPTOR2_LEN, /*  0:bLength */
    USB_DT_STRING,          /*  1:bDescriptorType */
    'C', 0x00,
    'D', 0x00,
    'C', 0x00,
    ' ', 0x00,
    'U', 0x00,
    'S', 0x00,
    'B', 0x00,
    ' ', 0x00,
    'D', 0x00,
    'e', 0x00,
    'm', 0x00,
    'o', 0x00,
    'n', 0x00,
    's', 0x00,
    't', 0x00,
    'r', 0x00,
    'a', 0x00,
    't', 0x00,
    'i', 0x00,
    'o', 0x00,
    'n', 0x00,
};

/* iInterface */
static uint8_t g_cdc_string_descriptor3[STRING_DESCRIPTOR3_LEN + ( STRING_DESCRIPTOR3_LEN % 2)] =
{
    STRING_DESCRIPTOR3_LEN, /*  0:bLength */
    USB_DT_STRING,          /*  1:bDescriptorType */
    'C', 0x00,
    'o', 0x00,
    'm', 0x00,
    'm', 0x00,
    'u', 0x00,
    'n', 0x00,
    'i', 0x00,
    'c', 0x00,
    'a', 0x00,
    't', 0x00,
    'i', 0x00,
    'o', 0x00,
    'n', 0x00,
    's', 0x00,
    ' ', 0x00,
    'D', 0x00,
    'e', 0x00,
    'v', 0x00,
    'i', 0x00,
    'c', 0x00,
    'e', 0x00,
    's', 0x00
};

/* iConfiguration */
static uint8_t g_cdc_string_descriptor4[STRING_DESCRIPTOR4_LEN + ( STRING_DESCRIPTOR4_LEN % 2)] =
{
    STRING_DESCRIPTOR4_LEN, /*  0:bLength */
    USB_DT_STRING,          /*  1:bDescriptorType */
    'F', 0x00,              /*  2:wLANGID[0] */
    'u', 0x00,
    'l', 0x00,
    'l', 0x00,
    '-', 0x00,
    'S', 0x00,
    'p', 0x00,
    'e', 0x00,
    'e', 0x00,
    'd', 0x00
};

/* iConfiguration */
static uint8_t g_cdc_string_descriptor5[STRING_DESCRIPTOR5_LEN + ( STRING_DESCRIPTOR5_LEN % 2)] =
{
    STRING_DESCRIPTOR5_LEN, /*  0:bLength */
    USB_DT_STRING,          /*  1:bDescriptorType */
    'H', 0x00,              /*  2:wLANGID[0] */
    'i', 0x00,
    '-', 0x00,
    'S', 0x00,
    'p', 0x00,
    'e', 0x00,
    'e', 0x00,
    'd', 0x00
};

/* iSerialNumber */
static uint8_t g_cdc_string_descriptor6[STRING_DESCRIPTOR6_LEN + ( STRING_DESCRIPTOR6_LEN % 2)] =
{
    STRING_DESCRIPTOR6_LEN, /*  0:bLength */
    USB_DT_STRING,          /*  1:bDescriptorType */
    '0', 0x00,              /*  2:wLANGID[0] */
    '0', 0x00,
    '0', 0x00,
    '0', 0x00,
    '0', 0x00,
    '0', 0x00,
    '0', 0x00,
    '0', 0x00,
    '0', 0x00,
    '0', 0x00,
    '0', 0x00,
    '0', 0x00,
    '1', 0x00,
};

static uint8_t *g_apl_string_table[] =
{
    g_cdc_string_descriptor0,
    g_cdc_string_descriptor1,
    g_cdc_string_descriptor2,
    g_cdc_string_descriptor3,
    g_cdc_string_descriptor4,
    g_cdc_string_descriptor5,
    g_cdc_string_descriptor6
};

usb_descriptor_t g_usb_descriptor =
{
    g_apl_device,                     // Device Descriptor
    g_apl_configuration,              // Configuration descriptor for FS
    g_apl_hs_configuration,           // Configuration descriptor for HS
    g_apl_qualifier_descriptor,       // Qualifier descriptor
    g_apl_string_table,               // String descriptor table
    sizeof(g_apl_string_table) /
        sizeof(g_apl_string_table[0]) // Size of string descriptor table
};

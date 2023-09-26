/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
* EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
* SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
* SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
* this software. By using this software, you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2019-2022 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/**
 *  GATT DATABASE QUICK REFERENCE TABLE:
 *  Abbreviations used for PROPERTIES:
 *      BC = Broadcast
 *      RD = Read
 *      WW = Write Without Response
 *      WR = Write
 *      NT = Notification
 *      IN = Indication
 *      RW = Reliable Write
 * 
 *  HANDLE | ATT_TYPE          | PROPERTIES  | ATT_VALUE                        | DEFINITION
 *  ============================================================================================
 *  GAP Service
 *  ============================================================================================
 *  0x0001 | 0x28,0x00         | RD          | 0xa8,0x59,0x71,0xbe,0x48,0x26... | GAP Service Declaration
 *  -------+-------------------+-------------+----------------------------------+---------------
 *  0x0002 | 0x28,0x03         | RD          | 0x0A,0x03,0x00,0x00,0x2A         | Device Name characteristic Declaration
 *  -------+-------------------+-------------+----------------------------------+---------------
 *  0x0003 | 0x00,0x2A         | RD,WR       | 0x00,0x00,0x00,0x00,0x00,0x00... | Device Name characteristic value
 *  -------+-------------------+-------------+----------------------------------+---------------
 *  0x0004 | 0x28,0x03         | RD          | 0x02,0x05,0x00,0x01,0x2A         | Appearance characteristic Declaration
 *  -------+-------------------+-------------+----------------------------------+---------------
 *  0x0005 | 0x01,0x2A         | RD          | 0x00,0x00                        | Appearance characteristic value
 *  -------+-------------------+-------------+----------------------------------+---------------
 *  0x0006 | 0x28,0x03         | RD          | 0x02,0x07,0x00,0x04,0x2A         | Peripheral Preferred Connection Parameters characteristic Declaration
 *  -------+-------------------+-------------+----------------------------------+---------------
 *  0x0007 | 0x04,0x2A         | RD          | 0x00,0x00,0x00,0x00,0x00,0x00... | Peripheral Preferred Connection Parameters characteristic value
 *  -------+-------------------+-------------+----------------------------------+---------------
 *  0x0008 | 0x28,0x03         | RD          | 0x02,0x09,0x00,0xA6,0x2A         | Central Address Resolution characteristic Declaration
 *  -------+-------------------+-------------+----------------------------------+---------------
 *  0x0009 | 0xA6,0x2A         | RD          | 0x00                             | Central Address Resolution characteristic value
 *  -------+-------------------+-------------+----------------------------------+---------------
 *  0x000A | 0x28,0x03         | RD          | 0x02,0x0B,0x00,0xC9,0x2A         | Resolvable Private Address Only characteristic Declaration
 *  -------+-------------------+-------------+----------------------------------+---------------
 *  0x000B | 0xC9,0x2A         | RD          | 0x00                             | Resolvable Private Address Only characteristic value
 *  ============================================================================================
 *  GATT Service
 *  ============================================================================================
 *  0x000C | 0x28,0x00         | RD          | 0x01,0x18                        | GATT Service Declaration
 *  -------+-------------------+-------------+----------------------------------+---------------
 *  0x000D | 0x28,0x02         | RD          | 0x01,0x00,0x0B,0x00              | GAP Service Included Declaration
 *  -------+-------------------+-------------+----------------------------------+---------------
 *  0x000E | 0x28,0x03         | RD          | 0x20,0x0F,0x00,0x05,0x2A         | Service Changed characteristic Declaration
 *  -------+-------------------+-------------+----------------------------------+---------------
 *  0x000F | 0x05,0x2A         | IN          | 0x00,0x00,0x00,0x00              | Service Changed characteristic value
 *  -------+-------------------+-------------+----------------------------------+---------------
 *  0x0010 | 0x02,0x29         | RD,WR       | 0x00,0x00                        | Client Characteristic Configuration descriptor
 *  ============================================================================================
 *  Immediate Alert Service
 *  ============================================================================================
 *  0x0011 | 0x28,0x00         | RD          | 0x02,0x18                        | Immediate Alert Service Declaration
 *  -------+-------------------+-------------+----------------------------------+---------------
 *  0x0012 | 0x28,0x02         | RD          | 0x0C,0x00,0x10,0x00,0x01,0x18    | GATT Service Included Declaration
 *  -------+-------------------+-------------+----------------------------------+---------------
 *  0x0013 | 0x28,0x03         | RD          | 0x04,0x14,0x00,0x06,0x2A         | Alert Level characteristic Declaration
 *  -------+-------------------+-------------+----------------------------------+---------------
 *  0x0014 | 0x06,0x2A         | WW          | 0x00                             | Alert Level characteristic value
 *  ============================================================================================
 
 */

/*******************************************************************************
* Includes   <System Includes> , "Project Includes"
*******************************************************************************/
#include <stdio.h>
#include "gatt_db.h"

/*****************************************************************************
* Global definition
******************************************************************************/
static const uint8_t gs_gatt_const_uuid_arr[] =
{
    /* Primary Service Declaration : 0 */
    0x00, 0x28,

    /* Secondary Service Declaration : 2 */
    0x01, 0x28,

    /* Included Service Declaration : 4 */
    0x02, 0x28,

    /* Characteristic Declaration : 6 */
    0x03, 0x28,

    /* GAP Service : 8 */
    0xa8, 0x59, 0x71, 0xbe, 0x48, 0x26, 0x7e, 0x91, 0x94, 0x40, 0x55, 0x8c, 0xf7, 0x0a, 0x5f, 0x39,

    /* Device Name : 24 */
    0x00, 0x2A,

    /* Appearance : 26 */
    0x01, 0x2A,

    /* Peripheral Preferred Connection Parameters : 28 */
    0x04, 0x2A,

    /* Central Address Resolution : 30 */
    0xA6, 0x2A,

    /* Resolvable Private Address Only : 32 */
    0xC9, 0x2A,

    /* GATT Service : 34 */
    0x01, 0x18,

    /* Service Changed : 36 */
    0x05, 0x2A,

    /* Client Characteristic Configuration : 38 */
    0x02, 0x29,

    /* Immediate Alert Service : 40 */
    0x02, 0x18,

    /* Alert Level : 42 */
    0x06, 0x2A,

};

static uint8_t gs_gatt_value_arr[] =
{
    /* Device Name */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    /* Appearance */
    0x00, 0x00,

    /* Peripheral Preferred Connection Parameters */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    /* Central Address Resolution */
    0x00,

    /* Resolvable Private Address Only */
    0x00,

    /* Alert Level */
    0x00,

};

static const uint8_t gs_gatt_const_value_arr[] =
{
    /* Device Name */
    0x0A,       // Properties
    0x03, 0x00, // Attr Handle
    0x00, 0x2A, // UUID

    /* Appearance */
    0x02,       // Properties
    0x05, 0x00, // Attr Handle
    0x01, 0x2A, // UUID

    /* Peripheral Preferred Connection Parameters */
    0x02,       // Properties
    0x07, 0x00, // Attr Handle
    0x04, 0x2A, // UUID

    /* Central Address Resolution */
    0x02,       // Properties
    0x09, 0x00, // Attr Handle
    0xA6, 0x2A, // UUID

    /* Resolvable Private Address Only */
    0x02,       // Properties
    0x0B, 0x00, // Attr Handle
    0xC9, 0x2A, // UUID

    /* Included Service : GAP Service */
    0x01, 0x00, // Start Handle
    0x0B, 0x00, // End Handle

    /* Service Changed */
    0x20,       // Properties
    0x0F, 0x00, // Attr Handle
    0x05, 0x2A, // UUID

    /* Included Service : GATT Service */
    0x0C, 0x00, // Start Handle
    0x10, 0x00, // End Handle
    0x01, 0x18, // UUID

    /* Alert Level */
    0x04,       // Properties
    0x14, 0x00, // Attr Handle
    0x06, 0x2A, // UUID

};

static const uint8_t gs_gatt_db_const_peer_specific_val_arr[] =
{
    /* Service Changed : Client Characteristic Configuration */
    0x00, 0x00,

};

#ifdef BSP_MCU_GROUP_RA4W1 /* RA4W1 */
static uint8_t gs_gatt_db_peer_specific_val_arr[sizeof(gs_gatt_db_const_peer_specific_val_arr)*(BLE_ABS_CFG_RF_CONNECTION_MAXIMUM+1)];
#else /* RX23W or RE01B */
static uint8_t gs_gatt_db_peer_specific_val_arr[sizeof(gs_gatt_db_const_peer_specific_val_arr)*(BLE_CFG_RF_CONN_MAX+1)];
#endif
static const st_ble_gatts_db_uuid_cfg_t gs_gatt_type_table[] =
{
    /* 0 : Primary Service Declaration */
    {
        /* UUID Offset */
        0,
        /* First Occurrence for Type */
        0x0001,
        /* Last Occurrence for Type */
        0x0011,
    },

    /* 1 : GAP Service */
    {
        /* UUID Offset */
        8,
        /* First Occurrence for Type */
        0x0001,
        /* Last Occurrence for Type */
        0x0000,
    },

    /* 2 : Characteristic Declaration */
    {
        /* UUID Offset */
        6,
        /* First Occurrence for Type */
        0x0002,
        /* Last Occurrence for Type */
        0x0013,
    },

    /* 3 : Device Name */
    {
        /* UUID Offset */
        24,
        /* First Occurrence for Type */
        0x0003,
        /* Last Occurrence for Type */
        0x0000,
    },

    /* 4 : Appearance */
    {
        /* UUID Offset */
        26,
        /* First Occurrence for Type */
        0x0005,
        /* Last Occurrence for Type */
        0x0000,
    },

    /* 5 : Peripheral Preferred Connection Parameters */
    {
        /* UUID Offset */
        28,
        /* First Occurrence for Type */
        0x0007,
        /* Last Occurrence for Type */
        0x0000,
    },

    /* 6 : Central Address Resolution */
    {
        /* UUID Offset */
        30,
        /* First Occurrence for Type */
        0x0009,
        /* Last Occurrence for Type */
        0x0000,
    },

    /* 7 : Resolvable Private Address Only */
    {
        /* UUID Offset */
        32,
        /* First Occurrence for Type */
        0x000B,
        /* Last Occurrence for Type */
        0x0000,
    },

    /* 8 : GATT Service */
    {
        /* UUID Offset */
        34,
        /* First Occurrence for Type */
        0x000C,
        /* Last Occurrence for Type */
        0x0000,
    },

    /* 9 : Included Service Declaration */
    {
        /* UUID Offset */
        4,
        /* First Occurrence for Type */
        0x000D,
        /* Last Occurrence for Type */
        0x0012,
    },

    /* 10 : Service Changed */
    {
        /* UUID Offset */
        36,
        /* First Occurrence for Type */
        0x000F,
        /* Last Occurrence for Type */
        0x0000,
    },

    /* 11 : Client Characteristic Configuration */
    {
        /* UUID Offset */
        38,
        /* First Occurrence for Type */
        0x0010,
        /* Last Occurrence for Type */
        0x0000,
    },

    /* 12 : Immediate Alert Service */
    {
        /* UUID Offset */
        40,
        /* First Occurrence for Type */
        0x0011,
        /* Last Occurrence for Type */
        0x0000,
    },

    /* 13 : Alert Level */
    {
        /* UUID Offset */
        42,
        /* First Occurrence for Type */
        0x0014,
        /* Last Occurrence for Type */
        0x0000,
    },

};

static const st_ble_gatts_db_attr_cfg_t gs_gatt_db_attr_table[] =
{
    /* Handle : 0x0000 */
    /* Blank */
    {
        /* Properties */
        0,
        /* Auxiliary Properties */
        BLE_GATT_DB_NO_AUXILIARY_PROPERTY,
        /* Value Size */
        1,
        /* Next Attribute Type Index */
        0x0001,
        /* UUID Offset */
        0,
        /* Value */
        NULL,
    },

    /* Handle : 0x0001 */
    /* GAP Service : Primary Service Declaration */
    {
        /* Properties */
        BLE_GATT_DB_READ,
        /* Auxiliary Properties */
        BLE_GATT_DB_FIXED_LENGTH_PROPERTY | BLE_GATT_DB_128_BIT_UUID_FORMAT,
        /* Value Size */
        16,
        /* Next Attribute Type Index */
        0x000C,
        /* UUID Offset */
        0,
        /* Value */
        (uint8_t *)(gs_gatt_const_uuid_arr + 8),
    },

    /* Handle : 0x0002 */
    /* Device Name : Characteristic Declaration */
    {
        /* Properties */
        BLE_GATT_DB_READ,
        /* Auxiliary Properties */
        BLE_GATT_DB_FIXED_LENGTH_PROPERTY,
        /* Value Size */
        5,
        /* Next Attribute Type Index */
        0x0004,
        /* UUID Offset */
        6,
        /* Value */
        (uint8_t *)(gs_gatt_const_value_arr + 0),
    },

    /* Handle : 0x0003 */
    /* Device Name */
    {
        /* Properties */
        BLE_GATT_DB_READ | BLE_GATT_DB_WRITE,
        /* Auxiliary Properties */
        BLE_GATT_DB_FIXED_LENGTH_PROPERTY,
        /* Value Size */
        128,
        /* Next Attribute Type Index */
        0x0000,
        /* UUID Offset */
        24,
        /* Value */
        (uint8_t *)(gs_gatt_value_arr + 0),
    },

    /* Handle : 0x0004 */
    /* Appearance : Characteristic Declaration */
    {
        /* Properties */
        BLE_GATT_DB_READ,
        /* Auxiliary Properties */
        BLE_GATT_DB_FIXED_LENGTH_PROPERTY,
        /* Value Size */
        5,
        /* Next Attribute Type Index */
        0x0006,
        /* UUID Offset */
        6,
        /* Value */
        (uint8_t *)(gs_gatt_const_value_arr + 5),
    },

    /* Handle : 0x0005 */
    /* Appearance */
    {
        /* Properties */
        BLE_GATT_DB_READ,
        /* Auxiliary Properties */
        BLE_GATT_DB_FIXED_LENGTH_PROPERTY,
        /* Value Size */
        2,
        /* Next Attribute Type Index */
        0x0000,
        /* UUID Offset */
        26,
        /* Value */
        (uint8_t *)(gs_gatt_value_arr + 128),
    },

    /* Handle : 0x0006 */
    /* Peripheral Preferred Connection Parameters : Characteristic Declaration */
    {
        /* Properties */
        BLE_GATT_DB_READ,
        /* Auxiliary Properties */
        BLE_GATT_DB_FIXED_LENGTH_PROPERTY,
        /* Value Size */
        5,
        /* Next Attribute Type Index */
        0x0008,
        /* UUID Offset */
        6,
        /* Value */
        (uint8_t *)(gs_gatt_const_value_arr + 10),
    },

    /* Handle : 0x0007 */
    /* Peripheral Preferred Connection Parameters */
    {
        /* Properties */
        BLE_GATT_DB_READ,
        /* Auxiliary Properties */
        BLE_GATT_DB_FIXED_LENGTH_PROPERTY,
        /* Value Size */
        8,
        /* Next Attribute Type Index */
        0x0000,
        /* UUID Offset */
        28,
        /* Value */
        (uint8_t *)(gs_gatt_value_arr + 130),
    },

    /* Handle : 0x0008 */
    /* Central Address Resolution : Characteristic Declaration */
    {
        /* Properties */
        BLE_GATT_DB_READ,
        /* Auxiliary Properties */
        BLE_GATT_DB_FIXED_LENGTH_PROPERTY,
        /* Value Size */
        5,
        /* Next Attribute Type Index */
        0x000A,
        /* UUID Offset */
        6,
        /* Value */
        (uint8_t *)(gs_gatt_const_value_arr + 15),
    },

    /* Handle : 0x0009 */
    /* Central Address Resolution */
    {
        /* Properties */
        BLE_GATT_DB_READ,
        /* Auxiliary Properties */
        BLE_GATT_DB_FIXED_LENGTH_PROPERTY,
        /* Value Size */
        1,
        /* Next Attribute Type Index */
        0x0000,
        /* UUID Offset */
        30,
        /* Value */
        (uint8_t *)(gs_gatt_value_arr + 138),
    },

    /* Handle : 0x000A */
    /* Resolvable Private Address Only : Characteristic Declaration */
    {
        /* Properties */
        BLE_GATT_DB_READ,
        /* Auxiliary Properties */
        BLE_GATT_DB_FIXED_LENGTH_PROPERTY,
        /* Value Size */
        5,
        /* Next Attribute Type Index */
        0x000E,
        /* UUID Offset */
        6,
        /* Value */
        (uint8_t *)(gs_gatt_const_value_arr + 20),
    },

    /* Handle : 0x000B */
    /* Resolvable Private Address Only */
    {
        /* Properties */
        BLE_GATT_DB_READ,
        /* Auxiliary Properties */
        BLE_GATT_DB_FIXED_LENGTH_PROPERTY,
        /* Value Size */
        1,
        /* Next Attribute Type Index */
        0x0000,
        /* UUID Offset */
        32,
        /* Value */
        (uint8_t *)(gs_gatt_value_arr + 139),
    },

    /* Handle : 0x000C */
    /* GATT Service : Primary Service Declaration */
    {
        /* Properties */
        BLE_GATT_DB_READ,
        /* Auxiliary Properties */
        BLE_GATT_DB_FIXED_LENGTH_PROPERTY,
        /* Value Size */
        2,
        /* Next Attribute Type Index */
        0x0011,
        /* UUID Offset */
        0,
        /* Value */
        (uint8_t *)(gs_gatt_const_uuid_arr + 34),
    },

    /* Handle : 0x000D */
    /* GAP Service : Included Service Declaration */
    {
        /* Properties */
        BLE_GATT_DB_READ,
        /* Auxiliary Properties */
        BLE_GATT_DB_FIXED_LENGTH_PROPERTY,
        /* Value Size */
        4,
        /* Next Attribute Type Index */
        0x0012,
        /* UUID Offset */
        4,
        /* Value */
        (uint8_t *)(gs_gatt_const_value_arr + 25),
    },

    /* Handle : 0x000E */
    /* Service Changed : Characteristic Declaration */
    {
        /* Properties */
        BLE_GATT_DB_READ,
        /* Auxiliary Properties */
        BLE_GATT_DB_FIXED_LENGTH_PROPERTY,
        /* Value Size */
        5,
        /* Next Attribute Type Index */
        0x0013,
        /* UUID Offset */
        6,
        /* Value */
        (uint8_t *)(gs_gatt_const_value_arr + 29),
    },

    /* Handle : 0x000F */
    /* Service Changed */
    {
        /* Properties */
        0,
        /* Auxiliary Properties */
        BLE_GATT_DB_FIXED_LENGTH_PROPERTY,
        /* Value Size */
        4,
        /* Next Attribute Type Index */
        0x0000,
        /* UUID Offset */
        36,
        /* Value */
        (NULL),
    },

    /* Handle : 0x0010 */
    /* Service Changed : Client Characteristic Configuration */
    {
        /* Properties */
        BLE_GATT_DB_READ | BLE_GATT_DB_WRITE,
        /* Auxiliary Properties */
        BLE_GATT_DB_FIXED_LENGTH_PROPERTY | BLE_GATT_DB_PEER_SPECIFIC_VAL_PROPERTY,
        /* Value Size */
        2,
        /* Next Attribute Type Index */
        0x0000,
        /* UUID Offset */
        38,
        /* Value */
        (uint8_t *)(gs_gatt_db_peer_specific_val_arr + 0),
    },

    /* Handle : 0x0011 */
    /* Immediate Alert Service : Primary Service Declaration */
    {
        /* Properties */
        BLE_GATT_DB_READ,
        /* Auxiliary Properties */
        BLE_GATT_DB_FIXED_LENGTH_PROPERTY,
        /* Value Size */
        2,
        /* Next Attribute Type Index */
        0x0000,
        /* UUID Offset */
        0,
        /* Value */
        (uint8_t *)(gs_gatt_const_uuid_arr + 40),
    },

    /* Handle : 0x0012 */
    /* GATT Service : Included Service Declaration */
    {
        /* Properties */
        BLE_GATT_DB_READ,
        /* Auxiliary Properties */
        BLE_GATT_DB_FIXED_LENGTH_PROPERTY,
        /* Value Size */
        6,
        /* Next Attribute Type Index */
        0x0000,
        /* UUID Offset */
        4,
        /* Value */
        (uint8_t *)(gs_gatt_const_value_arr + 34),
    },

    /* Handle : 0x0013 */
    /* Alert Level : Characteristic Declaration */
    {
        /* Properties */
        BLE_GATT_DB_READ,
        /* Auxiliary Properties */
        BLE_GATT_DB_FIXED_LENGTH_PROPERTY,
        /* Value Size */
        5,
        /* Next Attribute Type Index */
        0x0000,
        /* UUID Offset */
        6,
        /* Value */
        (uint8_t *)(gs_gatt_const_value_arr + 40),
    },

    /* Handle : 0x0014 */
    /* Alert Level */
    {
        /* Properties */
        BLE_GATT_DB_WRITE_WITHOUT_RSP,
        /* Auxiliary Properties */
        BLE_GATT_DB_FIXED_LENGTH_PROPERTY,
        /* Value Size */
        1,
        /* Next Attribute Type Index */
        0x0000,
        /* UUID Offset */
        42,
        /* Value */
        (uint8_t *)(gs_gatt_value_arr + 140),
    },

};

static const st_ble_gatts_db_char_cfg_t gs_gatt_characteristic[] =
{
    /* 0 : Device Name */
    {
        /* Number of Attributes */
        {
            2,
        },
        /* Start Handle */
        0x0002,
        /* Service Index */
        0,
    },

    /* 1 : Appearance */
    {
        /* Number of Attributes */
        {
            2,
        },
        /* Start Handle */
        0x0004,
        /* Service Index */
        0,
    },

    /* 2 : Peripheral Preferred Connection Parameters */
    {
        /* Number of Attributes */
        {
            2,
        },
        /* Start Handle */
        0x0006,
        /* Service Index */
        0,
    },

    /* 3 : Central Address Resolution */
    {
        /* Number of Attributes */
        {
            2,
        },
        /* Start Handle */
        0x0008,
        /* Service Index */
        0,
    },

    /* 4 : Resolvable Private Address Only */
    {
        /* Number of Attributes */
        {
            2,
        },
        /* Start Handle */
        0x000A,
        /* Service Index */
        0,
    },

    /* 5 : Service Changed */
    {
        /* Number of Attributes */
        {
            3,
        },
        /* Start Handle */
        0x000E,
        /* Service Index */
        1,
    },

    /* 6 : Alert Level */
    {
        /* Number of Attributes */
        {
            2,
        },
        /* Start Handle */
        0x0013,
        /* Service Index */
        2,
    },

};

static const st_ble_gatts_db_serv_cfg_t gs_gatt_service[] =
{
    /* GAP Service */
    {
        /* Num of Services */
        {
            1,
        },
        /* Description */
        0,
        /* Service Start Handle */
        0x0001,
        /* Service End Handle */
        0x000B,
        /* Characteristic Start Index */
        0,
        /* Characteristic End Index */
        4,
    },

    /* GATT Service */
    {
        /* Num of Services */
        {
            1,
        },
        /* Description */
        0,
        /* Service Start Handle */
        0x000C,
        /* Service End Handle */
        0x0010,
        /* Characteristic Start Index */
        5,
        /* Characteristic End Index */
        5,
    },

    /* Immediate Alert Service */
    {
        /* Num of Services */
        {
            1,
        },
        /* Description */
        0,
        /* Service Start Handle */
        0x0011,
        /* Service End Handle */
        0x0014,
        /* Characteristic Start Index */
        6,
        /* Characteristic End Index */
        6,
    },

};

st_ble_gatts_db_cfg_t g_gatt_db_table =
{
    gs_gatt_const_uuid_arr,
    gs_gatt_value_arr,
    gs_gatt_const_value_arr,
    gs_gatt_db_peer_specific_val_arr,
    gs_gatt_db_const_peer_specific_val_arr,
    gs_gatt_type_table,
    gs_gatt_db_attr_table,
    gs_gatt_characteristic,
    gs_gatt_service,
    ARRAY_SIZE(gs_gatt_service),
    ARRAY_SIZE(gs_gatt_characteristic),
    ARRAY_SIZE(gs_gatt_type_table),
    ARRAY_SIZE(gs_gatt_db_const_peer_specific_val_arr),
};
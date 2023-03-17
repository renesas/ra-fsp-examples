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
* Copyright (C) 2019-2020 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
 * File Name: r_ble_gaps.h
 * Description : The header file for GAP Service Server.
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @file
 * @defgroup gaps GAP Service Server
 * @{
 * @ingroup profile
 * @brief   The generic_access service contains generic information about the device.
 **********************************************************************************************************************/
#include "profile_cmn/r_ble_servs_if.h"
#include "gatt_db.h"

#ifndef R_BLE_GAPS_H
#define R_BLE_GAPS_H

/* Start user code for file includes and macro definitions. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/*----------------------------------------------------------------------------------------------------------------------
    Device Name Characteristic : The Device Name characteristic shall contain the name of the device.
----------------------------------------------------------------------------------------------------------------------*/
/***************************************************************************//**
 * @brief Device Name value structure.
*******************************************************************************/
typedef struct {
    uint8_t name[128]; /**< Name */
    uint8_t length; /**< Length */
} st_ble_gaps_dev_name_t;

/***************************************************************************//**
 * @brief     Set Device Name characteristic value to the local GATT database.
 * @param[in] p_value  Characteristic value to set.
 * @return    @ref ble_status_t
*******************************************************************************/
ble_status_t R_BLE_GAPS_SetDevName(const st_ble_gaps_dev_name_t *p_value);

/***************************************************************************//**
 * @brief     Get Device Name characteristic value from the local GATT database.
 * @param[out] p_value  Output location for the acquired descriptor value.
 * @return    @ref ble_status_t
*******************************************************************************/
ble_status_t R_BLE_GAPS_GetDevName(st_ble_gaps_dev_name_t *p_value);

/*----------------------------------------------------------------------------------------------------------------------
    Appearance Characteristic : The Appearance characteristic defines the representation of the external appearance of the device.
----------------------------------------------------------------------------------------------------------------------*/
/***************************************************************************//**
 * @brief Appearance Category enumeration.
*******************************************************************************/
typedef enum {
    BLE_GAPS_APPEARANCE_CATEGORY_UNKNOWN = 0, /**< Unknown */
    BLE_GAPS_APPEARANCE_CATEGORY_GENERIC_PHONE = 64, /**< Generic Phone */
    BLE_GAPS_APPEARANCE_CATEGORY_GENERIC_COMPUTER = 128, /**< Generic Computer */
    BLE_GAPS_APPEARANCE_CATEGORY_GENERIC_WATCH = 192, /**< Generic Watch */
    BLE_GAPS_APPEARANCE_CATEGORY_WATCH_SPORTS_WATCH = 193, /**< Watch: Sports Watch */
    BLE_GAPS_APPEARANCE_CATEGORY_GENERIC_CLOCK = 256, /**< Generic Clock */
    BLE_GAPS_APPEARANCE_CATEGORY_GENERIC_DISPLAY = 320, /**< Generic Display */
    BLE_GAPS_APPEARANCE_CATEGORY_GENERIC_REMOTE_CONTROL = 384, /**< Generic Remote Control */
    BLE_GAPS_APPEARANCE_CATEGORY_GENERIC_EYE_GLASSES = 448, /**< Generic Eye-glasses */
    BLE_GAPS_APPEARANCE_CATEGORY_GENERIC_TAG = 512, /**< Generic Tag */
    BLE_GAPS_APPEARANCE_CATEGORY_GENERIC_KEYRING = 576, /**< Generic Keyring */
    BLE_GAPS_APPEARANCE_CATEGORY_GENERIC_MEDIA_PLAYER = 640, /**< Generic Media Player */
    BLE_GAPS_APPEARANCE_CATEGORY_GENERIC_BARCODE_SCANNER = 704, /**< Generic Barcode Scanner */
    BLE_GAPS_APPEARANCE_CATEGORY_GENERIC_THERMOMETER = 768, /**< Generic Thermometer */
    BLE_GAPS_APPEARANCE_CATEGORY_THERMOMETER_EAR = 769, /**< Thermometer Ear */
    BLE_GAPS_APPEARANCE_CATEGORY_GENERIC_HEART_RATE_SENSOR = 832, /**< Generic Heart rate Sensor */
    BLE_GAPS_APPEARANCE_CATEGORY_HEART_RATE_SENSOR_HEART_RATE_BELT = 833, /**< Heart Rate Sensor Heart Rate Belt */
    BLE_GAPS_APPEARANCE_CATEGORY_GENERIC_BLOOD_PRESSURE = 896, /**< Generic Blood Pressure */
    BLE_GAPS_APPEARANCE_CATEGORY_BLOOD_PRESSURE_ARM = 897, /**< Blood Pressure: Arm */
    BLE_GAPS_APPEARANCE_CATEGORY_BLOOD_PRESSURE_WRIST = 898, /**< Blood Pressure: Wrist */
    BLE_GAPS_APPEARANCE_CATEGORY_HUMAN_INTERFACE_DEVICE = 960, /**< Human Interface Device (HID) */
    BLE_GAPS_APPEARANCE_CATEGORY_KEYBOARD = 961, /**< Keyboard */
    BLE_GAPS_APPEARANCE_CATEGORY_MOUSE = 962, /**< Mouse */
    BLE_GAPS_APPEARANCE_CATEGORY_JOYSTICK = 963, /**< Joystick */
    BLE_GAPS_APPEARANCE_CATEGORY_GAMEPAD = 964, /**< Gamepad */
    BLE_GAPS_APPEARANCE_CATEGORY_DIGITIZER_TABLET = 965, /**< Digitizer Tablet */
    BLE_GAPS_APPEARANCE_CATEGORY_CARD_READER = 966, /**< Card Reader */
    BLE_GAPS_APPEARANCE_CATEGORY_DIGITAL_PEN = 967, /**< Digital Pen */
    BLE_GAPS_APPEARANCE_CATEGORY_BARCODE_SCANNER = 968, /**< Barcode Scanner */
    BLE_GAPS_APPEARANCE_CATEGORY_GENERIC_GLUCOSE_METER = 1024, /**< Generic Glucose Meter */
    BLE_GAPS_APPEARANCE_CATEGORY_GENERIC_RUNNING_WALKING_SENSOR = 1088, /**< Generic: Running Walking Sensor */
    BLE_GAPS_APPEARANCE_CATEGORY_RUNNING_WALKING_SENSOR_IN_SHOE = 1089, /**< Running Walking Sensor: In-Shoe */
    BLE_GAPS_APPEARANCE_CATEGORY_RUNNING_WALKING_SENSOR_ON_SHOE = 1090, /**< Running Walking Sensor: On-Shoe */
    BLE_GAPS_APPEARANCE_CATEGORY_RUNNING_WALKING_SENSOR_ON_HIP = 1091, /**< Running Walking Sensor: On-Hip */
    BLE_GAPS_APPEARANCE_CATEGORY_GENERIC__CYCLING = 1152, /**< Generic Cycling */
    BLE_GAPS_APPEARANCE_CATEGORY_CYCLING_CYCLING_COMPUTER = 1153, /**< Cycling Cycling Computer */
    BLE_GAPS_APPEARANCE_CATEGORY_CYCLING_SPEED_SENSOR = 1154, /**< Cycling Speed Sensor */
    BLE_GAPS_APPEARANCE_CATEGORY_CYCLING_CADENCE_SENSOR = 1155, /**< Cycling Cadence Sensor */
    BLE_GAPS_APPEARANCE_CATEGORY_CYCLING_POWER_SENSOR = 1156, /**< Cycling: Power Sensor */
    BLE_GAPS_APPEARANCE_CATEGORY_CYCLING_SPEED_AND_CADENCE_SENSOR = 1157, /**< Cycling Speed and Cadence Sensor */
    BLE_GAPS_APPEARANCE_CATEGORY_GENERIC_PULSE_OXIMETER = 3136, /**< Generic Pulse Oximeter */
    BLE_GAPS_APPEARANCE_CATEGORY_FINGERTIP = 3137, /**< Fingertip */
    BLE_GAPS_APPEARANCE_CATEGORY_WRIST_WORN = 3138, /**< Wrist Worn */
    BLE_GAPS_APPEARANCE_CATEGORY_GENERIC__WEIGHT_SCALE = 3200, /**< Generic Weight Scale */
    BLE_GAPS_APPEARANCE_CATEGORY_GENERIC_OUTDOOR_SPORTS_ACTIVITY = 5184, /**< Generic Outdoor Sports Activity */
    BLE_GAPS_APPEARANCE_CATEGORY_LOCATION_DISPLAY_DEVICE = 5185, /**< Location Display Device */
    BLE_GAPS_APPEARANCE_CATEGORY_LOCATION_AND_NAVIGATION_DISPLAY_DEVICE = 5186, /**< Location and Navigation Display Device */
    BLE_GAPS_APPEARANCE_CATEGORY_LOCATION_POD = 5187, /**< Location Pod */
    BLE_GAPS_APPEARANCE_CATEGORY_LOCATION_AND_NAVIGATION_POD = 5188, /**< Location and Navigation Pod */
} e_ble_gaps_appearance_category_t;

/***************************************************************************//**
 * @brief     Set Appearance characteristic value to the local GATT database.
 * @param[in] p_value  Characteristic value to set.
 * @return    @ref ble_status_t
*******************************************************************************/
ble_status_t R_BLE_GAPS_SetAppearance(const uint16_t *p_value);

/***************************************************************************//**
 * @brief     Get Appearance characteristic value from the local GATT database.
 * @param[out] p_value  Output location for the acquired descriptor value.
 * @return    @ref ble_status_t
*******************************************************************************/
ble_status_t R_BLE_GAPS_GetAppearance(uint16_t *p_value);

/*----------------------------------------------------------------------------------------------------------------------
    Peripheral Preferred Connection Parameters Characteristic : The Peripheral Preferred Connection Parameters (PPCP) characteristic contains the preferred connection parameters of the Peripheral
----------------------------------------------------------------------------------------------------------------------*/
/***************************************************************************//**
 * @brief Peripheral Preferred Connection Parameters value structure.
*******************************************************************************/
typedef struct {
    uint16_t minimum_connection_interval; /**< Minimum Connection Interval */
    uint16_t maximum_connection_interval; /**< Maximum Connection Interval */
    uint16_t slave_latency; /**< Slave Latency */
    uint16_t connection_supervision_timeout_multiplier; /**< Connection Supervision Timeout Multiplier */
} st_ble_gaps_per_pref_conn_param_t;

/***************************************************************************//**
 * @brief     Set Peripheral Preferred Connection Parameters characteristic value to the local GATT database.
 * @param[in] p_value  Characteristic value to set.
 * @return    @ref ble_status_t
*******************************************************************************/
ble_status_t R_BLE_GAPS_SetPerPrefConnParam(const st_ble_gaps_per_pref_conn_param_t *p_value);

/***************************************************************************//**
 * @brief     Get Peripheral Preferred Connection Parameters characteristic value from the local GATT database.
 * @param[out] p_value  Output location for the acquired descriptor value.
 * @return    @ref ble_status_t
*******************************************************************************/
ble_status_t R_BLE_GAPS_GetPerPrefConnParam(st_ble_gaps_per_pref_conn_param_t *p_value);

/*----------------------------------------------------------------------------------------------------------------------
    Central Address Resolution Characteristic : The Peripheral should check if the peer device supports address resolution by reading the Central Address Resolution characteristic.
----------------------------------------------------------------------------------------------------------------------*/
/***************************************************************************//**
 * @brief     Set Central Address Resolution characteristic value to the local GATT database.
 * @param[in] p_value  Characteristic value to set.
 * @return    @ref ble_status_t
*******************************************************************************/
ble_status_t R_BLE_GAPS_SetCentAddrRslv(const uint8_t *p_value);

/***************************************************************************//**
 * @brief     Get Central Address Resolution characteristic value from the local GATT database.
 * @param[out] p_value  Output location for the acquired descriptor value.
 * @return    @ref ble_status_t
*******************************************************************************/
ble_status_t R_BLE_GAPS_GetCentAddrRslv(uint8_t *p_value);

/*----------------------------------------------------------------------------------------------------------------------
    Resolvable Private Address Only Characteristic : The device should check if the peer will only use Resolvable Private Addresses (RPAs) after bonding by reading the Resolvable Private Address Only characteristic.
----------------------------------------------------------------------------------------------------------------------*/
/***************************************************************************//**
 * @brief     Set Resolvable Private Address Only characteristic value to the local GATT database.
 * @param[in] p_value  Characteristic value to set.
 * @return    @ref ble_status_t
*******************************************************************************/
ble_status_t R_BLE_GAPS_SetRslvPrivAddrOnly(const uint8_t *p_value);

/***************************************************************************//**
 * @brief     Get Resolvable Private Address Only characteristic value from the local GATT database.
 * @param[out] p_value  Output location for the acquired descriptor value.
 * @return    @ref ble_status_t
*******************************************************************************/
ble_status_t R_BLE_GAPS_GetRslvPrivAddrOnly(uint8_t *p_value);

/*----------------------------------------------------------------------------------------------------------------------
    GAP Service Server
----------------------------------------------------------------------------------------------------------------------*/

/***************************************************************************//**
 * @brief GAP Service characteristic Index.
*******************************************************************************/
typedef enum {
    BLE_GAPS_DEV_NAME_IDX,
    BLE_GAPS_APPEARANCE_IDX,
    BLE_GAPS_PER_PREF_CONN_PARAM_IDX,
    BLE_GAPS_CENT_ADDR_RSLV_IDX,
    BLE_GAPS_RSLV_PRIV_ADDR_ONLY_IDX,
} e_ble_gaps_char_idx_t;

/***************************************************************************//**
 * @brief GAP Service event type.
*******************************************************************************/
typedef enum {
    /* Device Name */
    BLE_GAPS_EVENT_DEV_NAME_WRITE_REQ = BLE_SERVS_ATTR_EVENT(BLE_GAPS_DEV_NAME_IDX, BLE_SERVS_WRITE_REQ),
    BLE_GAPS_EVENT_DEV_NAME_WRITE_COMP = BLE_SERVS_ATTR_EVENT(BLE_GAPS_DEV_NAME_IDX, BLE_SERVS_WRITE_COMP),
    BLE_GAPS_EVENT_DEV_NAME_READ_REQ = BLE_SERVS_ATTR_EVENT(BLE_GAPS_DEV_NAME_IDX, BLE_SERVS_READ_REQ),
    /* Appearance */
    BLE_GAPS_EVENT_APPEARANCE_READ_REQ = BLE_SERVS_ATTR_EVENT(BLE_GAPS_APPEARANCE_IDX, BLE_SERVS_READ_REQ),
    /* Peripheral Preferred Connection Parameters */
    BLE_GAPS_EVENT_PER_PREF_CONN_PARAM_READ_REQ = BLE_SERVS_ATTR_EVENT(BLE_GAPS_PER_PREF_CONN_PARAM_IDX, BLE_SERVS_READ_REQ),
    /* Central Address Resolution */
    BLE_GAPS_EVENT_CENT_ADDR_RSLV_READ_REQ = BLE_SERVS_ATTR_EVENT(BLE_GAPS_CENT_ADDR_RSLV_IDX, BLE_SERVS_READ_REQ),
    /* Resolvable Private Address Only */
    BLE_GAPS_EVENT_RSLV_PRIV_ADDR_ONLY_READ_REQ = BLE_SERVS_ATTR_EVENT(BLE_GAPS_RSLV_PRIV_ADDR_ONLY_IDX, BLE_SERVS_READ_REQ),
} e_ble_gaps_event_t;

/***************************************************************************//**
 * @brief     Initialize GAP Service server.
 * @param[in] cb Service callback.
 * @return    @ref ble_status_t
*******************************************************************************/
ble_status_t R_BLE_GAPS_Init(ble_servs_app_cb_t cb);

/* Start user code for function declarations. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

#endif /* R_BLE_GAPS_H */

/** @} */

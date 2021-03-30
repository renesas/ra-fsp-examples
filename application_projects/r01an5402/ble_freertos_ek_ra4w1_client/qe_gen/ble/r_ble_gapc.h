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
* Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
 * File Name: r_ble_gapc.h
 * Version : 1.0
 * Description : The header file for Generic Access client.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * History : DD.MM.YYYY Version Description
 *         : 31.12.2999 1.00 First Release
 ***********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @file
 * @defgroup gapc Generic Access Client
 * @{
 * @ingroup profile
 * @brief   This is the client for the Generic Access Service.
 **********************************************************************************************************************/
#include "profile_cmn/r_ble_servc_if.h"

#ifndef R_BLE_GAPC_H
#define R_BLE_GAPC_H

/*----------------------------------------------------------------------------------------------------------------------
    Device Name Characteristic
----------------------------------------------------------------------------------------------------------------------*/
#define BLE_GAPC_DEV_NAME_UUID (0x2A00)
#define BLE_GAPC_DEV_NAME_LEN (128)
/***************************************************************************//**
 * @brief Device Name value structure.
*******************************************************************************/
typedef struct {
    char name[BLE_GAPC_DEV_NAME_LEN]; /**< Name */
    uint8_t length; /**< Length */
} st_ble_gapc_dev_name_t;

/***************************************************************************//**
 * @brief Device Name attribute handle value.
*******************************************************************************/
typedef struct {
    st_ble_gatt_hdl_range_t range;
} st_ble_gapc_dev_name_attr_hdl_t;

/***************************************************************************//**
 * @brief     Read Device Name characteristic value from the remote GATT database.
 * @param[in] conn_hdl  Connection handle.
 * @return    @ref ble_status_t
*******************************************************************************/
ble_status_t R_BLE_GAPC_ReadDevName(uint16_t conn_hdl);

/***************************************************************************//**
 * @brief     Write Device Name characteristic value to remote GATT database.
 * @param[in] conn_hdl  Connection handle.
 * @param[in] p_value Device Name characteristic value to write.
 * @return    @ref ble_status_t
*******************************************************************************/
ble_status_t R_BLE_GAPC_WriteDevName(uint16_t conn_hdl, const st_ble_gapc_dev_name_t *p_value);
;
/***************************************************************************//**
 * @brief      Get Device Name attribute handles.
 * @param[in]  p_addr Bluetooth device address for the attribute handles.
 * @param[out] p_hdl  The pointer to store the retrieved attribute handles.
 * @return    @ref ble_status_t
*******************************************************************************/
void R_BLE_GAPC_GetDevNameAttrHdl(const st_ble_dev_addr_t *p_addr, st_ble_gapc_dev_name_attr_hdl_t *p_hdl);

/*----------------------------------------------------------------------------------------------------------------------
    Appearance Characteristic
----------------------------------------------------------------------------------------------------------------------*/
#define BLE_GAPC_APPEARANCE_UUID (0x2A01)
#define BLE_GAPC_APPEARANCE_LEN (2)
/***************************************************************************//**
 * @brief Appearance Category enumeration.
*******************************************************************************/
typedef enum {
    BLE_GAPC_APPEARANCE_CATEGORY_UNKNOWN = 0, /**< Unknown */
    BLE_GAPC_APPEARANCE_CATEGORY_GENERIC_PHONE = 64, /**< Generic Phone */
    BLE_GAPC_APPEARANCE_CATEGORY_GENERIC_COMPUTER = 128, /**< Generic Computer */
    BLE_GAPC_APPEARANCE_CATEGORY_GENERIC_WATCH = 192, /**< Generic Watch */
    BLE_GAPC_APPEARANCE_CATEGORY_WATCH_SPORTS_WATCH = 193, /**< Watch: Sports Watch */
    BLE_GAPC_APPEARANCE_CATEGORY_GENERIC_CLOCK = 256, /**< Generic Clock */
    BLE_GAPC_APPEARANCE_CATEGORY_GENERIC_DISPLAY = 320, /**< Generic Display */
    BLE_GAPC_APPEARANCE_CATEGORY_GENERIC_REMOTE_CONTROL = 384, /**< Generic Remote Control */
    BLE_GAPC_APPEARANCE_CATEGORY_GENERIC_EYE_GLASSES = 448, /**< Generic Eye-glasses */
    BLE_GAPC_APPEARANCE_CATEGORY_GENERIC_TAG = 512, /**< Generic Tag */
    BLE_GAPC_APPEARANCE_CATEGORY_GENERIC_KEYRING = 576, /**< Generic Keyring */
    BLE_GAPC_APPEARANCE_CATEGORY_GENERIC_MEDIA_PLAYER = 640, /**< Generic Media Player */
    BLE_GAPC_APPEARANCE_CATEGORY_GENERIC_BARCODE_SCANNER = 704, /**< Generic Barcode Scanner */
    BLE_GAPC_APPEARANCE_CATEGORY_GENERIC_THERMOMETER = 768, /**< Generic Thermometer */
    BLE_GAPC_APPEARANCE_CATEGORY_THERMOMETER_EAR = 769, /**< Thermometer Ear */
    BLE_GAPC_APPEARANCE_CATEGORY_GENERIC_HEART_RATE_SENSOR = 832, /**< Generic Heart rate Sensor */
    BLE_GAPC_APPEARANCE_CATEGORY_HEART_RATE_SENSOR_HEART_RATE_BELT = 833, /**< Heart Rate Sensor Heart Rate Belt */
    BLE_GAPC_APPEARANCE_CATEGORY_GENERIC_BLOOD_PRESSURE = 896, /**< Generic Blood Pressure */
    BLE_GAPC_APPEARANCE_CATEGORY_BLOOD_PRESSURE_ARM = 897, /**< Blood Pressure: Arm */
    BLE_GAPC_APPEARANCE_CATEGORY_BLOOD_PRESSURE_WRIST = 898, /**< Blood Pressure: Wrist */
    BLE_GAPC_APPEARANCE_CATEGORY_HUMAN_INTERFACE_DEVICE = 960, /**< Human Interface Device (HID) */
    BLE_GAPC_APPEARANCE_CATEGORY_KEYBOARD = 961, /**< Keyboard */
    BLE_GAPC_APPEARANCE_CATEGORY_MOUSE = 962, /**< Mouse */
    BLE_GAPC_APPEARANCE_CATEGORY_JOYSTICK = 963, /**< Joystick */
    BLE_GAPC_APPEARANCE_CATEGORY_GAMEPAD = 964, /**< Gamepad */
    BLE_GAPC_APPEARANCE_CATEGORY_DIGITIZER_TABLET = 965, /**< Digitizer Tablet */
    BLE_GAPC_APPEARANCE_CATEGORY_CARD_READER = 966, /**< Card Reader */
    BLE_GAPC_APPEARANCE_CATEGORY_DIGITAL_PEN = 967, /**< Digital Pen */
    BLE_GAPC_APPEARANCE_CATEGORY_BARCODE_SCANNER = 968, /**< Barcode Scanner */
    BLE_GAPC_APPEARANCE_CATEGORY_GENERIC_GLUCOSE_METER = 1024, /**< Generic Glucose Meter */
    BLE_GAPC_APPEARANCE_CATEGORY_GENERIC_RUNNING_WALKING_SENSOR = 1088, /**< Generic: Running Walking Sensor */
    BLE_GAPC_APPEARANCE_CATEGORY_RUNNING_WALKING_SENSOR_IN_SHOE = 1089, /**< Running Walking Sensor: In-Shoe */
    BLE_GAPC_APPEARANCE_CATEGORY_RUNNING_WALKING_SENSOR_ON_SHOE = 1090, /**< Running Walking Sensor: On-Shoe */
    BLE_GAPC_APPEARANCE_CATEGORY_RUNNING_WALKING_SENSOR_ON_HIP = 1091, /**< Running Walking Sensor: On-Hip */
    BLE_GAPC_APPEARANCE_CATEGORY_GENERIC__CYCLING = 1152, /**< Generic Cycling */
    BLE_GAPC_APPEARANCE_CATEGORY_CYCLING_CYCLING_COMPUTER = 1153, /**< Cycling Cycling Computer */
    BLE_GAPC_APPEARANCE_CATEGORY_CYCLING_SPEED_SENSOR = 1154, /**< Cycling Speed Sensor */
    BLE_GAPC_APPEARANCE_CATEGORY_CYCLING_CADENCE_SENSOR = 1155, /**< Cycling Cadence Sensor */
    BLE_GAPC_APPEARANCE_CATEGORY_CYCLING_POWER_SENSOR = 1156, /**< Cycling: Power Sensor */
    BLE_GAPC_APPEARANCE_CATEGORY_CYCLING_SPEED_AND_CADENCE_SENSOR = 1157, /**< Cycling Speed and Cadence Sensor */
    BLE_GAPC_APPEARANCE_CATEGORY_GENERIC_PULSE_OXIMETER = 3136, /**< Generic Pulse Oximeter */
    BLE_GAPC_APPEARANCE_CATEGORY_FINGERTIP = 3137, /**< Fingertip */
    BLE_GAPC_APPEARANCE_CATEGORY_WRIST_WORN = 3138, /**< Wrist Worn */
    BLE_GAPC_APPEARANCE_CATEGORY_GENERIC__WEIGHT_SCALE = 3200, /**< Generic Weight Scale */
    BLE_GAPC_APPEARANCE_CATEGORY_GENERIC_OUTDOOR_SPORTS_ACTIVITY = 5184, /**< Generic Outdoor Sports Activity */
    BLE_GAPC_APPEARANCE_CATEGORY_LOCATION_DISPLAY_DEVICE = 5185, /**< Location Display Device */
    BLE_GAPC_APPEARANCE_CATEGORY_LOCATION_AND_NAVIGATION_DISPLAY_DEVICE = 5186, /**< Location and Navigation Display Device */
    BLE_GAPC_APPEARANCE_CATEGORY_LOCATION_POD = 5187, /**< Location Pod */
    BLE_GAPC_APPEARANCE_CATEGORY_LOCATION_AND_NAVIGATION_POD = 5188, /**< Location and Navigation Pod */
} e_ble_appearance_category_t;

/***************************************************************************//**
 * @brief Appearance attribute handle value.
*******************************************************************************/
typedef struct {
    st_ble_gatt_hdl_range_t range;
} st_ble_gapc_appearance_attr_hdl_t;

/***************************************************************************//**
 * @brief     Read Appearance characteristic value from the remote GATT database.
 * @param[in] conn_hdl  Connection handle.
 * @return    @ref ble_status_t
*******************************************************************************/
ble_status_t R_BLE_GAPC_ReadAppearance(uint16_t conn_hdl);

/***************************************************************************//**
 * @brief      Get Appearance attribute handles.
 * @param[in]  p_addr Bluetooth device address for the attribute handles.
 * @param[out] p_hdl  The pointer to store the retrieved attribute handles.
 * @return    @ref ble_status_t
*******************************************************************************/
void R_BLE_GAPC_GetAppearanceAttrHdl(const st_ble_dev_addr_t *p_addr, st_ble_gapc_appearance_attr_hdl_t *p_hdl);

/*----------------------------------------------------------------------------------------------------------------------
    Peripheral Preferred Connection Parameters Characteristic
----------------------------------------------------------------------------------------------------------------------*/
#define BLE_GAPC_PER_PREF_CONN_PARAM_UUID (0x2A04)
#define BLE_GAPC_PER_PREF_CONN_PARAM_LEN (8)
/***************************************************************************//**
 * @brief Peripheral Preferred Connection Parameters value structure.
*******************************************************************************/
typedef struct {
    uint16_t min_conn_intv; /**< Minimum Connection Interval */
    uint16_t max_conn_intv; /**< Maximum Connection Interval */
    uint16_t slave_latency; /**< Slave Latency */
    uint16_t conn_sup_timeout_multiplier; /**< Connection Supervision Timeout Multiplier */
} st_ble_gapc_per_pref_conn_param_t;

/***************************************************************************//**
 * @brief Peripheral Preferred Connection Parameters attribute handle value.
*******************************************************************************/
typedef struct {
    st_ble_gatt_hdl_range_t range;
} st_ble_gapc_per_pref_conn_param_attr_hdl_t;

/***************************************************************************//**
 * @brief     Read Peripheral Preferred Connection Parameters characteristic value from the remote GATT database.
 * @param[in] conn_hdl  Connection handle.
 * @return    @ref ble_status_t
*******************************************************************************/
ble_status_t R_BLE_GAPC_ReadPerPrefConnParam(uint16_t conn_hdl);

/***************************************************************************//**
 * @brief      Get Peripheral Preferred Connection Parameters attribute handles.
 * @param[in]  p_addr Bluetooth device address for the attribute handles.
 * @param[out] p_hdl  The pointer to store the retrieved attribute handles.
 * @return    @ref ble_status_t
*******************************************************************************/
void R_BLE_GAPC_GetPerPrefConnParamAttrHdl(const st_ble_dev_addr_t *p_addr, st_ble_gapc_per_pref_conn_param_attr_hdl_t *p_hdl);

/*----------------------------------------------------------------------------------------------------------------------
    Central Address Resolution Characteristic
----------------------------------------------------------------------------------------------------------------------*/
#define BLE_GAPC_CENT_ADDR_RSLV_UUID (0x2AA6)
#define BLE_GAPC_CENT_ADDR_RSLV_LEN (1)
/***************************************************************************//**
 * @brief Central Address Resolution attribute handle value.
*******************************************************************************/
typedef struct {
    st_ble_gatt_hdl_range_t range;
} st_ble_gapc_cent_addr_rslv_attr_hdl_t;

/***************************************************************************//**
 * @brief     Read Central Address Resolution characteristic value from the remote GATT database.
 * @param[in] conn_hdl  Connection handle.
 * @return    @ref ble_status_t
*******************************************************************************/
ble_status_t R_BLE_GAPC_ReadCentAddrRslv(uint16_t conn_hdl);

/***************************************************************************//**
 * @brief      Get Central Address Resolution attribute handles.
 * @param[in]  p_addr Bluetooth device address for the attribute handles.
 * @param[out] p_hdl  The pointer to store the retrieved attribute handles.
 * @return    @ref ble_status_t
*******************************************************************************/
void R_BLE_GAPC_GetCentAddrRslvAttrHdl(const st_ble_dev_addr_t *p_addr, st_ble_gapc_cent_addr_rslv_attr_hdl_t *p_hdl);

/*----------------------------------------------------------------------------------------------------------------------
    Resolvable Private Address Only Characteristic
----------------------------------------------------------------------------------------------------------------------*/
#define BLE_GAPC_RSLV_PRIV_ADDR_ONLY_UUID (0x2AC9)
#define BLE_GAPC_RSLV_PRIV_ADDR_ONLY_LEN (1)
/***************************************************************************//**
 * @brief Resolvable Private Address Only attribute handle value.
*******************************************************************************/
typedef struct {
    st_ble_gatt_hdl_range_t range;
} st_ble_gapc_rslv_priv_addr_only_attr_hdl_t;

/***************************************************************************//**
 * @brief     Read Resolvable Private Address Only characteristic value from the remote GATT database.
 * @param[in] conn_hdl  Connection handle.
 * @return    @ref ble_status_t
*******************************************************************************/
ble_status_t R_BLE_GAPC_ReadRslvPrivAddrOnly(uint16_t conn_hdl);

/***************************************************************************//**
 * @brief      Get Resolvable Private Address Only attribute handles.
 * @param[in]  p_addr Bluetooth device address for the attribute handles.
 * @param[out] p_hdl  The pointer to store the retrieved attribute handles.
 * @return    @ref ble_status_t
*******************************************************************************/
void R_BLE_GAPC_GetRslvPrivAddrOnlyAttrHdl(const st_ble_dev_addr_t *p_addr, st_ble_gapc_rslv_priv_addr_only_attr_hdl_t *p_hdl);


/*----------------------------------------------------------------------------------------------------------------------
    Generic Access Client
----------------------------------------------------------------------------------------------------------------------*/

/***************************************************************************//**
 * @brief Generic Access client event data.
*******************************************************************************/
typedef struct {
    uint16_t    conn_hdl;  /**< Connection handle */
    uint16_t    param_len; /**< Event parameter length */
    const void *p_param;   /**< Event parameter */
} st_ble_gapc_evt_data_t;

/***************************************************************************//**
 * @brief Generic Access characteristic ID.
*******************************************************************************/
typedef enum {
    BLE_GAPC_DEV_NAME_IDX,
    BLE_GAPC_APPEARANCE_IDX,
    BLE_GAPC_PER_PREF_CONN_PARAM_IDX,
    BLE_GAPC_CENT_ADDR_RSLV_IDX,
    BLE_GAPC_RSLV_PRIV_ADDR_ONLY_IDX,
} st_ble_gapc_char_idx_t;

/***************************************************************************//**
 * @brief Generic Access client event type.
*******************************************************************************/
typedef enum {
    /* Device Name */
    BLE_GAPC_EVENT_DEV_NAME_READ_RSP = BLE_SERVC_ATTR_EVENT(BLE_GAPC_DEV_NAME_IDX, BLE_SERVC_READ_RSP),
    BLE_GAPC_EVENT_DEV_NAME_WRITE_RSP = BLE_SERVC_ATTR_EVENT(BLE_GAPC_DEV_NAME_IDX, BLE_SERVC_WRITE_RSP),
    /* Appearance */
    BLE_GAPC_EVENT_APPEARANCE_READ_RSP = BLE_SERVC_ATTR_EVENT(BLE_GAPC_APPEARANCE_IDX, BLE_SERVC_READ_RSP),
    /* Peripheral Preferred Connection Parameters */
    BLE_GAPC_EVENT_PER_PREF_CONN_PARAM_READ_RSP = BLE_SERVC_ATTR_EVENT(BLE_GAPC_PER_PREF_CONN_PARAM_IDX, BLE_SERVC_READ_RSP),
    /* Central Address Resolution */
    BLE_GAPC_EVENT_CENT_ADDR_RSLV_READ_RSP = BLE_SERVC_ATTR_EVENT(BLE_GAPC_CENT_ADDR_RSLV_IDX, BLE_SERVC_READ_RSP),
    /* Resolvable Private Address Only */
    BLE_GAPC_EVENT_RSLV_PRIV_ADDR_ONLY_READ_RSP = BLE_SERVC_ATTR_EVENT(BLE_GAPC_RSLV_PRIV_ADDR_ONLY_IDX, BLE_SERVC_READ_RSP),
} e_ble_gapc_event_t;

/***************************************************************************//**
 * @brief     Initialize Generic Access client.
 * @param[in] cb Client callback.
 * @return    @ref ble_status_t
*******************************************************************************/
ble_status_t R_BLE_GAPC_Init(ble_servc_app_cb_t cb);

/***************************************************************************//**
 * @brief     Generic Access client discovery callback.
 * @param[in] conn_hdl Connection handle
 * @param[in] serv_idx Service instance index.
 * @param[in] type     Service discovery event type.
 * @param[in] p_param  Service discovery event parameter.
 * @return    @ref ble_status_t
*******************************************************************************/
void R_BLE_GAPC_ServDiscCb(uint16_t conn_hdl, uint8_t serv_idx, uint16_t type, void *p_param);

/***************************************************************************//**
 * @brief     Get Generic Access client attribute handle.
 * @param[in]  p_addr Bluetooth device address for the attribute handles.
 * @param[out] p_hdl  The pointer to store the retrieved attribute handles.
*******************************************************************************/
void R_BLE_GAPC_GetServAttrHdl(const st_ble_dev_addr_t *p_addr, st_ble_gatt_hdl_range_t *p_hdl);

#endif /* R_BLE_GAPC_H */

/** @} */

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

/*******************************************************************************************************************//**
 * @file
 * @defgroup profile_cmn_cli Profile Common Client Library
 * @{
 * @ingroup profile_cmn
 * @brief Profile Common Client Library Library
 * @details This library provides APIs to implement GATT profile client.
***********************************************************************************************************************/
#include "r_ble_serv_common.h"

#ifndef R_BLE_SERVC_IF_H
#define R_BLE_SERVC_IF_H

/***********************************************************************************************************************
 Macro definitions
 **********************************************************************************************************************/
/** @defgroup profile_cmn_cli_macro Macros 
 *  @{
 *  @brief Macro definition
 */

/** 
 * @def BLE_SERVC_MAX_NUM_OF_SAVED
 * @brief Max number of connection to be saved.
 */
#define BLE_SERVC_MAX_NUM_OF_SAVED      (BLE_PRF_CONN_MAX)

/** 
 * @def BLE_SERVC_MAX_NUM_OF_CLIENTS
 * @brief Max number of client service to be saved.
 */
#define BLE_SERVC_MAX_NUM_OF_CLIENTS    (10)

/* attr_idx:6bit, inst_idx:4bit, type_idx:1bit, evt_idx:5bit */
/** 
 * @def BLE_SERVC_MULTI_ATTR_EVENT
 * @brief Event type used in callback. This macro is used if same service is used.
 * @param[in] attr_idx Attribute index. 6bit is used.
 * @param[in] inst_idx Service index. 4bit is used.
 * @param[in] evt_idx Event index. 5bit is used.
 */
#define BLE_SERVC_MULTI_ATTR_EVENT(attr_idx, inst_idx, evt_idx) ((uint16_t)((attr_idx << 10) | (inst_idx << 6) | (evt_idx << 0)))

/** 
 * @def BLE_SERVC_ATTR_EVENT
 * @brief Event type used in callback. 
 * @param[in] attr_idx Attribute index. 6bit is used.
 * @param[in] evt_idx Event index. 5bit is used.
 */
#define BLE_SERVC_ATTR_EVENT(attr_idx, evt_idx) BLE_SERVC_MULTI_ATTR_EVENT(attr_idx, 0, evt_idx)

/** 
 * @enum e_ble_servc_event_t
 * @brief Client callback events.
 */
typedef enum {
    /** 
     * @brief Receive a write response.
     */
    BLE_SERVC_WRITE_RSP,
    /** 
     * @brief Receive a read response.
     */
    BLE_SERVC_READ_RSP,
    /** 
     * @brief Receive a notification.
     */
    BLE_SERVC_HDL_VAL_NTF,
    /** 
     * @brief Receive a indication.
     */
    BLE_SERVC_HDL_VAL_IND,
} e_ble_servc_event_t;
/*@}*/

/** @defgroup profile_cmn_cli_struct Structures
 *  @{
 *  @brief Structure definition
 */
/******************************************************************************************************************//**
 * @struct st_ble_servc_evt_data_t
 * @brief  st_ble_servc_evt_data_t includes connection handle and value for connection parameter.
 **********************************************************************************************************************/
typedef struct {
    uint16_t    conn_hdl;  /**< Connection handle. */
    uint16_t    param_len; /**< Event parameter length. */
    const void *p_param;   /**< Event parameter.\n Value is set after decode function is called. */
} st_ble_servc_evt_data_t;
/*@}*/

/** @defgroup profile_cmn_cli_callback Callbacks
 *  @{
 *  @brief Callback definition
 */
/*************************************************************************************************************//**
 * @brief   Callback invoked when write response received.
 * @param[in]   p_attr Information structure of characteristic or descriptor.
 * @param[in]   conn_hdl Connection handle.
 * @param[in]   result Result of write response.
***********************************************************************************************************************/
typedef void (*ble_servc_attr_write_rsp_t)(const void *p_attr, uint16_t conn_hdl, ble_status_t result);

/*************************************************************************************************************//**
 * @brief   Callback invoked when read response received.
 * @param[in]   p_attr Information structure of characteristic or descriptor.
 * @param[in]   conn_hdl Connection handle.
 * @param[in]   result Result of read response.
 * @param[in]   p_app_value Characteristic value or descriptor value of read response.
***********************************************************************************************************************/
typedef void (*ble_servc_attr_read_rsp_t)(const void *p_attr, uint16_t conn_hdl, ble_status_t result, const void *p_app_value);

/*************************************************************************************************************//**
 * @brief   Callback invoked when notification received.
 * @param[in]   p_attr Information structure of characteristic.
 * @param[in]   conn_hdl Connection handle.
 * @param[in]   p_app_value Characteristic value of notification.
***********************************************************************************************************************/
typedef void (*ble_servc_attr_hdl_val_ntf_t)(const void *p_attr, uint16_t conn_hdl, const void *p_app_value);

/*************************************************************************************************************//**
 * @brief   Callback invoked when indication received.
 * @param[in]   p_attr Information structure of characteristic.
 * @param[in]   conn_hdl Connection handle.
 * @param[in]   p_app_value Characteristic value of indication.
***********************************************************************************************************************/
typedef void (*ble_servc_attr_hdl_val_ind_t)(const void *p_attr, uint16_t conn_hdl, const void *p_app_value);

/*************************************************************************************************************//**
 * @brief   Callback invoked when events defined in service occurred.
 * @param[in]   type Event type of callback. Refer e_ble_XXX_event_t of each service.
 * @param[in]   result Result of event.
 * @param[in]   p_data Event data.
***********************************************************************************************************************/
typedef void (*ble_servc_app_cb_t)(uint16_t type, ble_status_t result, st_ble_servc_evt_data_t *p_data);
/*@}*/

/** @defgroup profile_cmn_cli_func Functions
 *  @{
 *  @brief Function definition
 */
/*************************************************************************************************************//**
 * @brief Decode function. Convert data from GATT database value to application data value.
 * @param[out]  p_app_value Application data value of characteristic or descriptor.
 * @param[in]   p_gatt_value GATT database value of characteristic or descriptor.
***********************************************************************************************************************/
typedef ble_status_t (*ble_servc_attr_decode_t)(void *p_app_value, const st_ble_gatt_value_t *p_gatt_value);

/*************************************************************************************************************//**
 * @brief Encode function. Convert data from application data value to GATT database value.
 * @param[in]   p_app_value Application data value of characteristic or descriptor.
 * @param[out]  p_gatt_value GATT database value of characteristic or descriptor.
***********************************************************************************************************************/
typedef ble_status_t (*ble_servc_attr_encode_t)(const void *p_app_value, st_ble_gatt_value_t *p_gatt_value);
/*@}*/

/** @defgroup profile_cmn_cli_struct Structures
 *  @{
 *  @brief Structure definition
 */
/******************************************************************************************************************//**
 * @struct st_ble_servc_desc_info_t
 * @brief  st_ble_servc_desc_info_t includes information about descriptor.
 **********************************************************************************************************************/
typedef struct {
    uint16_t                    uuid_16;        /**< 16bit UUID of descriptor. */
    const uint8_t              *uuid_128;       /**< 128bit UUID of descriptor. */
    uint8_t                     uuid_type;      /**< Select from 16bit UUID or 128bit UUID.\n 128bit UUID should be used for custom service.\n 16bit UUID = BLE_GATT_16_BIT_UUID_FORMAT\n 128bit UUID = BLE_GATT_128_BIT_UUID_FORMAT */
    uint8_t                     desc_idx;       /**< Index of descriptor. */
    uint8_t                     inst_idx;       /**< Index used if same descriptor is set in one characteristic. */
    uint16_t                   *p_attr_hdls;    /**< Attribute handle for each connection. */
    uint16_t                    app_size;       /**< Descriptor value size used in application. */
    uint16_t                    db_size;        /**< Descriptor value size used in GATT database. */
    ble_servc_attr_write_rsp_t  write_rsp_cb;   /**< Write response callback function. Set function if needed. */
    ble_servc_attr_read_rsp_t   read_rsp_cb;    /**< Read response callback function. Set function if needed. */
    ble_servc_attr_decode_t     decode;         /**< Decode function. */
    ble_servc_attr_encode_t     encode;         /**< Encode function. */
} st_ble_servc_desc_info_t;

/******************************************************************************************************************//**
 * @struct st_ble_servc_char_info_t
 * @brief  st_ble_servc_char_info_t includes information about characteristic.
 **********************************************************************************************************************/
typedef struct {
    uint16_t                         uuid_16;           /**< 16bit UUID of characteristic*/
    const uint8_t                   *uuid_128;          /**< 128bit UUID of characteristic*/
    uint8_t                          uuid_type;         /**< Select from 16bit UUID or 128bit UUID.\n 128bit UUID should be used for custom service.\n 16bit UUID = BLE_GATT_16_BIT_UUID_FORMAT\n 128bit UUID = BLE_GATT_128_BIT_UUID_FORMAT */
    st_ble_gatt_hdl_range_t         *p_attr_hdls;       /**< Attribute handle range of characteristic. */
    uint8_t                          char_idx;          /**< Index of characteristic. */
    uint8_t                          inst_idx;          /**< Index used if same characteristic is set in one service. */
    uint16_t                         app_size;          /**< Size of characteristic value in Application. */
    uint16_t                         db_size;           /**< Size of characteristic value in GATT database. */
    const st_ble_servc_desc_info_t **pp_descs;          /**< Set all descriptor information structure included in this characteristic. */
    uint8_t                          num_of_descs;      /**< Number of descriptors included in this characteristic. */
    ble_servc_attr_write_rsp_t       write_rsp_cb;      /**< Write response callback function. Set function if needed. */
    ble_servc_attr_read_rsp_t        read_rsp_cb;       /**< Read response callback function. Set function if needed. */
    ble_servc_attr_hdl_val_ntf_t     hdl_val_ntf_cb;    /**< Notification callback function. Set function if needed. */
    ble_servc_attr_hdl_val_ind_t     hdl_val_ind_cb;    /**< Indication callback function. Set function if needed. */
    ble_servc_attr_decode_t          decode;            /**< Decode function. */
    ble_servc_attr_encode_t          encode;            /**< Encode function. */
} st_ble_servc_char_info_t;

/******************************************************************************************************************//**
 * @struct st_ble_servc_info_t
 * @brief  st_ble_servc_info_t includes information about service.
 **********************************************************************************************************************/
typedef struct {
    const st_ble_servc_char_info_t **pp_chars;      /**< Set all characteristic information structure included in this service. */
    uint16_t                         num_of_chars;  /**< Number of characteristics included in this service. */
    st_ble_gatt_hdl_range_t         *p_attr_hdls;   /**< Attribute handle range of service. */
    ble_servc_app_cb_t               cb;            /**< Service event callback function. */
} st_ble_servc_info_t;
/*@}*/

/** @defgroup profile_cmn_cli_func Functions
 *  @{
 *  @brief Function definition
 */
/******************************************************************************************************************//**
 * @brief   Initialize profile common client library. 
 * @return  See @ref ble_status_t 
 **********************************************************************************************************************/
ble_status_t R_BLE_SERVC_Init(void);

/******************************************************************************************************************//**
 * @brief   Register client service to profile common client library.
 * @param[in]   p_info Client service to be registered.
 * @return  See @ref ble_status_t
 **********************************************************************************************************************/
ble_status_t R_BLE_SERVC_RegisterClient(const st_ble_servc_info_t *p_info);

/******************************************************************************************************************//**
 * @brief   Callback function used for service discovery.
 * @param[in]   p_info      Client server information structure to be discovered.
 * @param[in]   conn_hdl    Connection handle.
 * @param[in]   serv_idx    Index used if same service is included in one profile.
 * @param[in]   type        Event type of discovery.
 * @param[in]   p_param     Parameter of discovered information.
 **********************************************************************************************************************/
void R_BLE_SERVC_ServDiscCb(const st_ble_servc_info_t *p_info, uint16_t conn_hdl, uint8_t serv_idx, uint16_t type, void *p_param);

/******************************************************************************************************************//**
 * @brief   Get connection handle from BD address.
 * @param[in]   p_addr BD address of connected device.
 * @return  connection handle.
 **********************************************************************************************************************/
uint8_t R_BLE_SERVC_GetConnIdx(const st_ble_dev_addr_t *p_addr);

/******************************************************************************************************************//**
 * @brief   Send read request of characteristic value for read operation.
 * @param[in]   p_attr Characteristic sending read request.
 * @param[in]   conn_hdl Connection handle.
 * @return  See @ref ble_status_t
 **********************************************************************************************************************/
ble_status_t R_BLE_SERVC_ReadChar(const st_ble_servc_char_info_t *p_attr, uint16_t conn_hdl);

/******************************************************************************************************************//**
 * @brief   Send write request of characteristic value for write operation.
 * @param[in]   p_attr Characteristic sending write request.
 * @param[in]   conn_hdl Connection handle.
 * @param[in]   p_app_value Value to be sent on write request.
 * @return  See @ref ble_status_t
 **********************************************************************************************************************/
ble_status_t R_BLE_SERVC_WriteChar(const st_ble_servc_char_info_t *p_attr, uint16_t conn_hdl, const void *p_app_value);

/******************************************************************************************************************//**
 * @brief   Send write command of characteristic value for write without request operation.
 * @param[in]   p_attr Characteristic sending write command.
 * @param[in]   conn_hdl Connection handle.
 * @param[in]   p_app_value Value to be sent on write command.
 * @return  See @ref ble_status_t
 **********************************************************************************************************************/
ble_status_t R_BLE_SERVC_WriteCmdChar(const st_ble_servc_char_info_t *p_attr, uint16_t conn_hdl, const void *p_app_value);

/******************************************************************************************************************//**
 * @brief   Send read request of descriptor value for read operation.
 * @param[in]   p_attr Descriptor sending read request.
 * @param[in]   conn_hdl Connection handle.
 * @return  See @ref ble_status_t
 **********************************************************************************************************************/
ble_status_t R_BLE_SERVC_ReadDesc(const st_ble_servc_desc_info_t *p_attr, uint16_t conn_hdl);

/******************************************************************************************************************//**
 * @brief   Send read request of descriptor value for read operation or read long operation.
 * @param[in]   p_attr Descriptor sending read request.
 * @param[in]   conn_hdl Connection handle.
 * @param[in]   type Defines read operation or read long operation.\n 0 = read operation\n Other value = read long operation
 * @return  See @ref ble_status_t
 **********************************************************************************************************************/
ble_status_t R_BLE_SERVC_ReadDesc_with_Type(const st_ble_servc_desc_info_t *p_attr, uint16_t conn_hdl, int32_t type);

/******************************************************************************************************************//**
 * @brief   Send write request of descriptor value for write operation.
 * @param[in]   p_attr Descriptor sending write request.
 * @param[in]   conn_hdl Connection handle.
 * @param[in]   p_app_value Value to be sent on write request.
 * @return  See @ref ble_status_t
 **********************************************************************************************************************/
ble_status_t R_BLE_SERVC_WriteDesc(const st_ble_servc_desc_info_t *p_attr, uint16_t conn_hdl, const void *p_app_value);

/******************************************************************************************************************//**
 * @brief   Send write request of variable descriptor value for write operation.
 * @param[in]   p_attr Descriptor sending write request.
 * @param[in]   conn_hdl Connection handle.
 * @param[in]   p_app_value Data value and size to be sent on write request.
 * @return  See @ref ble_status_t
 **********************************************************************************************************************/
ble_status_t R_BLE_SERVC_WriteDesc_with_Size(const st_ble_servc_desc_info_t *p_attr, uint16_t conn_hdl, const st_ble_seq_data_t *p_app_value);

/******************************************************************************************************************//**
 * @brief   Send write request of descriptor value for write without request operation.
 * @param[in]   p_attr Descriptor sending write request.
 * @param[in]   conn_hdl Connection handle.
 * @param[in]   p_app_value Value to be sent on write request.
 * @return  See @ref ble_status_t
 **********************************************************************************************************************/
ble_status_t R_BLE_SERVC_WriteDesc2(const st_ble_servc_desc_info_t *p_attr, uint16_t conn_hdl, const void *p_app_value);

/******************************************************************************************************************//**
 * @brief   Callback function used for GATT client events.
 * @param[in]   type Type of event.
 * @param[in]   result Result of request sent to server.
 * @param[in]   p_data Event data.
 * @details You need to call this function to enable all client callback event included in profile.
 **********************************************************************************************************************/
void R_BLE_SERVC_GattcCb(uint16_t type, ble_status_t result, st_ble_gattc_evt_data_t * p_data);
/*@}*/

#endif /* R_BLE_SERVC_IF_H */
/* @} */

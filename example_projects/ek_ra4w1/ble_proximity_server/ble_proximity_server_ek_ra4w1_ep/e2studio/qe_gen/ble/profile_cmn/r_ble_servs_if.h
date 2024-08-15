/***********************************************************************************************************************
* Copyright (c) 2019 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @file
 * @defgroup profile_cmn_ser Profile Common Server Library
 * @{
 * @ingroup profile_cmn
 * @brief Profile Common Server Library Library
 * @details This library provides APIs to implement GATT profile server.
***********************************************************************************************************************/
#include "r_ble_serv_common.h"

#ifndef R_BLE_SERVS_IF_H
#define R_BLE_SERVS_IF_H

/***********************************************************************************************************************
 Macro definitions
 **********************************************************************************************************************/
/** @defgroup profile_cmn_ser_macro Macros 
 *  @{
 *  @brief Macro definition
 */

/* attr_idx:6bit, inst_idx:4bit, type_idx:1bit, evt_idx:5bit */
/** 
 * @def BLE_SERVS_MULTI_ATTR_EVENT
 * @brief Event type used in callback. This macro is used if same service is used.
 * @param[in] attr_idx Attribute index. 6bit is used.
 * @param[in] inst_idx Service index. 4bit is used.
 * @param[in] evt_idx Event index. 5bit is used.
 */
#define BLE_SERVS_MULTI_ATTR_EVENT(attr_idx, inst_idx, evt_idx) ((uint16_t)((attr_idx << 10) | (inst_idx << 6) | (evt_idx << 0)))

/** 
 * @def BLE_SERVS_ATTR_EVENT
 * @brief Event type used in callback. 
 * @param[in] attr_idx Attribute index. 6bit is used.
 * @param[in] evt_idx Event index. 5bit is used.
 */
#define BLE_SERVS_ATTR_EVENT(attr_idx, evt_idx) BLE_SERVS_MULTI_ATTR_EVENT(attr_idx, 0, evt_idx)

/*******************************************************************************************************************//**
 * @brief Server callback events.
***********************************************************************************************************************/
typedef enum {
    BLE_SERVS_WRITE_REQ   = 0x00,   /**< Receive a write request */
    BLE_SERVS_WRITE_CMD   = 0x01,   /**< Receive a write command */
    BLE_SERVS_WRITE_COMP  = 0x02,   /**< Send a write response */
    BLE_SERVS_READ_REQ    = 0x03,   /**< Receive a read request */
    BLE_SERVS_HDL_VAL_CNF = 0x04,   /**< Receive a confirmation */
} e_ble_servs_event_t;
/*@}*/

/** @defgroup profile_cmn_ser_struct Structures
 *  @{
 *  @brief Structure definition
 */
typedef struct {
    uint16_t    conn_hdl;  /**< Connection handle. */
    uint16_t    param_len; /**< Event parameter length. */
    const void *p_param;   /**< Event parameter.\n Value is set after decode function is called. */
} st_ble_servs_evt_data_t;
/*@}*/

/** @defgroup profile_cmn_ser_callback Callbacks
 *  @{
 *  @brief Callback definition
 */
/*************************************************************************************************************//**
 * @brief   Callback invoked when write request received.
 * @param[in]   p_attr Information structure of characteristic or descriptor.
 * @param[in]   conn_hdl Connection handle.
 * @param[in]   result Result of write request.
 * @param[in]   p_app_value Characteristic value or descriptor value of write request.
***********************************************************************************************************************/
typedef void (*ble_servs_attr_write_req_t)(const void *p_attr, uint16_t conn_hdl, ble_status_t result, const void *p_app_value);

/*************************************************************************************************************//**
 * @brief   Callback invoked when write command received.
 * @param[in]   p_attr Information structure of characteristic.
 * @param[in]   conn_hdl Connection handle.
 * @param[in]   result Result of write command.
 * @param[in]   p_app_value Characteristic value of write command.
***********************************************************************************************************************/
typedef void (*ble_servs_attr_write_cmd_t)(const void *p_attr, uint16_t conn_hdl, ble_status_t result, const void *p_app_value);

/*************************************************************************************************************//**
 * @brief   Callback invoked when write response sent.
 * @param[in]   p_attr Information structure of characteristic or descriptor.
 * @param[in]   conn_hdl Connection handle.
 * @param[in]   result Result of write response.
 * @param[in]   p_app_value Characteristic value or descriptor value of write response.
***********************************************************************************************************************/
typedef void (*ble_servs_attr_write_comp_t)(const void *p_attr, uint16_t conn_hdl, ble_status_t result, const void *p_app_value);

/*************************************************************************************************************//**
 * @brief   Callback invoked when read request received.
 * @param[in]   p_attr Information structure of characteristic or descriptor.
 * @param[in]   conn_hdl Connection handle.
***********************************************************************************************************************/
typedef void (*ble_servs_attr_read_req_t)(const void *p_attr, uint16_t conn_hdl);

/*************************************************************************************************************//**
 * @brief   Callback invoked when confirmation received.
 * @param[in]   p_attr Information structure of characteristic.
 * @param[in]   conn_hdl Connection handle.
***********************************************************************************************************************/
typedef void (*ble_servs_attr_hdl_val_cnf_t)(const void *p_attr, uint16_t conn_hdl);

/*************************************************************************************************************//**
 * @brief   Callback invoked when flow control callback occurred.
 * @param[in]   p_attr Information structure of characteristic.
***********************************************************************************************************************/
typedef void (*ble_servs_attr_flow_ctrl_t)(const void *p_attr);

/*************************************************************************************************************//**
 * @brief   Callback invoked when events defined in service occurred.
 * @param[in]   type Event type of callback. Refer e_ble_XXX_event_t of each service you will use.
 * @param[in]   result Result of event.
 * @param[in]   p_data Event data.
***********************************************************************************************************************/
typedef void (*ble_servs_app_cb_t)(uint16_t type, ble_status_t result, st_ble_servs_evt_data_t *p_data);
/*@}*/

/** @defgroup profile_cmn_ser_func Functions
 *  @{
 *  @brief Function definition
 */
/*************************************************************************************************************//**
 * @brief   Decode function. Convert data from GATT database value to application data value.
 * @param[out]  p_app_value Application data value of characteristic or descriptor.
 * @param[in]   p_gatt_value GATT database value of characteristic or descriptor.
***********************************************************************************************************************/
typedef ble_status_t (*ble_servs_attr_decode_t)(void *p_app_value, const st_ble_gatt_value_t *p_gatt_value);

/*************************************************************************************************************//**
 * @brief   Encode function. Convert data from application data value to GATT database value.
 * @param[in]   p_app_value Application data value of characteristic or descriptor.
 * @param[out]  p_gatt_value GATT database value of characteristic or descriptor.
***********************************************************************************************************************/
typedef ble_status_t (*ble_servs_attr_encode_t)(const void *p_app_value, st_ble_gatt_value_t *p_gatt_value);
/*@}*/

/** @defgroup profile_cmn_ser_struct Structures
 *  @{
 *  @brief Structure definition
 */
/******************************************************************************************************************//**
 * @struct st_ble_servs_desc_info_t
 * @brief  st_ble_servs_desc_info_t includes information about descriptor.
 **********************************************************************************************************************/
typedef struct _st_ble_desc_info_t{
    uint16_t                    attr_hdl;       /**< Attribute handle of descriptor */
    uint8_t                     desc_idx;       /**< Index of descriptor */
    uint8_t                     inst_idx;       /**< Index used if same descriptor is set in one characteristic */
    uint16_t                    app_size;       /**< Descriptor value size used in application. */
    uint16_t                    db_size;        /**< Descriptor value size used in GATT database. */
    ble_servs_attr_write_req_t  write_req_cb;   /**< Write request callback function. Set function if needed. */
    ble_servs_attr_write_comp_t write_comp_cb;  /**< Write response callback function. Set function if needed. */
    ble_servs_attr_read_req_t   read_req_cb;    /**< Read request callback function. Set function if needed. */
    ble_servs_attr_decode_t     decode;         /**< Decode function. */
    ble_servs_attr_encode_t     encode;         /**< Encode function. */
} st_ble_servs_desc_info_t;

/******************************************************************************************************************//**
 * @struct st_ble_servs_char_info_t
 * @brief  st_ble_servs_char_info_t includes information about characteristic.
 **********************************************************************************************************************/
typedef struct _st_ble_char_info_t {
    uint16_t                         start_hdl;         /**< First Attribute handle of this characteristic */
    uint16_t                         end_hdl;           /**< Last Attribute handle of this characteristic */
    uint8_t                          char_idx;          /**< Index of characteristic */
    uint8_t                          inst_idx;          /**< Index used if same characteristic is set in one service */
    uint16_t                         app_size;          /**< Size of characteristic value in Application. */
    uint16_t                         db_size;           /**< Size of characteristic value in GATT database. */
    const st_ble_servs_desc_info_t **pp_descs;          /**< Set all descriptor information structure included in this characteristic. */
    uint8_t                          num_of_descs;      /**< Number of descriptors included in this characteristic. */
    ble_servs_attr_write_req_t       write_req_cb;      /**< Write request callback function. Set function if needed. */
    ble_servs_attr_write_cmd_t       write_cmd_cb;      /**< Write command callback function. Set function if needed. */
    ble_servs_attr_write_comp_t      write_comp_cb;     /**< Write response callback function. Set function if needed. */
    ble_servs_attr_read_req_t        read_req_cb;       /**< Read request callback function. Set function if needed. */
    ble_servs_attr_hdl_val_cnf_t     hdl_val_cnf_cb;    /**< Confirmation callback function. Set function if needed. */
    ble_servs_attr_flow_ctrl_t       flow_ctrl_cb;      /**< Flow control callback function. Set function if needed. */
    ble_servs_attr_decode_t          decode;            /**< Decode function. */
    ble_servs_attr_encode_t          encode;            /**< Encode function. */
} st_ble_servs_char_info_t;

/******************************************************************************************************************//**
 * @struct st_ble_servs_info_t
 * @brief  st_ble_servs_info_t includes information about service.
 **********************************************************************************************************************/
typedef struct {
    const st_ble_servs_char_info_t **pp_chars;      /**< Set all characteristic information structure included in this service. */
    uint8_t                          num_of_chars;  /**< Number of characteristics included in this service. */
    ble_servs_app_cb_t               cb;            /**< Service event callback function. */
} st_ble_servs_info_t;
/*@}*/
/**/
/** @defgroup profile_cmn_ser_func Functions
 *  @{
 *  @brief Function definition
 */
/******************************************************************************************************************//**
 * @brief   Initialize profile common server library. 
 * @return  See @ref ble_status_t 
 **********************************************************************************************************************/
ble_status_t R_BLE_SERVS_Init(void);

/******************************************************************************************************************//**
 * @brief   Register server service information to profile common server library.
 * @param[in]   p_serv Server service to be registered.
 * @return  See @ref ble_status_t
 **********************************************************************************************************************/
ble_status_t R_BLE_SERVS_RegisterServer(const st_ble_servs_info_t *p_serv);

/******************************************************************************************************************//**
 * @brief   Send notification or indication.
 * @param[in]   p_attr Characteristic sending notification or indication.
 * @param[in]   conn_hdl Connection handle.
 * @param[in]   p_app_value Value to be sent on notification or indication.
 * @param[in]   is_notify Defines notification or indication.\n true = notification\n false = indication
 * @return  See @ref ble_status_t
 **********************************************************************************************************************/
ble_status_t R_BLE_SERVS_SendHdlVal(const st_ble_servs_char_info_t *p_attr, uint16_t conn_hdl,  const void *p_app_value, bool is_notify);

/******************************************************************************************************************//**
 * @brief   Get characteristic value from GATT database.
 * @param[in]   p_attr Characteristic getting value.
 * @param[in]   conn_hdl Connection handle.
 * @param[out]  p_app_value Characteristic value getting from GATT database.
 * @return  See @ref ble_status_t
 **********************************************************************************************************************/
ble_status_t R_BLE_SERVS_GetChar(const st_ble_servs_char_info_t *p_attr, uint16_t conn_hdl, void *p_app_value);

/******************************************************************************************************************//**
 * @brief   Set characteristic value to GATT database.
 * @param[in]   p_attr Characteristic setting value.
 * @param[in]   conn_hdl Connection handle.
 * @param[in]   p_app_value Characteristic value setting to GATT database.
 * @return  See @ref ble_status_t
 **********************************************************************************************************************/
ble_status_t R_BLE_SERVS_SetChar(const st_ble_servs_char_info_t *p_attr, uint16_t conn_hdl, const void *p_app_value);

/******************************************************************************************************************//**
 * @brief   Get descriptor value from GATT database.
 * @param[in]   p_attr Descriptor getting value.
 * @param[in]   conn_hdl Connection handle.
 * @param[out]  p_app_value Descriptor value getting from GATT database.
 * @return  See @ref ble_status_t
 **********************************************************************************************************************/
ble_status_t R_BLE_SERVS_GetDesc(const st_ble_servs_desc_info_t *p_attr, uint16_t conn_hdl, void *p_app_value);

/******************************************************************************************************************//**
 * @brief   Set descriptor value to GATT database.
 * @param[in]   p_attr Descriptor setting value.
 * @param[in]   conn_hdl Connection handle.
 * @param[in]   p_app_value Descriptor value setting to GATT database.
 * @return  See @ref ble_status_t
 **********************************************************************************************************************/
ble_status_t R_BLE_SERVS_SetDesc(const st_ble_servs_desc_info_t *p_attr, uint16_t conn_hdl, const void *p_app_value);

/******************************************************************************************************************//**
 * @brief   Callback function used for GATT server event.
 * @param[in]   type Type of event.
 * @param[in]   result Result of request sent to server.
 * @param[in]   p_data Event data.
 * @details You need to call this function to enable all server callback event included in profile.
 **********************************************************************************************************************/
void R_BLE_SERVS_GattsCb(uint16_t type, ble_status_t result, st_ble_gatts_evt_data_t *p_data);

/******************************************************************************************************************//**
 * @brief   Callback function used for vender specific event.
 * @param[in]   type Type of event.
 * @param[in]   result Result of request sent to server.
 * @param[in]   p_data Event data.
 * @details You need to call this function if you use flow control callback.
 **********************************************************************************************************************/
void R_BLE_SERVS_VsCb(uint16_t type, ble_status_t result, st_ble_vs_evt_data_t * p_data);
/*@}*/

#endif /* R_BLE_SERVS_IF_H */
/* @} */

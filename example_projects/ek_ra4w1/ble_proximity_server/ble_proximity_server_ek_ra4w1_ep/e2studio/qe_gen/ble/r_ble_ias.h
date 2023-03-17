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
 * File Name: r_ble_ias.h
 * Description : This module implements Immediate Alert Service Server.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * @file
 * @defgroup ias Immediate Alert Service Server 
 * @{
 * @ingroup profile
 * @brief   This service exposes a control point to allow a peer device to cause the device to immediately alert. 
 **********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "profile_cmn/r_ble_serv_common.h"

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#ifndef R_BLE_IAS_H
#define R_BLE_IAS_H


/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief Immediate Alert Service event data.
***********************************************************************************************************************/
typedef struct
{
    uint16_t  conn_hdl;  /**< Connection handle */
    uint16_t  param_len; /**< Event parameter length */
    void     *p_param;   /**< Event parameter */
} st_ble_ias_evt_data_t;

/*******************************************************************************************************************//**
 * @brief Immediate Alert Service event callback.
***********************************************************************************************************************/
typedef void (*ble_ias_app_cb_t)(uint16_t type, ble_status_t result, st_ble_ias_evt_data_t *p_data);

/*******************************************************************************************************************//**
 * @brief Immediate Alert Service event type.
***********************************************************************************************************************/
typedef enum 
{
    BLE_IAS_EVENT_ALERT_LEVEL_WRITE_CMD, /**< Alert Level characteristic write command event */
} e_ble_ias_event_t;

/*******************************************************************************************************************//**
 * @brief Alert Level enumeration.
***********************************************************************************************************************/
typedef enum 
{
    BLE_IAS_ALERT_LEVEL_ALERT_LEVEL_NO_ALERT   = 0, /**< Alert level no alert shall be done on the device*/
    BLE_IAS_ALERT_LEVEL_ALERT_LEVEL_MILD_ALERT = 1, /**< Alert level mild alert the device shall alert*/
    BLE_IAS_ALERT_LEVEL_ALERT_LEVEL_HIGH_ALERT = 2, /**< Alert level high alert the device shall alert the strongest possible*/
} e_ble_ias_alert_level_t;

/*******************************************************************************************************************//**
 * @brief Immediate Alert Service initialization parameters.
***********************************************************************************************************************/
typedef struct 
{
    ble_ias_app_cb_t cb; /**< Immediate Alert Service event callback */
} st_ble_ias_init_param_t;

/*******************************************************************************************************************//**
 * @brief Immediate Alert Service connection parameters.
***********************************************************************************************************************/
typedef struct 
{
    uint8_t dummy;
} st_ble_ias_connect_param_t;

/*******************************************************************************************************************//**
 * @brief Immediate Alert Service disconnection parameters.
***********************************************************************************************************************/
typedef struct 
{
    uint8_t dummy;
} st_ble_ias_disconnect_param_t;


/***********************************************************************************************************************
Exported global functions (to be accessed by other files)
***********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief     Initialize Immediate Alert Service.
 * @details   This function shall be called once at startup.
 * @param[in] p_param    Pointer to Immediate Alert Service initialization parameters.
 * @return
***********************************************************************************************************************/
ble_status_t R_BLE_IAS_Init(const st_ble_ias_init_param_t *p_param);

/*******************************************************************************************************************//**
 * @brief     Perform Immediate Alert Service connection settings.
 * @details   This function shall be called on each connection establishment.
 * @param[in] conn_hdl Connection handle.
 * @param[in] p_param    Pointer to Connection parameters.
 * @return    @ref ble_status_t
***********************************************************************************************************************/
ble_status_t R_BLE_IAS_Connect(uint16_t conn_hdl, const st_ble_ias_connect_param_t *p_param);

/*******************************************************************************************************************//**
 * @brief     Retrieve Immediate Alert Service connection specific settings before disconnection.
 * @details   This function shall be called on each disconnection.
 * @param[in] conn_hdl Connection handle.
 * @param[in] p_param    Pointer to Disconnection parameters.
 * @return    @ref ble_status_t
***********************************************************************************************************************/
ble_status_t R_BLE_IAS_Disconnect(uint16_t conn_hdl, st_ble_ias_disconnect_param_t *p_param);

/*******************************************************************************************************************//**
 * @brief     Return version of the IAC service server.
 * @return    version
***********************************************************************************************************************/
uint32_t R_BLE_IAS_GetVersion(void);

#endif /* R_BLE_IAS_H */

/** @} */

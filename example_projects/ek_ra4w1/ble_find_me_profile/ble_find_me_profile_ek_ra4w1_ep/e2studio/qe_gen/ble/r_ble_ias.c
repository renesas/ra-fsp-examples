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
 * File Name: r_ble_ias.c
 * Description : This module implements Immediate Alert Service Server.
 **********************************************************************************************************************/
/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include <string.h>
#include "profile_cmn/r_ble_serv_common.h"
#include "r_ble_ias.h"
#include "gatt_db.h"
#include "common_utils.h"

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#ifndef UNUSED_ARG
#define UNUSED_ARG(arg)         (void)(arg)
#endif /* UNUSED_ARG */

/* Version number */
#define BLE_IAS_PRV_VERSION_MAJOR (1)
#define BLE_IAS_PRV_VERSION_MINOR (0)

/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/

static ble_ias_app_cb_t gs_ias_cb;

/***********************************************************************************************************************
 * Function Name: set_cli_cnfg
 * Description  : Set Characteristic value notification configuration in local GATT database.
 * Arguments    : conn_hdl - handle to connection.
 *                attr_hadl - handle to the attribute
 *                cli_cnfg - configuration value
 * Return Value : ble_status_t
 **********************************************************************************************************************/
#if 0 /* unused function */
static ble_status_t set_cli_cnfg(uint16_t conn_hdl, uint16_t attr_hdl, uint16_t cli_cnfg)
{
    uint8_t data[2];

    BT_PACK_LE_2_BYTE(data, &cli_cnfg);

    st_ble_gatt_value_t gatt_value = 
    {
        .p_value   = data,
        .value_len = 2,
    };

    return R_BLE_GATTS_SetAttr(conn_hdl, attr_hdl, &gatt_value);
}
#endif /* unused function */

/***********************************************************************************************************************
 * Function Name: get_cli_cnfg
 * Description  : Get Characteristic value notification configuration from local GATT database.
 * Arguments    : conn_hdl - handle to connection.
 *                attr_hadl - handle to the attribute
 *                p_cli_cnfg - pointer to variable to store configuration value
 * Return Value : ble_status_t
 **********************************************************************************************************************/
#if 0 /* unused function */
static ble_status_t get_cli_cnfg(uint16_t conn_hdl, uint16_t attr_hdl, uint16_t *p_cli_cnfg)
{
    ble_status_t ret;
    st_ble_gatt_value_t gatt_value;

    ret = R_BLE_GATTS_GetAttr(conn_hdl, attr_hdl, &gatt_value);

    if (BLE_SUCCESS == ret)
    {
        BT_UNPACK_LE_2_BYTE(p_cli_cnfg, gatt_value.p_value);
    }

    return ret;
}
#endif /* unused function */

/***********************************************************************************************************************
 * Function Name: evt_write_cmd_alert_level
 * Description  : This function handles the Alert Level characteristic write request event.
 * Arguments    : conn_hdl - connection handle
 *                p_gatt_value - pointer to the characteristic value in the GATT database
 * Return Value : none
 **********************************************************************************************************************/
static void evt_write_cmd_alert_level(uint16_t conn_hdl, st_ble_gatt_value_t *p_gatt_value)
{
    uint8_t app_value;
    BT_UNPACK_LE_1_BYTE(&app_value, p_gatt_value->p_value);

    st_ble_ias_evt_data_t evt_data = 
    {
        .conn_hdl  = conn_hdl,
        .param_len = sizeof(app_value),
        .p_param   = &app_value,
    };

    gs_ias_cb(BLE_IAS_EVENT_ALERT_LEVEL_WRITE_CMD, BLE_SUCCESS, &evt_data);
}

/***********************************************************************************************************************
 * Function Name: ias_gatt_db_cb
 * Description  : Callback function for Immediate Alert GATT Server events.
 * Arguments    : conn_hdl - handle to the connection
 *                p_params - pointer to GATTS db parameter
 * Return Value : none
 **********************************************************************************************************************/
static void ias_gatts_db_cb(uint16_t conn_hdl, st_ble_gatts_db_params_t *p_params) // @suppress("Function length")
{
    switch (p_params->db_op)
    {
        case BLE_GATTS_OP_CHAR_PEER_WRITE_CMD:
        {
            if (BLE_IAS_ALERT_LEVEL_VAL_HDL == p_params->attr_hdl)
            {
                evt_write_cmd_alert_level(conn_hdl, &p_params->value);
            }
        } 
        break;
        default:
        {
            /* Nothing to do here. */
        } 
        break;
    }
}

/***********************************************************************************************************************
 * Function Name: ia_gatts_cb
 * Description  : Callback function for the GATT Server events.
 * Arguments    : type - event id
 *                result - ble status
 *                p_data - pointer to the event data
 * Return Value : none
 **********************************************************************************************************************/
static void ias_gatts_cb(uint16_t type, ble_status_t result, st_ble_gatts_evt_data_t *p_data)
{
    UNUSED_ARG(result);

    switch (type)
    {
        case BLE_GATTS_EVENT_DB_ACCESS_IND:
        {
            st_ble_gatts_db_access_evt_t *p_db_access_evt_param =
                (st_ble_gatts_db_access_evt_t *)p_data->p_param;

            ias_gatts_db_cb(p_data->conn_hdl, p_db_access_evt_param->p_params);
        } break;

        default:
        {
            /* Nothing to do here. */
        } break;
    }
}

ble_status_t R_BLE_IAS_Init(const st_ble_ias_init_param_t *p_param) // @suppress("API function naming")
{
    if (NULL == p_param)
    {
        return BLE_ERR_INVALID_PTR;
    }

    if (NULL == p_param->cb)
    {
        return BLE_ERR_INVALID_ARG;
    }

    R_BLE_GATTS_RegisterCb(ias_gatts_cb, 2);

    gs_ias_cb = p_param->cb;

    return BLE_SUCCESS;
}

ble_status_t R_BLE_IAS_Connect(uint16_t conn_hdl, const st_ble_ias_connect_param_t *p_param) // @suppress("API function naming")
{
    if (BLE_GAP_INVALID_CONN_HDL == conn_hdl)
    {
        return BLE_ERR_INVALID_HDL;
    }

    if (NULL != p_param)
    {
        /* Nothing to do here.*/
    }

    return BLE_SUCCESS;
}

ble_status_t R_BLE_IAS_Disconnect(uint16_t conn_hdl, st_ble_ias_disconnect_param_t *p_param) // @suppress("API function naming")
{
    if (BLE_GAP_INVALID_CONN_HDL == conn_hdl)
    {
        return BLE_ERR_INVALID_HDL;
    }

    if (NULL != p_param)
    {
        /* Nothing to do here. */
    }

    return BLE_SUCCESS;
}

uint32_t R_BLE_IAS_GetVersion(void) // @suppress("API function naming")
{
    uint32_t version;

    version = ((BLE_IAS_PRV_VERSION_MAJOR << 16) | (BLE_IAS_PRV_VERSION_MINOR << 8));

    return version;
}


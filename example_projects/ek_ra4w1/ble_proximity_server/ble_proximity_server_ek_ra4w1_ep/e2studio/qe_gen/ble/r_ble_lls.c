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
/***********************************************************************************************************************
 * File Name: r_ble_lls.c
 * Description : The source file for Link Loss Service Server.
 **********************************************************************************************************************/

#include "r_ble_lls.h"
#include "profile_cmn/r_ble_servs_if.h"
#include "gatt_db.h"

static st_ble_servs_info_t gs_servs_info;

/* Start user code for function prototype declarations and global variables. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */



/*----------------------------------------------------------------------------------------------------------------------
    Alert Level characteristic : The level of an alert a device is to sound. If this level is changed while the alert is being sounded,the new level should take effect.
----------------------------------------------------------------------------------------------------------------------*/

/* Alert Level characteristic definition */
static const st_ble_servs_char_info_t gs_alert_level_char = {
    .start_hdl    = BLE_LLS_ALERT_LEVEL_DECL_HDL,
    .end_hdl      = BLE_LLS_ALERT_LEVEL_VAL_HDL,
    .char_idx     = BLE_LLS_ALERT_LEVEL_IDX,
    .app_size     = sizeof(uint8_t),
    .db_size      = BLE_LLS_ALERT_LEVEL_LEN,
    .decode       = (ble_servs_attr_decode_t)decode_uint8_t,
    .encode       = (ble_servs_attr_encode_t)encode_uint8_t,
};

ble_status_t R_BLE_LLS_SetAlertLevel(uint16_t conn_hdl, const uint8_t *p_value)
{
    return R_BLE_SERVS_SetChar(&gs_alert_level_char, conn_hdl, (const void *)p_value);
}

ble_status_t R_BLE_LLS_GetAlertLevel(uint16_t conn_hdl, uint8_t *p_value)
{
    return R_BLE_SERVS_GetChar(&gs_alert_level_char, conn_hdl, (void *)p_value);
}

/*----------------------------------------------------------------------------------------------------------------------
    Link Loss Service Server
----------------------------------------------------------------------------------------------------------------------*/

/* Link Loss Service characteristics definition */
static const st_ble_servs_char_info_t *gspp_chars[] = {
    &gs_alert_level_char,
};

/* Link Loss Service server definition */
static st_ble_servs_info_t gs_servs_info = {
    .pp_chars     = gspp_chars,
    .num_of_chars = ARRAY_SIZE(gspp_chars),
};

ble_status_t R_BLE_LLS_Init(ble_servs_app_cb_t cb)
{
    if (NULL == cb)
    {
        return BLE_ERR_INVALID_PTR;
    }

    gs_servs_info.cb = cb;

    return R_BLE_SERVS_RegisterServer(&gs_servs_info);
}





/* Start user code for function definitions. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
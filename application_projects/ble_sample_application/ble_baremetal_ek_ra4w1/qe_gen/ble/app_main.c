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

/******************************************************************************
* File Name    : app_main.c
* Device(s)    : RA4W1
* Tool-Chain   : e2Studio
* Description  : This is a application file for peripheral role.
*******************************************************************************/

/******************************************************************************
 Includes   <System Includes> , "Project Includes"
*******************************************************************************/
#include <string.h>
#include "r_ble_api.h"
#include "rm_ble_abs.h"
#include "rm_ble_abs_api.h"
#include "gatt_db.h"
#include "profile_cmn/r_ble_servs_if.h"
#include "profile_cmn/r_ble_servc_if.h"
#include "hal_data.h"

/* This code is needed for using FreeRTOS */
#if (BSP_CFG_RTOS == 2)
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
#define BLE_EVENT_PATTERN   (0x0A0A)
EventGroupHandle_t  g_ble_event_group_handle;
#endif
#include "r_ble_lss.h"

/******************************************************************************
 User file includes
*******************************************************************************/
/* Start user code for file includes. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

#define BLE_LOG_TAG "app_main"
#define BLE_GATTS_QUEUE_ELEMENTS_SIZE       (14)
#define BLE_GATTS_QUEUE_BUFFER_LEN          (245)
#define BLE_GATTS_QUEUE_NUM                 (1)

/******************************************************************************
 User macro definitions
*******************************************************************************/
/* Start user code for macro definitions. Do not edit comment generated here */
#define BLE_CFG_BOARD_TYPE 1

#define BLE_BOARD_SW1_PIN   BSP_IO_PORT_04_PIN_02
#undef  BLE_BOARD_SW2_PIN
#define BLE_BOARD_SW1_IRQ   ELC_EVENT_ICU_IRQ4
#undef  BLE_BOARD_SW2_IRQ
#define BLE_BOARD_LED1_PIN  BSP_IO_PORT_01_PIN_06
#define BLE_BOARD_LED2_PIN  BSP_IO_PORT_04_PIN_04
/* End user code. Do not edit comment generated here */

/******************************************************************************
 Generated function prototype declarations
*******************************************************************************/
/* Internal functions */
void gap_cb(uint16_t type, ble_status_t result, st_ble_evt_data_t *p_data);
void gatts_cb(uint16_t type, ble_status_t result, st_ble_gatts_evt_data_t *p_data);
void gattc_cb(uint16_t type, ble_status_t result, st_ble_gattc_evt_data_t *p_data);
void vs_cb(uint16_t type, ble_status_t result, st_ble_vs_evt_data_t *p_data);
ble_status_t ble_init(void);
void app_main(void);

/******************************************************************************
 User function prototype declarations
*******************************************************************************/
/* Start user code for function prototype declarations. Do not edit comment generated here */
typedef enum
{
    BLE_BOARD_LED1, /**< LED1 */
    BLE_BOARD_LED2, /**< LED2 */

    BLE_BOARD_LED_MAX
} e_ble_led_t;

/*******************************************************************************************************************//**
 * @brief Switch number.
***********************************************************************************************************************/
typedef enum
{
    BLE_BOARD_SW1, /**< Switch1 */
    BLE_BOARD_SW2, /**< Switch2 */

    BLE_BOARD_SW_MAX
} e_ble_sw_t;


typedef void (*ble_sw_cb_t)(void);
static void timer_update();
static void sw_cb(void);
static void R_BLE_BOARD_Init(void);
static void R_BLE_BOARD_ToggleLEDState(e_ble_led_t led);
static void R_BLE_BOARD_SetLEDState(e_ble_led_t led, bool onoff);
static void R_BLE_BOARD_RegisterSwitchCb(e_ble_sw_t sw, ble_sw_cb_t cb);
static ble_sw_cb_t gs_sw_cb[BLE_BOARD_SW_MAX];
/* End user code. Do not edit comment generated here */

/******************************************************************************
 Generated global variables
*******************************************************************************/
/* Advertising Data */
static uint8_t gs_advertising_data[] =
{
    /* Flags */
    0x02, /**< Data Size */
    0x01, /**< Data Type */
    ( 0x1a ), /**< Data Value */

    /* Shortened Local Name */
    0x05, /**< Data Size */
    0x08, /**< Data Type */
    0x52, 0x42, 0x4c, 0x45, /**< Data Value */
};

/* Scan Response Data */
static uint8_t gs_scan_response_data[] =
{
    /* Complete Local Name */
    0x0A, /**< Data Size */
    0x09, /**< Data Type */
    0x54, 0x45, 0x53, 0x54, 0x5f, 0x52, 0x42, 0x4c, 0x45, /**< Data Value */
};

ble_abs_legacy_advertising_parameter_t g_ble_advertising_parameter =
{
 .p_peer_address             = NULL,       ///< Peer address.
 .slow_advertising_interval  = 0x00000060, ///< Slow advertising interval. 60.0(ms)
 .slow_advertising_period    = 0x0000,     ///< Slow advertising period.
 .p_advertising_data         = gs_advertising_data,             ///< Advertising data. If p_advertising_data is specified as NULL, advertising data is not set.
 .advertising_data_length    = ARRAY_SIZE(gs_advertising_data), ///< Advertising data length (in bytes).
 .p_scan_response_data       = gs_scan_response_data,             ///< Scan response data. If p_scan_response_data is specified as NULL, scan response data is not set.
 .scan_response_data_length  = ARRAY_SIZE(gs_scan_response_data), ///< Scan response data length (in bytes).
 .advertising_filter_policy  = BLE_ABS_ADVERTISING_FILTER_ALLOW_ANY, ///< Advertising Filter Policy.
 .advertising_channel_map    = ( BLE_GAP_ADV_CH_37 | BLE_GAP_ADV_CH_38 | BLE_GAP_ADV_CH_39 ), ///< Channel Map.
 .own_bluetooth_address_type = BLE_GAP_ADDR_RAND, ///< Own Bluetooth address type.
 .own_bluetooth_address      = { 0 },
};

/* GATT server callback parameters */
ble_abs_gatt_server_callback_set_t gs_abs_gatts_cb_param[] =
{
    {
        .gatt_server_callback_function = gatts_cb,
        .gatt_server_callback_priority = 1,
    },
    {
        .gatt_server_callback_function = NULL,
    }
};

/* GATT client callback parameters */
ble_abs_gatt_client_callback_set_t gs_abs_gattc_cb_param[] =
{
    {
        .gatt_client_callback_function = gattc_cb,
        .gatt_client_callback_priority = 1,
    },
    {
        .gatt_client_callback_function = NULL,
    }
};

/* GATT server Prepare Write Queue parameters */
static st_ble_gatt_queue_elm_t  gs_queue_elms[BLE_GATTS_QUEUE_ELEMENTS_SIZE];
static uint8_t gs_buffer[BLE_GATTS_QUEUE_BUFFER_LEN];
static st_ble_gatt_pre_queue_t gs_queue[BLE_GATTS_QUEUE_NUM] = {
    {
        .p_buf_start = gs_buffer,
        .buffer_len  = BLE_GATTS_QUEUE_BUFFER_LEN,
        .p_queue     = gs_queue_elms,
        .queue_size  = BLE_GATTS_QUEUE_ELEMENTS_SIZE,
    }
};

/* Connection handle */
uint16_t g_conn_hdl;

/******************************************************************************
 User global variables
*******************************************************************************/
/* Start user code for global variables. Do not edit comment generated here */
static bool g_interval_update_flag = true;
static uint8_t g_current_blinky_interval = 0x88;
/* End user code. Do not edit comment generated here */

/******************************************************************************
 Generated function definitions
*******************************************************************************/
/******************************************************************************
 * Function Name: gap_cb
 * Description  : Callback function for GAP API.
 * Arguments    : uint16_t type -
 *                  Event type of GAP API.
 *              : ble_status_t result -
 *                  Event result of GAP API.
 *              : st_ble_vs_evt_data_t *p_data - 
 *                  Event parameters of GAP API.
 * Return Value : none
 ******************************************************************************/
void gap_cb(uint16_t type, ble_status_t result, st_ble_evt_data_t *p_data)
{
/* Hint: Input common process of callback function such as variable definitions */
/* Start user code for GAP callback function common process. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

    switch(type)
    {
        case BLE_GAP_EVENT_STACK_ON:
        {
            /* Get BD address for Advertising */
            R_BLE_VS_GetBdAddr(BLE_VS_ADDR_AREA_REG, BLE_GAP_ADDR_RAND);
        } break;

        case BLE_GAP_EVENT_CONN_IND:
        {
            if (BLE_SUCCESS == result)
            {
                /* Store connection handle */
                st_ble_gap_conn_evt_t *p_gap_conn_evt_param = (st_ble_gap_conn_evt_t *)p_data->p_param;
                g_conn_hdl = p_gap_conn_evt_param->conn_hdl;
            }
            else
            {
                /* Restart advertising when connection failed */
                RM_BLE_ABS_StartLegacyAdvertising(&g_ble_abs0_ctrl, &g_ble_advertising_parameter);
            }
        } break;

        case BLE_GAP_EVENT_DISCONN_IND:
        {
            /* Restart advertising when disconnected */
            g_conn_hdl = BLE_GAP_INVALID_CONN_HDL;
            RM_BLE_ABS_StartLegacyAdvertising(&g_ble_abs0_ctrl, &g_ble_advertising_parameter);
        } break;

        case BLE_GAP_EVENT_CONN_PARAM_UPD_REQ:
        {
            /* Send connection update response with value received on connection update request */
            st_ble_gap_conn_upd_req_evt_t *p_conn_upd_req_evt_param = (st_ble_gap_conn_upd_req_evt_t *)p_data->p_param;

            st_ble_gap_conn_param_t conn_updt_param = {
                .conn_intv_min = p_conn_upd_req_evt_param->conn_intv_min,
                .conn_intv_max = p_conn_upd_req_evt_param->conn_intv_max,
                .conn_latency  = p_conn_upd_req_evt_param->conn_latency,
                .sup_to        = p_conn_upd_req_evt_param->sup_to,
            };

            R_BLE_GAP_UpdConn(p_conn_upd_req_evt_param->conn_hdl,
                              BLE_GAP_CONN_UPD_MODE_RSP,
                              BLE_GAP_CONN_UPD_ACCEPT,
                              &conn_updt_param);
        } break;

/* Hint: Add cases of GAP event macros defined as BLE_GAP_XXX */
/* Start user code for GAP callback function event process. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
    }
}

/******************************************************************************
 * Function Name: gatts_cb
 * Description  : Callback function for GATT Server API.
 * Arguments    : uint16_t type -
 *                  Event type of GATT Server API.
 *              : ble_status_t result -
 *                  Event result of GATT Server API.
 *              : st_ble_gatts_evt_data_t *p_data - 
 *                  Event parameters of GATT Server API.
 * Return Value : none
 ******************************************************************************/
void gatts_cb(uint16_t type, ble_status_t result, st_ble_gatts_evt_data_t *p_data)
{
/* Hint: Input common process of callback function such as variable definitions */
/* Start user code for GATT Server callback function common process. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

    R_BLE_SERVS_GattsCb(type, result, p_data);
    switch(type)
    {
/* Hint: Add cases of GATT Server event macros defined as BLE_GATTS_XXX */
/* Start user code for GATT Server callback function event process. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
    }
}

/******************************************************************************
 * Function Name: gattc_cb
 * Description  : Callback function for GATT Client API.
 * Arguments    : uint16_t type -
 *                  Event type of GATT Client API.
 *              : ble_status_t result -
 *                  Event result of GATT Client API.
 *              : st_ble_gattc_evt_data_t *p_data - 
 *                  Event parameters of GATT Client API.
 * Return Value : none
 ******************************************************************************/
void gattc_cb(uint16_t type, ble_status_t result, st_ble_gattc_evt_data_t *p_data)
{
/* Hint: Input common process of callback function such as variable definitions */
/* Start user code for GATT Client callback function common process. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

    R_BLE_SERVC_GattcCb(type, result, p_data);
    switch(type)
    {

/* Hint: Add cases of GATT Client event macros defined as BLE_GATTC_XXX */
/* Start user code for GATT Client callback function event process. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
    }
}

/******************************************************************************
 * Function Name: vs_cb
 * Description  : Callback function for Vendor Specific API.
 * Arguments    : uint16_t type -
 *                  Event type of Vendor Specific API.
 *              : ble_status_t result -
 *                  Event result of Vendor Specific API.
 *              : st_ble_vs_evt_data_t *p_data - 
 *                  Event parameters of Vendor Specific API.
 * Return Value : none
 ******************************************************************************/
void vs_cb(uint16_t type, ble_status_t result, st_ble_vs_evt_data_t *p_data)
{
/* Hint: Input common process of callback function such as variable definitions */
/* Start user code for vender specific callback function common process. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
    
    R_BLE_SERVS_VsCb(type, result, p_data);
    switch(type)
    {
        case BLE_VS_EVENT_GET_ADDR_COMP:
        {
            /* Start advertising when BD address is ready */
            st_ble_vs_get_bd_addr_comp_evt_t * get_address = (st_ble_vs_get_bd_addr_comp_evt_t *)p_data->p_param;
            memcpy(g_ble_advertising_parameter.own_bluetooth_address, get_address->addr.addr, BLE_BD_ADDR_LEN);
            RM_BLE_ABS_StartLegacyAdvertising(&g_ble_abs0_ctrl, &g_ble_advertising_parameter);
        } break;

/* Hint: Add cases of vender specific event macros defined as BLE_VS_XXX */
/* Start user code for vender specific callback function event process. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
    }
}

/******************************************************************************
 * Function Name: lss_cb
 * Description  : Callback function for LED Switch Service server feature.
 * Arguments    : uint16_t type -
 *                  Event type of LED Switch Service server feature.
 *              : ble_status_t result -
 *                  Event result of LED Switch Service server feature.
 *              : st_ble_servs_evt_data_t *p_data - 
 *                  Event parameters of LED Switch Service server feature.
 * Return Value : none
 ******************************************************************************/
static void lss_cb(uint16_t type, ble_status_t result, st_ble_servs_evt_data_t *p_data)
{
/* Hint: Input common process of callback function such as variable definitions */
/* Start user code for LED Switch Service Server callback function common process. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

    switch(type)
    {
/* Hint: Add cases of LED Switch Service server events defined in e_ble_lss_event_t */
/* Start user code for LED Switch Service Server callback function event process. Do not edit comment generated here */
    	case BLE_LSS_EVENT_BLINK_RATE_WRITE_REQ:
        {
            g_current_blinky_interval = *(uint8_t *)p_data->p_param;

            if (g_current_blinky_interval == 0x00)
            {
                R_BLE_BOARD_SetLEDState(BLE_BOARD_LED1, true);  //  LED OFF
            }
            else if (g_current_blinky_interval == 0xFF)
            {
                R_BLE_BOARD_SetLEDState(BLE_BOARD_LED1, false);  //  LED ON
            }
            g_interval_update_flag = true;
        } break;

        default:
            break;
/* End user code. Do not edit comment generated here */
    }    
}
/******************************************************************************
 * Function Name: ble_init
 * Description  : Initialize BLE and profiles.
 * Arguments    : none
 * Return Value : BLE_SUCCESS - SUCCESS
 *                BLE_ERR_INVALID_OPERATION -
 *                    Failed to initialize BLE or profiles.
 ******************************************************************************/
ble_status_t ble_init(void)
{
    ble_status_t status;
    fsp_err_t err;

    /* Initialize BLE */
    err = RM_BLE_ABS_Open(&g_ble_abs0_ctrl, &g_ble_abs0_cfg);
    if (FSP_SUCCESS != err)
    {
    	return err;
    }

    /* Initialize GATT Database */
    status = R_BLE_GATTS_SetDbInst(&g_gatt_db_table);
    if (BLE_SUCCESS != status)
    {
        return BLE_ERR_INVALID_OPERATION;
    }

    /* Initialize GATT server */
    status = R_BLE_SERVS_Init();
    if (BLE_SUCCESS != status)
    {
        return BLE_ERR_INVALID_OPERATION;
    }

    /*Initialize GATT client */
    status = R_BLE_SERVC_Init();
    if (BLE_SUCCESS != status)
    {
        return BLE_ERR_INVALID_OPERATION;
    }
    
    /* Set Prepare Write Queue */
    R_BLE_GATTS_SetPrepareQueue(gs_queue, BLE_GATTS_QUEUE_NUM);

    /* Initialize LED Switch Service server API */
    status = R_BLE_LSS_Init(lss_cb);
    if (BLE_SUCCESS != status)
    {
        return BLE_ERR_INVALID_OPERATION;
    }

    return status;
}

/******************************************************************************
 * Function Name: app_main
 * Description  : Application main function with main loop
 * Arguments    : none
 * Return Value : none
 ******************************************************************************/
void app_main(void)
{
#if (BSP_CFG_RTOS == 2)
    /* Create Event Group */
    g_ble_event_group_handle = xEventGroupCreate();
    assert(g_ble_event_group_handle);
#endif

    /* Initialize BLE and profiles */
    ble_init();

/* Hint: Input process that should be done before main loop such as calling initial function or variable definitions */
/* Start user code for process before main loop. Do not edit comment generated here */
   R_BLE_BOARD_Init();
   R_BLE_BOARD_RegisterSwitchCb(BLE_BOARD_SW1, sw_cb);
/* End user code. Do not edit comment generated here */

    /* main loop */
    while (1)
    {
        /* Process BLE Event */
        R_BLE_Execute();

/* When this BLE application works on the FreeRTOS */
#if (BSP_CFG_RTOS == 2)
        if(0 != R_BLE_IsTaskFree())
        {
            /* If the BLE Task has no operation to be processed, it transits block state until the event from RF transciever occurs. */
            xEventGroupWaitBits(g_ble_event_group_handle,
                                (EventBits_t)BLE_EVENT_PATTERN,
                                pdTRUE,
                                pdFALSE,
                                portMAX_DELAY);
        }
#endif

/* Hint: Input process that should be done during main loop such as calling processing functions */
/* Start user code for process during main loop. Do not edit comment generated here */
	    timer_update();
/* End user code. Do not edit comment generated here */
    }

/* Hint: Input process that should be done after main loop such as calling closing functions */
/* Start user code for process after main loop. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

    /* Terminate BLE */
    RM_BLE_ABS_Close(&g_ble_abs0_ctrl);
}

/******************************************************************************
 User function definitions
*******************************************************************************/
/* Start user code for function definitions. Do not edit comment generated here */
/******************************************************************************
 * Function Name: timer_cb
 * Description  : Callback function for timer for LED blink.
 * Arguments    : uint32_t timer_hdl -
 *                  Timer handle identifying a expired timer.
 * Return Value : none
 ******************************************************************************/
static void timer_update()
{
    static uint32_t remain_time;

    if(true == g_interval_update_flag){
        remain_time = 1 + (uint32_t) g_current_blinky_interval * 50000 / 0xFF;
        g_interval_update_flag = false;
    }else if ((0 == remain_time)&&(0 != g_current_blinky_interval)&&(0xFF != g_current_blinky_interval))
    {
        R_BLE_BOARD_ToggleLEDState (BLE_BOARD_LED1);
        remain_time = 1 + (uint32_t) g_current_blinky_interval * 50000 / 0xFF;
    }

    remain_time--;

}

static void sw_cb(void)
{
    uint8_t state = 1;

#if (BLE_CFG_BOARD_TYPE == 1)
#endif
    R_BLE_LSS_NotifySwitchState(g_conn_hdl, &state);
}

static void R_BLE_BOARD_SetLEDState(e_ble_led_t led, bool onoff)
{
    FSP_PARAMETER_NOT_USED (onoff);

    if (led == BLE_BOARD_LED1)
    {
#ifdef BLE_BOARD_LED1_PIN
        g_ioport.p_api->open(g_ioport.p_ctrl, g_ioport.p_cfg);
#if (BLE_CFG_BOARD_TYPE == 1)
        g_ioport.p_api->pinWrite(g_ioport.p_ctrl, BLE_BOARD_LED1_PIN, (onoff)   ?   BSP_IO_LEVEL_HIGH : BSP_IO_LEVEL_LOW);
#else
        g_ioport.p_api->pinWrite(g_ioport.p_ctrl, BLE_BOARD_LED1_PIN, (onoff)   ?   BSP_IO_LEVEL_LOW : BSP_IO_LEVEL_HIGH);
#endif
        g_ioport.p_api->close(g_ioport.p_ctrl);
#endif /* BLE_BOARD_LED1_PIN */
    }
    else if (led == BLE_BOARD_LED2)
    {
#ifdef BLE_BOARD_LED2_PIN
        g_ioport.p_api->open(g_ioport.p_ctrl, g_ioport.p_cfg);
#if (BLE_CFG_BOARD_TYPE == 1)
        g_ioport.p_api->pinWrite(g_ioport.p_ctrl, BLE_BOARD_LED2_PIN, (onoff)   ?   BSP_IO_LEVEL_HIGH : BSP_IO_LEVEL_LOW);
#else
        g_ioport.p_api->pinWrite(g_ioport.p_ctrl, BLE_BOARD_LED2_PIN, (onoff)   ?   BSP_IO_LEVEL_LOW : BSP_IO_LEVEL_HIGH);
#endif
        g_ioport.p_api->close(g_ioport.p_ctrl);
#endif /* BLE_BOARD_LED1_PIN */
    }
}

static void R_BLE_BOARD_ToggleLEDState(e_ble_led_t led)
{
    bsp_io_level_t    level;

    if (led == BLE_BOARD_LED1)
    {
#ifdef BLE_BOARD_LED1_PIN
        g_ioport.p_api->open(g_ioport.p_ctrl, g_ioport.p_cfg);
        g_ioport.p_api->pinRead(g_ioport.p_ctrl, BLE_BOARD_LED1_PIN, &level);
        g_ioport.p_api->pinWrite(g_ioport.p_ctrl, BLE_BOARD_LED1_PIN, (level == BSP_IO_LEVEL_HIGH)   ?   BSP_IO_LEVEL_LOW : BSP_IO_LEVEL_HIGH);
        g_ioport.p_api->close(g_ioport.p_ctrl);
#endif /* BLE_BOARD_LED1_PIN */
    }
    else if (led == BLE_BOARD_LED2)
    {
#ifdef BLE_BOARD_LED2_PIN
        g_ioport.p_api->open(g_ioport.p_ctrl, g_ioport.p_cfg);
        g_ioport.p_api->pinRead(g_ioport.p_ctrl, BLE_BOARD_LED2_PIN, &level);
        g_ioport.p_api->pinWrite(g_ioport.p_ctrl, BLE_BOARD_LED2_PIN, (level == BSP_IO_LEVEL_HIGH)   ?   BSP_IO_LEVEL_LOW : BSP_IO_LEVEL_HIGH);
        g_ioport.p_api->close(g_ioport.p_ctrl);
#endif /* BLE_BOARD_LED1_PIN */
    }
}

static void R_BLE_BOARD_RegisterSwitchCb(e_ble_sw_t sw, ble_sw_cb_t cb)
{
    FSP_PARAMETER_NOT_USED (cb);

    if (sw == BLE_BOARD_SW1)
    {
#ifdef BLE_BOARD_SW1_IRQ
        gs_sw_cb[0] = cb;
        g_ble_sw_irq.p_api->open(g_ble_sw_irq.p_ctrl, g_ble_sw_irq.p_cfg);
        g_ble_sw_irq.p_api->enable(g_ble_sw_irq.p_ctrl);
#endif
    }
    else if (sw == BLE_BOARD_SW2)
    {
#ifdef BLE_BOARD_SW2_IRQ
        R_BSP_GroupIrqWrite(BSP_GRP_IRQ_NMI_PIN, cb);
        R_ICU->NMIER_b.NMIEN = 1;
#endif
    }
}

static void irq_pin_set(void)
{
    g_ioport.p_api->open(g_ioport.p_ctrl, g_ioport.p_cfg);

#if (BLE_CFG_BOARD_TYPE == 0)           /* for customer board */

#elif (BLE_CFG_BOARD_TYPE == 1)         /* Promotion board */
    g_ioport.p_api->pinCfg(g_ioport.p_ctrl, BLE_BOARD_SW1_PIN,
                           IOPORT_CFG_PORT_DIRECTION_INPUT|IOPORT_CFG_IRQ_ENABLE|IOPORT_CFG_EVENT_FALLING_EDGE|IOPORT_CFG_PULLUP_ENABLE);

#elif (BLE_CFG_BOARD_TYPE == 2)         /* Reserve */

#elif (BLE_CFG_BOARD_TYPE == 3)         /* RF Evaluation board */
    g_ioport.p_api->pinCfg(g_ioport.p_ctrl, BLE_BOARD_SW1_PIN,
                           IOPORT_CFG_PORT_DIRECTION_INPUT|IOPORT_CFG_IRQ_ENABLE|IOPORT_CFG_EVENT_FALLING_EDGE|IOPORT_CFG_PULLUP_ENABLE);
    g_ioport.p_api->pinCfg(g_ioport.p_ctrl, BLE_BOARD_SW2_PIN,
                           IOPORT_CFG_PORT_DIRECTION_INPUT|IOPORT_CFG_IRQ_ENABLE|IOPORT_CFG_EVENT_FALLING_EDGE|IOPORT_CFG_PULLUP_ENABLE);

#endif                                  /* BLE_CFG_BOARD_TYPE == x */

    g_ioport.p_api->close(g_ioport.p_ctrl);
}

static void R_BLE_BOARD_Init(void)
{
    g_ioport.p_api->open(g_ioport.p_ctrl, g_ioport.p_cfg);

#ifdef BLE_BOARD_LED1_PIN
    g_ioport.p_api->pinCfg(g_ioport.p_ctrl, BLE_BOARD_LED1_PIN,
                           IOPORT_CFG_PORT_DIRECTION_OUTPUT|IOPORT_CFG_PORT_OUTPUT_HIGH);
#endif
#if BLE_BOARD_LED2_PIN
    g_ioport.p_api->pinCfg(g_ioport.p_ctrl, BLE_BOARD_LED2_PIN,
                           IOPORT_CFG_PORT_DIRECTION_OUTPUT|IOPORT_CFG_PORT_OUTPUT_LOW);
#endif

    g_ioport.p_api->close(g_ioport.p_ctrl);

    irq_pin_set();

    /* Set wake up trigger */
#if (BLE_CFG_BOARD_TYPE == 1)   /* Promotion board */
#elif (BLE_CFG_BOARD_TYPE == 2) /* RSSK */
#elif (BLE_CFG_BOARD_TYPE == 3) /* Evaluation boars */
    R_BLE_BOARD_RegisterSwitchCb(BLE_BOARD_SW2, ble_sw_cb);
#endif
}

void Callback_ble_sw_irq(external_irq_callback_args_t *p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);
    gs_sw_cb[0]();
}
/* End user code. Do not edit comment generated here */
/**********************************************************************************************************************
* Copyright (c) 2019 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*********************************************************************************************************************/

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
#include "r_ble_cli.h"
#include "r_ble_cmd_abs.h"
#include "r_ble_cmd_vs.h"
#include "r_ble_cmd_sys.h"
/* End user code. Do not edit comment generated here */

#define BLE_LOG_TAG "app_main"
#define BLE_GATTS_QUEUE_ELEMENTS_SIZE       (14)
#define BLE_GATTS_QUEUE_BUFFER_LEN          (245)
#define BLE_GATTS_QUEUE_NUM                 (1)

/******************************************************************************
 User macro definitions
*******************************************************************************/
/* Start user code for macro definitions. Do not edit comment generated here */
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
static void timer_update(void);
static void sw_cb(void);
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
    ( 0x06 ), /**< Data Value */

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
 .slow_advertising_interval  = 0x000000A0, ///< Slow advertising interval. 1,000.0(ms)
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
static bool g_led_blink_active = false;

st_ble_gap_conn_param_t g_conn_updt_param =
{
 .conn_intv_min = 0x0050,
 .conn_intv_max = 0x0050,
 .conn_latency = 0x0000,
 .sup_to = 0x0C80,
 .min_ce_length = 0x0000,
 .max_ce_length = 0x0000,
};

/* CommandLine parameters */
static const st_ble_cli_cmd_t * const gsp_cmds[] =
{
     &g_abs_cmd,
     &g_vs_cmd,
     &g_sys_cmd,
     &g_ble_cmd
};

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
    R_BLE_CMD_AbsGapCb(type, result, p_data);
/* End user code. Do not edit comment generated here */

    switch(type)
    {
        case BLE_GAP_EVENT_STACK_ON:
        {
            R_BLE_CLI_Printf("gap : BLE_GAP_EVENT_STACK_ON \n");

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

                /* Send Connection update to ensure RF Low Power */
                R_BLE_GAP_UpdConn(p_gap_conn_evt_param->conn_hdl,
                                    BLE_GAP_CONN_UPD_MODE_REQ,
                                    BLE_GAP_CONN_UPD_ACCEPT,
                                    &g_conn_updt_param);
            }
            else
            {
                /* Restart advertising when connection failed */
                RM_BLE_ABS_StartLegacyAdvertising(&g_ble_abs0_ctrl, &g_ble_advertising_parameter);
            }
        } break;

        case BLE_GAP_EVENT_DISCONN_IND:
        {
            /* LED OFF */
            g_ioport.p_api->pinWrite(g_ioport.p_ctrl, BSP_IO_PORT_04_PIN_04, BSP_IO_LEVEL_HIGH);
            g_led_blink_active = false;

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
    R_BLE_CMD_VsCb(type, result, p_data);
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
    FSP_PARAMETER_NOT_USED(result);
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
                /* LED OFF */
                g_ioport.p_api->pinWrite(g_ioport.p_ctrl, BSP_IO_PORT_01_PIN_06, BSP_IO_LEVEL_HIGH);
                g_led_blink_active = false;
            }
            else if (g_current_blinky_interval == 0xFF)
            {
                /* LED ON */
                g_ioport.p_api->pinWrite(g_ioport.p_ctrl, BSP_IO_PORT_01_PIN_06, BSP_IO_LEVEL_LOW);
                g_led_blink_active = false;
            }
            else
            {
                g_led_blink_active = true;
                g_interval_update_flag = true;
            }
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

    /* Initialize Low Power Module */
    g_lpm0.p_api->open(g_lpm0.p_ctrl, g_lpm0.p_cfg);

    /* Initialize BLE and profiles */
    ble_init();

/* Hint: Input process that should be done before main loop such as calling initial function or variable definitions */
/* Start user code for process before main loop. Do not edit comment generated here */

    /* Open IO port */
    g_ioport.p_api->open(g_ioport.p_ctrl, g_ioport.p_cfg);

    /* Enable Interrupt (push switch) */
    g_ble_sw_irq.p_api->open(g_ble_sw_irq.p_ctrl, g_ble_sw_irq.p_cfg);
    g_ble_sw_irq.p_api->enable(g_ble_sw_irq.p_ctrl);

    /* Configure CommandLine */
    R_BLE_CLI_Init();
    R_BLE_CLI_RegisterCmds(gsp_cmds, sizeof(gsp_cmds)/sizeof(gsp_cmds[0]));
    R_BLE_CLI_RegisterEventCb(NULL);
    R_BLE_CMD_SetResetCb((ble_event_cb_t)ble_init);
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

        /* Process Command Line */
        R_BLE_CLI_Process();

#if (BLE_CFG_CMD_LINE_EN == 1)

        /* Disable IRQ */
        __disable_irq();

        /* UART reception on-going ? */
        if (false != get_uart_reception())
        {
            set_uart_reception(false);
            __enable_irq();
        }
        else
        {
            /* UART transmission on-going ? Allow enter software standby by sys stby command ? */
            if (true != g_inhibit_software_standby && true != get_uart_transmission() && true != g_led_blink_active)
            {
                /* Check whether there are executable BLE task or not */
                if (0 != R_BLE_IsTaskFree())
                {
                    /* There are no executable BLE task */
                    /* Terminate Command line */
                    R_BLE_CLI_Terminate();

                    /* Enter low power mode */
                    g_lpm0.p_api->lowPowerModeEnter(g_lpm0.p_ctrl);

                    /* Enable interrupt for processing interrupt handler after wake up */
                    __enable_irq();

                    /* Resume Command line */
                    R_BLE_CLI_Init();

                }
                else
                {
                    /* There is BLE related task */
                    __enable_irq();
                }
            }
            else
                __enable_irq();
        }
#else   /* BLE_CFG_CMD_LINE_EN == 1 */

        /* Disable IRQ */
        __disable_irq();

        /* Check whether there are executable BLE task or not */
        if (0 != R_BLE_IsTaskFree() && true != g_led_blink_active)
        {
            /* There are no executable BLE task */
            /* Enter low power mode */
            g_lpm0.p_api->lowPowerModeEnter(g_lpm0.p_ctrl);

            /* Enable interrupt for processing interrupt handler after wake up */
            __enable_irq();
        }
        else
        {
            /* There is BLE related task */
            __enable_irq();
        }
#endif  /* BLE_CFG_CMD_LINE_EN == 1 */


        if (true == g_led_blink_active)
            timer_update();
/* End user code. Do not edit comment generated here */
    }

/* Hint: Input process that should be done after main loop such as calling closing functions */
/* Start user code for process after main loop. Do not edit comment generated here */

    /* Close Low Power Module */
    g_lpm0.p_api->close(g_lpm0.p_ctrl);

    /* Disable Interrupt (push switch) */
    g_ble_sw_irq.p_api->disable(g_ble_sw_irq.p_ctrl);
    g_ble_sw_irq.p_api->close(g_ble_sw_irq.p_ctrl);

    /* Close IO port */
    g_ioport.p_api->close(g_ioport.p_ctrl);

/* End user code. Do not edit comment generated here */

    /* Terminate BLE */
    RM_BLE_ABS_Close(&g_ble_abs0_ctrl);
}

/******************************************************************************
 User function definitions
*******************************************************************************/
/* Start user code for function definitions. Do not edit comment generated here */
/******************************************************************************
 * Function Name: timer_update
 * Description  : Decrement counter for measuring LED blink interval.
 * Arguments    : none
 * Return Value : none
 ******************************************************************************/
static void timer_update(void)
{
    static uint32_t remain_time;
    bsp_io_level_t  level;

    if(true == g_interval_update_flag){
        remain_time = (uint32_t) g_current_blinky_interval * (uint32_t)(BSP_HOCO_HZ / 5000);
        g_interval_update_flag = false;
    }else if ((0 == remain_time)&&(0 != g_current_blinky_interval)&&(0xFF != g_current_blinky_interval))
    {
        /* Toggle LED */
        g_ioport.p_api->pinRead(g_ioport.p_ctrl, BSP_IO_PORT_01_PIN_06, &level);
        g_ioport.p_api->pinWrite(g_ioport.p_ctrl, BSP_IO_PORT_01_PIN_06,
                                 (level == BSP_IO_LEVEL_HIGH)   ?   BSP_IO_LEVEL_LOW : BSP_IO_LEVEL_HIGH);

        remain_time = (uint32_t) g_current_blinky_interval * (uint32_t)(BSP_HOCO_HZ / 5000);
    }

    remain_time--;
}

/******************************************************************************
 * Function Name: sw_cb
 * Description  : Send notification when pushing switch.
 * Arguments    : none
 * Return Value : none
 ******************************************************************************/
static void sw_cb(void)
{
    uint8_t state = 1;
    R_BLE_LSS_NotifySwitchState(g_conn_hdl, &state);
    g_inhibit_software_standby = true;
}

/******************************************************************************
 * Function Name: Callback_ble_sw_irq
 * Description  : Callback for push switch interrupt.
 * Arguments    : external_irq_callback_args_t *p_args
 * Return Value : none
 ******************************************************************************/
void Callback_ble_sw_irq(external_irq_callback_args_t *p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);
    R_BLE_SetEvent(sw_cb);
}
/* End user code. Do not edit comment generated here */

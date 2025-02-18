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

/******************************************************************************
* File Name    : app_main.c
* Device(s)    : RA4W1
* Tool-Chain   : e2Studio
* Description  : This is a application file for central role.
*******************************************************************************/

/******************************************************************************
 Includes   <System Includes> , "Project Includes"
*******************************************************************************/
#include <string.h>
#include "r_ble_api.h"
#include "rm_ble_abs.h"
#include "rm_ble_abs_api.h"
#include "discovery/r_ble_disc.h"
#include "gatt_db.h"
#include "profile_cmn/r_ble_servs_if.h"
#include "profile_cmn/r_ble_servc_if.h"
#include "hal_data.h"

/* This code is needed for using Azure RTOS */
#if (BSP_CFG_RTOS == 1)
#include "tx_api.h"

#define  EXECUTE_STACK_SIZE  2048

static TX_SEMAPHORE gs_ble_exe_smpr;
void                *g_ble_event_group_handle = &gs_ble_exe_smpr;
static TX_THREAD    gs_ble_execute_task;
static uint8_t      gs_ble_execute_task_stack[EXECUTE_STACK_SIZE];
static TX_THREAD    *gs_ble_core_task_ptr;
static void         ble_execute_task_func(unsigned long Parameters);
/* This code is needed for using FreeRTOS */
#elif (BSP_CFG_RTOS == 2)
#if !defined(BLE_CFG_SYNCHRONIZATION_TYPE)
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
#define BLE_EVENT_PATTERN   (0x0A0A)
void *g_ble_event_group_handle;
#else /* !defined(BLE_CFG_SYNCHRONIZATION_TYPE) */

#if (BLE_CFG_SYNCHRONIZATION_TYPE == 0)
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#define EXECUTE_STACK_SIZE (512)

static SemaphoreHandle_t    gs_ble_exe_smpr;
void                        *g_ble_event_group_handle;
static TaskHandle_t         gs_ble_core_task_ptr;
static TaskHandle_t         gs_ble_execute_task;
static void                 ble_execute_task_func(void *pvParameters);
#elif (BLE_CFG_SYNCHRONIZATION_TYPE == 1)
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
#define BLE_EVENT_PATTERN   (0x0A0A)
void *g_ble_event_group_handle;
#endif /* (BLE_CFG_SYNCHRONIZATION_TYPE == x) */
#endif /* !defined(BLE_CFG_SYNCHRONIZATION_TYPE) */
#endif /* (BSP_CFG_RTOS == x) */
#include "r_ble_gapc.h"
#include "r_ble_gatc.h"
#include "r_ble_lsc.h"
#include "r_ble_gaps.h"

/******************************************************************************
 User file includes
*******************************************************************************/
/* Start user code for file includes. Do not edit comment generated here */
#include "ble_core_task.h"
#include "task_function.h"
#include "r_ble_cli.h"
#include "r_ble_cmd_abs.h"
#include "r_ble_cmd_vs.h"
#include "r_ble_cmd_sys.h"
/* End user code. Do not edit comment generated here */

/******************************************************************************
 User macro definitions
*******************************************************************************/
#define BLE_LOG_TAG "app_main"

/* Queue for Prepare Write Operation. Change if needed. */
#define BLE_GATTS_QUEUE_ELEMENTS_SIZE       (14)
#define BLE_GATTS_QUEUE_BUFFER_LEN          (245)
#define BLE_GATTS_QUEUE_NUM                 (1)

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
void disc_comp_cb(uint16_t conn_hdl);
ble_status_t ble_init(void);
void app_main(void);

/******************************************************************************
 User function prototype declarations
*******************************************************************************/
/* Start user code for function prototype declarations. Do not edit comment generated here */
extern void r_ble_wake_up_task_from_isr (void * EventGroupHandle);
extern void lsc_task_entry(unsigned long Parameters);
extern void delete_lsc_task_rsrc(void);
/* End user code. Do not edit comment generated here */

/******************************************************************************
 Generated global variables
*******************************************************************************/
/* GAP Service UUID */
static uint8_t GAPC_UUID[] = { 0x00, 0x18 };
/* GATT Service UUID */
static uint8_t GATC_UUID[] = { 0x01, 0x18 };
/* LED Switch Service UUID */
static uint8_t LSC_UUID[] = { 0xE0, 0xFC, 0x8E, 0x8E, 0x96, 0xB4, 0x01, 0xAB, 0x67, 0x42, 0x05, 0x5F, 0x26, 0x19, 0x83, 0x58 };
/* Service discovery parameters */
static st_ble_disc_entry_t gs_disc_entries[] =
{
    {
        .p_uuid     = GAPC_UUID,
        .uuid_type  = BLE_GATT_16_BIT_UUID_FORMAT,
        .serv_cb    = R_BLE_GAPC_ServDiscCb,
    },
    {
        .p_uuid     = GATC_UUID,
        .uuid_type  = BLE_GATT_16_BIT_UUID_FORMAT,
        .serv_cb    = R_BLE_GATC_ServDiscCb,
    },
    {
        .p_uuid     = LSC_UUID,
        .uuid_type  = BLE_GATT_128_BIT_UUID_FORMAT,
        .serv_cb    = R_BLE_LSC_ServDiscCb,
    },
};

/* Scan phy parameters */
static ble_abs_scan_phy_parameter_t gs_scan_phy_parameter =
{
    .fast_scan_interval      = 0x0060, /* 60.0(ms) */
    .fast_scan_window        = 0x0030, /* 30.0(ms) */
    .slow_scan_interval      = 0x0800, /* 1,280.0(ms) */
    .slow_scan_window        = 0x0012, /* 11.25(ms) */
    .scan_type               = BLE_GAP_SCAN_PASSIVE
};

/* Scan parameters */
static ble_abs_scan_parameter_t gs_scan_parameter =
{
    .p_phy_parameter_1M        = &gs_scan_phy_parameter,
    .fast_scan_period          = 0x0000, /* 0(ms) */
    .slow_scan_period          = 0x0000,
    .device_scan_filter_policy = BLE_GAP_SCAN_ALLOW_ADV_ALL,
    .filter_duplicate          = BLE_GAP_SCAN_FILT_DUPLIC_ENABLE,
};

/* Connection phy parameters */
static ble_abs_connection_phy_parameter_t gs_connection_phy_parameter =
{
    .connection_interval      = 0x0028, /* 50.0(ms) */
    .supervision_timeout      = 0x0200, /* 5,120(ms) */
    .connection_slave_latency = 0x0000,
};

/* Connection device address */
static ble_device_address_t gs_connection_device_address;

/* Connection parameters */
static ble_abs_connection_parameter_t gs_connection_parameter =
{
    .p_connection_phy_parameter_1M = &gs_connection_phy_parameter,
    .p_device_address              = &gs_connection_device_address,
    .filter_parameter              = BLE_GAP_INIT_FILT_USE_ADDR,
    .connection_timeout            = 0x05, /* 5(s) */
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

/* GATT server Prepare Write Queue parameters. Define BLE_APP_PREPARE_WRITE_DISABLE macro to reduce memory usage. */
#if (BLE_APP_PREPARE_WRITE_DISABLE == 0)
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
#endif /* BLE_APP_PREPARE_WRITE_DISABLE */

/* Connection handle */
uint16_t g_conn_hdl;

/******************************************************************************
 User global variables
*******************************************************************************/
/* Start user code for global variables. Do not edit comment generated here */
extern st_ble_cli_cmd_t g_lsc_cmd;

/* CommandLine parameters */
static const st_ble_cli_cmd_t * const gsp_cmds[] =
{
     &g_abs_cmd,
     &g_vs_cmd,
     &g_sys_cmd,
     &g_ble_cmd,
	 &g_lsc_cmd,
};

TX_THREAD       ble_lsc_task;
static uint8_t  ble_lsc_task_stack[512];
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
            /* Start scan when BLE protocol stack is ready */
            /* Include header file that contained BLE Abstraction (rm_ble_abs) module instance, when application work on RTOS. */
//            RM_BLE_ABS_StartScanning(&g_ble_abs0_ctrl, &gs_scan_parameter);
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
                /* Restart scan when connection failed */
                /* Include header file that contained BLE Abstraction (rm_ble_abs) module instance, when application work on RTOS. */
//                RM_BLE_ABS_StartScanning(&g_ble_abs0_ctrl, &gs_scan_parameter);
            }
        } break;

        case BLE_GAP_EVENT_DISCONN_IND:
        {
            /* Restart scan when disconnected */
            g_conn_hdl = BLE_GAP_INVALID_CONN_HDL;
            /* Include header file that contained BLE Abstraction (rm_ble_abs) module instance, when application work on RTOS. */
//            RM_BLE_ABS_StartScanning(&g_ble_abs0_ctrl, &gs_scan_parameter);
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
                .min_ce_length = 0xFFFF,
                .max_ce_length = 0xFFFF,
            };

            R_BLE_GAP_UpdConn(p_conn_upd_req_evt_param->conn_hdl,
                              BLE_GAP_CONN_UPD_MODE_RSP,
                              BLE_GAP_CONN_UPD_ACCEPT,
                              &conn_updt_param);
        } break;

        case BLE_GAP_EVENT_ADV_REPT_IND:
        {
#if 0        	
            /* Stop scan and store BD address when advertising data found */
            st_ble_gap_adv_rept_evt_t *p_adv_rept_evt_param = (st_ble_gap_adv_rept_evt_t *)p_data->p_param;
            gs_connection_parameter.p_device_address = &gs_connection_device_address;

            switch(p_adv_rept_evt_param->adv_rpt_type)
            {
                case 0x00: /* Advertising Report */
                {
                    st_ble_gap_adv_rept_t *p_adv_rept_param = (st_ble_gap_adv_rept_t *)p_adv_rept_evt_param->param.p_adv_rpt;
                    gs_connection_parameter.p_device_address->type = p_adv_rept_param->addr_type;
                    memcpy(gs_connection_parameter.p_device_address->addr, p_adv_rept_param->p_addr, BLE_BD_ADDR_LEN);
                }break;

                case 0x01: /* Extended Advertising Report */
                {
                    st_ble_gap_ext_adv_rept_t *p_adv_rept_param = (st_ble_gap_ext_adv_rept_t *)p_adv_rept_evt_param->param.p_ext_adv_rpt;
                    gs_connection_parameter.p_device_address->type = p_adv_rept_param->addr_type;
                    memcpy(gs_connection_parameter.p_device_address->addr, p_adv_rept_param->p_addr, BLE_BD_ADDR_LEN);
                }break;
            }

            R_BLE_GAP_StopScan();
#endif
        } break;

        case BLE_GAP_EVENT_SCAN_OFF:
        {
            if (NULL != gs_connection_parameter.p_device_address)
            {
                /* Send connection request after scan stopped */
                /* Include header file that contained BLE Abstraction (rm_ble_abs) module instance, when application work on RTOS. */
//                RM_BLE_ABS_CreateConnection(&g_ble_abs0_ctrl, &gs_connection_parameter);
            }
        } break;

        case BLE_GAP_EVENT_SCAN_ON:
        {
            gs_connection_parameter.p_device_address = NULL;
        } break;

/* Hint: Add cases of GAP event macros defined as BLE_GAP_XXX */
/* Start user code for GAP callback function event process. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
    }

/* Start user code for GAP callback function closing process. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
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

/* Start user code for GATT Server callback function closing process. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
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
        case BLE_GATTC_EVENT_CONN_IND:
        {
            R_BLE_CLI_Printf("Start Service Discovery\n");
            /* Start discovery operation after connection established */
            R_BLE_DISC_Start(p_data->conn_hdl, gs_disc_entries, ARRAY_SIZE(gs_disc_entries), disc_comp_cb);

            /* Create LED Switch Service Task */
            tx_thread_create(&ble_lsc_task, (CHAR*) "BLE_LSC_TASK", lsc_task_entry, g_conn_hdl,
                                    &ble_lsc_task_stack, 512, 4, 4, TX_NO_TIME_SLICE, TX_AUTO_START);
        } break;

/* Hint: Add cases of GATT Client event macros defined as BLE_GATTC_XXX */
/* Start user code for GATT Client callback function event process. Do not edit comment generated here */
        case BLE_GATTC_EVENT_DISCONN_IND:
        {
            /* task delete */
            delete_lsc_task_rsrc();

        } break;
/* End user code. Do not edit comment generated here */
    }

/* Start user code for GATT Client callback function closing process. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
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
/* Hint: Add cases of vender specific event macros defined as BLE_VS_XXX */
/* Start user code for vender specific callback function event process. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
    }

/* Start user code for vender specific callback function closing process. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
}

/******************************************************************************
 * Function Name: gaps_cb
 * Description  : Callback function for GAP Service server feature.
 * Arguments    : uint16_t type -
 *                  Event type of GAP Service server feature.
 *              : ble_status_t result -
 *                  Event result of GAP Service server feature.
 *              : st_ble_servs_evt_data_t *p_data - 
 *                  Event parameters of GAP Service server feature.
 * Return Value : none
 ******************************************************************************/
static void gaps_cb(uint16_t type, ble_status_t result, st_ble_servs_evt_data_t *p_data)
{
/* Hint: Input common process of callback function such as variable definitions */
/* Start user code for GAP Service Server callback function common process. Do not edit comment generated here */
    FSP_PARAMETER_NOT_USED(result);
    FSP_PARAMETER_NOT_USED(p_data);
/* End user code. Do not edit comment generated here */

    switch(type)
    {
/* Hint: Add cases of GAP Service server events defined in e_ble_gaps_event_t */
/* Start user code for GAP Service Server callback function event process. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
    }

/* Start user code for GAP Service Server callback function closing process. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
}

/******************************************************************************
 * Function Name: gapc_cb
 * Description  : Callback function for GAP Service client feature.
 * Arguments    : uint16_t type -
 *                  Event type of GAP Service client feature.
 *              : ble_status_t result -
 *                  Event result of GAP Service client feature.
 *              : st_ble_servc_evt_data_t *p_data - 
 *                  Event parameters of GAP Service client feature.
 * Return Value : none
 ******************************************************************************/
static void gapc_cb(uint16_t type, ble_status_t result, st_ble_servc_evt_data_t *p_data)
{
/* Hint: Input common process of callback function such as variable definitions */
/* Start user code for GAP Service Client callback function common process. Do not edit comment generated here */
    FSP_PARAMETER_NOT_USED(result);
    FSP_PARAMETER_NOT_USED(p_data);
/* End user code. Do not edit comment generated here */

    switch(type)
    {
/* Hint: Add cases of GAP Service client events defined in e_ble_gapc_event_t */
/* Start user code for GAP Service Client callback function event process. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
    }

/* Start user code for GAP Service Client callback function closing process. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
}

/******************************************************************************
 * Function Name: gatc_cb
 * Description  : Callback function for GATT Service client feature.
 * Arguments    : uint16_t type -
 *                  Event type of GATT Service client feature.
 *              : ble_status_t result -
 *                  Event result of GATT Service client feature.
 *              : st_ble_servc_evt_data_t *p_data - 
 *                  Event parameters of GATT Service client feature.
 * Return Value : none
 ******************************************************************************/
static void gatc_cb(uint16_t type, ble_status_t result, st_ble_servc_evt_data_t *p_data)
{
/* Hint: Input common process of callback function such as variable definitions */
/* Start user code for GATT Service Client callback function common process. Do not edit comment generated here */
    FSP_PARAMETER_NOT_USED(result);
    FSP_PARAMETER_NOT_USED(p_data);
/* End user code. Do not edit comment generated here */

    switch(type)
    {
/* Hint: Add cases of GATT Service client events defined in e_ble_gatc_event_t */
/* Start user code for GATT Service Client callback function event process. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
    }

/* Start user code for GATT Service Client callback function closing process. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
}

/******************************************************************************
 * Function Name: lsc_cb
 * Description  : Callback function for LED Switch Service client feature.
 * Arguments    : uint16_t type -
 *                  Event type of LED Switch Service client feature.
 *              : ble_status_t result -
 *                  Event result of LED Switch Service client feature.
 *              : st_ble_servc_evt_data_t *p_data - 
 *                  Event parameters of LED Switch Service client feature.
 * Return Value : none
 ******************************************************************************/
static void lsc_cb(uint16_t type, ble_status_t result, st_ble_servc_evt_data_t *p_data)
{
/* Hint: Input common process of callback function such as variable definitions */
/* Start user code for LED Switch Service Client callback function common process. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

    switch(type)
    {
/* Hint: Add cases of LED Switch Service client events defined in e_ble_lsc_event_t */
/* Start user code for LED Switch Service Client callback function event process. Do not edit comment generated here */
    case BLE_LSC_EVENT_SWITCH_STATE_HDL_VAL_NTF:
    {
        if (BLE_SUCCESS == result)
            R_BLE_CLI_Printf("lsc : Receive Notification from Server \n");
    } break;

    case BLE_LSC_EVENT_BLINK_RATE_READ_RSP:
    {
        if (BLE_SUCCESS == result)
            R_BLE_CLI_Printf("lsc : LED blink rate = 0x%X \n", *(uint8_t *)((st_ble_lsc_evt_data_t *)(p_data)->p_param));
    } break;

    default:
        break;
/* End user code. Do not edit comment generated here */
    }

/* Start user code for LED Switch Service Client callback function closing process. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
}
/******************************************************************************
 * Function Name: disc_comp_cb
 * Description  : Callback function for Service Discovery.
 * Arguments    : none
 * Return Value : none
 ******************************************************************************/
void disc_comp_cb(uint16_t conn_hdl)
{
/* Hint: Input process such as GATT operation */
/* Start user code for Discovery Complete callback function. Do not edit comment generated here */
    R_BLE_CLI_Printf("Done Service Discovery conn_hdl = 0x%04X\n", conn_hdl);
/* End user code. Do not edit comment generated here */
    return;
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
    /* Include header file that contained BLE Abstraction (rm_ble_abs) module instance, when application work on RTOS. */
    err = RM_BLE_ABS_Open(&g_ble_abs0_ctrl, &g_ble_abs0_cfg);
    if (FSP_SUCCESS != err)
    {
        return BLE_ERR_INVALID_OPERATION;
    }

/* Start user code for global value initialization. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

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

    /* Initialize GATT client */
    status = R_BLE_SERVC_Init();
    if (BLE_SUCCESS != status)
    {
        return BLE_ERR_INVALID_OPERATION;
    }

    /* Set Prepare Write Queue. Define BLE_APP_PREPARE_WRITE_DISABLE macro to reduce memory usage. */
#if (BLE_APP_PREPARE_WRITE_DISABLE == 0)
    R_BLE_GATTS_SetPrepareQueue(gs_queue, BLE_GATTS_QUEUE_NUM);
#endif /* BLE_APP_PREPARE_WRITE_DISABLE */
    /* Initialize GATT Discovery Library */
    status = R_BLE_DISC_Init();
    if (BLE_SUCCESS != status)
    {
        return BLE_ERR_INVALID_OPERATION;
    }

    /* Initialize GAP Service server API */
    status = R_BLE_GAPS_Init(gaps_cb);
    if (BLE_SUCCESS != status)
    {
        return BLE_ERR_INVALID_OPERATION;
    }

    /* Initialize GAP Service client API */
    status = R_BLE_GAPC_Init(gapc_cb);
    if (BLE_SUCCESS != status)
    {
        return BLE_ERR_INVALID_OPERATION;
    }

    /* Initialize GATT Service client API */
    status = R_BLE_GATC_Init(gatc_cb);
    if (BLE_SUCCESS != status)
    {
        return BLE_ERR_INVALID_OPERATION;
    }

    /* Initialize LED Switch Service client API */
    status = R_BLE_LSC_Init(lsc_cb);
    if (BLE_SUCCESS != status)
    {
        return BLE_ERR_INVALID_OPERATION;
    }

/* Start user code for user function initialization. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

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

#if !defined(BLE_CFG_SYNCHRONIZATION_TYPE)
    /* Create Event Group */
    g_ble_event_group_handle = (void *)xEventGroupCreate();
    assert(g_ble_event_group_handle);
#else /* !defined(BLE_CFG_SYNCHRONIZATION_TYPE) */
#if (BLE_CFG_SYNCHRONIZATION_TYPE == 0)
    /* Create Semaphore */
    gs_ble_exe_smpr = xSemaphoreCreateBinary();
    assert(gs_ble_exe_smpr);
    g_ble_event_group_handle = (void *)gs_ble_exe_smpr;
#elif (BLE_CFG_SYNCHRONIZATION_TYPE == 1)
    /* Create Event Group */
    g_ble_event_group_handle = (void *)xEventGroupCreate();
    assert(g_ble_event_group_handle);
#endif /* (BLE_CFG_SYNCHRONIZATION_TYPE == x) */
#endif /* !defined(BLE_CFG_SYNCHRONIZATION_TYPE) */
#endif /* (BSP_CFG_RTOS == 2) */

    /* Initialize BLE and profiles */
    ble_init();

/* When this BLE application works on the Azure RTOS */
#if (BSP_CFG_RTOS == 1)

    /* Create Semaphore */
    tx_semaphore_create(&gs_ble_exe_smpr, "BLE_CORE_TASK_SEMAPHOR", TX_NO_INHERIT);

    /* Get Own thread handle */
    gs_ble_core_task_ptr = tx_thread_identify();

    /* Create BLE Execute Task */
    tx_thread_create(&gs_ble_execute_task, (CHAR*) "BLE_EXECUTE_TASK", ble_execute_task_func, (ULONG) NULL,
                            &gs_ble_execute_task_stack, EXECUTE_STACK_SIZE, 1, 1, TX_NO_TIME_SLICE, TX_AUTO_START);

#elif (BSP_CFG_RTOS == 2)
#if defined(BLE_CFG_SYNCHRONIZATION_TYPE)
#if (BLE_CFG_SYNCHRONIZATION_TYPE == 0)
    /* Get Current Task handle */
    gs_ble_core_task_ptr = xTaskGetCurrentTaskHandle();

    /* Create Execute Task */
    xTaskCreate(ble_execute_task_func, "execute_task", EXECUTE_STACK_SIZE, NULL, configMAX_PRIORITIES-1, &gs_ble_execute_task);
#endif /* (BLE_CFG_SYNCHRONIZATION_TYPE == x) */
#endif /* defined(BLE_CFG_SYNCHRONIZATION_TYPE) */
#endif /* (BSP_CFG_RTOS == x) */

/* Hint: Input process that should be done before main loop such as calling initial function or variable definitions */
/* Start user code for process before main loop. Do not edit comment generated here */

    FSP_PARAMETER_NOT_USED(gs_connection_parameter);
    FSP_PARAMETER_NOT_USED(gs_scan_parameter);
    /* Configure CommandLine */
    R_BLE_CLI_Init();
    R_BLE_CLI_RegisterCmds(gsp_cmds, sizeof(gsp_cmds)/sizeof(gsp_cmds[0]));
    R_BLE_CLI_RegisterEventCb(r_ble_wake_up_task_from_isr);
    R_BLE_CMD_SetResetCb((ble_event_cb_t)ble_init);
/* End user code. Do not edit comment generated here */

    /* main loop */
    while (1)
    {
/* Hint: Input process that should be done before BLE_Execute in main loop */
/* Start user code for process before BLE_Execute. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

#if (BSP_CFG_RTOS == 1)
/* When this BLE application works on the Azure RTOS */
        if(0 != R_BLE_IsTaskFree())
        {
            tx_thread_suspend(gs_ble_core_task_ptr);
        }
        else
        {
            tx_semaphore_put(&gs_ble_exe_smpr);
        }
#elif (BSP_CFG_RTOS == 2)
/* When this BLE application works on the FreeRTOS */
#if !defined(BLE_CFG_SYNCHRONIZATION_TYPE) || (BLE_CFG_SYNCHRONIZATION_TYPE == 1)
        /* Process BLE Event */
        R_BLE_Execute();

        if(0 != R_BLE_IsTaskFree())
        {
            /* If the BLE Task has no operation to be processed, it transits block state until the event from RF transciever occurs. */
            xEventGroupWaitBits((EventGroupHandle_t)g_ble_event_group_handle,
                                (EventBits_t)BLE_EVENT_PATTERN,
                                pdTRUE,
                                pdFALSE,
                                portMAX_DELAY);
        }
#else /* !defined(BLE_CFG_SYNCHRONIZATION_TYPE) || (BLE_CFG_SYNCHRONIZATION_TYPE == 1) */
#if (BLE_CFG_SYNCHRONIZATION_TYPE == 0)
        if(0 != R_BLE_IsTaskFree())
        {
            vTaskSuspend(NULL);
        }
        else
        {
            xSemaphoreGive(gs_ble_exe_smpr);
        }

#endif /* (BLE_CFG_SYNCHRONIZATION_TYPE == x) || (BLE_CFG_SYNCHRONIZATION_TYPE == 1) */
#endif /* !defined(BLE_CFG_SYNCHRONIZATION_TYPE) */
#else /* (BSP_CFG_RTOS == x) */
        /* Process BLE Event */
        R_BLE_Execute();
#endif /* (BSP_CFG_RTOS == x) */

/* Hint: Input process that should be done during main loop such as calling processing functions */
/* Start user code for process during main loop. Do not edit comment generated here */
        /* Process Command Line */
        R_BLE_CLI_Process();
/* End user code. Do not edit comment generated here */
    }

/* Hint: Input process that should be done after main loop such as calling closing functions */
/* Start user code for process after main loop. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

#if (BSP_CFG_RTOS == 1)
    tx_thread_terminate(&gs_ble_execute_task);
    tx_thread_delete(&gs_ble_execute_task);
#elif (BSP_CFG_RTOS == 2)
#if !defined (BLE_CFG_SYNCHRONIZATION_TYPE)
    vEventGroupDelete((EventGroupHandle_t)g_ble_event_group_handle);
#else /* !defined (BLE_CFG_SYNCHRONIZATION_TYPE) */
#if (BLE_CFG_SYNCHRONIZATION_TYPE == 0)
    vTaskDelete(gs_ble_execute_task);
#elif (BLE_CFG_SYNCHRONIZATION_TYPE == 1)
    vEventGroupDelete((EventGroupHandle_t)g_ble_event_group_handle);
#endif /* (BLE_CFG_SYNCHRONIZATION_TYPE == x) */
#endif /* !defined (BLE_CFG_SYNCHRONIZATION_TYPE) */
#endif /* (BSP_CFG_RTOS == x) */

    /* Terminate BLE */
    /* Include header file that contained BLE Abstraction (rm_ble_abs) module instance, when application work on RTOS. */
    RM_BLE_ABS_Close(&g_ble_abs0_ctrl);

}

/* When this BLE application works on the Azure RTOS */
#if (BSP_CFG_RTOS == 1)
static void ble_execute_task_func(unsigned long Parameters)
{
    FSP_PARAMETER_NOT_USED(Parameters);
    while(1)
    {
        tx_semaphore_get(&gs_ble_exe_smpr, TX_WAIT_FOREVER);
        while(0 == R_BLE_IsTaskFree())
            R_BLE_Execute();

        tx_thread_resume(gs_ble_core_task_ptr);
    }
}
#else /* (BSP_CFG_RTOS == x) */
#if defined (BLE_CFG_SYNCHRONIZATION_TYPE)
#if (BLE_CFG_SYNCHRONIZATION_TYPE == 0)
static void ble_execute_task_func(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED(pvParameters);

    while(1)
    {
        xSemaphoreTake(gs_ble_exe_smpr, portMAX_DELAY);
        while(0 == R_BLE_IsTaskFree())
            R_BLE_Execute();

        vTaskResume(gs_ble_core_task_ptr);
    }
}
#endif /* (BLE_CFG_SYNCHRONIZATION_TYPE == 0) */
#endif /* defined(BLE_CFG_SYNCHRONIZATION_TYPE) */
#endif /* (BSP_CFG_RTOS == x) */

/******************************************************************************
 User function definitions
*******************************************************************************/
/* Start user code for function definitions. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/**********************************************************************************************************************
* Copyright (c) 2019 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*********************************************************************************************************************/

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

/* This code is needed for using FreeRTOS */
#if (BSP_CFG_RTOS == 2)
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
#define BLE_EVENT_PATTERN   (0x0A0A)
EventGroupHandle_t  g_ble_event_group_handle;
#endif
#include "r_ble_lsc.h"

/******************************************************************************
 User file includes
*******************************************************************************/
/* Start user code for file includes. Do not edit comment generated here */
#include "ble_core_task.h"
#include "r_ble_lsc.h"
#include "task_function.h"
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
#define BLE_EVENT_PATTERN   (0x0A0A)
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
extern void lsc_task_entry(void *pvParameters);
extern void r_ble_wake_up_task_from_isr (void * EventGroupHandle);
extern void delete_lsc_task_rsrc(void);
/* End user code. Do not edit comment generated here */

/******************************************************************************
 Generated global variables
*******************************************************************************/
/* LED Switch Service UUID */
static uint8_t LSC_UUID[] = { 0xE0, 0xFC, 0x8E, 0x8E, 0x96, 0xB4, 0x01, 0xAB, 0x67, 0x42, 0x05, 0x5F, 0x26, 0x19, 0x83, 0x58 };
/* Service discovery parameters */
static st_ble_disc_entry_t gs_disc_entries[] =
{
    {
        .p_uuid     = LSC_UUID,
        .uuid_type  = BLE_GATT_128_BIT_UUID_FORMAT,
        .serv_cb    = R_BLE_LSC_ServDiscCb,
    },
};

/* Scan phy parameters */
static ble_abs_scan_phy_parameter_t gs_scan_phy_parameter =
{
    .fast_scan_interval      = 0x0000, /* 0.0(ms) */
    .fast_scan_window        = 0x0000, /* 0.0(ms) */
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

/* Pairing parameters */
static ble_abs_pairing_parameter_t gs_abs_pairing_param =
{
    .io_capabilitie_local_device = BLE_GAP_IOCAP_NOINPUT_NOOUTPUT,
    .mitm_protection_policy      = BLE_GAP_SEC_MITM_BEST_EFFORT,
    .secure_connection_only      = BLE_GAP_SC_BEST_EFFORT,
    .local_key_distribute        = BLE_GAP_KEY_DIST_ENCKEY,
    .remote_key_distribute       = 0,
    .maximum_key_size            = 16,
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
EventGroupHandle_t              g_ble_event_group_handle;
TaskHandle_t                    g_lsc_task;

extern st_ble_cli_cmd_t g_lsc_cmd;

/* CommandLine parameters */
static const st_ble_cli_cmd_t * const gsp_cmds[] =
{
     &g_abs_cmd,
     &g_vs_cmd,
     &g_sys_cmd,
     &g_lsc_cmd,
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
            g_conn_hdl = BLE_GAP_INVALID_CONN_HDL;
            /* Start scan when BLE protocol stack is ready */
//            RM_BLE_ABS_StartScanning(&g_ble_abs0_ctrl, &gs_scan_parameter);
        } break;

        case BLE_GAP_EVENT_CONN_IND:
        {
            if (BLE_SUCCESS == result)
            {
                /* Store connection handle */
                st_ble_gap_conn_evt_t *p_gap_conn_evt_param = (st_ble_gap_conn_evt_t *)p_data->p_param;
                g_conn_hdl = p_gap_conn_evt_param->conn_hdl;
                R_BLE_CLI_Printf("gap : BLE_GAP_EVENT_CONN_IND Handle = 0x%X\n", g_conn_hdl);
            }
            else
            {
                /* Restart scan when connection failed */
//                RM_BLE_ABS_StartScanning(&g_ble_abs0_ctrl, &gs_scan_parameter);
            }
        } break;

        case BLE_GAP_EVENT_DISCONN_IND:
        {
            /* Restart scan when disconnected */
            g_conn_hdl = BLE_GAP_INVALID_CONN_HDL;
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
            st_ble_gap_adv_rept_evt_t *p_adv_rept_param = (st_ble_gap_adv_rept_evt_t *)p_data->p_param;
            st_ble_gap_ext_adv_rept_t *p_ext_adv_rept_param = (st_ble_gap_ext_adv_rept_t *)p_adv_rept_param->param.p_ext_adv_rpt;
            gs_connection_parameter.p_device_address->type = p_ext_adv_rept_param->addr_type;
            memcpy(gs_connection_parameter.p_device_address->addr, p_ext_adv_rept_param->p_addr, BLE_BD_ADDR_LEN);
            R_BLE_GAP_StopScan();
#endif
        } break;

        case BLE_GAP_EVENT_SCAN_OFF:
        {
            /* Send connection request after scan stopped */
//            RM_BLE_ABS_CreateConnection(&g_ble_abs0_ctrl, &gs_connection_parameter);
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
        case BLE_GATTC_EVENT_CONN_IND:
        {
            /* Start discovery operation after connection established */
            R_BLE_CLI_Printf("Start Service Discovery\n");

            R_BLE_DISC_Start(p_data->conn_hdl, gs_disc_entries, ARRAY_SIZE(gs_disc_entries), disc_comp_cb);

            /* Create GATT application task */
            xTaskCreate(lsc_task_entry, "lsc_task", 128, &g_conn_hdl, 4, &g_lsc_task);

        } break;

        case BLE_GATTC_EVENT_DISCONN_IND:
        {

            /* task delete */
            delete_lsc_task_rsrc();

        } break;

        default:
            /* Do nothing */
            break;

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
/* Hint: Add cases of vender specific event macros defined as BLE_VS_XXX */
/* Start user code for vender specific callback function event process. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
    }
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
    /* Initialize GATT Discovery Library */
    status = R_BLE_DISC_Init();
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
    FSP_PARAMETER_NOT_USED(gs_abs_pairing_param);
    FSP_PARAMETER_NOT_USED(gs_connection_parameter);
    FSP_PARAMETER_NOT_USED(gs_scan_parameter);

    /* Configure CommandLine */
    R_BLE_CLI_Init();
    R_BLE_CLI_RegisterCmds(gsp_cmds, ARRAY_SIZE(gsp_cmds));
    R_BLE_CLI_RegisterEventCb(r_ble_wake_up_task_from_isr);
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
/* End user code. Do not edit comment generated here */

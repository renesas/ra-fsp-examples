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
#if (BSP_CFG_RTOS == 2 || BSP_CFG_RTOS_USED == 1)
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
#define BLE_EVENT_PATTERN   (0x0A0A)
EventGroupHandle_t  g_ble_event_group_handle;
#endif
#include "r_ble_gats.h"

/******************************************************************************
 User file includes
*******************************************************************************/
/* Start user code for file includes. Do not edit comment generated here */
#include "r_ble_spp.h"
#include <stdbool.h>
#include "common_init.h"
#include "common_utils.h"
#include "xmodem.h"
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
bool     is_flash_image_found( uint8_t * p_FlashAddressStart, uint8_t * p_FlashAddressEnd );
uint32_t get_fw_image_length( uint8_t * p_FlashAddressStart, uint8_t * p_FlashAddressEnd );
bool     get_fw_version(void);
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

    /* Complete Local Name */
    0x07, /**< Data Size */
    0x09, /**< Data Type */
    0x52, 0x59, 0x5a, 0x30, 0x31, 0x32, /**< Data Value */
};

ble_abs_legacy_advertising_parameter_t g_ble_advertising_parameter =
{
 .p_peer_address             = NULL,       ///< Peer address.
 .slow_advertising_interval  = 0x00000640, ///< Slow advertising interval. 1,000.0(ms)
 .slow_advertising_period    = 0x0000,     ///< Slow advertising period.
 .p_advertising_data         = gs_advertising_data,             ///< Advertising data. If p_advertising_data is specified as NULL, advertising data is not set.
 .advertising_data_length    = ARRAY_SIZE(gs_advertising_data), ///< Advertising data length (in bytes).
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
st_ble_vs_get_fw_version_comp_evt_t g_ble_version_data =
{
 .major   = 0x00,       ///< FW Version Major
 .minor   = 0x00,       ///< FW Version Minor
 .special = 0x00,       ///< Special NA
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
        case BLE_VS_EVENT_GET_FW_VERSION_COMP:
        {
            /* Received BLE Module FW Version info */
            st_ble_vs_get_fw_version_comp_evt_t * get_version = (st_ble_vs_get_fw_version_comp_evt_t *)p_data->p_param;
            g_ble_version_data.major = get_version->major;
            g_ble_version_data.minor = get_version->minor;
        } break;
/* End user code. Do not edit comment generated here */
    }
}

/******************************************************************************
 * Function Name: gats_cb
 * Description  : Callback function for GATT Service server feature.
 * Arguments    : uint16_t type -
 *                  Event type of GATT Service server feature.
 *              : ble_status_t result -
 *                  Event result of GATT Service server feature.
 *              : st_ble_servs_evt_data_t *p_data - 
 *                  Event parameters of GATT Service server feature.
 * Return Value : none
 ******************************************************************************/
static void gats_cb(uint16_t type, ble_status_t result, st_ble_servs_evt_data_t *p_data)
{
/* Hint: Input common process of callback function such as variable definitions */
/* Start user code for GATT Service Server callback function common process. Do not edit comment generated here */
    FSP_PARAMETER_NOT_USED (result);  // silence compiler warnings
    FSP_PARAMETER_NOT_USED (p_data);
/* End user code. Do not edit comment generated here */

    switch(type)
    {
/* Hint: Add cases of GATT Service server events defined in e_ble_gats_event_t */
/* Start user code for GATT Service Server callback function event process. Do not edit comment generated here */
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

    /* Initialize GATT Service server API */
    status = R_BLE_GATS_Init(gats_cb);
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
#if (BSP_CFG_RTOS == 2 || BSP_CFG_RTOS_USED == 1)
    /* Create Event Group */
    g_ble_event_group_handle = xEventGroupCreate();
    assert(g_ble_event_group_handle);
#endif


/* Hint: Input process that should be done before main loop such as calling initial function or variable definitions */
/* Start user code for process before main loop. Do not edit comment generated here */
    fsp_err_t err;
    ble_status_t status;

#ifdef DEBUG_RTT_VIEWER
    APP_PRINT("\r\nInitialize BLE Driver\r\n");
#endif

    /* Initialize BLE Driver */
    err = RM_BLE_ABS_Open(&g_ble_abs0_ctrl, &g_ble_abs0_cfg);
#ifdef DEBUG_RTT_VIEWER
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n ** Initialize BLE Driver : failed ** \r\n");
        APP_ERR_PRINT("\r\n ** Check that RYZ012 is connected to Eval Kit ** \r\n");
        APP_ERR_PRINT("\r\n ** Check that RYZ012 is inserted with correct pin orientation ** \r\n");
        APP_ERR_TRAP(err);
    }
    APP_PRINT("\r\nBLE Driver Init : Success\r\n");
#else
    assert(FSP_SUCCESS == err);
#endif

    /* Get RYZ012 FW Version that is running */
    get_fw_version();

#ifdef DEBUG_RTT_VIEWER
    APP_PRINT("\r\nCheck for RYZ012 FW Image in MCU Memory\r\n");
#endif
    bool b_image = false;
    b_image = is_flash_image_found( (uint8_t *) RYZ012_FWIMAGE_START_ADDRESS, (uint8_t *) RYZ012_FWIMAGE_END_ADDRESS );

#ifdef DEBUG_RTT_VIEWER
    APP_PRINT("\r\nRYZ012 FW Image %s\r\n", (b_image ? "Found": "Not Found") );
#endif

#ifdef DEBUG_RTT_VIEWER
    APP_PRINT("\r\nStarted RYZ012 Module Firmware Update\r\n");
#endif

    /* Call Vendor Specific API : Run the RYZ012 Module Firmware Update : MCU based procedure. */
    if ( b_image )
    {
        /* A FW Image is present
         * Determine the FW Image Length as programmed by XMODEM.
         * NOTE: If not programmed by XMODEM, the get_fw_image_length(...) function
         * will need to be modified to determine the Image end of file.
         **/
        uint32_t image_len;
        image_len = get_fw_image_length( (uint8_t *) RYZ012_FWIMAGE_START_ADDRESS, (uint8_t *) RYZ012_FWIMAGE_END_ADDRESS );

#ifdef DEBUG_RTT_VIEWER
        APP_PRINT("\r\nRYZ012 FW Image size = %u bytes\r\n", image_len );
#endif
        if ( ( 0 < image_len ) && (( RYZ012_FWIMAGE_END_ADDRESS - RYZ012_FWIMAGE_START_ADDRESS ) > image_len ))
        {
            status = R_BLE_VS_UpdateModuleFirmware( (uint8_t const *) RYZ012_FWIMAGE_START_ADDRESS, image_len);
        }
        else
        {
            // Image size not in range
            status = BLE_ERR_INVALID_DATA;
        }
    }
    else
    {
        status = BLE_ERR_NOT_FOUND;  // RYZ012 flash image not found @ MCU RYZ012_FWIMAGE_START_ADDRESS
    }

#ifdef DEBUG_RTT_VIEWER
    if (BLE_SUCCESS != status)
    {
        APP_ERR_PRINT("\r\n ** RYZ012 Module Firmware Update : Failed ** \r\n");
        switch ( status )
        {
            case BLE_ERR_INVALID_DATA :
                APP_ERR_PRINT("\r\n ** FW Image Size Invalid** \r\n");
                break;
            case BLE_ERR_NOT_FOUND :
                APP_ERR_PRINT("\r\n ** No FW Image Found in Memory @  RYZ012_FWIMAGE_START_ADDRESS** \r\n");
                break;
            default :
                break;
        }
        APP_ERR_TRAP(status);
    }
    APP_PRINT("\r\nRYZ012 Module Firmware Update : Success\r\n");
#else
    assert(BLE_SUCCESS == status);
#endif

    /* Close the BLE driver so that it can be re-initialized and run updated firmware. */
    err = RM_BLE_ABS_Close(&g_ble_abs0_ctrl);

#ifdef DEBUG_RTT_VIEWER
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n ** Close BLE Driver : Failed ** \r\n");
        APP_ERR_TRAP(err);
    }
    APP_PRINT("\r\nBLE Driver Close : Success\r\n");
#else
    assert(FSP_SUCCESS == err);
#endif


#ifdef DEBUG_RTT_VIEWER
    APP_PRINT("\r\nInit BLE Driver and Profiles\r\n");
#endif

    /* Initialize BLE and profiles */
    status = ble_init();

#ifdef DEBUG_RTT_VIEWER
    if (FSP_SUCCESS != status)
    {
        APP_ERR_PRINT("\r\n ** BLE Driver and Profiles Init : Failed ** \r\n");
        APP_ERR_TRAP(status);
    }
    APP_PRINT("\r\nBLE Driver and Profile Init : Success\r\n");
#else
    assert(BLE_SUCCESS == status);
#endif

    /* Get RYZ012 FW Version that is running */
    get_fw_version();

#ifdef DEBUG_RTT_VIEWER
    APP_PRINT("\r\nRunning RYZ012 FW\r\n");
#endif
/* End user code. Do not edit comment generated here */

    /* main loop */
    while (1)
    {
        /* Process BLE Event */
        R_BLE_Execute();

/* When this BLE application works on the FreeRTOS */
#if (BSP_CFG_RTOS == 2 || BSP_CFG_RTOS_USED == 1)
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

/* Look for an RYZ012 FW Image in MCU Code Flash */
bool is_flash_image_found( uint8_t * p_FlashAddressStart, uint8_t * p_FlashAddressEnd )
{
    bool  image_found;
    uint8_t * p_src;

    image_found = false;
    p_src = p_FlashAddressStart;
    do
    {
        if ( 0xFF != *p_src )
        {
            image_found = true;
        }
        p_src++;
    } while ( (false == image_found) && ((uint8_t *)p_FlashAddressEnd > p_src) );
    return image_found;
}

/* Determine the RYZ012 Firmware Image Length in MCU Code Flash */
uint32_t get_fw_image_length( uint8_t * p_FlashAddressStart, uint8_t * p_FlashAddressEnd )
{
    bool end = false;
    uint32_t * p_src;
    uint32_t len = 0;

    p_src = (uint32_t *) p_FlashAddressStart;
    do
    {
        if ( 0x1A1A1A1A == *p_src )
        {
            /* four consecutive 0x1A bytes found, as written by XMODEM transfer
             * this is the end of the image.
             * NOTE: If FW image is transferred using a method
             * other than XMODEM, end of file determination will be different
             * and require modification here.
             * XMODEM transfer writes "0x1A1A1A1A" after the end of the file in Code flash
             * and is not part of the FW binary file itself.
             */
            end = true;
            if (len >= 12) len -= 12;  /* compensate for the 12 bytes counted beyond end of file */
        }
        else
        {
            p_src += 4;   /* next address to check, 0x60000, 0x60010, 0x60020 */
            len += 16;    /* count the 16 bytes found at the address */
        }
    } while ( ( false == end ) && ( (uint32_t *)p_FlashAddressEnd > p_src ));
    return len;
}

/* Get the Firmware Version running on RYZ012 Module */
bool get_fw_version(void)
{
    static r_ble_spp_payload_t payload_data;
    r_ble_spp_cmd_rsp_t ret_val = R_BLE_SPP_SUCCESS;
    ble_status_t status;
    #define MIN_FW_VERSION_BYTES  3  /* Payload must have 3 or more bytes */

    status = R_BLE_VS_GetFirmwareVersion();
    if (BLE_SUCCESS != status)
    {
#ifdef DEBUG_RTT_VIEWER
        APP_ERR_PRINT("\r\n ** Get RYZ012 FW Version : Failed ** \r\n");
#endif
        return false;
    }

    /* To get the FW version data immediately use R_BLE_SPP_SPI_Read(&payload_data)
     * otherwise, use Process BLE Event by calling R_BLE_Execute() and waiting
     * for callback event BLE_VS_EVENT_GET_FW_VERSION_COMP to be received in vs_cb(...)
     * vs_cb(uint16_t type, ble_status_t result, st_ble_vs_evt_data_t *p_data)
     */
    R_BSP_SoftwareDelay(10, BSP_DELAY_UNITS_MILLISECONDS);
    ret_val = R_BLE_SPP_SPI_Read(&payload_data);
    if ( R_BLE_SPP_SUCCESS == ret_val )
    {
        if ( ( BLE_VS_EVENT_GET_FW_VERSION_COMP == payload_data.event_id ) && ( MIN_FW_VERSION_BYTES <= payload_data.out_len ) )
        {
            /* payload contains fw version data */
            g_ble_version_data.major = payload_data.out_data[1];
            g_ble_version_data.minor = payload_data.out_data[2];
#ifdef DEBUG_RTT_VIEWER
            APP_PRINT("\r\nRYZ012 FW Version v%d.%d \r\n", g_ble_version_data.major, g_ble_version_data.minor);
#endif
            return true;
        }

#ifdef DEBUG_RTT_VIEWER
        APP_ERR_PRINT("\r\n ** Get RYZ012 FW Version : Failed ** \r\n");
#endif
        return false;
    }

#ifdef DEBUG_RTT_VIEWER
    APP_PRINT("RYZ012 FW Version data read Error: %x\r\n", ret_val);
#endif
    return false;
}
/* End user code. Do not edit comment generated here */

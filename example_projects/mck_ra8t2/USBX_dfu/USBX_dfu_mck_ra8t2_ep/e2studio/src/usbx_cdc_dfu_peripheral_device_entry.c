/***********************************************************************************************************************
 * File Name    : usbx_cdc_dfu_peripheral_device_entry.c
 * Description  : Contains macros and functions used usbx_cdc_dfu_peripheral_device functionality.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2022 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "usbx_cdc_dfu_peripheral_device.h"
#include "usbx_cdc_dfu_peri_device_common.h"
#include "common_utils.h"
#include "bsp_api.h"
#include "ux_system.h"
#include "ux_api.h"

/*******************************************************************************************************************//**
 * @addtogroup usbx_dfu_ep
 * @{
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * CDC Class Private Function prototypes
 **********************************************************************************************************************/
static void usbx_cdc_instance_activate(VOID * peri_cdc_device_instance);
static void usbx_cdc_instance_deactivate(VOID * peri_cdc_device_instance);
static UINT usbx_cdc_dfu_device_status_change_callback(ULONG status);

/*******************************************************************************************************************//**
 * Private global variables
 **********************************************************************************************************************/
static UX_SLAVE_CLASS_CDC_ACM_PARAMETER g_ux_device_class_cdc_parameter;

static UX_SLAVE_CLASS_CDC_ACM * volatile gp_peri_cdc_device;
/* Mem-pool size of 2*18k is required for USBX device class pre-built libraries
 * and it is valid only if it with default USBX configurations. */
static uint32_t g_ux_pool_memory[MEMPOOL_SIZE / BYTE_SIZE] = {RESET_VALUE};

static UCHAR g_recBuffer[] = "RUN TIMR Mode";

static UCHAR  g_receiveBuffer[CDC_DATA_LENGTH] = {RESET_VALUE};
static ULONG  g_cdc_actual_length = RESET_VALUE;

/*******************************************************************************************************************//**
 * Global variables and functions
 **********************************************************************************************************************/
void current_drivers_un_init(void);
void dfu_device_init(void);

/*******************************************************************************************************************//**
 * @brief     In this function, it activates the PCDC instance.
 * @param[IN] peri_cdc_device_instance    Pointer to the area store the instance pointer.
 * @retval    None
 **********************************************************************************************************************/
static void usbx_cdc_instance_activate(VOID * peri_cdc_device_instance)
{
    /* Save the peripheral CDC device instance */
    gp_peri_cdc_device = (UX_SLAVE_CLASS_CDC_ACM *) peri_cdc_device_instance;
    tx_semaphore_put(&g_peri_cdc_dfu_device_connect_semaphore0);
    PRINT_INFO_STR("RA USBX peripheral CDC Device ACTIVATED");
    tx_thread_sleep(THREAD_SLEEP_10);
}

/*******************************************************************************************************************//**
 * @brief     In this function, it deactivates the PCDC instance.
 * @param[IN] peri_cdc_device_instance    Pointer to the area store the instance pointer.
 * @retval    none
 **********************************************************************************************************************/
static void usbx_cdc_instance_deactivate(VOID * peri_cdc_device_instance)
{
    FSP_PARAMETER_NOT_USED(peri_cdc_device_instance);
    /* Deactivate the peripheral CDC device instance */
    gp_peri_cdc_device = NULL;
    tx_semaphore_put(&g_peri_cdc_dfu_device_connect_semaphore0);
    PRINT_INFO_STR("RA USBX peripheral CDC Device DEACTIVATED");
    tx_thread_sleep(THREAD_SLEEP_10);
}

/*******************************************************************************************************************//**
 * @brief     Callback function for USBX Peripheral CDC device status changes.
 * @param[IN] status     USB status value indicating the current state or event.
 * @retval    UX_SUCCESS Always returns success to indicate the status was handled.
 **********************************************************************************************************************/
UINT usbx_cdc_dfu_device_status_change_callback(ULONG status)
{
    if(UX_DEVICE_ATTACHED == status)
    {
        PRINT_INFO_STR("RA USBX Peripheral CDC Device ATTACHED");
        tx_thread_sleep(THREAD_SLEEP_10);
    }
    else if(UX_DEVICE_ADDRESSED == status)
    {
        PRINT_INFO_STR("RA USBX Peripheral CDC Device ADDRESSED");
        tx_thread_sleep(THREAD_SLEEP_10);
    }
    else if(UX_DEVICE_CONFIGURED == status)
    {
        PRINT_INFO_STR("RA USBX Peripheral CDC Device CONFIGURED");
        tx_thread_sleep(THREAD_SLEEP_10);
    }
    else if(UX_DEVICE_SUSPENDED == status)
    {
        PRINT_INFO_STR("RA USBX Peripheral CDC Device SUSPENDED");
        tx_thread_sleep(THREAD_SLEEP_10);
    }
    else if(UX_DEVICE_RESUMED == status)
    {
        PRINT_INFO_STR("RA USBX Peripheral CDC Device RESUMED");
        tx_thread_sleep(THREAD_SLEEP_10);
    }
    else if(UX_DEVICE_FORCE_DISCONNECT == status)
    {
        PRINT_INFO_STR("RA USBX Peripheral CDC Force Disconnect");
        current_drivers_un_init();
    }
    else if(UX_DEVICE_REMOVED == status)
    {
        if (_ux_system_slave -> ux_system_slave_device_dfu_state_machine == UX_SYSTEM_DFU_STATE_APP_DETACH)
        {
            tx_event_flags_set (&ra_dfu_detach_event, DETACH_EVENT_FLAG, TX_OR);
            gp_peri_cdc_device = NULL;
        }

        PRINT_INFO_STR("RA USBX Peripheral CDC Device REMOVED");
        tx_thread_sleep(THREAD_SLEEP_10);
    }
    return UX_SUCCESS;
}

/* USBX CDC entry function */
void usbx_cdc_dfu_peripheral_device_entry(void)
{
    UINT status = UX_SUCCESS;
    ULONG actual_flags = RESET_VALUE;

    g_download_done = false;
    g_dfuManifestation_done = false;

    R_BSP_PinAccessEnable();
    rtt_thread_init_check ();
    /* Print the banner and EP info */
    app_rtt_print_data (RTT_OUTPUT_MESSAGE_BANNER, RESET_VALUE, NULL);

    /* ux_system_initialization */
    status = ux_system_initialize(g_ux_pool_memory, MEMPOOL_SIZE, UX_NULL, UX_ZERO);
    if (UX_SUCCESS != status)
    {
        PRINT_ERR_STR("ux_system_initialize API failed");
        ERROR_TRAP(status);
    }
    PRINT_INFO_STR("ux_system_initialize successfully");

    /* Initialize the RA USBX peripheral CDC-DFU device */
    usbx_peri_cdc_dfu_init();

    PRINT_INFO_STR("RA USBX Peripheral CDC-DFU Device Initialization Completed");
    tx_thread_sleep(THREAD_SLEEP_10);

    /* Wait for USB peripheral CDC-DFU device connection status */
    tx_semaphore_get(&g_peri_cdc_dfu_device_connect_semaphore0, UX_WAIT_FOREVER);

    tx_thread_sleep(THREAD_SLEEP_200);

    while(true)
    {
        status = tx_event_flags_get(&ra_dfu_detach_event,DETACH_EVENT_FLAG,TX_OR_CLEAR,&actual_flags, TX_NO_WAIT);
        if (TX_SUCCESS != status && DETACH_EVENT_FLAG != actual_flags)
        {
            tx_thread_sleep(THREAD_SLEEP_200);
            if (NULL != gp_peri_cdc_device)
            {
                PRINT_INFO_STR("Get CDC data from Host PC");
                /* Read from the CDC class */
                if (FSP_SUCCESS == _ux_device_class_cdc_acm_read(gp_peri_cdc_device, g_receiveBuffer, CDC_DATA_LENGTH,\
                                                                 &g_cdc_actual_length))
                {
                    g_receiveBuffer[g_cdc_actual_length] = NULL_0;
                    if (ZERO == g_cdc_actual_length)
                    {
                        PRINT_INFO_STR("RA USBX Device Preparing To Switch From Run-Time to DFU mode");
                        tx_thread_sleep(THREAD_SLEEP_200);
                        continue;
                    }
                    status = _ux_device_class_cdc_acm_write(gp_peri_cdc_device, g_receiveBuffer, g_cdc_actual_length,\
                                                            &g_cdc_actual_length);
                    if (UX_SUCCESS != status)
                    {
                        PRINT_ERR_STR("_ux_device_class_cdc_acm_write API failed");
                    }
                    else
                    {
                    PRINT_INFO_STR("_ux_device_class_cdc_acm_write successfully");
                    }
                    tx_thread_sleep(THREAD_SLEEP_20);
                    memset(g_receiveBuffer,RESET_VALUE,sizeof(g_receiveBuffer));
                    g_cdc_actual_length = CDC_ACTUAL_LENGTH;
                    status = _ux_device_class_cdc_acm_write(gp_peri_cdc_device, g_recBuffer, g_cdc_actual_length, \
                                                            &g_cdc_actual_length);
                    if (UX_SUCCESS != status)
                    {
                        PRINT_ERR_STR("_ux_device_class_cdc_acm_write API failed");
                    }
                    else
                    {
                    PRINT_INFO_STR("_ux_device_class_cdc_acm_write successfully");
                    }
                    tx_thread_sleep(THREAD_SLEEP_200);
                }
                else
                {
                    PRINT_INFO_STR("RA USBX Peripheral CDC Device Read Failed");
                    tx_thread_sleep(THREAD_SLEEP_20);
                }

            }
            else if (true == g_download_done)
            {
                g_download_done = false;
                tx_thread_sleep(THREAD_SLEEP_100);
                status =  _ux_utility_memory_compare(g_pre_dfu_data, g_dfu_data, g_data_cnt);
                if (UX_SUCCESS != status)
                {
                    PRINT_ERR_STR("Download data integrity Failed");
                    tx_thread_sleep(THREAD_SLEEP_10);
                }else
                {
                    PRINT_INFO_STR("Download data integrity PASS");
                    tx_thread_sleep(THREAD_SLEEP_10);
                }
                tx_thread_sleep(THREAD_SLEEP_10);
                g_data_cnt = ZERO;
                g_block_no_cnt = ZERO;
                memset(g_dfu_data, RESET_VALUE, sizeof(g_dfu_data));
                tx_thread_sleep(THREAD_SLEEP_100);

            }
            else if (true == g_dfuManifestation_done)
            {
                g_dfuManifestation_done = false;
                tx_thread_sleep(THREAD_SLEEP_100);
                NVIC_SystemReset();
            }
            else
            {
                tx_thread_sleep(THREAD_SLEEP_10);
            }
        }
        else
        {
            tx_thread_sleep(THREAD_SLEEP_20);
        }
    }
}

/*******************************************************************************************************************//**
 * @brief     In this function, it initialize the RA USBX peripheral CDC-DFU device.
 * @param[IN] None
 * @retval    None
 **********************************************************************************************************************/
void usbx_peri_cdc_dfu_init(void)
{
    UINT status = UX_SUCCESS;

    /* ux_device stack initialization */
    status = ux_device_stack_initialize(g_device_framework_hi_speed,
                                        DEVICE_FRAME_LENGTH_CDC_HI_SPEED,
                                        g_device_framework_full_speed,
                                        DEVICE_FRAME_LENGTH_CDC_FULL_SPEED,
                                        g_string_framework,
                                        STRING_FRAMEWORK_CDC_LENGTH,
                                        g_language_id_framework,
                                        CDC_LANGUAGE_ID_FRAME_WORK_LENGTH,
                                        usbx_cdc_dfu_device_status_change_callback);
    /* Error Handle */
    if (UX_SUCCESS != status)
    {
        PRINT_ERR_STR("ux_device_stack_initialize API failed");
        ERROR_TRAP(status);
    }
    PRINT_INFO_STR("ux_device_stack_initialize successfully");

    /* The activate command is used when the host has sent a SET_CONFIGURATION command
       and this interface has to be mounted. Endpoints have to be mounted and
       the peripheral CDC-DFU device thread needs to be activated. */
    g_ux_device_class_cdc_parameter.ux_slave_class_cdc_acm_instance_activate = usbx_cdc_instance_activate;

    /* The deactivate command is used when the device has been extracted.
     The device endpoints have to be dismounted and the peripheral CDC-DFU device thread canceled. */
    g_ux_device_class_cdc_parameter.ux_slave_class_cdc_acm_instance_deactivate = usbx_cdc_instance_deactivate;

    /* ux_device stack class registration */
    status = ux_device_stack_class_register (_ux_system_slave_class_cdc_acm_name, _ux_device_class_cdc_acm_entry,\
                                             CONFIG_NUMB, INTERFACE_NUMB0, (void*) &g_ux_device_class_cdc_parameter);
    /* Error Handle */
    if (UX_SUCCESS != status)
    {
        PRINT_ERR_STR("ux_device_stack_class_register API failed for CDC ACM");
        ERROR_TRAP(status);
    }
    PRINT_INFO_STR("ux_device_stack_class_register successfully for CDC ACM");

    g_ux_dfu_peri_class_parameter.ux_slave_class_dfu_parameter_framework = g_device_framework_full_speed;
    g_ux_dfu_peri_class_parameter.ux_slave_class_dfu_parameter_framework_length = DEVICE_FRAME_LENGTH_CDC_FULL_SPEED;

    /* ux_device stack class registration */
    status = ux_device_stack_class_register (_ux_system_slave_class_dfu_name, ux_device_class_dfu_entry, CONFIG_NUMB,\
                                             INTERFACE_NUMB2, (void*) &g_ux_dfu_peri_class_parameter);

    if (UX_SUCCESS != status)
    {
        PRINT_ERR_STR("ux_device_stack_class_register API failed for DFU");
        ERROR_TRAP(status);
    }
    PRINT_INFO_STR("ux_device_stack_class_register successfully for DFU");
    /* Start RA CDC-DFU USB module */
    status = R_USB_Open(&g_basic1_ctrl, &g_basic1_cfg);
    if (UX_SUCCESS != status)
    {
        PRINT_ERR_STR("R_USB_Open API failed");
        ERROR_TRAP(status);
    }
    PRINT_INFO_STR("R_USB_Open successfully");
}

/*******************************************************************************************************************//**
 * @brief     In this function, all loaded current drives are unloaded.
 * @param[IN] none
 * @retval    none
 **********************************************************************************************************************/
void current_drivers_un_init(void)
{
    UINT status = UX_SUCCESS;

    if (_ux_system_slave -> ux_system_slave_device_dfu_state_machine == UX_SYSTEM_DFU_STATE_APP_DETACH)
    {
        /* USB(CDC) module stops when USB_IP0 is specified */
        status = R_USB_Close(&g_basic1_ctrl);
        if (UX_SUCCESS != status)
        {
            PRINT_ERR_STR("R_USB_Close API failed");
            tx_thread_sleep(THREAD_SLEEP_10);
        }
        else
        {
        PRINT_INFO_STR("R_USB_Close successfully");
        }
        /* Unregister DFU class device */
        status= ux_device_stack_class_unregister(_ux_system_slave_class_dfu_name, ux_device_class_dfu_entry);
        if (UX_SUCCESS != status)
        {
            PRINT_ERR_STR("ux_device_stack_class_unregister API failed for DFU");
            tx_thread_sleep(THREAD_SLEEP_10);
        }
        else
        {
        PRINT_INFO_STR("ux_device_stack_class_unregister successfully for DFU");
        }
        /* Unregister CDC class device */
        status= ux_device_stack_class_unregister(_ux_system_slave_class_cdc_acm_name, _ux_device_class_cdc_acm_entry);
        if (UX_SUCCESS != status)
        {
            PRINT_ERR_STR("ux_device_stack_class_unregister API failed for CDC ACM");
            tx_thread_sleep(THREAD_SLEEP_10);
        }
        else
        {
        PRINT_INFO_STR("ux_device_stack_class_unregister successfully for CDC ACM");
        }
        /* This function un-initializes the generic portion of the device */
        status =_ux_device_stack_uninitialize();
        if (UX_SUCCESS != status)
        {
            PRINT_ERR_STR("_ux_device_stack_uninitialize API failed");
            tx_thread_sleep(THREAD_SLEEP_10);
        }
        else
        {
        PRINT_INFO_STR("_ux_device_stack_uninitialize successfully");
        }
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup  usbx_dfu_ep)
 **********************************************************************************************************************/

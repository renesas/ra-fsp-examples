/***********************************************************************************************************************
 * File Name    : usbx_dfu_example_app.c
 * Description  : Example application layer for the USBX DFU EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - usbx_peri_dfu_thread_entry.c calls a single function: usbx_peri_dfu_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "usbx_dfu_example_app.h"
#include "usbx_cdc_dfu_example_app.h"

/* Global variables */
volatile bool g_download_done = false;
volatile bool g_dfu_manifestation_done = false;
UX_SLAVE_CLASS_DFU_PARAMETER g_ux_dfu_peri_class_parameter;
UCHAR g_dfu_data[BUFFER_SIZE] = {RESET_VALUE};
ULONG g_data_cnt = RESET_VALUE;
ULONG g_block_no_cnt = RESET_VALUE;

/* Private variables */
static UX_SLAVE_CLASS_DFU * volatile sp_peri_cdc_dfu_device;

/* Track initialized DFU resources for error cleanup */
static bool s_dfu_usb_opened = false;
static bool s_dfu_device_stack_initialized = false;

/* Private functions */
static void dfu_device_init(void);
static void usbx_dfu_instance_activate(void * peri_dfu_device_instance);
static void usbx_dfu_instance_deactivate(void * peri_dfu_device_instance);
static UINT ux_slave_class_dfu_notify(void * dfu, ULONG notification);
static UINT ux_slave_class_dfu_get_status(void * dfu, ULONG * media_status);
static UINT usbx_dfu_write_callback(void * dfu, ULONG block_number, UCHAR * data_pointer, ULONG length,
                                    ULONG * media_status);
static UINT usbx_dfu_device_status_change_callback(ULONG status);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the USBX DFU example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void usbx_peri_dfu_example_app_run(void)
{
    UINT status = TX_SUCCESS;
    ULONG actual_flags = RESET_VALUE;
    while (true)
    {
        /* Check for DFU DETACH event */
        status = tx_event_flags_get(&ra_dfu_detach_event, DETACH_EVENT_FLAG, TX_OR_CLEAR, &actual_flags,
                                    TX_WAIT_FOREVER);
        if ((TX_SUCCESS == status) && (DETACH_EVENT_FLAG == actual_flags))
        {
            /* Complete the Run-Time resource cleanup */
            current_drivers_un_init();

            /* Initialize the device in DFU mode */
            dfu_device_init();
        }
        else
        {
            tx_thread_sleep(THREAD_SLEEP_10);
        }
    }
}

/*******************************************************************************************************************//**
 * @brief     This function cleans up initialized USBX DFU resources.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void usbx_dfu_cleanup(void)
{
    fsp_err_t err = FSP_SUCCESS;

    if (s_dfu_usb_opened)
    {
        err = usb_deinit(&g_basic0_ctrl);
        if ((FSP_SUCCESS == err) || (FSP_ERR_USB_NOT_OPEN == err))
        {
            s_dfu_usb_opened = false;
        }
    }

    /* Do not explicitly unregister the DFU class because its entry function
     * does not support UX_SLAVE_CLASS_COMMAND_UNINITIALIZE. */
    if (s_dfu_device_stack_initialized)
    {
        if (UX_SUCCESS == ux_device_stack_uninitialize())
        {
            s_dfu_device_stack_initialized = false;
        }
    }

    usbx_cdc_dfu_cleanup();
}

/*******************************************************************************************************************//**
 * @brief     This function initializes the USBX peripheral DFU device.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void dfu_device_init(void)
{
    UINT status = RESET_VALUE;
    fsp_err_t err = FSP_SUCCESS;

    tx_thread_sleep(THREAD_SLEEP_50);

    /* Initialize the USBX device stack */
    status = ux_device_stack_initialize(NULL,
                                        0U,
                                        g_dfu_device_framework_full_speed,
                                        DFU_DEVICE_FRAME_LENGTH_FULL_SPEED,
                                        g_dfu_string_framework,
                                        DFU_STRING_FRAMEWORK_LENGTH,
                                        g_language_id_framework,
                                        DFU_LANGUAGE_ID_FRAME_WORK_LENGTH,
                                        &usbx_dfu_device_status_change_callback);
    if (UX_SUCCESS != status)
    {
        PRINT_ERR_STR("ux_device_stack_initialize API failed");
        ERROR_TRAP(status);
        return;
    }
    s_dfu_device_stack_initialized = true;
    PRINT_INFO_STR("USBX device stack initialized successfully");

    g_ux_dfu_peri_class_parameter.ux_slave_class_dfu_parameter_instance_activate = usbx_dfu_instance_activate;
    g_ux_dfu_peri_class_parameter.ux_slave_class_dfu_parameter_instance_deactivate = usbx_dfu_instance_deactivate;
    g_ux_dfu_peri_class_parameter.ux_slave_class_dfu_parameter_framework = g_dfu_device_framework_full_speed;
    g_ux_dfu_peri_class_parameter.ux_slave_class_dfu_parameter_framework_length = DFU_DEVICE_FRAME_LENGTH_FULL_SPEED;
    g_ux_dfu_peri_class_parameter.ux_slave_class_dfu_parameter_write = usbx_dfu_write_callback;
    g_ux_dfu_peri_class_parameter.ux_slave_class_dfu_parameter_get_status = ux_slave_class_dfu_get_status;
    g_ux_dfu_peri_class_parameter.ux_slave_class_dfu_parameter_notify = ux_slave_class_dfu_notify;

    /* Register the DFU class */
    status = ux_device_stack_class_register(_ux_system_slave_class_dfu_name, ux_device_class_dfu_entry, CONFIG_NUMB,
                                            INTERFACE_NUMB0, (void *)&g_ux_dfu_peri_class_parameter);

    if (UX_SUCCESS != status)
    {
        PRINT_ERR_STR("ux_device_stack_class_register API failed for DFU");
        ERROR_TRAP(status);
        return;
    }
    PRINT_INFO_STR("USBX DFU class registered successfully");

    /* Initialize the DFU USB module */
    err = usb_init(&g_basic0_ctrl, &g_basic0_cfg);
    if (FSP_SUCCESS != err)
    {
        PRINT_ERR_STR("usb_init for DFU USB failed");
        ERROR_TRAP(err);
        return;
    }
    s_dfu_usb_opened = true;
    PRINT_INFO_STR("USBX peripheral DFU device initialized successfully");
}

/*******************************************************************************************************************//**
 * @brief     This function activates the DFU instance.
 * @param[in] peri_dfu_device_instance  Pointer to the area that stores the instance pointer.
 * @retval    None.
 **********************************************************************************************************************/
static void usbx_dfu_instance_activate(void * peri_dfu_device_instance)
{
    /* Save the peripheral DFU device instance */
    sp_peri_cdc_dfu_device = (UX_SLAVE_CLASS_DFU *)peri_dfu_device_instance;
    tx_semaphore_put(&g_peri_cdc_dfu_device_connect_semaphore0);
    PRINT_INFO_STR("USBX peripheral DFU device activated");
    tx_thread_sleep(THREAD_SLEEP_10);
}

/*******************************************************************************************************************//**
 * @brief     This function deactivates the DFU instance.
 * @param[in] peri_dfu_device_instance  Pointer to the area that stores the instance pointer.
 * @retval    None.
 **********************************************************************************************************************/
static void usbx_dfu_instance_deactivate(void * peri_dfu_device_instance)
{
    FSP_PARAMETER_NOT_USED(peri_dfu_device_instance);

    /* Deactivate the peripheral DFU device instance */
    sp_peri_cdc_dfu_device = NULL;
    tx_semaphore_put(&g_peri_cdc_dfu_device_connect_semaphore0);
    PRINT_INFO_STR("USBX peripheral DFU device deactivated");
    tx_thread_sleep(THREAD_SLEEP_10);
}

/*******************************************************************************************************************//**
 * @brief     This function handles DFU class notifications.
 * @param[in] dfu                       Pointer to the DFU class instance (not used).
 * @param[in] notification              Notification code indicating the DFU operation phase.
 * @retval    UX_SUCCESS                Always returns success to indicate the notification was processed.
 **********************************************************************************************************************/
static UINT ux_slave_class_dfu_notify(void * dfu, ULONG notification)
{
    FSP_PARAMETER_NOT_USED(dfu);

    if (UX_SLAVE_CLASS_DFU_NOTIFICATION_BEGIN_DOWNLOAD == notification)
    {
        PRINT_INFO_STR("USBX peripheral DFU device download started");
        tx_thread_sleep(THREAD_SLEEP_10);
    }
    else if (UX_SLAVE_CLASS_DFU_NOTIFICATION_END_DOWNLOAD == notification)
    {
        PRINT_INFO_STR("USBX peripheral DFU device download completed");
        tx_thread_sleep(THREAD_SLEEP_10);
        g_download_done = true;
    }
    else if (UX_SLAVE_CLASS_DFU_NOTIFICATION_ABORT_DOWNLOAD == notification)
    {
        PRINT_INFO_STR("USBX peripheral DFU device download aborted");
        tx_thread_sleep(THREAD_SLEEP_10);
    }
    else if (UX_SLAVE_CLASS_DFU_NOTIFICATION_BEGIN_UPLOAD == notification)
    {
        PRINT_INFO_STR("USBX peripheral DFU device upload started");
        tx_thread_sleep(THREAD_SLEEP_10);
    }
    else if (UX_SLAVE_CLASS_DFU_NOTIFICATION_END_UPLOAD == notification)
    {
        g_block_no_cnt = 0U;
        PRINT_INFO_STR("USBX peripheral DFU device upload completed");
        tx_thread_sleep(THREAD_SLEEP_10);
    }
    else if (UX_SLAVE_CLASS_DFU_NOTIFICATION_ABORT_UPLOAD == notification)
    {
        PRINT_INFO_STR("USBX peripheral DFU device upload aborted");
        tx_thread_sleep(THREAD_SLEEP_10);
    }
    else
    {
        /* Do nothing */
    }

    return UX_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief      This function provides the current DFU media status.
 * @param[in]  dfu                      Pointer to the DFU class instance (not used).
 * @param[out] media_status             Pointer to a variable to receive the media status.
 * @retval     UX_SUCCESS               Always returns success to indicate the status was retrieved.
 **********************************************************************************************************************/
static UINT ux_slave_class_dfu_get_status(void * dfu, ULONG * media_status)
{
    /* Set the DFU media status */
    FSP_PARAMETER_NOT_USED(dfu);
    *media_status = UX_SLAVE_CLASS_DFU_MEDIA_STATUS_OK;
    return UX_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief      This function writes data from USB DFU to the MCU.
 * @param[in]  dfu                      Pointer to the DFU class instance (not used).
 * @param[in]  block_number             Block number being written by the host.
 * @param[in]  data_pointer             Pointer to the data buffer containing received bytes.
 * @param[in]  length                   Number of bytes to write.
 * @param[out] media_status             Pointer to a variable to store the media status result.
 * @retval     UX_SUCCESS               Always returns success to indicate the write operation was handled.
 **********************************************************************************************************************/
static UINT usbx_dfu_write_callback(void * dfu, ULONG block_number, UCHAR * data_pointer,
                                    ULONG length, ULONG * media_status)
{
    FSP_PARAMETER_NOT_USED(dfu);
    char buffer[TERM_BUFFER_SIZE] = {RESET_VALUE};

    for (ULONG i = 0; i < length; i++)
    {
        g_dfu_data[g_data_cnt++] = *(data_pointer + i);
    }

    if (g_block_no_cnt != block_number)
    {
        PRINT_ERR_STR("Block number error");
    }

    g_block_no_cnt++;
    sprintf(buffer, "Bytes written: %lu", g_data_cnt);
    PRINT_INFO_STR(buffer);

    *media_status = UX_SLAVE_CLASS_DFU_MEDIA_STATUS_OK;

    return UX_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief     This function is a callback for USBX Peripheral DFU device status changes.
 * @param[in] status                    USB status code indicating the DFU device state or event.
 * @retval    UX_SUCCESS                Returned to indicate the status change was handled successfully.
 **********************************************************************************************************************/
static UINT usbx_dfu_device_status_change_callback(ULONG status)
{
    if (UX_DEVICE_ATTACHED == status)
    {
        PRINT_INFO_STR("USBX peripheral DFU device attached");
        tx_thread_sleep(THREAD_SLEEP_10);
    }
    else if (UX_DEVICE_ADDRESSED == status)
    {
        PRINT_INFO_STR("USBX peripheral DFU device addressed");
        tx_thread_sleep(THREAD_SLEEP_10);
    }
    else if (UX_DEVICE_CONFIGURED == status)
    {
        PRINT_INFO_STR("USBX peripheral DFU device configured");
        tx_thread_sleep(THREAD_SLEEP_10);
    }
    else if (UX_DEVICE_SUSPENDED == status)
    {
        PRINT_INFO_STR("USBX peripheral DFU device suspended");
        tx_thread_sleep(THREAD_SLEEP_10);
    }
    else if (UX_DEVICE_RESUMED == status)
    {
        PRINT_INFO_STR("USBX peripheral DFU device resumed");
        tx_thread_sleep(THREAD_SLEEP_10);
    }
    else if (UX_DEVICE_FORCE_DISCONNECT == status)
    {
        PRINT_INFO_STR("USBX peripheral DFU device force disconnected");
        tx_thread_sleep(THREAD_SLEEP_10);
        if (_ux_system_slave -> ux_system_slave_device_dfu_state_machine == UX_SYSTEM_DFU_STATE_DFU_MANIFEST_WAIT_RESET)
        {
            /* Request a system reset after the DFU manifestation sequence completes */
            g_dfu_manifestation_done = true;
        }
    }
    else
    {
        /* Do nothing */
    }

    return UX_SUCCESS;
}

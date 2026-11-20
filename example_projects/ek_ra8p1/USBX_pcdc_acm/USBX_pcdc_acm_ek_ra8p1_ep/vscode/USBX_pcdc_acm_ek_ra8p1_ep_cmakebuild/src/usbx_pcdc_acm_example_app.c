/***********************************************************************************************************************
 * File Name    : usbx_pcdc_acm_example_app.c
 * Description  : Example application layer for the USBX PCDC ACM EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - usbx_pcdc_acm_thread_entry.c calls a single function: usbx_pcdc_acm_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "usbx_pcdc_acm_ep.h"
#include "usbx_pcdc_acm_example_app.h"

/* Private variables */
static uint32_t s_ux_pool_memory[MEMPOOL_SIZE / BYTE_SIZE];
static UX_SLAVE_CLASS_CDC_ACM_PARAMETER s_ux_device_class_cdc_acm0_parameter;
static UX_SLAVE_CLASS_CDC_ACM * volatile s_cdc = UX_NULL;
static ULONG s_actual_length;
static uint8_t s_buf[DATA_LEN];
static bool s_print_status = false;
static bool s_usbx_system_initialized = false;
static bool s_usbx_device_stack_initialized = false;
static bool s_cdc_class_registered = false;
static bool s_usb_opened = false;

/* Private functions */
static void usbx_pcdc_acm_operations(void);
static void ux_cdc_device0_instance_activate(void * cdc_instance);
static void ux_cdc_device0_instance_deactivate(void * cdc_instance);
static bool usb_connection_status_check(void);
static bool usbx_pcdc_transfer_interrupted(UINT status);
static UINT usbx_status_callback(ULONG status);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the USBX PCDC ACM example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void usbx_pcdc_acm_example_app_run(void)
{
    UINT status = UX_SUCCESS;
    fsp_err_t err = FSP_SUCCESS;

    /* Wait for the terminal thread to be ready before printing */
    term_thread_init_check();

    /* Print the EP banner and info */
    term_print_data(TERM_OP_MSG_BANNER, RESET_VALUE, NULL);

    /* Initialize USBX system */
    status = ux_system_initialize(s_ux_pool_memory, MEMPOOL_SIZE, UX_NULL, RESET_VALUE);
    if (UX_SUCCESS != status)
    {
        PRINT_ERR_STR("ux_system_initialize API failed");
        ERROR_TRAP(status);
        return;
    }
    s_usbx_system_initialized = true;
    PRINT_INFO_STR("USBX system initialized successfully");

    /* Initialize USBX device stack */
    status = ux_device_stack_initialize(g_device_framework_hi_speed, DEVICE_FRAME_LENGTH_HIGH_SPEED,
                                        g_device_framework_full_speed, DEVICE_FRAME_LENGTH_FULL_SPEED,
                                        g_string_framework, STRING_FRAMEWORK_LENGTH,
                                        g_language_id_framework, LANGUAGE_ID_FRAME_WORK_LENGTH,
                                        &usbx_status_callback);
    if (UX_SUCCESS != status)
    {
        PRINT_ERR_STR("ux_device_stack_initialize API failed");
        ERROR_TRAP(status);
        return;
    }
    s_usbx_device_stack_initialized = true;
    PRINT_INFO_STR("USBX device stack initialized successfully");

    /* The activate command is used when the host has sent a SET_CONFIGURATION command
     * and this interface has to be mounted. Both Bulk endpoints have to be mounted
     * and the cdc_acm thread needs to be activated */
    s_ux_device_class_cdc_acm0_parameter.ux_slave_class_cdc_acm_instance_activate = ux_cdc_device0_instance_activate;

    /* The deactivate command is used when the device has been extracted.
     * The device endpoints have to be dismounted and the cdc_acm thread canceled */
    s_ux_device_class_cdc_acm0_parameter.ux_slave_class_cdc_acm_instance_deactivate =
    ux_cdc_device0_instance_deactivate;

    /* Register USBX CDC ACM class */
    status = ux_device_stack_class_register(_ux_system_slave_class_cdc_acm_name,
                                            _ux_device_class_cdc_acm_entry, CONFIG_NUMB,
                                            INTERFACE_NUMB0, (void *)&s_ux_device_class_cdc_acm0_parameter);
    if (UX_SUCCESS != status)
    {
        PRINT_ERR_STR("ux_device_stack_class_register API failed");
        ERROR_TRAP(status);
        return;
    }
    s_cdc_class_registered = true;
    PRINT_INFO_STR("USBX CDC ACM class registered successfully");

    /* Initialize peripheral mode based on the configured USB speed */
    if (USB_SPEED_FS == g_basic0_cfg.usb_speed)
    {
        /* Initialize peripheral mode for Full-Speed */
        status = usb_peri_usbx_initialize(R_USB_FS0_BASE);
        if (UX_SUCCESS != status)
        {
            PRINT_ERR_STR("usb_peri_usbx_initialize API for USB Full-Speed failed");
            ERROR_TRAP(status);
            return;
        }
    }
    else if (USB_SPEED_HS == g_basic0_cfg.usb_speed)
    {
        /* Initialize peripheral mode for High-Speed */
        status = usb_peri_usbx_initialize(R_USB_HS0_BASE);
        if (UX_SUCCESS != status)
        {
            PRINT_ERR_STR("usb_peri_usbx_initialize API for USB High-Speed failed");
            ERROR_TRAP(status);
            return;
        }
    }
    else
    {
        /* Do nothing */
    }

    PRINT_INFO_STR("USBX peripheral initialized successfully");

    /* Open USB driver */
    err = usb_init();
    if (FSP_SUCCESS != err)
    {
        PRINT_ERR_STR("usb_init failed");
        ERROR_TRAP(err);
        return;
    }
    s_usb_opened = true;
    PRINT_INFO_STR("USB driver opened successfully");
    PRINT_INFO_STR("Open another terminal for USBX PCDC ACM and press any key to verify the operation");

    /* USBX PCDC ACM operations will echo the user input on serial terminal */
    while (true)
    {
        usbx_pcdc_acm_operations();
        tx_thread_sleep(1);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function un-initializes the USB module, USBX Device stack, and USBX system.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void usbx_pcdc_acm_uninit(void)
{
    fsp_err_t err = FSP_SUCCESS;
    UINT status = UX_SUCCESS;

    if (s_usb_opened)
    {
        err = usb_uninit();
        if ((FSP_SUCCESS == err) || (FSP_ERR_USB_NOT_OPEN == err))
        {
            s_usb_opened = false;
        }
    }

    if (s_cdc_class_registered)
    {
        status = ux_device_stack_class_unregister(_ux_system_slave_class_cdc_acm_name,
                                                  _ux_device_class_cdc_acm_entry);
        if (UX_SUCCESS == status)
        {
            s_cdc_class_registered = false;
        }
    }

    if (s_usbx_device_stack_initialized)
    {
        status = ux_device_stack_uninitialize();
        if (UX_SUCCESS == status)
        {
            s_usbx_device_stack_initialized = false;
        }
    }

    if (s_usbx_system_initialized)
    {
        status = ux_system_uninitialize();
        if (UX_SUCCESS == status)
        {
            s_usbx_system_initialized = false;
        }
    }
}

/*******************************************************************************************************************//**
 * @brief     This function performs the USB write/read operation and echoes back the user input on serial terminal.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void usbx_pcdc_acm_operations(void)
{
    UINT status = UX_SUCCESS;
    uint32_t data_size = RESET_VALUE;
    UX_SLAVE_CLASS_CDC_ACM * p_cdc = UX_NULL;
    volatile UX_SLAVE_DEVICE *device;
    device = &_ux_system_slave->ux_system_slave_device;

    /* Verify the status of USB: wait until the USB device is attached and enumerated */
    if (!usb_connection_status_check())
    {
        return;
    }

    /* Wait until USB device is configured to slave */
    while (device->ux_slave_device_state != UX_DEVICE_CONFIGURED)
    {
        /* Stop waiting if the CDC instance is deactivated, e.g. the USB device is detached */
        if (UX_NULL == s_cdc)
        {
            return;
        }

        tx_thread_sleep(1);
    }

    /* Use the CDC instance which is valid at this point */
    p_cdc = s_cdc;
    if (UX_NULL == p_cdc)
    {
        return;
    }

    /* Clear the buffer */
    memset(s_buf, RESET_VALUE, sizeof(s_buf));

    /* USB reads the input data from the user from serial terminal */
    status = ux_device_class_cdc_acm_read(p_cdc, s_buf, DATA_LEN, &s_actual_length);
    if (UX_SUCCESS != status)
    {
        if (!usbx_pcdc_transfer_interrupted(status))
        {
            PRINT_ERR_STR("ux_device_class_cdc_acm_read API failed");
            ERROR_TRAP(status);
        }
        else
        {
            /* The USB device was detached */
        }

        return;
    }

    /* Update the data length from the read input */
    data_size = s_actual_length;

    /* Write back the read data on to the serial terminal */
    status = ux_device_class_cdc_acm_write(p_cdc, s_buf, data_size, &s_actual_length);
    if (UX_SUCCESS != status)
    {
        if (!usbx_pcdc_transfer_interrupted(status))
        {
            PRINT_ERR_STR("ux_device_class_cdc_acm_write API failed");
            ERROR_TRAP(status);
        }
        else
        {
            /* The USB device was detached */
        }

        return;
    }

    /* USB writes the display message on serial terminal */
    status = ux_device_class_cdc_acm_write(p_cdc, (UCHAR *)WRITE_DATA, WRITE_DATA_LEN, &s_actual_length);
    if (UX_SUCCESS != status)
    {
        if (!usbx_pcdc_transfer_interrupted(status))
        {
            PRINT_ERR_STR("ux_device_class_cdc_acm_write API failed");
            ERROR_TRAP(status);
        }
        else
        {
            /* The USB device was detached */
        }

        return;
    }
}

/*******************************************************************************************************************//**
 * @brief     This function activates the CDC instance.
 * @param[in] cdc_instance              Pointer to the CDC ACM instance.
 * @retval    None.
 **********************************************************************************************************************/
static void ux_cdc_device0_instance_activate(void * cdc_instance)
{
    /* Save the CDC instance */
    s_cdc = (UX_SLAVE_CLASS_CDC_ACM *)cdc_instance;
    tx_event_flags_set(&g_cdcacm_event_flags0, ~CDCACM_DEACTIVATE_FLAG, TX_AND);
    tx_event_flags_set(&g_cdcacm_event_flags0, CDCACM_ACTIVATE_FLAG, TX_OR);
}

/*******************************************************************************************************************//**
 * @brief     This function deactivates the CDC instance.
 * @param[in] cdc_instance              Pointer to the CDC ACM instance.
 * @retval    None.
 **********************************************************************************************************************/
static void ux_cdc_device0_instance_deactivate(void * cdc_instance)
{
    FSP_PARAMETER_NOT_USED(cdc_instance);
    s_cdc = UX_NULL;
    tx_event_flags_set(&g_cdcacm_event_flags0, ~CDCACM_ACTIVATE_FLAG, TX_AND);
    tx_event_flags_set(&g_cdcacm_event_flags0, CDCACM_DEACTIVATE_FLAG, TX_OR);
}

/*******************************************************************************************************************//**
 * @brief     This function waits for the USB device to be attached and enumerated, and notifies the user by
 *            printing the status message. It also reports the USB device removal.
 * @param[in] None.
 * @retval    true                      The USB device is attached and the CDC instance is activated.
 * @retval    false                     The USB device is removed or an error occurred.
 **********************************************************************************************************************/
static bool usb_connection_status_check(void)
{
    ULONG actual_flags = RESET_VALUE;
    UINT status = TX_SUCCESS;

    /* Wait for the USB attach event */
    if (true != s_print_status)
    {
        status = tx_event_flags_get(&g_cdcacm_event_flags0, CDCACM_FLAG, TX_OR, &actual_flags, TX_WAIT_FOREVER);
        if (TX_SUCCESS != status)
        {
            PRINT_ERR_STR("tx_event_flags_get API failed");
            ERROR_TRAP(status);
            return false;
        }

        PRINT_INFO_STR("USB device is attached");

        /* Flag is updated */
        s_print_status = true;
    }

    /* Wait for the enumeration (CDC instance activation), or the USB device removal */
    if (UX_NULL == s_cdc)
    {
        status = tx_event_flags_get(&g_cdcacm_event_flags0, CDCACM_ACTIVATE_FLAG | CDCACM_REMOVED_FLAG, TX_OR,
                                    &actual_flags, TX_WAIT_FOREVER);
        if (TX_SUCCESS != status)
        {
            PRINT_ERR_STR("tx_event_flags_get API failed");
            ERROR_TRAP(status);
            return false;
        }

        if (actual_flags & CDCACM_REMOVED_FLAG)
        {
            /* Clear the flag */
            s_print_status = false;

            PRINT_INFO_STR("USB device is removed");
            return false;
        }

        PRINT_INFO_STR("USB enumeration completed");
    }

    return true;
}

/*******************************************************************************************************************//**
 * @brief     This function checks whether a failed USB transfer is caused by the USB device being detached,
 *            which is an expected situation and not an error.
 * @param[in] status                    Return status of the failed USB transfer.
 * @retval    true                      The transfer was interrupted because the USB device was detached.
 * @retval    false                     The transfer failed for another reason.
 **********************************************************************************************************************/
static bool usbx_pcdc_transfer_interrupted(UINT status)
{
    ULONG actual_flags = RESET_VALUE;

    /* The device is no longer configured or the bus was reset */
    if ((UX_CONFIGURATION_HANDLE_UNKNOWN == status) || (UX_TRANSFER_BUS_RESET == status))
    {
        return true;
    }

    /* Give the callback time to update the state after the transfer is aborted */
    tx_thread_sleep(CDCACM_SETTLE_TIME);

    return ((UX_NULL == s_cdc) ||
            (TX_SUCCESS == tx_event_flags_get(&g_cdcacm_event_flags0, CDCACM_DEACTIVATE_FLAG | CDCACM_REMOVED_FLAG,
                                              TX_OR, &actual_flags, TX_NO_WAIT)));
}

/*******************************************************************************************************************//**
 * @brief     This function handles the USBX device status change events.
 * @param[in] status                    USBX device status event code.
 * @retval    UX_SUCCESS                Always returns success.
 **********************************************************************************************************************/
static UINT usbx_status_callback(ULONG status)
{
    switch (status)
    {
        case UX_DEVICE_ATTACHED:
        {
            tx_event_flags_set(&g_cdcacm_event_flags0, ~CDCACM_REMOVED_FLAG, TX_AND);
            tx_event_flags_set(&g_cdcacm_event_flags0, CDCACM_FLAG, TX_OR);
            break;
        }

        case UX_DEVICE_REMOVED:
        {
            tx_event_flags_set(&g_cdcacm_event_flags0, ~CDCACM_FLAG, TX_AND);
            tx_event_flags_set(&g_cdcacm_event_flags0, CDCACM_REMOVED_FLAG, TX_OR);
            break;
        }

        default:
        {
            /* Do nothing */
            break;
        }
    }

    return UX_SUCCESS;
}

/***********************************************************************************************************************
 * File Name    : usbx_phid_example_app.c
 * Description  : Example application layer for the USBX PHID EP.
 *                - All printing via printf (routed to UART via user stdio library) in the terminal thread.
 *                - usbx_phid_thread_entry.c calls a single function: usbx_phid_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "usbx_phid_ep.h"
#include "usbx_phid_example_app.h"

/* Private variables */
static UX_SLAVE_CLASS_HID_PARAMETER s_ux_device_class_hid_parameter;
static UX_SLAVE_CLASS_HID_EVENT s_hid_event;
static uint32_t s_ux_pool_memory[MEMPOOL_SIZE / sizeof(uint32_t)];
static UX_SLAVE_CLASS_HID * volatile s_hid = UX_NULL;
static UCHAR s_key = FIRST_KEY;
static bool s_print_status = false;
static bool s_write_flag = false;
static bool s_usbx_system_initialized = false;
static bool s_usbx_device_stack_initialized = false;
static bool s_hid_class_registered = false;
static bool s_usb_opened = false;

/* Private functions */
static UINT usbx_phid_operation(void);
static UINT usbx_hid_key_send(void);
static void ux_hid_instance_activate(void * instance);
static void ux_hid_instance_deactivate(void * instance);
static UINT usbx_status_callback(ULONG status);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the USBX PHID example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void usbx_phid_example_app_run(void)
{
    UINT status = UX_SUCCESS;
    fsp_err_t err = FSP_SUCCESS;
    ULONG actual_flags = RESET_VALUE;

    /* Wait for the terminal thread to be ready before printing */
    term_thread_init_check();

    /* Print the EP banner and information */
    term_print_data(TERM_OP_MSG_BANNER, RESET_VALUE, NULL);

    /* Initialize the USBX system */
    status = ux_system_initialize((CHAR *)s_ux_pool_memory, MEMPOOL_SIZE, UX_NULL, RESET_VALUE);
    if (UX_SUCCESS != status)
    {
        PRINT_ERR_STR("ux_system_initialize API failed");
        ERROR_TRAP(status);
        return;
    }
    s_usbx_system_initialized = true;
    PRINT_INFO_STR("USBX system initialized successfully");

    /* Initialize the USBX device stack */
    status = ux_device_stack_initialize(UX_NULL, RESET_VALUE, g_device_framework_full_speed,
                                        DEVICE_FRAME_LENGTH_SPEED, g_string_framework, STRING_FRAMEWORK_LENGTH,
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

    /* Set the callback invoked when the HID instance is activated */
    s_ux_device_class_hid_parameter.ux_slave_class_hid_instance_activate = ux_hid_instance_activate;

    /* Set the callback invoked when the HID instance is deactivated */
    s_ux_device_class_hid_parameter.ux_slave_class_hid_instance_deactivate = ux_hid_instance_deactivate;

    /* Add the HID report address and descriptor length */
    s_ux_device_class_hid_parameter.ux_device_class_hid_parameter_report_address = g_apl_report;
    s_ux_device_class_hid_parameter.ux_device_class_hid_parameter_report_length = REPORT_DESCRIPTOR_LENGTH;

    /* Register the USBX HID class */
    status = ux_device_stack_class_register(_ux_system_slave_class_hid_name, _ux_device_class_hid_entry, CONFIG_NUMB,
                                            INTERFACE_NUMB0, (void *)&s_ux_device_class_hid_parameter);
    if (UX_SUCCESS != status)
    {
        PRINT_ERR_STR("ux_device_stack_class_register API failed");
        ERROR_TRAP(status);
        return;
    }
    s_hid_class_registered = true;
    PRINT_INFO_STR("USBX HID class registered successfully");

    /* Open the USB driver */
    err = usb_init();
    if (FSP_SUCCESS != err)
    {
        PRINT_ERR_STR("usb_init failed");
        ERROR_TRAP(err);
        return;
    }
    s_usb_opened = true;
    PRINT_INFO_STR("USB driver opened successfully");

    /* Wait for the enumeration event */
    status = tx_event_flags_get(&g_hid_event_flags0, HID_ACTIVATE_FLAG, TX_OR, &actual_flags, TX_WAIT_FOREVER);
    if (TX_SUCCESS != status)
    {
        PRINT_ERR_STR("tx_event_flags_get API failed");
        ERROR_TRAP(status);
        return;
    }

    if (actual_flags & HID_ACTIVATE_FLAG)
    {
        PRINT_INFO_STR("USB enumeration completed");
    }
    else
    {
        PRINT_INFO_STR("USB instance has been deactivated");
    }

    while (true)
    {
        /* Perform the PHID operation */
        status = usbx_phid_operation();
        if (UX_SUCCESS != status)
        {
            return;
        }

        tx_thread_sleep(1);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function un-initializes the USB module, USBX HID class, device stack, and system.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void usbx_phid_uninit(void)
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

    if (s_hid_class_registered)
    {
        status = ux_device_stack_class_unregister(_ux_system_slave_class_hid_name, _ux_device_class_hid_entry);
        if (UX_SUCCESS == status)
        {
            s_hid_class_registered = false;
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
 * @brief     This function performs the PHID operation and sends the keys to the host on user input.
 * @param[in] None.
 * @retval    UX_SUCCESS                Operated successfully.
 * @retval    Any other error codes     Failed to operate.
 **********************************************************************************************************************/
static UINT usbx_phid_operation(void)
{
    UINT status = UX_SUCCESS;
    UINT tx_status = TX_SUCCESS;
    ULONG actual_flags = RESET_VALUE;

    /* Wait for the USB attach event, or the USB device removal */
    status = tx_event_flags_get(&g_hid_event_flags0, HID_FLAG | HID_REMOVED_FLAG, TX_OR, &actual_flags,
                                TX_WAIT_FOREVER);
    if (TX_SUCCESS != status)
    {
        PRINT_ERR_STR("tx_event_flags_get API failed");
        ERROR_TRAP(status);
        return status;
    }

    if (actual_flags & HID_REMOVED_FLAG)
    {
        /* Clear the flag */
        s_print_status = false;

        PRINT_INFO_STR("USB device is removed. Connect to host PC before operating keyboard.");

        /* Wait for the USB device to be attached again */
        status = tx_event_flags_get(&g_hid_event_flags0, HID_FLAG, TX_OR, &actual_flags, TX_WAIT_FOREVER);
        if (TX_SUCCESS != status)
        {
            PRINT_ERR_STR("tx_event_flags_get API failed");
            ERROR_TRAP(status);
            return status;
        }
    }

    if (true != s_print_status)
    {
        PRINT_INFO_STR("USB device is attached");
        s_print_status = true;
    }

    /* Check whether the user input has been given */
    if (false == s_write_flag)
    {
        /* Guide the user to provide the input */
        term_print_data(TERM_OP_MSG_GUIDE, RESET_VALUE, NULL);

        /* Wait for the user input from the terminal, or the USB device removal */
        while (!uart_console_has_char())
        {
            tx_status = tx_event_flags_get(&g_hid_event_flags0, HID_REMOVED_FLAG, TX_OR, &actual_flags, TX_NO_WAIT);
            if (TX_SUCCESS == tx_status)
            {
                /* The USB device is removed, so the next call reports it */
                return UX_SUCCESS;
            }
            else if (TX_NO_EVENTS != tx_status)
            {
                PRINT_ERR_STR("tx_event_flags_get API failed");
                ERROR_TRAP(tx_status);
                return tx_status;
            }
            else
            {
                /* Do nothing */
            }

            tx_thread_sleep(1);
        }

        /* Notify the user to open Notepad, then wait to allow the user to open it */
        term_print_data(TERM_OP_MSG_NOTE, RESET_VALUE, NULL);
        tx_thread_sleep(DELAY_3000_MS);

        /* The entered keys only trigger the operation, so their values are discarded */
        while (uart_console_has_char())
        {
            getchar();
        }

        /* Set the flag to start sending the keys */
        s_write_flag = true;
    }
    else
    {
        status = usbx_hid_key_send();
    }

    return status;
}

/*******************************************************************************************************************//**
 * @brief     This function sends the current key to the host and moves to the next key.
 * @param[in] None.
 * @retval    UX_SUCCESS                Sent successfully.
 * @retval    Any other error codes     Failed to send.
 **********************************************************************************************************************/
static UINT usbx_hid_key_send(void)
{
    UINT status = UX_SUCCESS;

    /* Set the HID keyboard event length */
    s_hid_event.ux_device_class_hid_event_length = EVENT_LENGTH;

    /* The first byte is a modifier byte */
    s_hid_event.ux_device_class_hid_event_buffer[MODIFIER_BYTE_INDEX] = RESET_VALUE;

    /* The second byte is reserved */
    s_hid_event.ux_device_class_hid_event_buffer[RESERVED_BYTE_INDEX] = RESET_VALUE;

    /* The next 6 bytes are keycodes. Only one key is used here. */
    s_hid_event.ux_device_class_hid_event_buffer[KEYCODE_BYTE_INDEX] = s_key;

    /* Set the keyboard event */
    status = ux_device_class_hid_event_set(s_hid, &s_hid_event);
    if (UX_SUCCESS != status)
    {
        PRINT_ERR_STR("ux_device_class_hid_event_set API failed");
        ERROR_TRAP(status);
        return status;
    }

    /* The next event has the key released */
    s_hid_event.ux_device_class_hid_event_buffer[KEYCODE_BYTE_INDEX] = RESET_VALUE;
    s_hid_event.ux_device_class_hid_event_length = EVENT_LENGTH;

    /* Set the keyboard event */
    status = ux_device_class_hid_event_set(s_hid, &s_hid_event);
    if (UX_SUCCESS != status)
    {
        PRINT_ERR_STR("ux_device_class_hid_event_set API failed");
        ERROR_TRAP(status);
        return status;
    }

    tx_thread_sleep(DELAY_100_MS);

    /* Check for the end of the key list */
    if (s_key != (FIRST_KEY + END_KEY))
    {
        /* Next key */
        s_key++;
    }
    else
    {
        /* Start over again */
        s_key = FIRST_KEY;

        /* Clear the flag */
        s_write_flag = false;
    }

    return status;
}

/*******************************************************************************************************************//**
 * @brief     This function activates the HID instance.
 * @param[in] instance                  Pointer to the HID instance.
 * @retval    None.
 **********************************************************************************************************************/
static void ux_hid_instance_activate(void * instance)
{
    /* Save the HID instance */
    s_hid = (UX_SLAVE_CLASS_HID *)instance;

    /* Set event flag to notify that the HID instance has been activated */
    tx_event_flags_set(&g_hid_event_flags0, HID_ACTIVATE_FLAG, TX_OR);
}

/*******************************************************************************************************************//**
 * @brief     This function deactivates the HID instance.
 * @param[in] instance                  Pointer to the HID instance.
 * @retval    None.
 **********************************************************************************************************************/
static void ux_hid_instance_deactivate(void * instance)
{
    FSP_PARAMETER_NOT_USED(instance);

    /* Clear the HID instance */
    s_hid = UX_NULL;

    /* Set event flag to notify that the HID instance has been deactivated */
    tx_event_flags_set(&g_hid_event_flags0, HID_DEACTIVATE_FLAG, TX_OR);
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
            tx_event_flags_set(&g_hid_event_flags0, ~HID_REMOVED_FLAG, TX_AND);
            tx_event_flags_set(&g_hid_event_flags0, HID_FLAG, TX_OR);
            break;
        }

        case UX_DEVICE_REMOVED:
        {
            tx_event_flags_set(&g_hid_event_flags0, ~HID_FLAG, TX_AND);
            tx_event_flags_set(&g_hid_event_flags0, HID_REMOVED_FLAG, TX_OR);
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

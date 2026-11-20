/***********************************************************************************************************************
 * File Name    : usbx_host_hub_example_app.c
 * Description  : Example application layer for the USBX Host Hub EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - usbx_host_hid_key_board_hub_thread_entry.c calls a single function:
 *                  usbx_host_hid_key_board_hub_example_app_run().
 *                - usbx_host_hid_mouse_hub_thread_entry.c calls a single function:
 *                  usbx_host_hid_mouse_hub_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdint.h>
#include "ux_api.h"
#include "ux_system.h"
#include "ux_host_class_hub.h"
#include "usbx_host_hub_ep.h"
#include "usbx_host_hub_example_app.h"

/* Private variables */
static UX_HOST_CLASS_HID * sp_hid_class_instance[UX_HOST_CLASS_HID_MAX_CLIENTS];
static uint32_t s_ux_pool_memory[MEMPOOL_SIZE / BYTE_SIZE];
static bool s_usbx_system_initialized = false;
static bool s_usbx_host_stack_initialized = false;
static bool s_usb_opened = false;

/* Private functions */
static void usbx_host_hub_hid_class_initialize(void);
static UINT ux_host_hid_hub_class_system_change_function(ULONG event, UX_HOST_CLASS * host_class, void * instance);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the HID Keyboard HUB Thread.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void usbx_host_hid_key_board_hub_example_app_run(void)
{
    ULONG keyboard_key[BUFF_SIZE] = {RESET_VALUE};
    ULONG keyboard_state = RESET_VALUE;
    ULONG actual_value = RESET_VALUE;
    UINT host_cl_count = RESET_VALUE;
    UINT tx_state = RESET_VALUE;
    UINT status = RESET_VALUE;

    /* Wait for the terminal thread to be ready before printing */
    term_thread_init_check();

    /* Print the EP banner and info */
    term_print_data(TERM_OP_MSG_BANNER, RESET_VALUE, NULL);

    /* Enable access to the PFS registers */
    R_BSP_PinAccessEnable();

    /* Initialize the USBX system */
    status = ux_system_initialize(s_ux_pool_memory, MEMPOOL_SIZE, UX_NULL, 0);
    if (UX_SUCCESS != status)
    {
        PRINT_ERR_STR("ux_system_initialize API failed");
        ERROR_TRAP(status);
        return;
    }
    s_usbx_system_initialized = true;
    PRINT_INFO_STR("USBX system initialized successfully");

    /* Initialize the USBX Host Hub HID class */
    usbx_host_hub_hid_class_initialize();

    /* Wait until HUB device instance activated */
    tx_semaphore_get(&g_hub_device_connect, TX_WAIT_FOREVER);

    /* Get the information and check the HID mouse function thread state and resume the thread */
    status = tx_thread_info_get(&usbx_host_hid_mouse_hub_thread, TX_NULL, &tx_state,
                                TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL);
    if (TX_SUCCESS != status)
    {
        PRINT_ERR_STR("tx_thread_info_get failed");
        ERROR_TRAP(status);
        return;
    }

    /* Check the HID mouse function thread state */
    if (TX_SUSPENDED == tx_state)
    {
        /* Resume the thread */
        status = tx_thread_resume(&usbx_host_hid_mouse_hub_thread);
        if (TX_SUCCESS != status)
        {
            PRINT_ERR_STR("tx_thread_resume failed");
            ERROR_TRAP(status);
            return;
        }
    }

    while (true)
    {
        /* Check HID keyboard device connection status */
        status = tx_event_flags_get(&g_key_board_connect_event_flags, KEYBOARD_DEVICE_CONNECT_FLAG,
                                    TX_OR, &actual_value, TX_WAIT_FOREVER);
        if (TX_SUCCESS != status)
        {
            PRINT_ERR_STR("tx_event_flags_get failed for keyboard");
            ERROR_TRAP(status);
            return;
        }

        for (host_cl_count = RESET_VALUE; host_cl_count < UX_HOST_CLASS_HID_MAX_CLIENTS; host_cl_count++)
        {
            /* Check for the valid HID instance */
            if (NULL != sp_hid_class_instance[host_cl_count])
            {
                UX_HOST_CLASS_HID_CLIENT * p_hid_client =
                sp_hid_class_instance[host_cl_count]->ux_host_class_hid_client;

                /* Check for HID class keyboard */
                if (UX_SUCCESS == _ux_utility_memory_compare(p_hid_client->ux_host_class_hid_client_name,
                                                             _ux_system_host_class_hid_client_keyboard_name,
                                                             _ux_utility_string_length_get
                                                             (_ux_system_host_class_hid_client_keyboard_name)))
                {
                    /* Get the key and keyboard state */
                    status = ux_host_class_hid_keyboard_key_get(
                             (UX_HOST_CLASS_HID_KEYBOARD *)p_hid_client->ux_host_class_hid_client_local_instance,
                             (ULONG *)keyboard_key, &keyboard_state);
                    if ((UX_SUCCESS == status) || (UX_NO_KEY_PRESS == status))
                    {
                        /* Check if a key event is reported */
                        if (RESET_VALUE != keyboard_state)
                        {
                            if (RESET_VALUE != keyboard_key[BUFF_INDEX])
                            {
                                if ((KEY_HIGH_VAL >= keyboard_key[BUFF_INDEX]) &&
                                    (KEY_LOW_VAL <= keyboard_key[BUFF_INDEX]))
                                {
                                    char key_pressed[2] =
                                    {
                                        (char) keyboard_key[BUFF_INDEX],
                                        NULL_CHAR
                                    };

                                    term_print_data(TERM_OP_MSG_PRINT_KEY_PRESS,
                                                    sizeof(key_pressed), key_pressed);
                                }
                                else
                                {
                                    PRINT_ERR_STR("Pressed key is out of specified key range");
                                }
                            }
                        }

                        tx_thread_sleep(SLEEP_TIME_1);
                        keyboard_key[BUFF_INDEX] = RESET_VALUE;
                        keyboard_state = RESET_VALUE;
                    }
                    else
                    {
                        /* The instance may have just been removed, yield the CPU to the USBX threads */
                        tx_thread_sleep(SLEEP_TIME_1);
                    }
                }
                else
                {
                    tx_thread_sleep(SLEEP_TIME_5);
                }
            }
            else
            {
                tx_thread_sleep(SLEEP_TIME_10);
            }
        }
    }
}

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the HID Mouse HUB Thread.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void usbx_host_hid_mouse_hub_example_app_run(void)
{
    UINT status = TX_SUCCESS;
    ULONG mouse_buttons = RESET_VALUE;
    SLONG mouse_x_position = RESET_VALUE;
    SLONG mouse_y_position = RESET_VALUE;
    SLONG mouse_x_position_old = RESET_VALUE;
    SLONG mouse_y_position_old = RESET_VALUE;
    ULONG actual_value = RESET_VALUE;

    while (true)
    {
        /* Check HID mouse device connection status */
        status = tx_event_flags_get(&g_hid_mouse_connect_event_flags, MOUSE_DEVICE_CONNECT_FLAG,
                                    TX_OR, &actual_value, TX_WAIT_FOREVER);
        if (TX_SUCCESS != status)
        {
            PRINT_ERR_STR("tx_event_flags_get failed for mouse");
            ERROR_TRAP(status);
            return;
        }

        for (UINT cl_count = RESET_VALUE; cl_count < UX_HOST_CLASS_HID_MAX_CLIENTS; cl_count++)
        {
            if (NULL != sp_hid_class_instance[cl_count])
            {
                UX_HOST_CLASS_HID_CLIENT * hid_client =
                sp_hid_class_instance[cl_count]->ux_host_class_hid_client;

                if (UX_SUCCESS == _ux_utility_memory_compare(hid_client->ux_host_class_hid_client_name,
                                                             _ux_system_host_class_hid_client_mouse_name,
                                                             _ux_utility_string_length_get
                                                             (_ux_system_host_class_hid_client_mouse_name)))
                {
                    /* Get a mouse state from the mouse */
                    status = ux_host_class_hid_mouse_buttons_get((UX_HOST_CLASS_HID_MOUSE *)hid_client
                                                                 ->ux_host_class_hid_client_local_instance,
                                                                 &mouse_buttons);
                    if (UX_HOST_CLASS_INSTANCE_UNKNOWN == status)
                    {
                        /* The mouse was just removed; skip this round and wait for the next connection */
                        break;
                    }
                    else if (UX_SUCCESS != status)
                    {
                        PRINT_ERR_STR("ux_host_class_hid_mouse_buttons_get API failed");
                        ERROR_TRAP(status);
                        return;
                    }
                    else
                    {
                        /* Do nothing */
                    }

                    /* Check if the left button pressed */
                    if (mouse_buttons & UX_HOST_CLASS_HID_MOUSE_BUTTON_1_PRESSED)
                    {
                        term_print_data(TERM_OP_MSG_PRINT_LEFT_BUTTON_PRESS, RESET_VALUE, NULL);
                        tx_thread_sleep(SLEEP_TIME_100);
                    }

                    /* Check if the right button pressed */
                    if (mouse_buttons & UX_HOST_CLASS_HID_MOUSE_BUTTON_2_PRESSED)
                    {
                        term_print_data(TERM_OP_MSG_PRINT_RIGHT_BUTTON_PRESS, RESET_VALUE, NULL);
                        tx_thread_sleep(SLEEP_TIME_100);
                    }

                    /* Check if the position moved */
                    status = ux_host_class_hid_mouse_position_get(
                             (UX_HOST_CLASS_HID_MOUSE *)hid_client->ux_host_class_hid_client_local_instance,
                             &mouse_x_position, &mouse_y_position);
                    if (UX_HOST_CLASS_INSTANCE_UNKNOWN == status)
                    {
                        /* The mouse was just removed; skip this round and wait for the next connection */
                        break;
                    }
                    else if (UX_SUCCESS != status)
                    {
                        PRINT_ERR_STR("ux_host_class_hid_mouse_position_get API failed");
                        ERROR_TRAP(status);
                        return;
                    }
                    else
                    {
                        /* Do nothing */
                    }

                    if ((mouse_x_position_old != mouse_x_position) || (mouse_y_position_old != mouse_y_position))
                    {
                        term_mouse_position_t mouse_position =
                        {
                            .x = (int32_t)mouse_x_position,
                            .y = (int32_t)mouse_y_position
                        };

                        term_print_data(TERM_OP_MSG_PRINT_MOUSE_POSITION,
                                        sizeof(mouse_position), &mouse_position);
                        tx_thread_sleep(SLEEP_TIME_10);
                        mouse_x_position_old = mouse_x_position;
                        mouse_y_position_old = mouse_y_position;
                    }
                    else
                    {
                        /* Yield CPU so that lower priority threads (terminal, USBX enum) can run */
                        tx_thread_sleep(SLEEP_TIME_1);
                    }
                }
                else
                {
                    tx_thread_sleep(SLEEP_TIME_1);
                }
            }
            else
            {
                tx_thread_sleep(SLEEP_TIME_10);
            }
        }
    }
}

/*******************************************************************************************************************//**
 * @brief     This function notifies changes happened to the USBX Host class HID.
 * @param[in] event                     Event code from happened to the USBX Host system.
 * @param[in] host_class                Pointer to the UX_HOST_CLASS structure.
 * @param[in] instance                  Pointer to a USBX Host class instance for which the event occurred.
 * @retval    UX_SUCCESS                Operated successfully.
 * @retval    Any other error codes     Failed to operate.
 **********************************************************************************************************************/
static UINT ux_host_hid_hub_class_system_change_function(ULONG event, UX_HOST_CLASS * host_class, void * instance)
{
    FSP_PARAMETER_NOT_USED(host_class);
    UINT icount = RESET_VALUE;
    UX_HOST_CLASS_HID_CLIENT * lp_temp_hid_client;
    UX_HOST_CLASS_HID * lp_hid_class_instance;

    /* Callback user function for the USBX Host class HID */
    /* Check if there is a device insertion */
    if (UX_DEVICE_INSERTION == event)
    {
        while ((icount < UX_HOST_CLASS_HID_MAX_CLIENTS) && (NULL != sp_hid_class_instance[icount]))
        {
            icount++;
        }

        if (UX_HOST_CLASS_HID_MAX_CLIENTS == icount)
        {
            PRINT_ERR_STR("No available slot for HID class instance");
            return UX_MEMORY_INSUFFICIENT;
        }

        /* Get a pointer to a USB Host HID class instance */
        sp_hid_class_instance[icount] = (UX_HOST_CLASS_HID *)instance;
        lp_temp_hid_client = sp_hid_class_instance[icount]->ux_host_class_hid_client;

        /* Check for the HUB class */
        if ((UX_HOST_CLASS_HUB_CLASS == sp_hid_class_instance[icount]
             ->ux_host_class_hid_device->ux_device_descriptor.bDeviceClass))
        {
            PRINT_INFO_STR("Hub device is attached");

            /* HUB device instance activated */
            tx_semaphore_put(&g_hub_device_connect);
        }

        /* Check for the HID class keyboard */
        else if (UX_SUCCESS == _ux_utility_memory_compare(lp_temp_hid_client->ux_host_class_hid_client_name,
                                                          _ux_system_host_class_hid_client_keyboard_name,
                                                          _ux_utility_string_length_get
                                                          (_ux_system_host_class_hid_client_keyboard_name)))
        {
            PRINT_INFO_STR("Keyboard device is attached");

            /* Set keyboard connection event */
            tx_event_flags_set(&g_key_board_connect_event_flags, KEYBOARD_DEVICE_CONNECT_FLAG, TX_OR);
        }

        /* Check for the HID class mouse */
        else if (UX_SUCCESS == _ux_utility_memory_compare(lp_temp_hid_client->ux_host_class_hid_client_name,
                                                          _ux_system_host_class_hid_client_mouse_name,
                                                          _ux_utility_string_length_get
                                                          (_ux_system_host_class_hid_client_mouse_name)))
        {
            PRINT_INFO_STR("Mouse device is attached");

            /* Set mouse device connection event */
            tx_event_flags_set(&g_hid_mouse_connect_event_flags, MOUSE_DEVICE_CONNECT_FLAG, TX_OR);
        }
        else
        {
            /* Other HID interface of a composite device, not handled by this EP */
        }

        tx_thread_sleep(SLEEP_TIME_1);
    }

    /* Check if there is a device removal */
    else if (UX_DEVICE_REMOVAL == event)
    {
        while ((icount < UX_HOST_CLASS_HID_MAX_CLIENTS) && (sp_hid_class_instance[icount] != instance))
        {
            icount++;
        }

        if (UX_HOST_CLASS_HID_MAX_CLIENTS == icount)
        {
            PRINT_ERR_STR("Removal: instance not found in table");
            return UX_SUCCESS;
        }

        /* Get a pointer to a USB Host HID class instance */
        lp_hid_class_instance = (UX_HOST_CLASS_HID *)instance;
        lp_temp_hid_client = lp_hid_class_instance->ux_host_class_hid_client;

        /* Release the table slot first so the application threads stop using the removed instance */
        sp_hid_class_instance[icount] = NULL;

        /* Check if the HUB device is removed */
        if ((UX_HOST_CLASS_HUB_CLASS ==
             lp_hid_class_instance->ux_host_class_hid_device->ux_device_descriptor.bDeviceClass))
        {
            PRINT_INFO_STR("Hub device is detached");
        }

        /* Check if the HID class keyboard is removed */
        else if (UX_SUCCESS == _ux_utility_memory_compare(lp_temp_hid_client->ux_host_class_hid_client_name,
                                                          _ux_system_host_class_hid_client_keyboard_name,
                                                          _ux_utility_string_length_get
                                                          (_ux_system_host_class_hid_client_keyboard_name)))
        {
            PRINT_INFO_STR("Keyboard device is detached");

            /* Set keyboard removed event */
            tx_event_flags_set(&g_key_board_connect_event_flags, KEYBOARD_DEVICE_REMOVED_FLAG, TX_AND);
        }

        /* Check if the HID class mouse is removed */
        else if (UX_SUCCESS == _ux_utility_memory_compare(lp_temp_hid_client->ux_host_class_hid_client_name,
                                                          _ux_system_host_class_hid_client_mouse_name,
                                                          _ux_utility_string_length_get
                                                          (_ux_system_host_class_hid_client_mouse_name)))
        {
            PRINT_INFO_STR("Mouse device is detached");

            /* Set mouse device removed event */
            tx_event_flags_set(&g_hid_mouse_connect_event_flags, MOUSE_DEVICE_REMOVED_FLAG, TX_AND);
        }
        else
        {
            /* Other HID interface of a composite device, not handled by this EP */
        }

        tx_thread_sleep(SLEEP_TIME_1);
    }
    else
    {
        /* Do nothing */
    }

    return UX_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief     This function initializes the USBX Host stack and USB module.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void usbx_host_hub_hid_class_initialize(void)
{
    UINT status = UX_SUCCESS;
    fsp_err_t err = FSP_SUCCESS;

    /* Host stack initialization */
    status = ux_host_stack_initialize(ux_host_hid_hub_class_system_change_function);
    if (UX_SUCCESS != status)
    {
        PRINT_ERR_STR("ux_host_stack_initialize API failed");
        ERROR_TRAP(status);
        return;
    }
    s_usbx_host_stack_initialized = true;
    PRINT_INFO_STR("USBX host stack initialized successfully");
    tx_thread_sleep(SLEEP_TIME_100);

    /* Start the USB module by calling open function */
    err = usb_init();
    if (FSP_SUCCESS != err)
    {
        PRINT_ERR_STR("usb_init failed");
        ERROR_TRAP(err);
        return;
    }
    s_usb_opened = true;
    PRINT_INFO_STR("USBX host HUB HID device initialized successfully");
}

/*******************************************************************************************************************//**
 * @brief     This function un-initializes the USB module, USBX Host stack, and USBX system.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void usbx_host_hub_hid_class_uninit(void)
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

    if (s_usbx_host_stack_initialized)
    {
        status = ux_host_stack_uninitialize();
        if (UX_SUCCESS == status)
        {
            s_usbx_host_stack_initialized = false;
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

/***********************************************************************************************************************
 * File Name    : hhid_thread_entry.c
 * Description  : Contains data structures and functions used in hhid_thread_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "usbx_hhid_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup USBX_hhid_ep
 * @{
 **********************************************************************************************************************/

/* Global variables */
static uint8_t    g_ux_pool_memory[MEMPOOL_SIZE];
static uint8_t    keyboard_key[BUFF_SIZE] = {RESET_VALUE};
ULONG             g_mouse_buttons         = RESET_VALUE;
ULONG             g_keyboard_state        = RESET_VALUE;
SLONG             g_mouse_x_position      = RESET_VALUE;
SLONG             g_mouse_y_position      = RESET_VALUE;
static ULONG      actual_flags            = RESET_VALUE;
mouse_pos_t       g_mouse_axis            = {0x00};
UX_HOST_CLASS_HID * gp_hid_class_instance;

/* Private functions */
static void deinit_usb_device(void);
static void deinit_host_stack(void);

/*******************************************************************************************************************//**
 * @brief     This function checks the USB device status and notifies to perform operation.
 * @param[in] event        Updates USB events on attaching and detaching device.
 * @param[in] *host_class  Pointer to host class device structure.
 * @param[in] instance.
 * @retval    None.
 **********************************************************************************************************************/
static UINT apl_host_hid_change_function (ULONG event, UX_HOST_CLASS * host_class, VOID * instance)
{
    FSP_PARAMETER_NOT_USED(host_class);

    if (UX_DEVICE_INSERTION == event)
    {
        /* Get a pointer to a USB Host HID Class instance */
        gp_hid_class_instance = (UX_HOST_CLASS_HID *)instance;
        /* Set the event flag as USB Host device is plugged in */
        tx_event_flags_set(&g_usb_plug_events, EVENT_USB_PLUG_IN, TX_OR);
    }
    else if (UX_DEVICE_REMOVAL == event)
    {
        /* Update USB Host HID Class instance to NULL */
        gp_hid_class_instance = NULL;
        /* Set the event flag as USB Host device is plugged out */
        tx_event_flags_set(&g_usb_plug_events, EVENT_USB_PLUG_OUT, TX_OR);
    }
    else
    {
        /* Do nothing */
    }
    return UX_SUCCESS;
}

/* HHID_Thread entry function */
void hhid_thread_entry(void)
{
    UINT      ux_return = UX_SUCCESS;
    fsp_err_t err       = FSP_SUCCESS;
    UINT      error     = RESET_VALUE;
    UINT      status    = TX_SUCCESS;

    /* Initialize the RTT Thread */
    rtt_thread_init_check();

    /* Print the banner and EP info */
    app_rtt_print_data(RTT_OUTPUT_MESSAGE_BANNER, RESET_VALUE, NULL);

    /* ux_system initialization */
    ux_return = ux_system_initialize((CHAR *)g_ux_pool_memory, MEMPOOL_SIZE, UX_NULL, RESET_VALUE);
    if (UX_SUCCESS != ux_return)
    {
        PRINT_ERR_STR("ux_system_initialize API failed");
        ERROR_TRAP(ux_return);
    }

    PRINT_INFO_STR("ux_system initialized successfully");

    /* ux_host_stack initialization */
    ux_return = ux_host_stack_initialize(apl_host_hid_change_function);
    if (UX_SUCCESS != ux_return)
    {
        PRINT_ERR_STR("ux_host_stack_initialize API failed");
        ERROR_TRAP(ux_return);
    }
    PRINT_INFO_STR("ux_host_stack initialized successfully");

    /* Open USB driver */
    err = R_USB_Open(&g_basic0_ctrl, &g_basic0_cfg);
    if (FSP_SUCCESS != err)
    {
        PRINT_ERR_STR("R_USB_Open API failed");
        deinit_host_stack();
        ERROR_TRAP(err);
    }
    PRINT_INFO_STR("Initialize completed");

    PRINT_INFO_STR("Connect mouse or keyboard to the USB Host connector");

    /* Wait until the device is inserted */
    status = tx_event_flags_get (&g_usb_plug_events, EVENT_USB_PLUG_IN, TX_AND_CLEAR, &actual_flags, TX_WAIT_FOREVER);
    if ((TX_SUCCESS == status) && (EVENT_USB_PLUG_IN == actual_flags))
    {
        PRINT_INFO_STR("USB device is inserted");
    }

    actual_flags = RESET_VALUE;
    while (true)
    {
        /* Check if USB is plugged out */
        status = tx_event_flags_get (&g_usb_plug_events, EVENT_USB_PLUG_OUT, TX_AND_CLEAR, &actual_flags, TX_NO_WAIT);
        if ((TX_SUCCESS == status) && (EVENT_USB_PLUG_OUT == actual_flags))
        {
            PRINT_INFO_STR("Connect mouse or keyboard to the USB Host connector");
            actual_flags = RESET_VALUE;
            /*  Wait until the device is inserted */
           status = tx_event_flags_get(&g_usb_plug_events, EVENT_USB_PLUG_IN,\
                                       TX_AND_CLEAR, &actual_flags, TX_WAIT_FOREVER);
            if ((TX_SUCCESS == status) && (EVENT_USB_PLUG_IN == actual_flags))
            {
                PRINT_INFO_STR("USB device is inserted");
            }
        }
        /* Below section of code will execute when USB Host Device is inserted */
        else
        {
            if (NULL != gp_hid_class_instance)
            {
                UX_HOST_CLASS_HID_CLIENT * hid_client = gp_hid_class_instance->ux_host_class_hid_client;

                if (UX_SUCCESS == ux_utility_memory_compare(hid_client->ux_host_class_hid_client_name,\
                    _ux_system_host_class_hid_client_keyboard_name,\
                    _ux_utility_string_length_get(_ux_system_host_class_hid_client_keyboard_name)))
                {
                    /* Get the keyboard key pressed */
                    error = ux_host_class_hid_keyboard_key_get(\
                            (UX_HOST_CLASS_HID_KEYBOARD *)hid_client->ux_host_class_hid_client_local_instance,\
                            (ULONG *)keyboard_key, &g_keyboard_state);
                    /* Get a keyboard state from the keyboard */
                    if (UX_SUCCESS == error || UX_NO_KEY_PRESS == error)
                    {
                        /* Check if CAPS lock key is pressed */
                        if (g_keyboard_state != RESET_VALUE || keyboard_key[BUFF_INDEX] !=RESET_VALUE)
                        {
                            if (RESET_VALUE != keyboard_key[BUFF_INDEX])
                            {
                                if ((KEY_HIGH_VAL >= keyboard_key[BUFF_INDEX]) &&\
                                        (KEY_LOW_VAL <= keyboard_key[BUFF_INDEX]))
                                {
                                    app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_PRINT_KEY_PRESS,\
                                                       sizeof(keyboard_key), keyboard_key);
                                }
                                else
                                {
                                    PRINT_ERR_STR("Pressed key is out of specified key range");
                                }
                            }
                        }

                        tx_thread_sleep(TEN);
                        keyboard_key[BUFF_INDEX] = RESET_VALUE;
                        g_keyboard_state= RESET_VALUE;
                    }
                    else
                    {
                        PRINT_ERR_STR("ux_host_class_hid_keyboard_key_get API failed");
                        deinit_host_stack();
                        deinit_usb_device();
                        ERROR_TRAP(error);
                    }
                }
                else if (UX_SUCCESS == ux_utility_memory_compare (hid_client->ux_host_class_hid_client_name,\
                                       _ux_system_host_class_hid_client_mouse_name,\
                                       _ux_utility_string_length_get(_ux_system_host_class_hid_client_mouse_name)))
                {
                    /* Get a mouse state from the mouse */
                    error = ux_host_class_hid_mouse_buttons_get(\
                            (UX_HOST_CLASS_HID_MOUSE *)hid_client->ux_host_class_hid_client_local_instance,\
                            (ULONG *)&g_mouse_buttons);

                    if (UX_SUCCESS == error)
                    {
                        /* Check if the left button is pressed */
                        if (g_mouse_buttons & UX_HOST_CLASS_HID_MOUSE_BUTTON_1_PRESSED)
                        {
                            app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_PRINT_LEFT_BUTTON_PRESS,\
                                               sizeof(UINT *), &g_mouse_buttons);
                        }
                        /* Check if the right button is pressed */
                        else if (g_mouse_buttons & UX_HOST_CLASS_HID_MOUSE_BUTTON_2_PRESSED)
                        {
                            app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_PRINT_RIGHT_BUTTON_PRESS,\
                                               sizeof(UINT *), &g_mouse_buttons);
                        }
                    }
                    else
                    {
                        PRINT_ERR_STR("ux_host_class_hid_mouse_buttons_get API failed");
                        deinit_host_stack();
                        deinit_usb_device();
                        ERROR_TRAP(error);
                    }

                    /* Check if the position moved */
                    error = ux_host_class_hid_mouse_position_get (\
                            (UX_HOST_CLASS_HID_MOUSE *)hid_client->ux_host_class_hid_client_local_instance,
                            (SLONG *)&g_mouse_x_position, (SLONG *)&g_mouse_y_position);
                    if (UX_SUCCESS == error)
                    {
                        if ((g_mouse_axis.mouse_x_position_old != g_mouse_x_position) ||\
                                (g_mouse_axis.mouse_y_position_old != g_mouse_y_position))
                        {
                            g_mouse_axis.mouse_x_position_old = g_mouse_x_position;
                            g_mouse_axis.mouse_y_position_old = g_mouse_y_position;
                            app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_PRINT_MOUSE_POSITION,\
                                    sizeof(g_mouse_axis), &g_mouse_axis);
                        }
                    }
                    else
                    {
                        PRINT_ERR_STR("ux_host_class_hid_mouse_position_get API failed");
                        deinit_host_stack();
                        deinit_usb_device();
                        ERROR_TRAP(error);
                    }
                }
                else
                {
                    PRINT_INFO_STR("Connected device is unsupported, connect a keyboard or mouse");
                }
            }
            else
            {
                PRINT_ERR_STR("USB instance error");
            }
        }
      tx_thread_sleep(TEN);
    }
}

/***********************************************************************************************************************
 * @brief       This function closes the opened USB instance.
 * @param[in]   None.
 * @retval      None.
 **********************************************************************************************************************/
static void deinit_usb_device(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Close opened USB interface */
    err = R_USB_Close(&g_basic0_ctrl);
    /* Handle error in-case of failure */
    if (UX_SUCCESS != err)
    {
        PRINT_ERR_STR("R_USB_Close API failed");
    }
}

/***********************************************************************************************************************
 * @brief       This function closes the opened host stack.
 * @param[in]   None.
 * @retval      None.
 **********************************************************************************************************************/
static void deinit_host_stack(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Un-initializes all the host code for USB */
    err = ux_host_stack_uninitialize();
    /* Handle error in-case of failure */
    if (UX_SUCCESS != err)
    {
        PRINT_ERR_STR("ux_host_stack_uninitialize API failed");
    }
}

/***********************************************************************************************************************
 * @} (end addtogroup usbx_hhid_ep)
 **********************************************************************************************************************/

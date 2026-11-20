/***********************************************************************************************************************
 * File Name    : usbx_hhid_example_app.c
 * Description  : Example application layer for the USBX HHID EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - hhid_thread_entry.c calls a single function: usbx_hhid_example_app_run().
 *                - terminal_thread_entry.c calls a single function: terminal_thread_entry_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdbool.h>
#include "terminal.h"

/* Private variables */
static ULONG s_actual_flags = RESET_VALUE;
static mouse_pos_t s_mouse_axis = {0x00};

/* Private functions */
static void keyboard_operation(void);
static void mouse_operation(void);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the HHID thread of the USBX HHID example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void usbx_hhid_example_app_run(void)
{
    UINT err = UX_SUCCESS;
    UINT tx_err = TX_SUCCESS;

    /* Initialize the terminal thread */
    term_thread_init_check();

    /* Print the banner and EP info */
    term_print_data(TERM_OP_MSG_BANNER, RESET_VALUE, NULL);

    /* USBX HHID initialization */
    err = usbx_hhid_init();
    if (UX_SUCCESS != err)
    {
        PRINT_ERR_STR("\r\nusbx_hhid_init failed");
        ERROR_TRAP(err);
        return;
    }
    PRINT_INFO_STR("\r\nInitialize USBX HHID completed");

    /* Wait until the device is inserted */
    PRINT_INFO_STR("\r\nConnect mouse or keyboard to the USB Host connector");
    tx_err = usbx_hhid_wait_for_connection(EVENT_USB_PLUG_IN, &s_actual_flags, TX_WAIT_FOREVER);
    if ((TX_SUCCESS == tx_err) && (EVENT_USB_PLUG_IN == s_actual_flags))
    {
        PRINT_INFO_STR("\r\nUSB device is inserted");
    }
    s_actual_flags = RESET_VALUE;

    while (true)
    {
        /* Check if USB is plugged out */
        tx_err = usbx_hhid_wait_for_connection(EVENT_USB_PLUG_OUT, &s_actual_flags, TX_NO_WAIT);
        if ((TX_SUCCESS == tx_err) && (EVENT_USB_PLUG_OUT == s_actual_flags))
        {
            s_actual_flags = RESET_VALUE;

            /* Wait until the device is inserted */
            PRINT_INFO_STR("\r\nConnect mouse or keyboard to the USB Host connector");
            tx_err = usbx_hhid_wait_for_connection(EVENT_USB_PLUG_IN, &s_actual_flags, TX_WAIT_FOREVER);
            if ((TX_SUCCESS == tx_err) && (EVENT_USB_PLUG_IN == s_actual_flags))
            {
                PRINT_INFO_STR("\r\nUSB device is inserted");
            }
        }
        else
        {
            switch (usbx_hhid_device_get())
            {
                case KEYBOARD:
                {
                    keyboard_operation();
                    break;
                }

                case MOUSE:
                {
                    mouse_operation();
                    break;
                }

                case UNSUPPORTED:
                {
                    PRINT_ERR_STR("\r\nConnected device is unsupported");
                    break;
                }

                case NONE:
                {
                    PRINT_ERR_STR("\r\nUSB instance error");
                    break;
                }

                default:
                {
                    break;
                }
            }
        }
        tx_thread_sleep(10U);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function performs keyboard input processing for the connected USB HID keyboard.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void keyboard_operation(void)
{
    UINT err = UX_SUCCESS;
    uint8_t keyboard_key[BUFF_SIZE] = {RESET_VALUE};
    ULONG keyboard_state = RESET_VALUE;

    /* Get the keyboard key and state. */
    err = usbx_hhid_keyboard_key_get(keyboard_key, &keyboard_state);
    if ((UX_SUCCESS == err) || (UX_NO_KEY_PRESS == err))
    {
        /* Check if the keyboard state or key value is updated. */
        if ((RESET_VALUE != keyboard_state) || (RESET_VALUE != keyboard_key[0U]))
        {
            /* Check if a key is pressed. */
            if (RESET_VALUE != keyboard_key[0U])
            {
                /* Check if the pressed key is within the specified range. */
                if ((KEY_HIGH_VAL >= keyboard_key[0U]) && (KEY_LOW_VAL <= keyboard_key[0U]))
                {
                    term_print_data(TERM_OP_MSG_KEY_PRESS, sizeof(keyboard_key), keyboard_key);
                }
                else
                {
                    PRINT_ERR_STR("\r\nPressed key is out of specified key range");
                }
            }
        }
        tx_thread_sleep(10);
    }
    else
    {
        PRINT_ERR_STR("\r\nusbx_hhid_keyboard_key_get failed");
        usbx_hhid_deinit();
        ERROR_TRAP(err);
        return;
    }
}

/*******************************************************************************************************************//**
 * @brief     This function processes button and position input from the connected USB HID mouse.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void mouse_operation(void)
{
    UINT err = UX_SUCCESS;
    ULONG mouse_buttons = RESET_VALUE;
    SLONG mouse_x_position = RESET_VALUE;
    SLONG mouse_y_position = RESET_VALUE;

    /* Get the mouse button state. */
    err = usbx_hhid_mouse_buttons_get(&mouse_buttons);
    if (UX_SUCCESS != err)
    {
        PRINT_ERR_STR("\r\nusbx_hhid_mouse_buttons_get failed");
        ERROR_TRAP(err);
        return;
    }

    /* Check if the button is pressed. */
    if (mouse_buttons & UX_HOST_CLASS_HID_MOUSE_BUTTON_1_PRESSED)
    {
        term_print_data(TERM_OP_MSG_LEFT_BUTTON_PRESS, sizeof(UINT *), &mouse_buttons);
    }
    else if (mouse_buttons & UX_HOST_CLASS_HID_MOUSE_BUTTON_2_PRESSED)
    {
        term_print_data(TERM_OP_MSG_RIGHT_BUTTON_PRESS, sizeof(UINT *), &mouse_buttons);
    }
    else
    {
        /* Do nothing */
    }

    /* Get the mouse position. */
    err = usbx_hhid_mouse_position_get(&mouse_x_position, &mouse_y_position);
    if (UX_SUCCESS != err)
    {
        PRINT_ERR_STR("\r\nusbx_hhid_mouse_position_get failed");
        ERROR_TRAP(err);
        return;
    }

    /* Check if the mouse position has changed. */
    if ((s_mouse_axis.mouse_x_pos_old != mouse_x_position) || (s_mouse_axis.mouse_y_pos_old != mouse_y_position))
    {
        s_mouse_axis.mouse_x_pos_old = mouse_x_position;
        s_mouse_axis.mouse_y_pos_old = mouse_y_position;
        term_print_data(TERM_OP_MSG_MOUSE_POS, sizeof(s_mouse_axis), &s_mouse_axis);
    }
}

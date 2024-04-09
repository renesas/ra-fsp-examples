/***********************************************************************************************************************
 * File Name    : usbx_host_hid_mouse_hub_entry.c
 * Description  : Contains data structures and functions used in usbx_host_hid_mouse_hub_entry.c.
 **********************************************************************************************************************/
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
 * Copyright (C) 2022 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#include <usbx_host_hid_mouse_hub.h>
#include "ux_api.h"
#include "ux_system.h"
#include "ux_host_class_hid.h"
#include "ux_host_class_hid_mouse.h"
#include "usbx_host_hub.h"
#include "common_utils.h"

/*******************************************************************************************************************//**
 * @addtogroup usbx_host_hub_ep
 * @{
 **********************************************************************************************************************/


/******************************************************************************
 Private global variables and functions
 ******************************************************************************/
static char mouse_data_buff[BUFFER_SIZE_MIN]           = {RESET_VALUE};
extern UX_HOST_CLASS_HID * gp_hid_class_instance[UX_HOST_CLASS_HID_MAX_CLIENTS];

/* HID Mouse HUB entry function */
void usbx_host_hid_mouse_hub_entry(void)
{
    ULONG    mouse_buttons          = RESET_VALUE;
    SLONG    mouse_x_position       = RESET_VALUE;
    SLONG    mouse_y_position       = RESET_VALUE;
    SLONG    mouse_x_position_old   = RESET_VALUE;
    SLONG    mouse_y_position_old   = RESET_VALUE;
    ULONG    actual_value           = RESET_VALUE;

    while (true)
    {
        /* Check HID mouse device connection status. */
        tx_event_flags_get (&g_hid_mouse_connect_event_flags, MOUSE_DEVICE_CONNECT_FLAG, TX_OR, &actual_value, TX_WAIT_FOREVER);

        for (int cl_count = RESET_VALUE ; cl_count < UX_HOST_CLASS_HID_MAX_CLIENTS; cl_count++)
        {
            if (NULL != gp_hid_class_instance[cl_count])
            {
                UX_HOST_CLASS_HID_CLIENT * hid_client = gp_hid_class_instance[cl_count]->ux_host_class_hid_client;


                if (UX_SUCCESS == _ux_utility_memory_compare(   hid_client->ux_host_class_hid_client_name,
                                                                _ux_system_host_class_hid_client_mouse_name,
                                                                _ux_utility_string_length_get(_ux_system_host_class_hid_client_mouse_name)))
                {
                    /* Get a mouse state from the mouse.  */
                    ux_host_class_hid_mouse_buttons_get ((UX_HOST_CLASS_HID_MOUSE *)hid_client->ux_host_class_hid_client_local_instance, &mouse_buttons);

                    /* Check if the left button pressed */
                    if (mouse_buttons & UX_HOST_CLASS_HID_MOUSE_BUTTON_1_PRESSED)
                    {
                        PRINT_INFO_STR("HID Mouse left button clicked");
                        tx_thread_sleep(SLEEP_TIME_100);
                    }
                    /* Check if the right button pressed */
                    if (mouse_buttons & UX_HOST_CLASS_HID_MOUSE_BUTTON_2_PRESSED)
                    {
                        PRINT_INFO_STR("HID Mouse right button clicked");
                        tx_thread_sleep(SLEEP_TIME_100);
                    }

                    /* Check if the position moved */
                    ux_host_class_hid_mouse_position_get ((UX_HOST_CLASS_HID_MOUSE *)hid_client->ux_host_class_hid_client_local_instance, &mouse_x_position, &mouse_y_position);

                    if ((mouse_x_position_old != mouse_x_position) || (mouse_y_position_old != mouse_y_position))
                    {
                        sprintf(mouse_data_buff,"HID Mouse x position = %ld and y position = %ld",mouse_x_position, mouse_y_position);
                        PRINT_INFO_STR(mouse_data_buff);
                        tx_thread_sleep(SLEEP_TIME_10);
                        mouse_x_position_old = mouse_x_position;
                        mouse_y_position_old = mouse_y_position;
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
 * @} (end addtogroup usbx_host_hub_ep)
 **********************************************************************************************************************/


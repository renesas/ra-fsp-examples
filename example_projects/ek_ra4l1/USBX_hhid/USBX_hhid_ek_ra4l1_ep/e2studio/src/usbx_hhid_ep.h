/***********************************************************************************************************************
* File Name    : usbx_hhid_ep.h
* Description  : Contains data structures and functions used in usbx_hhid_ep.h.
***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef USBX_HHID_EP_H_
#define USBX_HHID_EP_H_

#include "ux_api.h"
#include "r_usb_basic.h"
#include "ux_host_class_hid.h"
#include "ux_host_class_hid_keyboard.h"
#include "ux_host_class_hid_mouse.h"
#include "hhid_thread.h"

/* To display EP info on the RTT viewer */
#define EP_INFO        "\r\nThis project demonstrates the basic functionality of USBX Host HID Module \r\n"\
                       "driver with Azure RTOS running on Renesas RA MCUs based on Renesas FSP.\r\n"\
                       "USB Keyboard and USB Mouse will be connected as host device to the RA board\r\n"\
                       "which is configured as usbx_hhid host.\r\n"\
                       "When Keyboard is connected as host device, the RA boards reads key value entered\r\n"\
                       "by the user on the keyboard connected and prints it on the RTT Viewer.\r\n"\
                       "On connecting Mouse as host device, the RA board reads user connected mouse\r\n"\
                       "and will print the X & Y coordinates on the RTT Viewer.\r\n"\
                       "Status of application will be displayed on J-Link RTT Viewer \r\n"\

/* macros to print info, error and trap the error.*/
#define PRINT_INFO_STR(str)  ({app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_INFO_STR, sizeof(str), (str));})
#define PRINT_ERR_STR(str)   ({app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_ERR_STR, sizeof(str), (str));})
#define ERROR_TRAP(err)      ({app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_ERR_TRAP, sizeof(UINT *), &(err));})

#if defined BOARD_RA4L1_EK
#define     MEMPOOL_SIZE               (32768)    // Memmory pool size
#else
#define     MEMPOOL_SIZE               (63488)    // Memmory pool size
#endif
#define     DEVICE_INSERTION           (0x01U)    // Device Inserted
#define     DEVICE_REMOVAL             (0x02U)    // Device Removed
#define     EVENT_USB_PLUG_IN          (1UL << 0) // Event Plugged In
#define     EVENT_USB_PLUG_OUT         (1UL << 1) // Event Plugged Out
#define     UX_NO_KEY_PRESS            (255)      // No key press
#define     NULL_CHAR                  ('\0')     // NULL Character
#define     BUFF_SIZE                  (8U)       // keyboard Buffer size
#define     BUFF_INDEX                 (0U)       // Starting buffer index
#define     HID_CLIENT                 (0U)       // Starting HID client buffer index
#define     KEY_HIGH_VAL               (127U)     // Highest value key supported
#define     KEY_LOW_VAL                (32U)      // Lowest value key supported
#define     TEN                        (10U)      // Time for thread sleep

typedef struct st_mouse_pos
{
    SLONG      mouse_x_position_old;
    SLONG      mouse_y_position_old;
}mouse_pos_t;

#endif /* USBX_HHID_EP_H_ */

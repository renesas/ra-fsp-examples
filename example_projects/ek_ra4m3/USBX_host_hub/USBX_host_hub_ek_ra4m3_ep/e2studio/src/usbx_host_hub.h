/***********************************************************************************************************************
 * File Name    : usbx_host_hid_hub.h
 * Description  : Contains macros used in usbx_host_hid_key_board_hub_entry.c and usbx_host_hid_mouse_hub_entry.c
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2022 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef USBX_HOST_HID_HUB_H_
#define USBX_HOST_HID_HUB_H_
#include "ux_api.h"
#include "ux_system.h"

/*******************************************************************************************************************//**
 * @addtogroup usbx_host_hub_ep
 * @{
 **********************************************************************************************************************/

#define ON                                      (1U)
#define OFF                                     (0U)
#define UX_ZERO                                 (0U)

#define BYTE_SIZE                               (4U)
#define BUFFER_SIZE_MIN                         (200U)
#define BUFF_SIZE                               (8U)
#define BUFF_INDEX                              (0U)
#define KEY_HIGH_VAL                            (127U)
#define KEY_LOW_VAL                             (32U)

/* Memory pool size is used as per Azure guidelines */
#define MEMPOOL_SIZE                            (63488U)
#define UX_NO_KEY_PRESS                         (255U)

#define KEYBOARD_DEVICE_CONNECT_FLAG            ((ULONG)0x0001)
#define KEYBOARD_DEVICE_REMOVED_FLAG            ((ULONG)0x0000)

#define MOUSE_DEVICE_CONNECT_FLAG               ((ULONG)0x0001)
#define MOUSE_DEVICE_REMOVED_FLAG               ((ULONG)0x0000)

#define SLEEP_TIME_1                            (1U)
#define SLEEP_TIME_5                            (5U)
#define SLEEP_TIME_10                           (10U)
#define SLEEP_TIME_50                           (50U)
#define SLEEP_TIME_100                          (100U)

/* macros to print info, error and trap the error.*/
#define PRINT_INFO_STR(str)         (app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_INFO_STR, sizeof(str), (str)))
#define PRINT_ERR_STR(str)          (app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_ERR_STR, sizeof(str), (str)))
#define PRINT_NEXT_LINE(n)          (app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_NEXT_LINE, sizeof(n), &(n)))
#define PRINT_DATA(data)            (app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_PRINT_DATA, sizeof(data), &(data)))
#define ERROR_TRAP(err)             (app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_ERR_TRAP, sizeof(UINT *), &(err)))

/* To display EP info on the RTT viewer */
#define EP_INFO        "\r\nThis project demonstrates the basic functionality of USBX Hub Module running on\r\n"\
                       "Renesas RA MCUs based on Renesas FSP using AzureRTOS. User will be able to use the\r\n"\
                       "RA board as a Host device by connecting it to the Keyboard and Mouse via Host Hub.\r\n"\
                       "Status of application (Keyboard data, Mouse(x&y) direction and error messages) will\r\n"\
                       "be displayed on J-Link RTT Viewer.\r\n"\


extern TX_THREAD usbx_host_hid_mouse_hub;

/* Private function prototypes */
void usbx_host_hud_hid_class_initialize(void);
void usbx_host_hud_hid_class_un_init(void);


#endif /* USBX_HOST_HID_HUB_H_ */

/*******************************************************************************************************************//**
 * @} (end addtogroup usbx_host_hub_ep)
 **********************************************************************************************************************/

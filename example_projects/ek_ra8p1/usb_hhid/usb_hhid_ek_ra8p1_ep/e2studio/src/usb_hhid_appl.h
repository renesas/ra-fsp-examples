/***********************************************************************************************************************
 * File Name    : usb_hhid_appl.h
 * Description  : Contains data structures and functions used in usb_hhid_appl.h
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef USB_HHID_APPL_H_
#define USB_HHID_APPL_H_

/* Macro definitions */
#define SET_PROTOCOL               (USB_HID_SET_PROTOCOL | USB_HOST_TO_DEV | USB_CLASS | USB_INTERFACE)
#define BOOT_PROTOCOL              (0U)
#define BUFSIZE                    (8U)
#define DATASIZE                   (12U)
#define TASK_DELAY                 (1U)
#define NULL_CHAR                  ('\0')
#define ROW_SIZE                   (40U)
#define COL_SIZE                   (25U)
#define SET_ZERO                   (0x0000)
#define ONE                        (1U)
#define INDEX                      (2U)
#define BUFF_SIZE                  (25U)
#define KEY_MOD_LSHIFT             (0x02)
#define KEY_MOD_RSHIFT             (0x20)

#define EP_INFO   "\r\nThis example project demonstrates the basic functionalities of USB Host Human Interface\r\n"\
                  "Device Class (HHID) driver with FreeRTOS on Renesas RA MCUs based on Renesas FSP.\r\n"\
                  "The USB HHID driver configures keyboard as an HHID device. On pressing a key from\r\n"\
                  "the keyboard, the received keycode is decoded and displayed on the J-Link RTT Viewer.\r\n"\
                  "Error and info messages will also be printed on the J-Link RTT Viewer.\r\n\n\n"

/* Function declaration */
void deinit_usb(void);

#endif /* USB_HHID_APPL_H_ */

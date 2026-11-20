/***********************************************************************************************************************
 * File Name    : usb_hhid_ep.h
 * Description  : Contains macros, variables, and functions used in usb_hhid_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef USB_HHID_EP_H_
#define USB_HHID_EP_H_

#include "usb_hhid_thread.h"
#include "terminal.h"

/* Macro for the reset value */
#define RESET_VALUE             (0x00)

/* Macros for USB HID protocol configuration */
#define SET_PROTOCOL            (USB_HID_SET_PROTOCOL | USB_HOST_TO_DEV | USB_CLASS | USB_INTERFACE)
#define BOOT_PROTOCOL           (0U)
#define TASK_DELAY              (1U)

/* Macro for USB HID boot keyboard report size */
#define KEYBOARD_REPORT_SIZE    (8U)

/* External variable - Defined in usb_hhid_ep.c */
extern volatile bool g_err_flag;

/* Function declarations */
fsp_err_t usb_init(void);
fsp_err_t usb_hhid_max_packet_size_get(usb_ctrl_t * const p_api_ctrl, uint16_t * p_size, uint8_t direction,
                                       uint8_t device_address);
fsp_err_t usb_read(usb_ctrl_t * const p_api_ctrl, uint8_t * p_buf, uint32_t size, uint8_t destination);
fsp_err_t usb_host_control_transfer(usb_ctrl_t * const p_api_ctrl, usb_setup_t * p_setup, uint8_t * p_buf,
                                    uint8_t device_address);
void usb_deinit(void);

#endif /* USB_HHID_EP_H_ */

/***********************************************************************************************************************
 * File Name    : usb_hcdc_ep.h
 * Description  : Contains macros, variables, and functions used in usb_hcdc_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef USB_HCDC_EP_H_
#define USB_HCDC_EP_H_

#include <stdbool.h>
#include "hal_data.h"
#include "usb_hcdc_thread.h"

/* Macro for the reset value */
#define RESET_VALUE                 (0x00)

/* Macros for menu selection keys */
#define KIT_INFO                    ('1')
#define NEXT_STEPS                  ('2')
#define CARRIAGE_RETURN             ('\r')

/* Macros for CDC class requests */
#define SET_LINE_CODING             (USB_CDC_SET_LINE_CODING | USB_HOST_TO_DEV | USB_CLASS | USB_INTERFACE)
#define GET_LINE_CODING             (USB_CDC_GET_LINE_CODING | USB_DEV_TO_HOST | USB_CLASS | USB_INTERFACE)
#define SET_CONTROL_LINE_STATE      (USB_CDC_SET_CONTROL_LINE_STATE | USB_HOST_TO_DEV | USB_CLASS | USB_INTERFACE)

/* Macro for the line coding length */
#define LINE_CODING_LENGTH          (0x07U)

/* Macros for USB data transfer lengths */
#define CDC_READ_DATA_LEN           (512)
#define CDC_WRITE_DATA_LEN          (1)

/* Macro for the queue wait time, in ticks (do not block) */
#define NO_WAIT_TIME                (0)

/* External variable - Defined in usb_hcdc_ep.c */
extern volatile bool g_err_flag;

/* Function declarations */
fsp_err_t usb_init(void);
fsp_err_t usb_read(usb_ctrl_t * const p_api_ctrl, uint8_t * p_buf, uint32_t size, uint8_t destination);
fsp_err_t usb_write(usb_ctrl_t * const p_api_ctrl, uint8_t const * const p_buf, uint32_t size, uint8_t destination);
fsp_err_t usb_host_control_transfer(usb_ctrl_t * const p_api_ctrl, usb_setup_t * p_setup, uint8_t * p_buf,
                                    uint8_t device_address);
void usb_deinit(void);

#endif /* USB_HCDC_EP_H_ */

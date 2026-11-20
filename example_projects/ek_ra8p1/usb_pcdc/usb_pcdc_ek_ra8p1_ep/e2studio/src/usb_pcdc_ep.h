/***********************************************************************************************************************
 * File Name    : usb_pcdc_ep.h
 * Description  : Contains macros, variables, and functions used in usb_pcdc_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef USB_PCDC_EP_H_
#define USB_PCDC_EP_H_

#include <string.h>
#include "hal_data.h"

/* Macro for the reset value */
#define RESET_VALUE                 (0x00)

/* Macro for the number of the string descriptors */
#define NUM_STRING_DESCRIPTOR       (7U)

/* Macro for the length of the CDC line coding data */
#define LINE_CODING_LENGTH          (0x07U)

/* Macro for the size of the buffer used to read the data sent from the terminal */
#define READ_BUF_SIZE               (8U)

/* External variables - Defined in usb_pcdc_ep.c */
extern uint8_t g_read_buf[READ_BUF_SIZE];
extern bool g_b_usb_attach;
extern bool g_b_usb_write_complete;

/* External variables - Defined in r_usb_pcdc_descriptor.c */
extern uint8_t g_apl_device[];
extern uint8_t g_apl_configuration[];
extern uint8_t g_apl_hs_configuration[];
extern uint8_t g_apl_qualifier_descriptor[];
extern uint8_t * g_apl_string_table[];

/* Function declarations */
fsp_err_t usb_init(void);
fsp_err_t usb_event_get(usb_event_info_t * p_event_info, usb_status_t * p_usb_event);
fsp_err_t usb_read(void);
fsp_err_t usb_write(const char * p_data);
fsp_err_t usb_class_request(usb_event_info_t * p_event_info);

#endif /* USB_PCDC_EP_H_ */

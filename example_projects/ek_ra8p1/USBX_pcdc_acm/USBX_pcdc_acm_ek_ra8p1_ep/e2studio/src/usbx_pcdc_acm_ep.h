/***********************************************************************************************************************
 * File Name    : usbx_pcdc_acm_ep.h
 * Description  : Contains data structures and functions used in usbx_pcdc_acm_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef USBX_PCDC_ACM_EP_H_
#define USBX_PCDC_ACM_EP_H_

#include <stdbool.h>
#include "usbx_pcdc_acm_thread.h"
#include "terminal.h"

/* Macro for the reset value */
#define RESET_VALUE                         (0x00)

/* Macro for the null char */
#define NULL_CHAR                           ('\0')

/* Macros for the USBX device configuration and data buffers */
#define DEVICE_FRAME_LENGTH_HIGH_SPEED      (103U)
#define DEVICE_FRAME_LENGTH_FULL_SPEED      (93U)
#define STRING_FRAMEWORK_LENGTH             (105U)
#define LANGUAGE_ID_FRAME_WORK_LENGTH       (2U)
#define CONFIG_NUMB                         (1U)
#define INTERFACE_NUMB0                     (0x00)
#define MEMPOOL_SIZE                        (18432U)
#define BYTE_SIZE                           (4U)
#define DATA_LEN                            (2048U)
#define WRITE_DATA                          "\r\n\r\nEnter any key to echo back the entered data\r\nUser input: "
#define WRITE_DATA_LEN                      (sizeof(WRITE_DATA) - 1U)

/* Macros for the CDC ACM connection events */
#define CDCACM_FLAG                         ((ULONG)0x0001)
#define CDCACM_REMOVED_FLAG                 ((ULONG)0x0002)
#define CDCACM_ACTIVATE_FLAG                ((ULONG)0x0004)
#define CDCACM_DEACTIVATE_FLAG              ((ULONG)0x0008)

/* Macro for the USB transfer settle time */
#define CDCACM_SETTLE_TIME                  (20U)

/* Macros for printing information and error messages */
#define PRINT_INFO_STR(str)                 (term_print_data(TERM_OP_MSG_INFO_STR, sizeof(str), (str)))
#define PRINT_ERR_STR(str)                  (term_print_data(TERM_OP_MSG_ERR_STR, sizeof(str), (str)))
#define ERROR_TRAP(err)                     (term_print_data(TERM_OP_MSG_ERR_TRAP, sizeof(UINT), &(err)))

/* External variables */
extern uint8_t g_device_framework_full_speed[];
extern uint8_t g_device_framework_hi_speed[];
extern uint8_t g_language_id_framework[];
extern uint8_t g_string_framework[];

/* External function */
extern uint32_t usb_peri_usbx_initialize(uint32_t dcd_io);

/* Function declarations */
fsp_err_t usb_init(void);
fsp_err_t usb_uninit(void);

#endif /* USBX_PCDC_ACM_EP_H_ */

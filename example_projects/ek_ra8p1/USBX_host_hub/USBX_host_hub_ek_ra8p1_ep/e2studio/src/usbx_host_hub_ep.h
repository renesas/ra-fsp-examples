/***********************************************************************************************************************
 * File Name    : usbx_host_hub_ep.h
 * Description  : Contains data structures and functions used in usbx_host_hub_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef USBX_HOST_HUB_EP_H_
#define USBX_HOST_HUB_EP_H_

#include <stdbool.h>
#include "usbx_host_hid_key_board_hub_thread.h"
#include "usbx_host_hid_mouse_hub_thread.h"
#include "terminal.h"

/* Macro for the reset value */
#define RESET_VALUE                     (0x00)

/* Macro for the null char */
#define NULL_CHAR                       ('\0')

/* Macros for buffer sizes and indexes */
#define BYTE_SIZE                       (4U)
#define BUFF_SIZE                       (8U)
#define BUFF_INDEX                      (0U)
#define KEY_HIGH_VAL                    (127U)
#define KEY_LOW_VAL                     (32U)

/* Macro for the USBX memory pool size */
#if defined (BOARD_RA4L1_EK)
#define MEMPOOL_SIZE                    (31744U)
#else
#define MEMPOOL_SIZE                    (63488U)
#endif /* BOARD_RA4L1_EK */

/* Macro for no key press status */
#define UX_NO_KEY_PRESS                 (255U)

/* Macros for keyboard device events */
#define KEYBOARD_DEVICE_CONNECT_FLAG    ((ULONG)0x0001)
#define KEYBOARD_DEVICE_REMOVED_FLAG    ((ULONG)0x0000)

/* Macros for mouse device events */
#define MOUSE_DEVICE_CONNECT_FLAG       ((ULONG)0x0001)
#define MOUSE_DEVICE_REMOVED_FLAG       ((ULONG)0x0000)

/* Macros for thread sleep times */
#define SLEEP_TIME_1                    (1U)
#define SLEEP_TIME_5                    (5U)
#define SLEEP_TIME_10                   (10U)
#define SLEEP_TIME_100                  (100U)

/* Macros to print info, error and trap the error */
#define PRINT_INFO_STR(str)             (term_print_data(TERM_OP_MSG_INFO_STR, sizeof(str), (str)))
#define PRINT_ERR_STR(str)              (term_print_data(TERM_OP_MSG_ERR_STR, sizeof(str), (str)))
#define ERROR_TRAP(err)                 (term_print_data(TERM_OP_MSG_ERR_TRAP, sizeof(UINT), &(err)))

/* External variable */
extern TX_THREAD usbx_host_hid_mouse_hub_thread;

/* Function declarations */
fsp_err_t usb_init(void);
fsp_err_t usb_uninit(void);

#endif /* USBX_HOST_HUB_EP_H_ */

/***********************************************************************************************************************
 * File Name    : usbx_dfu_ep.h
 * Description  : Contains data structures and functions used in usbx_dfu_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef USBX_DFU_EP_H_
#define USBX_DFU_EP_H_

#include <stdbool.h>
#include "terminal.h"
#include "usbx_peri_dfu_thread.h"

/* Macro for the reset value */
#define RESET_VALUE                             (0x00)

/* Macro for the null char */
#define NULL_CHAR                               ('\0')

/* Macros for USB interface and configuration numbers */
#define INTERFACE_NUMB0                         (0x00)
#define INTERFACE_NUMB2                         (0x02)
#define CONFIG_NUMB                             (1U)

/* Macro for byte size conversion */
#define BYTE_SIZE                               (4U)

/* Macros for USBX memory pool and DFU buffer sizes */
#if defined (BOARD_RA4L1_EK)
#define MEMPOOL_SIZE                            (18432U)
#define BUFFER_SIZE                             (4096U)
#else
#define MEMPOOL_SIZE                            (2 * 18432U)
#define BUFFER_SIZE                             (300 * 100U)
#endif /* BOARD_RA4L1_EK */

/* Macro for the terminal buffer size */
#define TERM_BUFFER_SIZE                        (50U)

/* Macro for the CDC Full-Speed device framework length */
#define DEVICE_FRAME_LENGTH_CDC_FULL_SPEED      (119U)

/* Macro for the CDC High-Speed device framework length: CDC (103) + IAD (8) + DFU (9 * 2) */
#define DEVICE_FRAME_LENGTH_CDC_HI_SPEED        (129U)

/* Macros for the DFU device framework lengths */
#define DFU_DEVICE_FRAME_LENGTH_HIGH_SPEED      (55U)
#define DFU_DEVICE_FRAME_LENGTH_FULL_SPEED      (45U)

/* Macros for USBX string and language framework lengths */
#define STRING_FRAMEWORK_CDC_LENGTH             (105U)
#define CDC_LANGUAGE_ID_FRAME_WORK_LENGTH       (2U)
#define DFU_STRING_FRAMEWORK_LENGTH             (94U)
#define DFU_LANGUAGE_ID_FRAME_WORK_LENGTH       (2U)

/* Macros for CDC data lengths */
#define CDC_DATA_LENGTH                         (1024U)
#define CDC_ACTUAL_LENGTH                       (15U)

/* Macros for thread sleep periods */
#define THREAD_SLEEP_10                         (10U)
#define THREAD_SLEEP_20                         (20U)
#define THREAD_SLEEP_50                         (50U)
#define THREAD_SLEEP_100                        (100U)
#define THREAD_SLEEP_200                        (200U)

/* Macro for the DFU detach event flag */
#define DETACH_EVENT_FLAG                       ((ULONG) 0x01)

/* Macros for information, error, and error trap messages */
#define PRINT_INFO_STR(str)                     (term_print_data(TERM_OP_MSG_INFO_STR, sizeof(str), (str)))
#define PRINT_ERR_STR(str)                      (term_print_data(TERM_OP_MSG_ERR_STR, sizeof(str), (str)))
#define ERROR_TRAP(err)                         (term_print_data(TERM_OP_MSG_ERR_TRAP, sizeof(UINT), &(err)))

/* External variables */
extern uint8_t g_device_framework_full_speed[];
extern uint8_t g_device_framework_hi_speed[];
extern uint8_t g_string_framework[];
extern uint8_t g_language_id_framework[];
extern UCHAR g_dfu_device_framework_full_speed[];
extern UCHAR g_dfu_string_framework[];

/* Function declarations */
fsp_err_t usb_init(usb_ctrl_t * const p_api_ctrl, usb_cfg_t const * const p_cfg);
fsp_err_t usb_deinit(usb_ctrl_t * const p_api_ctrl);

#endif /* USBX_DFU_EP_H_ */

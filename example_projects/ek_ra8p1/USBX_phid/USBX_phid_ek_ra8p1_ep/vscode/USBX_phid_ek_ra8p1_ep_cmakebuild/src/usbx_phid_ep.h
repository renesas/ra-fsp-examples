/***********************************************************************************************************************
 * File Name    : usbx_phid_ep.h
 * Description  : Contains macros, variables, and functions used in usbx_phid_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef USBX_PHID_EP_H_
#define USBX_PHID_EP_H_

#include <stdbool.h>
#include "usbx_phid_thread.h"
#include "terminal.h"
#include "ux_api.h"
#include "ux_system.h"
#include "ux_device_class_hid.h"

/* Macro for the reset value */
#define RESET_VALUE                     (0x00)

/* Macro for the null character */
#define NULL_CHAR                       ('\0')

/* Macro for the USBX memory pool size */
#define MEMPOOL_SIZE                    (18432U)

/* Macro for the HID report descriptor length */
#define REPORT_DESCRIPTOR_LENGTH        (76U)

/* Macro for the delay between two keys in milliseconds */
#define DELAY_100_MS                    (100U)

/* Macro for the delay for the user to open Notepad in milliseconds */
#define DELAY_3000_MS                   (3000U)

/* Macros for the USBX device framework lengths */
#define DEVICE_FRAME_LENGTH_SPEED       (52U)
#define STRING_FRAMEWORK_LENGTH         (53U)
#define LANGUAGE_ID_FRAME_WORK_LENGTH   (2U)

/* Macros for the HID class registration */
#define CONFIG_NUMB                     (1U)
#define INTERFACE_NUMB0                 (0x00U)

/* Macros for the HID event flags */
#define HID_FLAG                        ((ULONG)0x0001)
#define HID_REMOVED_FLAG                ((ULONG)0x0002)
#define HID_ACTIVATE_FLAG               ((ULONG)0x0004)
#define HID_DEACTIVATE_FLAG             ((ULONG)0x0008)

/* Macro for the first key code - Corresponds to the character 'a' */
#define FIRST_KEY                       (0x04U)

/* Macro for the number of keys after the first key - The last key corresponds to the number '0' */
#define END_KEY                         (35U)

/* Macro for the length of the HID keyboard event */
#define EVENT_LENGTH                    (8U)

/* Macros for the byte indexes within the HID keyboard event */
#define MODIFIER_BYTE_INDEX             (0U)
#define RESERVED_BYTE_INDEX             (1U)
#define KEYCODE_BYTE_INDEX              (2U)

/* External variables - Defined in rm_usbx_phid_descriptor.c */
extern uint8_t g_device_framework_full_speed[];
extern uint8_t g_string_framework[];
extern uint8_t g_language_id_framework[];
extern uint8_t g_apl_report[REPORT_DESCRIPTOR_LENGTH];

/* Function declarations */
fsp_err_t usb_init(void);
fsp_err_t usb_uninit(void);

#endif /* USBX_PHID_EP_H_ */

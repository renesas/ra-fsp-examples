/***********************************************************************************************************************
* File Name    : usbx_hhid_ep.h
* Description  : Contains macros, data structures and functions used in usbx_hhid_ep.h.
***********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef USBX_HHID_EP_H_
#define USBX_HHID_EP_H_

#include "ux_api.h"
#include "r_usb_basic.h"
#include "ux_host_class_hid.h"
#include "ux_host_class_hid_keyboard.h"
#include "ux_host_class_hid_mouse.h"
#include "hhid_thread.h"

/* Macro for the reset value */
#define RESET_VALUE                 (0x00)

/* Macro for the null char */
#define NULL_CHAR                   ('\0')

/* Macro for memory pool size */
#if defined BOARD_RA4L1_EK
#define MEMPOOL_SIZE               (32768U)
#else
#define MEMPOOL_SIZE               (63488U)
#endif /* Specific board */

/* Macros for the USB plug-in/plug-out events and device insertion/removal */
#define EVENT_USB_PLUG_IN          (1UL << 0)
#define EVENT_USB_PLUG_OUT         (1UL << 1)

/* Macros for keyboard option */
#define KEY_HIGH_VAL               (127U)
#define KEY_LOW_VAL                (32U)
#define UX_NO_KEY_PRESS            (255U)
#define BUFF_SIZE                  (8U)

typedef struct st_mouse_pos
{
    SLONG mouse_x_pos_old;
    SLONG mouse_y_pos_old;
} mouse_pos_t;

typedef enum e_usbx_hhid_device
{
    NONE = 0,
    KEYBOARD,
    MOUSE,
    UNSUPPORTED
} usbx_hhid_device_t;

/* Function declarations */
UINT usbx_hhid_init(void);
void usbx_hhid_deinit(void);
UINT usbx_hhid_wait_for_connection(ULONG requested_event, ULONG * actual_flags, ULONG wait_option);
usbx_hhid_device_t usbx_hhid_device_get(void);
UINT usbx_hhid_keyboard_key_get(uint8_t * const p_key, ULONG * const p_state);
UINT usbx_hhid_mouse_buttons_get(ULONG * const p_buttons);
UINT usbx_hhid_mouse_position_get(SLONG * const p_x, SLONG * const p_y);

#endif /* USBX_HHID_EP_H_ */

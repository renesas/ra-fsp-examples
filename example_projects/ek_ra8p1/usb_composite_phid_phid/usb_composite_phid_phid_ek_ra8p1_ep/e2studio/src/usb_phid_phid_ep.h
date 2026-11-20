/***********************************************************************************************************************
 * File Name    : usb_phid_phid_ep.h
 * Description  : Contains macros, variables, data structures, and functions used in usb_phid_phid_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef USB_PHID_PHID_EP_H_
#define USB_PHID_PHID_EP_H_

#include <stdbool.h>
#include "phid_handler_thread.h"

/* Macro for the reset value */
#define RESET_VALUE                     (0x00)

/* Macro definitions */
#define PHID_RD_LENGTH                  (76U)   /* Keyboard report descriptor length */
#define PHID2_RD_LENGTH                 (50U)   /* Mouse report descriptor length */
#define HID_D_LENGTH                    (9U)    /* HID descriptor length */
#define PHID_D_INDEX                    (18U)   /* Keyboard HID descriptor index */
#define PHID2_D_INDEX                   (43U)   /* Mouse HID descriptor index */

#define KEY_DATA_LEN                    (8U)    /* Keyboard report buffer length */
#define KEY_DATA_INDEX                  (2U)    /* Key first press index for key report */
#define USAGE_ID_A                      (0x04)  /* Keyboard usage id of A */
#define USAGE_ID_0                      (0x27)  /* Keyboard usage id of 0 */
#define KEY_SIZE_NUM                    (2U)

#define MOUSE_DATA_LEN                  (3U)    /* Mouse report buffer length */
#define MOUSE_X_INDEX                   (1U)    /* X value index for Mouse report */
#define MOUSE_Y_INDEX                   (2U)    /* Y value index for Mouse report */
#define MOUSE_SPEED                     (10U)   /* Mouse move speed */
#define MOUSE_STAND                     (0x00)  /* Mouse stand value */
#define MOUSE_X_RIGHT(x)                ((x))   /* The sign for X speed value */
#define MOUSE_X_LEFT(x)                 (-(x))  /* The sign for X speed value */
#define MOUSE_Y_DOWN(x)                 ((x))   /* The sign for Y speed value */
#define MOUSE_Y_UP(x)                   (-(x))  /* The sign for Y speed value */

/* Cumulative Mouse-move step thresholds per direction */
#define MOVE_RIGHT_TIMES                (30U)
#define MOVE_DOWN_TIMES                 (60U)
#define MOVE_LEFT_TIMES                 (90U)
#define MOVE_UP_TIMES                   (120U)

#define TASK_DELAY                      (1U)     /* Delay for task */
#define IDLE_VAL_INDEX                  (1U)     /* Index value */
#define DELAY_1_SEC                     (1000U)  /* 1 second delay */
#define DELAY_2_SEC                     (2000U)  /* 2 seconds delay */
#define DELAY_3_SEC                     (3000U)  /* 3 seconds delay */

/* Extern variables - Defined in usb_phid_phid_ep.c */
extern bool g_b_key_flag;
extern bool g_b_mouse_flag;

/* Extern variables - Defined in r_usb_phid_phid_descriptor.c */
extern uint8_t g_apl_configuration[];
extern uint8_t g_apl_report_keyboard[];
extern uint8_t g_apl_report_mouse[];

/* Function declarations */
fsp_err_t usb_init(void);
void usb_deinit(void);
fsp_err_t usb_enumeration(usb_event_info_t * p_event);
fsp_err_t usb_status_update(usb_event_info_t * p_event);
fsp_err_t usb_write_operation(usb_event_info_t * p_event);

#endif /* USB_PHID_PHID_EP_H_ */

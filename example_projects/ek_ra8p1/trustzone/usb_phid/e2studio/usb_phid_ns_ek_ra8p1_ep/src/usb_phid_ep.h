/***********************************************************************************************************************
 * File Name    : usb_phid_ep.h
 * Description  : Contains macros, variables, and functions used in usb_phid_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef USB_PHID_EP_H_
#define USB_PHID_EP_H_

#include "phid_main_thread.h"
#include "terminal.h"

/* Macro for the reset value */
#define RESET_VALUE                         (0x00)

/* Macro for report descriptor */
#define USB_RECEIVE_REPORT_DESCRIPTOR       (76U)

/* Macro for HID descriptor */
#define USB_RECEIVE_HID_DESCRIPTOR          (9U)

/* Macro for data length */
#define DATA_LEN                            (8U)

/* Macro for numlock data size */
#define SIZE_NUM                            (2U)

/* Macro for keyboard usage id of A */
#define USAGE_ID_A                          (0x04)

/* Macro for keyboard usage id of 0 */
#define USAGE_ID_0                          (0x27)

/* Macro for write delay */
#define DELAY                               (100U)

/* Macro for third index for USB buffer */
#define INDEX_THIRD                         (2U)

/* Macro for task delay */
#define TASK_DELAY                          (1U)

/* Macro for configure descriptor length */
#define CD_LENGTH                           (18U)

/* Macro for index value */
#define IDLE_VAL_INDEX                      (1U)

/* Macro for size of send data buffer */
#define BUFF_SIZE                           (16U)

/* Macro for alignment */
#define ALIGN                               (4U)

/* Macro for 3 seconds delay */
#define DELAY_3_SEC                         (3000U)

/* External variables - Defined in r_usb_phid_descriptor.c */
extern uint8_t g_apl_configuration[];
extern uint8_t g_apl_report[];

/* External variable - Defined in usb_phid_ep.c */
extern volatile usb_event_info_t * gp_usb_phid_event;

/* Function declarations */
fsp_err_t usb_init(void);
fsp_err_t usb_enumeration(void);
fsp_err_t usb_status_update(void);
fsp_err_t usb_write_operation(void);
void usb_deinit(void);

#endif /* USB_PHID_EP_H_ */

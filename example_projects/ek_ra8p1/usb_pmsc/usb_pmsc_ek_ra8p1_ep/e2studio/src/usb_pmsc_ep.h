/***********************************************************************************************************************
 * File Name    : usb_pmsc_ep.h
 * Description  : Contains macros, variables, data structures, and functions used in usb_pmsc_ep.h.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef USB_PMSC_EP_H_
#define USB_PMSC_EP_H_

#include <stdbool.h>
#include "hal_data.h"
#include "r_usb_basic.h"
#include "pmsc_thread.h"

/* Macro for the reset value */
#define RESET_VALUE                         (0x00)

/* Macro for USB PMSC configuration */
#define NUM_STRING_DESCRIPTOR               (7U)

/* External variables */
extern uint8_t g_apl_device[];
extern uint8_t g_apl_configuration[];
extern uint8_t g_apl_hs_configuration[];
extern uint8_t g_apl_qualifier_descriptor[];
extern uint8_t * gp_apl_string_table[];
extern volatile usb_event_info_t * gp_usb_event;

/* Function declarations */
fsp_err_t usb_init(void);
BaseType_t usb_pmsc_event_get(void);
void usb_deinit(void);
#endif /* USB_PMSC_EP_H_ */

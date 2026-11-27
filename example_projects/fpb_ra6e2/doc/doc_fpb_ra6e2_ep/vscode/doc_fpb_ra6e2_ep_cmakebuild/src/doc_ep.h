/***********************************************************************************************************************
 * File Name    : doc_ep.h
 * Description  : Contains declarations of data structures and functions used in doc_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef DOC_EP_H_
#define DOC_EP_H_

#include <stdbool.h>
#include "hal_data.h"

/* Macro for the first data written for the subtraction event */
#define DOC_DATA_SUB                    (0xA5A4)

/* Macro for the second data written for the subtraction event, causes underflow */
#define DOC_DATA_UNDERFLOW              (0x0002)

/* Macro for the first data written for the addition event */
#define DOC_DATA_ADD                    (0x5A5A)

/* Macro for the second data written for the addition event, causes overflow */
#define DOC_DATA_OVERFLOW               (0x0001)

/* Macro for the data written for the comparison match event */
#define DOC_DATA_COMPARISON_MATCH       (0xA5A5)

/* Macro for the data written for the comparison mismatch event */
#define DOC_DATA_COMPARISON_MISMATCH    (0xB5B5)

/* Macro for the reset value */
#define RESET_VALUE                     (0x00)

/* Macro for the LED level that indicates a DOC event was detected */
#if defined (BOARD_RA4W1_EK) || defined (BOARD_RA6T1_RSSK) || defined (BOARD_RA4T1_MCK) ||\
    defined (BOARD_RA8T1_MCK) || defined (BOARD_RA6T2_MCK) || defined (BOARD_RA6T3_MCK) ||\
    defined (BOARD_RA8T2_MCK)
#define LED_ON                          (BSP_IO_LEVEL_LOW)
#else
#define LED_ON                          (BSP_IO_LEVEL_HIGH)
#endif

/* Extern variables */
extern bsp_leds_t g_bsp_leds;
extern volatile bool g_doc_event_flag;

/* Function declarations */
fsp_err_t doc_init(void);
fsp_err_t doc_write(uint32_t data);
void doc_deinit(void);

#endif /* DOC_EP_H_ */

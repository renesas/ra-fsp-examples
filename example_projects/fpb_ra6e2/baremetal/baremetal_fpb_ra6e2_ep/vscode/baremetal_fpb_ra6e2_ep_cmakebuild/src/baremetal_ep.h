/***********************************************************************************************************************
 * File Name    : baremetal_ep.h
 * Description  : Contains macros, variables, data structures and functions used in baremetal_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef BAREMETAL_EP_H_
#define BAREMETAL_EP_H_

#include <stdbool.h>
#include "hal_data.h"

/* Macro for the reset value */
#define RESET_VALUE                     (0x00)

/* Data written to DOC register for comparison match event */
#define DOC_DATA_COMPARISON_MATCH       (0xA5A5U)

/* Data written to DOC register for comparison mismatch event */
#define DOC_DATA_COMPARISON_MISMATCH    (0xB5B5U)

/* Wait time for the LED indication delay in microseconds */
#define WAIT_TIME                       (500U)

/* Wait time for DOC mismatch detection */
#define IRQ_WAIT_TIME                   (10U)

/* Macros for LED status */
#if defined (BOARD_RA8T1_MCK) || defined (BOARD_RA4T1_MCK) || defined (BOARD_RA6T3_MCK) || defined (BOARD_RA8T2_MCK)
#define LED_ON                          (BSP_IO_LEVEL_LOW)
#define LED_OFF                         (BSP_IO_LEVEL_HIGH)
#else
#define LED_ON                          (BSP_IO_LEVEL_HIGH)
#define LED_OFF                         (BSP_IO_LEVEL_LOW)
#endif /* Specific board */

/* Enumeration to select which LED to turn ON */
typedef enum
{
    LED_CASE_1 = 1,
    LED_CASE_2 = 2,
    LED_CASE_3 = 3
} led_state_t;

/* For on-board LEDs */
extern bsp_leds_t g_bsp_leds;

/* Function declarations */
fsp_err_t baremetal_process(void);
void baremetal_doc_isr(void);

#endif /* BAREMETAL_EP_H_ */

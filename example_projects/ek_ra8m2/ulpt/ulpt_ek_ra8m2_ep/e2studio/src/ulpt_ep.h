/***********************************************************************************************************************
 * File Name    : ulpt_ep.h
 * Description  : Contains declarations of data structures and functions used in ulpt_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2023 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"

#ifndef ULPT_EP_H_
#define ULPT_EP_H_

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

/* Macro for FSP module status */
#define MODULE_CLOSED       (0U)

/* Macros to check if the LED count is zero */
#define LED_COUNT_ZERO      (0U)
#define LED_ONE             (0U)

/* Macros to define time-period value */
#define PERIOD_1S           (1U)
#define PERIOD_2S           (2U)
#define PERIOD_3S           (3U)
#define TIMES_MAX           (3U)

/* Macros for RTT Viewer */
#define SELECT_INDEX        (0U)
#define SELECT_1S           ('1')
#define SELECT_2S           ('2')
#define SELECT_3S           ('3')
#define NULL_CHAR           ('\0')
#define MENU_SELECT         "\r\nSelect period values for both one-shot and periodic timers"\
                            "\r\n1. Period value is 1 second"\
                            "\r\n2. Period value is 2 seconds"\
                            "\r\n3. Period value is 3 seconds\r\n"

/* Macro for select the LED to blink in ULPT Periodic mode */
#define LED_USE             (0U)

typedef enum e_led_power
{
    LED_ON = BSP_IO_LEVEL_LOW,
    LED_OFF = BSP_IO_LEVEL_HIGH
} led_power_t;

extern bool g_internal_reset_flag;

/* Function declarations */
fsp_err_t hw_module_init(void);
fsp_err_t ulpt_set_period(void);
fsp_err_t ulpt_periodic_operation(void);
fsp_err_t ulpt_one_shot_operation(void);
void hw_module_deinit(void);
void handle_error(fsp_err_t err, const char * err_func, const char * err_str);

#endif /* ULPT_EP_H_ */

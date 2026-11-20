/***********************************************************************************************************************
 * File Name    : ulpt_ep.h
 * Description  : Contains macros, data structures, and functions used in ulpt_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2023 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef ULPT_EP_H_
#define ULPT_EP_H_

#include <stdbool.h>
#include "hal_data.h"

/* Macro for the reset value */
#define RESET_VALUE         (0x00)

/* Macro for the used LED to blink in ULPT Periodic mode */
#define LED_USE             (0U)

/* Macro for the buffer size */
#define BUFFER_SIZE         (16U)

/* Macro for the null char */
#define NULL_CHAR           ('\0')

/* Macros to define time-period value */
#define PERIOD_1S           (1U)
#define PERIOD_2S           (2U)
#define PERIOD_3S           (3U)
#define TIMES_MAX           (3U)

/* Macros for menu selection */
#define SELECT_1S           (1U)
#define SELECT_2S           (2U)
#define SELECT_3S           (3U)

typedef enum e_led_power
{
    LED_ON = BSP_IO_LEVEL_LOW,
    LED_OFF = BSP_IO_LEVEL_HIGH
} led_power_t;

/* External variable - Used for on-board LED */
extern bsp_leds_t g_bsp_leds;

/* External variable - Defined in ulpt_ep.c */
extern volatile bool g_internal_reset_flag;

/* Function declarations */
fsp_err_t ulpt_lpm_open(void);
fsp_err_t ulpt_low_power_mode_enter(void);
fsp_err_t ulpt_one_shot_timer_open(void);
fsp_err_t ulpt_periodic_timer_open(void);
fsp_err_t ulpt_timer_period_set(timer_ctrl_t * p_ctrl, uint32_t period_in_second);
fsp_err_t ulpt_periodic_start(void);
fsp_err_t ulpt_one_shot_start(void);
fsp_err_t ulpt_periodic_stop(void);
bool ulpt_periodic_underflow_consume(void);
bool ulpt_periodic_led_toggle(void);
void hw_module_deinit(void);

#endif /* ULPT_EP_H_ */

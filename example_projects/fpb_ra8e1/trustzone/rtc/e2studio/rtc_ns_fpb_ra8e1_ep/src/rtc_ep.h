/***********************************************************************************************************************
 * File Name    : rtc_ep.h
 * Description  : Contains macros, variables, and functions used in rtc_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef RTC_EP_H_
#define RTC_EP_H_

#include <stdbool.h>
#include <stddef.h>
#include "hal_data.h"

/* Macro for the reset value */
#define RESET_VALUE                 (0x00)

/* Macro for the input buffer size */
#define BUFFER_SIZE                 (32U)

/* Macros for commands to be received through the terminal */
#define SET_TIME                    (1U)
#define SET_ALARM                   (2U)
#define SET_PERIODIC_IRQ            (3U)
#define GET_CURRENT_TIME            (4U)

/* Macro for null character */
#define NULL_CHAR                   ('\0')

/* Macro for delay to be added */
#define LED_DELAY                   (2U)

/* Macros to adjust month and year values */
#define MON_ADJUST_VALUE            (1)
#define YEAR_ADJUST_VALUE           (1900)

/* Macro to convert ASCII character to integer digit */
#define ASCII_TO_DIGIT(ch)          ((int)((ch) - '0'))

/* Macros for user input processing */
#define PLACE_VALUE_TEN             (10)
#define PLACE_VALUE_HUNDRED         (100)
#define PLACE_VALUE_THOUSAND        (1000)

/* Macros for LED status based on specific board */
#if defined (BOARD_RA8T2_MCK)
#define LED_ON                      (BSP_IO_LEVEL_LOW)
#define LED_OFF                     (BSP_IO_LEVEL_HIGH)
#else
#define LED_ON                      (BSP_IO_LEVEL_HIGH)
#define LED_OFF                     (BSP_IO_LEVEL_LOW)
#endif /* BOARD_RA8T2_MCK */

/* Extern variables - Defined in rtc_ep.c */
extern volatile bool g_alarm_irq_flag;
extern volatile bool g_periodic_irq_flag;
extern volatile bool g_invalid_sequence;

/* Extern variable - Used for on-board LED */
extern bsp_leds_t g_bsp_leds;

/* Function declarations */
fsp_err_t rtc_init(void);
fsp_err_t set_rtc_calendar_time(char read_time[], rtc_time_t * time);
fsp_err_t set_rtc_calendar_alarm(char read_alarm[], rtc_alarm_time_t * alarm_time);
fsp_err_t set_rtc_periodic_rate(void);
fsp_err_t get_rtc_calendar_time(rtc_time_t * time);
void rtc_date_readability_update(rtc_time_t * time);
void rtc_deinit(void);
void rtc_callback(rtc_callback_args_t *p_args);
fsp_err_t led_level_toggle(void);
fsp_err_t led_level_set(bsp_io_level_t led_level);

#endif /* RTC_EP_H_ */

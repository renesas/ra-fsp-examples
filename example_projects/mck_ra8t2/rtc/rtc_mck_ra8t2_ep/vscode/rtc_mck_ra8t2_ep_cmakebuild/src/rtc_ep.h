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
#include "hal_data.h"

/* Macros for RTC operation command */
#define SET_TIME                                    (1U)
#define SET_ALARM                                   (2U)
#define SET_PERIODIC_IRQ                            (3U)
#define GET_CURRENT_TIME                            (4U)

/* Macro for the input buffer size */
#define BUFFER_SIZE                                 (32U)

/* Macro for the reset value */
#define RESET_VALUE                                 (0x00)

/* Macro for the null character */
#define NULL_CHAR                                   ('\0')

/* Macro for delay to be added */
#define LED_DELAY                                   (2U)

#if (BSP_PERIPHERAL_RTC_C_PRESENT)
/* In Zeller algorithm value of (-[Y/100] + [Y/400]) is 15 for Y = 2000 to Y = 2099) */
#define ZELLER_ALGM_CONST_FIFTEEN                   (15U)

/* Macro definitions for February and March months */
#define FEBRUARY_MONTH                              (2U)
#define MARCH_MONTH                                 (3U)

#define FIRST_DAY_OF_A_MONTH                        (1U)

/* Month: Valid range between 0 to 11 */
#define MONTHS_IN_A_YEAR                            (11)
#define YEAR_VALUE_MIN                              (100)
#define YEAR_VALUE_MAX                              (199)
#define LAST_DAY_OF_LEAP_FEB_MONTH                  (29U)
#endif /* BSP_PERIPHERAL_RTC_C_PRESENT */

/* Macros to adjust month and year values */
#define MON_ADJUST_VALUE                            (1)
#define YEAR_ADJUST_VALUE                           (1900)

/* Macro to enable 7 bit used to set days of Alarm Day-of-Week Register to 0111 1111 */
#define ALARMWW                                     (127U)

/* Macro to convert ASCII character to integer digit */
#define ASCII_TO_DIGIT(ch)                          ((int)((ch) - '0'))

/* Macros for user input processing */
#define PLACE_VALUE_TEN                             (10)
#define PLACE_VALUE_HUNDRED                         (100)
#define PLACE_VALUE_THOUSAND                        (1000)

/* Macros for LED status based on specific board */
#if defined (BOARD_RA8T2_MCK)
#define LED_ON                                      (BSP_IO_LEVEL_LOW)
#define LED_OFF                                     (BSP_IO_LEVEL_HIGH)
#else
#define LED_ON                                      (BSP_IO_LEVEL_HIGH)
#define LED_OFF                                     (BSP_IO_LEVEL_LOW)
#endif /* BOARD_RA8T2_MCK */

#if (BSP_PERIPHERAL_RTC_C_PRESENT)
#define MODULE_NAME                                 "r_rtc_c"
#define MODULE_PADDING                              "          "
#define R_RTC_OPEN(ctrl, cfg)                       (R_RTC_C_Open((ctrl), (cfg)))
#define R_RTC_CALENDAR_TIME_SET(ctrl, time)         (R_RTC_C_CalendarTimeSet((ctrl), (time)))
#define R_RTC_CALENDAR_TIME_GET(ctrl, time)         (R_RTC_C_CalendarTimeGet((ctrl), (time)))
#define R_RTC_CALENDAR_ALARM_SET(ctrl, alarm)       (R_RTC_C_CalendarAlarmSet((ctrl), (alarm)))
#define R_RTC_CALENDAR_ALARM_GET(ctrl, alarm)       (R_RTC_C_CalendarAlarmGet((ctrl), (alarm)))
#define R_RTC_PERIODIC_IRQ_RATE_SET(ctrl, rate)     (R_RTC_C_PeriodicIrqRateSet((ctrl), (rate)))
#define R_RTC_CLOSE(ctrl)                           (R_RTC_C_Close(ctrl))
#elif (BSP_PERIPHERAL_RTC_PRESENT) || (BSP_PERIPHERAL_IRTC_PRESENT)
#define MODULE_NAME                                 "r_rtc"
#define MODULE_PADDING                              "            "
#define R_RTC_OPEN(ctrl, cfg)                       (R_RTC_Open((ctrl), (cfg)))
#define R_RTC_CALENDAR_TIME_SET(ctrl, time)         (R_RTC_CalendarTimeSet((ctrl), (time)))
#define R_RTC_CALENDAR_TIME_GET(ctrl, time)         (R_RTC_CalendarTimeGet((ctrl), (time)))
#define R_RTC_CALENDAR_ALARM_SET(ctrl, alarm)       (R_RTC_CalendarAlarmSet((ctrl), (alarm)))
#define R_RTC_CALENDAR_ALARM_GET(ctrl, alarm)       (R_RTC_CalendarAlarmGet((ctrl), (alarm)))
#define R_RTC_PERIODIC_IRQ_RATE_SET(ctrl, rate)     (R_RTC_PeriodicIrqRateSet((ctrl), (rate)))
#define R_RTC_CLOSE(ctrl)                           (R_RTC_Close(ctrl))
#endif /* BSP_PERIPHERAL_RTC_C_PRESENT || BSP_PERIPHERAL_RTC_PRESENT || BSP_PERIPHERAL_IRTC_PRESENT */

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
fsp_err_t led_level_toggle(void);
fsp_err_t led_level_set(bsp_io_level_t led_level);
void rtc_date_readability_update(rtc_time_t * time);
void rtc_deinit(void);

#endif /* RTC_EP_H_ */

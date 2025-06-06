/***********************************************************************************************************************
 * File Name    : rtc_ep.h
 * Description  : Contains declarations of data structures and functions used in rtc_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef RTC_EP_H_
#define RTC_EP_H_

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
/* Macros for commands to be received through RTT input */
#define SET_TIME                            (1U)
#define SET_ALARM                           (2U)
#define SET_PERIODIC_IRQ                    (3U)
#define GET_CURRENT_TIME                    (4U)
#define GET_CURRENT_TIME                    (4U)

#define EP_INFO                "\r\nThe example project demonstrates RTC module."\
                               "\r\nUsing RTT input, the user can set RTC Calendar time and Calendar alarm"\
                               "\r\nThe user can enable periodic interrupt and view the current RTC Calendar time.\r\n"

#define MENU_INFO              "\r\n**************** RTC Application Menu ****************\r\n"\
                               "1 :  Set RTC Calendar Date and Time\r\n"\
                               "2 :  Set RTC Calendar Alarm Date and Time\r\n"\
                               "3 :  Set RTC Periodic IRQ Rate and Start it\r\n"\
                               "4 :  Get Current RTC Date and Time\r\n"\
                               "User Input :  "

/* Macros to flag the status */
#define SET_FLAG                            (0x01)
#define RESET_FLAG                          (0x00)

/* Macro for ASCII value of zero */
#define ASCII_ZERO                          (48)
/* Macro for null character */
#define NULL_CHAR                           ('\0')

/* Macro for checking if no byte is received */
#define BYTES_RECEIVED_ZERO                 (0U)

/* Macro for delay to be added */
#define LED_DELAY                           (2U)

#if (BSP_PERIPHERAL_RTC_C_PRESENT == 1U)
/* In Zeller algorithm value of (-[Y/100] + [Y/400]) is 15 for Y = 2000 to Y = 2099) */
#define ZELLER_ALGM_CONST_FIFTEEN           (15)

/* Macros definition for February and March months */
#define FEBRUARY_MONTH                      (2U)
#define MARCH_MONTH                         (3U)

#define FIRST_DAY_OF_A_MONTH                (1)
/* Month : valid range between 0 to 11. */
#define MONTHS_IN_A_YEAR                    (11)
#define LAST_DAY_OF_LEAP_FEB_MONTH          (29U)
#define YEAR_VALUE_MIN                      (100)
#define YEAR_VALUE_MAX                      (199)
#endif

/* Macros to adjust month and year values */
#define MON_ADJUST_VALUE                    (1)
#define YEAR_ADJUST_VALUE                   (1900)

/* Macro to enable 7 bit used to set days of Alarm Day-of-Week Register to 0111 1111 */
#define ALARMWW                             (127)

/* Macros for RTT input processing */
#define PLACE_VALUE_TEN                     (10)
#define PLACE_VALUE_HUNDRED                 (100)
#define PLACE_VALUE_THOUSAND                (1000)

/***********************************************************************************************************************
 * User-defined APIs
 **********************************************************************************************************************/

/* Function declarations */
fsp_err_t rtc_init(void);
fsp_err_t set_rtc_calendar_time(void);
fsp_err_t set_rtc_calendar_alarm(void);
fsp_err_t set_rtc_periodic_rate(void);
fsp_err_t get_rtc_calendar_time(void);
void rtc_date_readability_update(rtc_time_t * time);
void rtc_deinit(void);

#endif /* RTC_EP_H_ */

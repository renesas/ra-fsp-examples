/***********************************************************************************************************************
 * File Name    : rtc_ep.h
 * Description  : Contains declarations of data structures and functions used in rtc_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef RTC_EP_H_
#define RTC_EP_H_

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

/* Macros for commands to be received through RTT input */
#define SET_TIME            (1U)
#define SET_ALARM           (2U)
#define SET_PERIODIC_IRQ    (3U)
#define GET_CURRENT_TIME    (4U)

#define EP_INFO             "\r\nThe example project demonstrates RTC module."\
		                    "\r\nUsing RTT input, the user can set RTC calendar time and calendar alarm."\
							"\r\nThe user can enable periodic interrupt and view the current RTC calendar time.\r\n"

#define MENU_INFO           "\r\n**************** RTC Application Menu ****************\r\n"\
                            "1 :  Set RTC calendar Date and Time\r\n"\
                            "2 :  Set RTC calendar Alarm Date and Time\r\n"\
                            "3 :  Set RTC periodic IRQ rate and start it\r\n"\
                            "4 :  Get current RTC Date and Time\r\n"\
                            "User Input :  "

/* Macros to flag the status */
#define SET_FLAG                (0x01)
#define RESET_FLAG              (0x00)

/* Macro for ASCII value of zero */
#define ASCII_ZERO              (48)
/* Macro for null character */
#define NULL_CHAR               ('\0')

/* Macro for checking if no byte is received */
#define BYTES_RECEIVED_ZERO     (0U)

/* Macro for delay to be added */
#define LED_DELAY               (2U)
/* Macros to adjust month and year values */
#define MON_ADJUST_VALUE        (1)
#define YEAR_ADJUST_VALUE       (1900)

/* Macros for RTT input processing */
#define PLACE_VALUE_TEN         (10)
#define PLACE_VALUE_HUNDRED     (100)
#define PLACE_VALUE_THOUSAND    (1000)

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
void rtc_callback(rtc_callback_args_t *p_args);

#endif /* RTC_EP_H_ */

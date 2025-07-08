/***********************************************************************************************************************
 * File Name    : rtc.h
 * Description  : This file contains the declarations for RTC initialization,
 *                and the function for control RTC.
 * This header file is intended for use with the corresponding implementation
 * in rtc.c.
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef RTC_H_
#define RTC_H_

#include "apn_cpu0.h"

#define SET_TIME         (1U)
#define SET_ALARM        (2U)
#define NMI_STOP_TSN     (3U)

/* MACRO for ASCII value of zero */
#define ASCII_ZERO       (48)

/* MACRO for null character */
#define NULL_CHAR        ('\0')

/* MACROs for input processing */
#define PLACE_VALUE_TEN           (10)
#define PLACE_VALUE_HUNDRED       (100)
#define PLACE_VALUE_THOUSAND      (1000)
#define BYTES_RECEIVED_ZERO       (0)
/*MACROs to adjust month and year values */
#define MON_ADJUST_VALUE          (1)
#define YEAR_ADJUST_VALUE         (1900)

/***********************************************************************************************************************
 * User-defined APIs
 **********************************************************************************************************************/
void rtc_init(void);
void set_rtc_periodic_second(void);
void set_rtc_calendar_alarm(uint8_t sec_alarm);
void rtc_date_readability_update(rtc_time_t * time);
void rtc_deinit(void);
fsp_err_t set_rtc_calendar_time(uint8_t *pBuffer);

#endif /* RTC_H_ */

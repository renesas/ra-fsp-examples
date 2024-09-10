/**********************************************************************************************************************
 * File Name    : vir_time.h
 * Description  : Contains declarations of data structures and functions used in vir_time.c.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * Copyright (c) 2024 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *********************************************************************************************************************/

#ifndef VIR_TIME_H_
#define VIR_TIME_H_

#include "sntp_client_thread.h"
#include "ctype.h"
#include "time.h"

/**********************************************************************************************************************
 * Macro definitions
 *********************************************************************************************************************/
/* Macros to print info, error and trap the error */
#define PRINT_INFO_STR(str)  (app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_INFO_STR, sizeof(str), (str)))
#define PRINT_ERR_STR(str)   (app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_ERR_STR, sizeof(str), (str)))
#define ERROR_TRAP(err)      (app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_ERR_TRAP, sizeof(UINT *), &(err)))

/* MACRO for null character */
#define NULL_CHAR   ('\0')

/* MACRO for system timer */
#define YEAR_SHIFT                          (9)
#define MONTH_SHIFT                         (5)
#define HOUR_SHIFT                          (12)
#define MINUTE_SHIFT                        (6)
#define YEAR_MASK                           (0x7F)
#define MONTH_MASK                          (0x0F)
#define DAY_MASK                            (0x1F)
#define HOUR_MASK                           (0x1F)
#define MINUTE_MASK                         (0x3F)
#define SECOND_MASK                         (0x3F)
#define BASE_YEAR                           (1980)
#define MAXIMUM_YEAR                        (2107)
#define MAXIMUM_MONTH                       (12)
#define MAXIMUM_HOUR                        (23)
#define MAXIMUM_MINUTE                      (59)
#define MAXIMUM_SECOND                      (59)
#define INITIAL_DATE                        (0x4A21)   /* 01-01-2017 */
#define INITIAL_TIME                        (0x0000)   /* 12:00 am   */

#define INVALID_YEAR                        (0x12)
#define INVALID_MONTH                       (0x13)
#define INVALID_DAY                         (0x14)
#define INVALID_HOUR                        (0x15)
#define INVALID_MINUTE                      (0x16)
#define INVALID_SECOND                      (0x17)
#define PTR_ERROR                           (0x18)
#define SUCCESS                             (0)

/* MACRO for seconds update */
#define UPDATE_RATE_IN_SECONDS    (1)

/* Defines the number of ThreadX timer ticks required to achieve the update rate specified by
 * FX_UPDATE_RATE_IN_SECONDS defined previously. By default, the ThreadX timer tick is 10ms,
 * so the default value for this constant is 1000.  If TX_TIMER_TICKS_PER_SECOND is defined,
 * this value is derived from TX_TIMER_TICKS_PER_SECOND */
#define UPDATE_RATE_IN_TICKS    (TX_TIMER_TICKS_PER_SECOND * UPDATE_RATE_IN_SECONDS)

#define MONTH_STR_LEN                   (3U)
#define MONTH_INDEX                     (0U)
#define DATE_INDEX                      (4U)
#define YEAR_INDEX                      (7U)
#define HOUR_INDEX                      (0U)
#define MIN_INDEX                       (3U)
#define SEC_INDEX                       (6U)

/* MACRO for max bytes received */
#define MAX_BYTES            (32U)
#define TIME_BUFF_SIZE       (10U)

/* MACROs to adjust month and year values */
#define MON_ADJUST_VALUE      (1)
#define YEAR_ADJUST_VALUE     (1900)

/* Time zone offset value can be changed as per required timezone
 * to change the offset value, Refer -
 * https://www.epochconverter.com/timezones
 * All region time zone offset value are present in above URL */
/* Time zone offset value as per IST */
/* Eg.For Kolkata (IST) the offset is +19800, Hence the #define TIME_ZONE_OFFSET (+19800) */
#define TIME_ZONE_OFFSET      (19800)

/**********************************************************************************************************************
 * Typedef definitions
 *********************************************************************************************************************/
/* ENUM for ASCII code */
typedef enum e_ascii_codes
{
    ASCII_ZERO_CODE = 48,
    ASCII_CHAR_A    = 65,
    ASCII_LOWER_A   = 0x61,
    ASCII_LOWER_N   = 0x6E,
    ASCII_LOWER_R   = 0x72,
    ASCII_LOWER_P   = 0x70
}ascii_codes_t;

/* ENUM for month conversion processing */
typedef enum e_month_convert_val
{
    JAN_JUN_JUL  = 0x4A,
    FEB          = 0x46,
    MAR_MAY      = 0x4D,
    APR_AUG      = 0x41,
    SEP          = 0x53,
    OCT          = 0x4F,
    NOV          = 0x4E,
    DEC          = 0x44
}month_convert_val_t;

/* Structure to store date and time */
typedef struct tm vtime_t;

/**********************************************************************************************************************
 * Public function prototypes
 *********************************************************************************************************************/
UINT timer_init_and_display_time(void);
UINT update_time_stamp_to_system_block(char * const time_data);

#endif /* VIR_TIME_H_ */

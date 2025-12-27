/***********************************************************************************************************************
 * File Name    : rtc_app.h
 * Description  : Contains declarations of data structures and functions used in rtc_app.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef RTC_APP_H_
#define RTC_APP_H_

#include "sntp_client_thread.h"
#include "ctype.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

/* Macros to print information, errors, and trap errors */
#define PRINT_INFO_STR(str)     (app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_INFO_STR, sizeof(str), (str)))
#define PRINT_ERR_STR(str)      (app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_ERR_STR, sizeof(str), (str)))
#define ERROR_TRAP(err)         (app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_ERR_TRAP, sizeof(UINT *), &(err)))

/* Macro for the null character */
#define NULL_CHAR               ('\0')

/* Macros defining the maximum number of bytes received */
#define MAX_BYTES               (32U)
#define TIME_BUFF_SIZE          (10U)

/* Macros to adjust month and year values */
#define MON_ADJUST_VALUE        (1)
#define YEAR_ADJUST_VALUE       (1900)

/* The time zone offset value can be modified as needed.
 * To change the offset, refer to:
 * https://www.epochconverter.com/timezones
 * The offset values for all regions are listed on the above website.
 * The default offset below corresponds to IST (Indian Standard Time). */
#define TIME_ZONE_OFFSET        (19800)

/* Enumeration for ASCII codes used in month parsing */
typedef enum e_ascii_codes
{
    ASCII_ZERO_CODE = 48,
    ASCII_CHAR_A    = 65,
    ASCII_LOWER_A   = 0x61,
    ASCII_LOWER_N   = 0x6E,
    ASCII_LOWER_R   = 0x72,
    ASCII_LOWER_P   = 0x70
} ascii_codes_t;

/* Enumeration for month conversion processing */
typedef enum e_month_convert_val
{
    JAN_JUN_JUL = 0x4A,
    FEB         = 0x46,
    MAR_MAY     = 0x4D,
    APR_AUG     = 0x41,
    SEP         = 0x53,
    OCT         = 0x4F,
    NOV         = 0x4E,
    DEC         = 0x44
} month_convert_val_t;

/* Function declarations */
fsp_err_t update_time_stamp_to_rtc_block(char * const time_data);
fsp_err_t rtc_init_and_display_time(void);
fsp_err_t set_rtc_calendar_time(rtc_time_t * set_time);
fsp_err_t get_rtc_calendar_time(rtc_time_t * get_time);
void rtc_deinit(void);
void rtc_month_year_readability_update(rtc_time_t * time);
void rtc_month_year_time_format_update(rtc_time_t * time);
void assign_month_value(rtc_time_t * time, char * read_buffer);
void convert_network_timestamp_to_rtc_block(rtc_time_t * time, char * read_buffer);

#endif /* RTC_APP_H_ */

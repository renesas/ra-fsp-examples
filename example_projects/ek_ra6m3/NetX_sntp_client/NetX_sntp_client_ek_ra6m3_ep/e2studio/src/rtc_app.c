/***********************************************************************************************************************
 * File Name    : rtc_app.c
 * Description  : Contains data structures and functions used in rtc_app.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "rtc_app.h"

/*******************************************************************************************************************//**
 * @addtogroup rtc_ep
 * @{
 **********************************************************************************************************************/

/* Variable to store time */
static rtc_time_t g_set_time =  {
    .tm_hour    =  RESET_VALUE,
    .tm_isdst   =  RESET_VALUE,
    .tm_mday    =  RESET_VALUE,
    .tm_min     =  RESET_VALUE,
    .tm_mon     =  RESET_VALUE,
    .tm_sec     =  RESET_VALUE,
    .tm_wday    =  RESET_VALUE,
    .tm_yday    =  RESET_VALUE,
    .tm_year    =  RESET_VALUE,
   };
static rtc_time_t g_present_time =  {
 .tm_hour    =  RESET_VALUE,
 .tm_isdst   =  RESET_VALUE,
 .tm_mday    =  RESET_VALUE,
 .tm_min     =  RESET_VALUE,
 .tm_mon     =  RESET_VALUE,
 .tm_sec     =  RESET_VALUE,
 .tm_wday    =  RESET_VALUE,
 .tm_yday    =  RESET_VALUE,
 .tm_year    =  RESET_VALUE,
                                    };

/*******************************************************************************************************************//**
 * @brief       This functions initializes RTC module and display RTC Time Block.
 * @param[IN]   None
 * @retval      FSP_SUCCESS                  Upon successful open of RTC module
 * @retval      Any Other Error code apart from FSP_SUCCESS  Unsuccessful open
 **********************************************************************************************************************/
fsp_err_t rtc_init_and_display_time(void)
{
    fsp_err_t err = FSP_SUCCESS;     // Error status

    /* Open RTC module */
    err = R_RTC_Open(&g_rtc_ctrl, &g_rtc_cfg);
    if (FSP_SUCCESS != err)
    {
        PRINT_ERR_STR ("RTC module open failed.\r\nRestart the Application.");
        return err;
    }
    PRINT_INFO_STR("RTC Initialization completed successfully.");

    /* Extract current Date and Time from compiler MACROS */
    char read_time[MAX_BYTES] = {NULL_CHAR};
    strcat(read_time, __DATE__);
    strcat(read_time," ");
    strcat(read_time, __TIME__);

    /* Date and Time extracted from MACROs are converted to time format so that,
     *  this can be used to add time stamping to RTC */
    g_set_time.tm_mday = atoi(&read_time[4]);
    g_set_time.tm_year = atoi(&read_time[7]);
    g_set_time.tm_hour = atoi(&read_time[12]);
    g_set_time.tm_min  = atoi(&read_time[15]);
    g_set_time.tm_sec  = atoi(&read_time[18]);

    /* Convert returned month name to month value */
    assign_month_value(&g_set_time, &read_time[0]);
    /* Modify the month and year in standard time format */
    rtc_month_year_time_format_update(&g_set_time);

    /* Set project build time to RTC.*/
    err = set_rtc_calendar_time(&g_set_time);
    if(FSP_SUCCESS != err)
    {
        PRINT_ERR_STR ("set_rtc_calendar_time function failed.");
        /* close the RTC module.*/
        rtc_deinit();
        return err;
    }

    /* Get RTC current time.*/
    err = get_rtc_calendar_time(&g_present_time);
    if(FSP_SUCCESS != err)
    {
        PRINT_ERR_STR ("get_rtc_calendar_time function failed.");
        /* close the RTC module.*/
        rtc_deinit();
        return err;
    }

    /* Modify the month and year in user readable format */
    rtc_month_year_readability_update(&g_present_time);
    /* Print the current RTC Date and Time in user readable format */
    PRINT_INFO_STR("Acquired current RTC date and time successfully, Current RTC info is :");
    app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_PRINT_DATE_TIME, sizeof(rtc_time_t), &g_present_time);
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This functions sets the Calendar time.
 * @param[IN]   None
 * @retval      FSP_SUCCESS                  Upon successful Calendar time set
 * @retval      Any Other Error code apart from FSP_SUCCESS  Unsuccessful Calendar time set
 **********************************************************************************************************************/
fsp_err_t set_rtc_calendar_time(rtc_time_t * set_time)
{
    fsp_err_t err = FSP_SUCCESS;     // Error status

    /* Set the time provided by user */
    err = R_RTC_CalendarTimeSet(&g_rtc_ctrl, set_time);
    if (FSP_SUCCESS != err)
    {
        PRINT_ERR_STR("R_RTC_CalendarTimeSet API failed.");
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This functions gets the current RTC time.
 * @param[IN]   None
 * @retval      FSP_SUCCESS                  Upon successful current RTC time get
 * @retval      Any Other Error code apart from FSP_SUCCESS  Unsuccessful current RTC time get
 **********************************************************************************************************************/
fsp_err_t get_rtc_calendar_time(rtc_time_t * get_time)
{
    fsp_err_t err = FSP_SUCCESS;     // Error status

    /* Get the current RTC Calendar time */
    err = R_RTC_CalendarTimeGet(&g_rtc_ctrl, get_time);
    if (FSP_SUCCESS != err)
    {
        PRINT_ERR_STR("R_RTC_CalendarTimeGet API failed.");
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief This function closes opened RTC module before the project ends up in an Error Trap.
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
void rtc_deinit(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Close RTC module */
    err = R_RTC_Close(&g_rtc_ctrl);
    if (FSP_SUCCESS != err)
    {
        /* RTC Close failure message */
        PRINT_ERR_STR("** RTC module Close failed ** ");
    }
}

/*******************************************************************************************************************//**
 * @brief       This functions assigns month value based on network returned month.
 * @param[IN]   time                    date and time to be modified
 * @param[IN]   read_buffer             Buffer that contains user input time values
 * @retval      None
 **********************************************************************************************************************/
void assign_month_value(rtc_time_t * time, char * read_buffer)
{
    /* This compiler MACRO that had date value is processed using the following conversion, where ASCII value is converted to
     * integer value and month value is assigned. */
    switch (read_buffer[0])
    {
        case JAN_JUN_JUL:
        {
            if(ASCII_LOWER_A == read_buffer[1])
            {
                time->tm_mon = JANUARY;
            }
            else if(ASCII_LOWER_N == read_buffer[2])
            {
                time->tm_mon = JUNE;
            }
            else
            {
                time->tm_mon = JULY;
            }
        }
        break;
        case FEB:
        {
            time->tm_mon = FEBRUARY;
        }
        break;
        case MAR_MAY:
        {
            if(ASCII_LOWER_R == read_buffer[2])
            {
                time->tm_mon = MARCH;
            }
            else
            {
                time->tm_mon = MAY;
            }
        }
        break;
        case APR_AUG:
        {
            if(ASCII_LOWER_P == read_buffer[1])
            {
                time->tm_mon = APRIL;
            }
            else
            {
                time->tm_mon = AUGUST;
            }
        }
        break;
        case SEP:
        {
            time->tm_mon = SEPTEMBER;
        }
        break;
        case OCT:
        {
            time->tm_mon = OCTOBER;
        }
        break;
        case NOV:
        {
            time->tm_mon = NOVEMBER;
        }
        break;
        case DEC:
        {
            time->tm_mon = DECEMBER;
        }
        break;
        default :
        {

        }
        break;
    }
}

/*******************************************************************************************************************//**
 * @brief       This functions updates network time stamp to RTC time format.
 * @param[IN]   time_data    Network time data
 * @retval      None
 **********************************************************************************************************************/
fsp_err_t update_time_stamp_to_rtc_block(char * const time_data)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Variable to store time */
    rtc_time_t set_time         = {RESET_VALUE};
    rtc_time_t disp_time        = {RESET_VALUE};
    rtc_time_t current_rtc_time = {RESET_VALUE};
    rtc_time_t present_rtc_time = {RESET_VALUE};
    time_t     epoch_val        = (time_t)RESET_VALUE;

    /* SNTP Client query Date and Time are converted to time format so that,
     *  this can be used to add time stamping and comparison with current RTC time. */
    convert_network_timestamp_to_rtc_block(&set_time, &time_data[0]);

    /* Convert returned month name to month value */
    assign_month_value(&set_time, &time_data[0]);
    /* Modify the month and year in standard time format */
    rtc_month_year_time_format_update(&set_time);

    /* convert sntp client time to epoch value*/
    epoch_val = mktime(&set_time);
    /* update epoch value as per time zone. */
    epoch_val += TIME_ZONE_OFFSET;

    /* Convert the epoch value to user readable format.*/
    disp_time = *localtime(&epoch_val);

    /* Modify the month and year in user readable format */
    rtc_month_year_readability_update(&disp_time);
    /* Print the current Network Date and Time in user readable format */
    PRINT_INFO_STR("Acquired network date and time successfully, Current Network info is :");
    app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_PRINT_DATE_TIME, sizeof(rtc_time_t), &disp_time);

    /* Get RTC current time.*/
    err = get_rtc_calendar_time(&current_rtc_time);
    if(FSP_SUCCESS != err)
    {
        PRINT_ERR_STR ("get_rtc_calendar_time function failed.");
        return err;
    }

    /* Modify the month and year in user readable format */
    rtc_month_year_readability_update(&current_rtc_time);

    /* Compare network time and current RTC time.*/
    if(RESET_VALUE == memcmp(&current_rtc_time, &disp_time, sizeof(rtc_time_t)))
    {
        PRINT_INFO_STR("No Mismatch observed between network time and current RTC time, so time update to RTC is not required.");
    }
    else
    {
        PRINT_INFO_STR("Mismatch observed between network time and current RTC time, so updating current RTC time...");
        /* Modify the month and year in time format */
        rtc_month_year_time_format_update(&disp_time);
        /* Set network time to RTC.*/
        err = set_rtc_calendar_time(&disp_time);
        if(FSP_SUCCESS != err)
        {
            PRINT_ERR_STR ("set_rtc_calendar_time function failed.");
            return err;
        }

        /* Get RTC current time.*/
        err = get_rtc_calendar_time(&present_rtc_time);
        if(FSP_SUCCESS != err)
        {
            PRINT_ERR_STR ("get_rtc_calendar_time function failed.");
            return err;
        }

        /* Modify the month and year in user readable format */
        rtc_month_year_readability_update(&present_rtc_time);
        /* Print the updated RTC Date and Time in user readable format */
        PRINT_INFO_STR("Updated RTC with current network time and updated RTC info :");
        app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_PRINT_DATE_TIME, sizeof(rtc_time_t), &present_rtc_time);
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This functions converts network date and time value into RTC block format.
 * @param[IN]   time                    date and time to be modified
 * @param[IN]   read_buffer             Buffer that contains user input time values
 * @retval      None
 **********************************************************************************************************************/
void convert_network_timestamp_to_rtc_block(rtc_time_t * time, char * read_buffer)
{
    CHAR * delimeter = NULL;
    int32_t time_data[TIME_BUFF_SIZE] = {RESET_VALUE};
    uint8_t cnt = RESET_VALUE;

    /* extract all delimiters. */
    while((delimeter = (char *)strtok_r(read_buffer," ,:.", &read_buffer)))
    {
        time_data[cnt++] = atoi(delimeter);
    }

    /* update the extracted time stamps to RTC time blocks.*/
    time->tm_mday = time_data[1];
    time->tm_year = time_data[2];
    time->tm_hour = time_data[3];
    time->tm_min  = time_data[4];
    time->tm_sec  = time_data[5];
}

/*******************************************************************************************************************//**
 * @brief       This functions modifies the month and year in readable format to the user.
 * @param[IN]   time                    month and year to be modified
 * @retval      None
 **********************************************************************************************************************/
void rtc_month_year_readability_update(rtc_time_t * time)
{
    time->tm_mon  +=  MON_ADJUST_VALUE;
    time->tm_year +=  YEAR_ADJUST_VALUE;
}

/*******************************************************************************************************************//**
 * @brief       This functions modifies the month and year to time format.
 * @param[IN]   time                    month and year to be modified
 * @retval      None
 **********************************************************************************************************************/
void rtc_month_year_time_format_update(rtc_time_t * time)
{
    time->tm_mon  -=  MON_ADJUST_VALUE;
    time->tm_year -=  YEAR_ADJUST_VALUE;
}

/*******************************************************************************************************************//**
 * @} (end addtogroup rtc_ep)
 **********************************************************************************************************************/

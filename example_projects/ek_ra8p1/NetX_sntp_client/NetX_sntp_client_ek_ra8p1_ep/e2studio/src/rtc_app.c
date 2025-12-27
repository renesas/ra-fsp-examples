/***********************************************************************************************************************
 * File Name    : rtc_app.c
 * Description  : Contains data structures and functions used in rtc_app.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
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
static rtc_time_t g_set_time =
{
 .tm_hour  = RESET_VALUE,
 .tm_isdst = RESET_VALUE,
 .tm_mday  = RESET_VALUE,
 .tm_min   = RESET_VALUE,
 .tm_mon   = RESET_VALUE,
 .tm_sec   = RESET_VALUE,
 .tm_wday  = RESET_VALUE,
 .tm_yday  = RESET_VALUE,
 .tm_year  = RESET_VALUE,
};
static rtc_time_t g_present_time =
{
 .tm_hour  = RESET_VALUE,
 .tm_isdst = RESET_VALUE,
 .tm_mday  = RESET_VALUE,
 .tm_min   = RESET_VALUE,
 .tm_mon   = RESET_VALUE,
 .tm_sec   = RESET_VALUE,
 .tm_wday  = RESET_VALUE,
 .tm_yday  = RESET_VALUE,
 .tm_year  = RESET_VALUE,
};

/*******************************************************************************************************************//**
 * @brief       This function initializes the RTC module and displays RTC time block.
 * @param[in]   None.
 * @retval      FSP_SUCCESS     RTC module opened successfully.
 * @retval      Any other error code apart from FSP_SUCCESS indicates an unsuccessful open.
 **********************************************************************************************************************/
fsp_err_t rtc_init_and_display_time(void)
{
    fsp_err_t err = FSP_SUCCESS;     /* Error status */

    /* Open RTC module */
    err = R_RTC_Open(&g_rtc_ctrl, &g_rtc_cfg);
    if (FSP_SUCCESS != err)
    {
        PRINT_ERR_STR("RTC module open failed.\r\nRestart the application.");
        return err;
    }
    PRINT_INFO_STR("RTC initialization completed successfully.");

    /* Extract the current date and time from compiler macros */
    char read_time[MAX_BYTES] = {NULL_CHAR};
    strcat(read_time, __DATE__);
    strcat(read_time," ");
    strcat(read_time, __TIME__);

    /* The date and time extracted from macros are converted to time format
     * so that they can be used to add a timestamp to the RTC. */
    g_set_time.tm_mday = atoi(&read_time[4]);
    g_set_time.tm_year = atoi(&read_time[7]);
    g_set_time.tm_hour = atoi(&read_time[12]);
    g_set_time.tm_min  = atoi(&read_time[15]);
    g_set_time.tm_sec  = atoi(&read_time[18]);

    /* Convert returned month name to month value */
    assign_month_value(&g_set_time, &read_time[0]);
    /* Adjust the month and year to standard time format */
    rtc_month_year_time_format_update(&g_set_time);

    /* Set project build time to RTC */
    err = set_rtc_calendar_time(&g_set_time);
    if (FSP_SUCCESS != err)
    {
        PRINT_ERR_STR("set_rtc_calendar_time function failed.");
        /* Close the RTC module */
        rtc_deinit();
        return err;
    }

    /* Get RTC current time */
    err = get_rtc_calendar_time(&g_present_time);
    if (FSP_SUCCESS != err)
    {
        PRINT_ERR_STR("get_rtc_calendar_time function failed.");
        /* Close the RTC module */
        rtc_deinit();
        return err;
    }

    /* Adjust the month and year to a user-readable format */
    rtc_month_year_readability_update(&g_present_time);
    /* Print the current RTC date and time in user-readable format */
    PRINT_INFO_STR("Acquired current RTC date and time successfully, current RTC info is:");
    app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_PRINT_DATE_TIME, sizeof(rtc_time_t), &g_present_time);
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function sets the calendar time in the RTC module.
 * @param[in]   set_time        Pointer to the time structure containing the desired calendar time.
 * @retval      FSP_SUCCESS     Calendar time set successfully.
 * @retval      Any other error code apart from FSP_SUCCESS indicates an unsuccessful calendar time set.
 **********************************************************************************************************************/
fsp_err_t set_rtc_calendar_time(rtc_time_t * set_time)
{
    fsp_err_t err = FSP_SUCCESS;     /* Error status */

    /* Set the time provided by user */
    err = R_RTC_CalendarTimeSet(&g_rtc_ctrl, set_time);
    if (FSP_SUCCESS != err)
    {
        PRINT_ERR_STR("R_RTC_CalendarTimeSet API failed.");
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function gets the current RTC time.
 * @param[in]   get_time        Pointer to the structure that will store the current RTC time.
 * @retval      FSP_SUCCESS     RTC time retrieved successfully.
 * @retval      Any other error code apart from FSP_SUCCESS indicates an unsuccessful RTC time read.
 **********************************************************************************************************************/
fsp_err_t get_rtc_calendar_time(rtc_time_t * get_time)
{
    fsp_err_t err = FSP_SUCCESS;     /* Error status */

    /* Get the current RTC Calendar time */
    err = R_RTC_CalendarTimeGet(&g_rtc_ctrl, get_time);
    if (FSP_SUCCESS != err)
    {
        PRINT_ERR_STR("R_RTC_CalendarTimeGet API failed.");
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function closes the RTC module before the project ends in an error trap.
 * @param[in]   None.
 * @retval      None.
 **********************************************************************************************************************/
void rtc_deinit(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Close RTC module */
    err = R_RTC_Close(&g_rtc_ctrl);
    if (FSP_SUCCESS != err)
    {
        /* Failed to close the RTC module */
        PRINT_ERR_STR("** R_RTC_Close API failed **");
    }
}

/*******************************************************************************************************************//**
 * @brief       Assigns the month value based on the month returned from the network.
 * @param[in]   time         Date and time structure to be modified.
 * @param[in]   read_buffer  Buffer that contains the received time string.
 * @retval      None.
 **********************************************************************************************************************/
void assign_month_value(rtc_time_t * time, char * read_buffer)
{
    /* The date value from the compiler macro is processed using the following conversion,
     * where ASCII characters are converted to integers and the corresponding month value is assigned. */
    switch (read_buffer[0])
    {
        case JAN_JUN_JUL:
        {
            if (ASCII_LOWER_A == read_buffer[1])
            {
                time->tm_mon = JANUARY;
            }
            else if (ASCII_LOWER_N == read_buffer[2])
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
            if (ASCII_LOWER_R == read_buffer[2])
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
            if (ASCII_LOWER_P == read_buffer[1])
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
            /* Do nothing */
        }
        break;
    }
}

/*******************************************************************************************************************//**
 * @brief       This function updates the network timestamp to the RTC time format.
 * @param[in]   time_data    Pointer to the character buffer containing the network time data.
 * @retval      None.
 **********************************************************************************************************************/
fsp_err_t update_time_stamp_to_rtc_block(char * const time_data)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Variable to store time */
    rtc_time_t set_time         = {0};
    rtc_time_t disp_time        = {0};
    rtc_time_t current_rtc_time = {0};
    rtc_time_t present_rtc_time = {0};
    time_t     epoch_val        = (time_t)0;

    /* The SNTP client’s queried date and time are converted to time format
     * so that they can be used to add a timestamp and compare with the current RTC time. */
    convert_network_timestamp_to_rtc_block(&set_time, &time_data[0]);

    /* Convert the returned month name to its corresponding month value */
    assign_month_value(&set_time, &time_data[0]);
    /* Adjust the month and year to the standard time format */
    rtc_month_year_time_format_update(&set_time);

    /* Convert SNTP client time to epoch value */
    epoch_val = mktime(&set_time);
    /* Update the epoch value according to the time zone */
    epoch_val += TIME_ZONE_OFFSET;

    /* Convert the epoch value to a user-readable format */
    disp_time = *localtime(&epoch_val);

    /* Adjust the month and year to a user-readable format */
    rtc_month_year_readability_update(&disp_time);
    /* Print the current network date and time in a user-readable format */
    PRINT_INFO_STR("Acquired network date and time successfully. Current Network info:");
    app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_PRINT_DATE_TIME, sizeof(rtc_time_t), &disp_time);

    /* Get the RTC current time */
    err = get_rtc_calendar_time(&current_rtc_time);
    if (FSP_SUCCESS != err)
    {
        PRINT_ERR_STR("get_rtc_calendar_time function failed.");
        return err;
    }

    /* Adjust the month and year to a user-readable format */
    rtc_month_year_readability_update(&current_rtc_time);

    /* Compare network time and current RTC time */
    if (RESET_VALUE == memcmp(&current_rtc_time, &disp_time, sizeof(rtc_time_t)))
    {
        PRINT_INFO_STR("No mismatch observed between network time and current RTC time. RTC update is not required.");
    }
    else
    {
        PRINT_INFO_STR("Mismatch observed between network time and current RTC time. Updating RTC time...");
        /* Adjust the month and year in time format */
        rtc_month_year_time_format_update(&disp_time);
        /* Set network time to RTC */
        err = set_rtc_calendar_time(&disp_time);
        if (FSP_SUCCESS != err)
        {
            PRINT_ERR_STR("set_rtc_calendar_time function failed.");
            return err;
        }

        /* Get the updated RTC time */
        err = get_rtc_calendar_time(&present_rtc_time);
        if (FSP_SUCCESS != err)
        {
            PRINT_ERR_STR("get_rtc_calendar_time function failed.");
            return err;
        }

        /* Adjust the month and year to a user-readable format */
        rtc_month_year_readability_update(&present_rtc_time);
        /* Print the updated RTC date and time in a user-readable format */
        PRINT_INFO_STR("RTC updated with current network time. Updated RTC info:");
        app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_PRINT_DATE_TIME, sizeof(rtc_time_t), &present_rtc_time);
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function converts network date and time values into RTC block format.
 * @param[in]   time            Date and time to be modified.
 * @param[in]   read_buffer     Buffer that contains user input time values.
 * @retval      None.
 **********************************************************************************************************************/
void convert_network_timestamp_to_rtc_block(rtc_time_t * time, char * read_buffer)
{
    char * delimiter = NULL;
    int time_data[TIME_BUFF_SIZE] = {RESET_VALUE};
    uint8_t cnt = RESET_VALUE;

    /* Extract all delimiters */
    while ((delimiter = (char *)strtok_r(read_buffer," ,:.", &read_buffer)))
    {
        time_data[cnt++] = atoi(delimiter);
    }

    /* Update the extracted timestamp to the RTC time structure */
    time->tm_mday = time_data[1];
    time->tm_year = time_data[2];
    time->tm_hour = time_data[3];
    time->tm_min  = time_data[4];
    time->tm_sec  = time_data[5];
}

/*******************************************************************************************************************//**
 * @brief       This function adjusts the month and year to a user-readable format.
 * @param[in]   time    Pointer to the time structure whose month and year will be adjusted.
 * @retval      None.
 **********************************************************************************************************************/
void rtc_month_year_readability_update(rtc_time_t * time)
{
    time->tm_mon  += MON_ADJUST_VALUE;
    time->tm_year += YEAR_ADJUST_VALUE;
}

/*******************************************************************************************************************//**
 * @brief       This function adjusts the month and year to RTC time format.
 * @param[in]   time    Pointer to the time structure whose month and year will be adjusted.
 * @retval      None.
 **********************************************************************************************************************/
void rtc_month_year_time_format_update(rtc_time_t * time)
{
    time->tm_mon  -= MON_ADJUST_VALUE;
    time->tm_year -= YEAR_ADJUST_VALUE;
}

/*******************************************************************************************************************//**
 * @} (end addtogroup rtc_ep)
 **********************************************************************************************************************/

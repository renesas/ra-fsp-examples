/**********************************************************************************************************************
 * File Name    : vir_time.c
 * Description  : Contains data structures and functions used in vir_time.c.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * Copyright (c) 2024 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *********************************************************************************************************************/

#include "common_utils.h"
#include "vir_time.h"

/**********************************************************************************************************************
 * Private variables to store timer
 *********************************************************************************************************************/
TX_TIMER g_virtual_timer;
UINT system_date;
UINT system_time;
static vtime_t g_time = {RESET_VALUE};

/**********************************************************************************************************************
 * Private function prototypes
 *********************************************************************************************************************/
static void vir_time_entry();
static UINT vir_time_set(UINT hour, UINT minute, UINT second);
static UINT vir_date_set(UINT year, UINT month, UINT day);
static UINT vir_date_get(INT *year, INT *month, INT *day);
static UINT vir_time_get(INT *hour, INT *minute, INT *second);
static void sys_month_year_readability_update(vtime_t * time);
static void sys_month_year_time_format_update(vtime_t * time);
static void assign_month_value(vtime_t * time, char * read_buffer);
static void convert_network_timestamp_to_system_block(vtime_t * time, char * read_buffer);

/**********************************************************************************************************************
 * Function Name: timer_init_and_display_time
 * Description  : This function initializes and displays timer.
 * Arguments    : None
 * Return value : SUCCESS   Upon successful create timer
 *                Any other Error code apart from SUCCESS
 *********************************************************************************************************************/
UINT timer_init_and_display_time(void)
{
    UINT err = SUCCESS;

    /* Initialize to store the timer */
    system_date = INITIAL_DATE;
    system_time = INITIAL_TIME;

    /* Create timer based on the tick count of Azure RTOS */
    tx_timer_create(&g_virtual_timer, "System Virtual", vir_time_entry, 0,
                    UPDATE_RATE_IN_TICKS, UPDATE_RATE_IN_TICKS, TX_AUTO_ACTIVATE);

    /* Extract current Date and Time from compiler MACROS */
    char read_time[MAX_BYTES] = {NULL_CHAR};
    strcat(read_time, __DATE__);
    strcat(read_time," ");
    strcat(read_time, __TIME__);

    /* Date and Time extracted from MACROs are converted to time format so that,
     * this can be used to add time stamping to system timer */
    g_time.tm_mday = atoi(&read_time[4]);
    g_time.tm_year = atoi(&read_time[7]);
    g_time.tm_hour = atoi(&read_time[12]);
    g_time.tm_min  = atoi(&read_time[15]);
    g_time.tm_sec  = atoi(&read_time[18]);

    /* Convert returned month name to month value */
    assign_month_value(&g_time, &read_time[0]);

    /* Set project build Year, Month, Date to system timer */
    err = vir_date_set(g_time.tm_year, g_time.tm_mon, g_time.tm_mday);
    if (SUCCESS != err)
    {
        PRINT_ERR_STR ("\r\nSystem date set failed.\r\n");
        return err;
    }
    /* Set project build Hour, Minute, Second to system timer */
    err = vir_time_set(g_time.tm_hour, g_time.tm_min, g_time.tm_sec);
    if (SUCCESS != err)
    {
        PRINT_ERR_STR ("\r\nSystem time set failed.\r\n");
        return err;
    }

    /* Get project build Year, Month, Date from system timer */
    err = vir_date_get(&g_time.tm_year, &g_time.tm_mon, &g_time.tm_mday);
    if (SUCCESS != err)
    {
        PRINT_ERR_STR ("\r\nSystem date get failed.\r\n");
        return err;
    }
    /* Get project build Hour, Minute, Second from system timer */
    err = vir_time_get(&g_time.tm_hour, &g_time.tm_min, &g_time.tm_sec);
    if (SUCCESS != err)
    {
        PRINT_ERR_STR ("\r\nSystem time get failed.\r\n");
        return err;
    }

    /* Print the current system Date and Time in user readable format */
    PRINT_INFO_STR("Created current system date and time successfully, Current system Date and time info:");
    app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_PRINT_DATE_TIME, sizeof(vtime_t),&g_time);
    return err;
}
/**********************************************************************************************************************
 * End of function timer_init_and_display_time
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: vir_time_entry
 * Description  : This function is a callback for created system timer.
 *                The callback happens every UPDATE_RATE_IN_SECONDS to update the system clock.
 * Arguments    : None
 * Return value : None
 *********************************************************************************************************************/
static void vir_time_entry()
{
    UINT second;
    UINT minute;
    UINT hour;
    UINT day;
    UINT month;
    UINT year;
    /* Break the current date time into separate fields for easier work! */
    second =  (system_time & SECOND_MASK);
    minute =  (system_time >> MINUTE_SHIFT) & MINUTE_MASK;
    hour =    (system_time >> HOUR_SHIFT) & HOUR_MASK;
    day =     system_date & DAY_MASK;
    month =   (system_date >> MONTH_SHIFT) & MONTH_MASK;
    year =    ((system_date >> YEAR_SHIFT) & YEAR_MASK) + BASE_YEAR;

    /* Update the second */
    second =  second + UPDATE_RATE_IN_SECONDS;

    /* Adjust the minute field */
    if (second > MAXIMUM_SECOND)
    {
        minute =  minute + second / 60;
        second =  second % 60;

        /* Adjust the hour field */
        if (minute > MAXIMUM_MINUTE)
        {
            hour =    hour + minute / 60;
            minute =  minute % 60;

            /* Adjust the day field */
            if (hour > MAXIMUM_HOUR)
            {
                hour =  0;
                day++;

                /* Adjust the month field */
                switch (month)
                {
                case 1:                 /* January */
                {
                    /* Check for end of the month */
                    if (day > 31)
                    {
                        /* Move to next month */
                        day = 1;
                        month++;
                    }
                    break;
                }
                case 2:                 /* February */
                {
                    /* Check for leap year. We don't need to check for leap
                       century her (century years divisible by 400) since 2000
                       is and this FAT format only supports years to 2107 */
                    if ((year % 4) == 0)
                    {
                        /* Leap year in February... check for 29 days
                           instead of 28 */
                        if (day > 29)
                        {
                            /* Adjust the month */
                            day =  1;
                            month++;
                        }
                    }
                    else
                    {
                        if (day > 28)
                        {
                            /* Adjust the month */
                            day = 1;
                            month++;
                        }
                    }
                    break;
                }
                case 3:                 /* March */
                {
                    /* Check for end of the month */
                    if (day > 31)
                    {
                        /* Move to next month */
                        day = 1;
                        month++;
                    }
                    break;
                }
                case 4:                 /* April */
                {
                    /* Check for end of the month */
                    if (day > 30)
                    {
                        /* Move to next month */
                        day = 1;
                        month++;
                    }
                    break;
                }
                case 5:                 /* May */
                {
                    /* Check for end of the month */
                    if (day > 31)
                    {
                        /* Move to next month */
                        day = 1;
                        month++;
                    }
                    break;
                }
                case 6:                 /* June */
                {
                    /* Check for end of the month */
                    if (day > 30)
                    {
                        /* Move to next month */
                        day = 1;
                        month++;
                    }
                    break;
                }
                case 7:                 /* July */
                {
                    /* Check for end of the month */
                    if (day > 31)
                    {
                        /* Move to next month */
                        day = 1;
                        month++;
                    }
                    break;
                }
                case 8:                 /* August */
                {
                    /* Check for end of the month */
                    if (day > 31)
                    {
                        /* Move to next month */
                        day = 1;
                        month++;
                    }
                    break;
                }
                case 9:                 /* September */
                {
                    /* Check for end of the month */
                    if (day > 30)
                    {
                        /* Move to next month */
                        day = 1;
                        month++;
                    }
                    break;
                }
                case 10:                /* October */
                {
                    /* Check for end of the month */
                    if (day > 31)
                    {
                        /* Move to next month */
                        day = 1;
                        month++;
                    }
                    break;
                }
                case 11:                /* November */
                {
                    /* Check for end of the month */
                    if (day > 30)
                    {
                        /* Move to next month */
                        day = 1;
                        month++;
                    }
                    break;
                }
                case 12:                /* December */
                {
                    /* Check for end of the month */
                    if (day > 31)
                    {
                        /* Move to next month */
                        day = 1;
                        month = 1;

                        /* Also move to next year */
                        year++;

                        /* Check for a year that exceeds the representation
                           in this format */
                        if (year > MAXIMUM_YEAR)
                        {
                            return;
                        }
                    }
                    break;
                }

                default:                /* Invalid month! */
                    return;             /* Skip updating date/time! */
                }
            }
        }
    }

    /* Set the new system date */
    system_date =  ((year - BASE_YEAR) << YEAR_SHIFT) |
                        (month << MONTH_SHIFT) | day;

    /* Set the new system time */
    system_time  =  (hour << HOUR_SHIFT) |
                        (minute << MINUTE_SHIFT) | (second);
}
/**********************************************************************************************************************
 * End of function vir_time_entry
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: vir_date_set
 * Description  : This function sets the date.
 * Arguments    : day, month, year
 * Return value : SUCCESS   Upon date set successfully
 *                Any other Error code apart from SUCCESS
 *********************************************************************************************************************/
static UINT vir_date_set (UINT year, UINT month, UINT day)
{
    /* Check for invalid year */
    if ((year < BASE_YEAR) || (year > MAXIMUM_YEAR))
    {
        return(INVALID_YEAR);
    }
    /* Check for invalid day */
    if (day < 1)
    {
        return(INVALID_DAY);
    }
    /* Check for invalid day */
    switch (month)
    {
    case 1:
    {
        /* Check for 31 days */
        if (day > 31)
        {
            return(INVALID_DAY);
        }
        break;
    }
    case 2:
    {
        /* Check for leap year */
        if ((year % 4) == 0)
        {
            /* Leap year, February has 29 days */
            if (day > 29)
            {
                return(INVALID_DAY);
            }
        }
        else
        {
            /* Otherwise, non-leap year. February has 28 days */
            if (day > 28)
            {
                return(INVALID_DAY);
            }
        }
        break;
    }
    case 3:
    {
        /* Check for 31 days */
        if (day > 31)
        {
            return(INVALID_DAY);
        }
        break;
    }
    case 4:
    {
        /* Check for 30 days */
       if (day > 30)
        {
            return(INVALID_DAY);
        }
        break;
    }
    case 5:
    {
        /* Check for 31 days */
        if (day > 31)
        {
            return(INVALID_DAY);
        }
        break;
    }
    case 6:
    {
        /* Check for 30 days */
        if (day > 30)
        {
            return(INVALID_DAY);
        }
        break;
    }
    case 7:
    {
        /* Check for 31 days */
        if (day > 31)
        {
            return(INVALID_DAY);
        }
        break;
    }
    case 8:
    {
        /* Check for 31 days */
        if (day > 31)
        {
            return(INVALID_DAY);
        }
        break;
    }
    case 9:
    {
        /* Check for 30 days */
        if (day > 30)
        {
            return(INVALID_DAY);
        }
        break;
    }
    case 10:
    {
        /* Check for 31 days */
        if (day > 31)
        {
            return(INVALID_DAY);
        }
        break;
    }
    case 11:
    {
        /* Check for 30 days */
        if (day > 30)
        {
            return(INVALID_DAY);
        }
        break;
    }
    case 12:
    {
        /* Check for 31 days */
        if (day > 31)
        {
            return(INVALID_DAY);
        }
        break;
    }

    default:
        /* Invalid month */
        return(INVALID_MONTH);
    }

    /* Set the new system date */
    system_date =  ((year - BASE_YEAR) << YEAR_SHIFT) |
        (month << MONTH_SHIFT) | day;

    /* Return successful status */
    return(SUCCESS);
}
/**********************************************************************************************************************
 * End of function vir_date_set
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: vir_time_set
 * Description  : This function sets the time.
 * Arguments    : second, minute, hour
 * Return value : SUCCESS   Upon time set successfully
 *                Any other Error code apart from SUCCESS
 *********************************************************************************************************************/
static UINT vir_time_set(UINT hour, UINT minute, UINT second)
{
    /* Check for invalid hour */
    if (hour > MAXIMUM_HOUR)
    {
        return(INVALID_HOUR);
    }
    /* Check for invalid minute */
    if (minute > MAXIMUM_MINUTE)
    {
        return(INVALID_MINUTE);
    }
    /* Check for invalid second */
    if (second > MAXIMUM_SECOND)
    {
        return(INVALID_SECOND);
    }

    /* Set the new system time */
    system_time  =  (hour << HOUR_SHIFT) |
        (minute << MINUTE_SHIFT) | (second);

    /* Return successful status */
    return(SUCCESS);
}
/**********************************************************************************************************************
 * End of function vir_time_set
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: vir_date_get
 * Description  : This function gets the date.
 * Arguments    : day, month, year
 * Return value : SUCCESS   Upon date get successfully
 *                Any other Error code apart from SUCCESS
 *********************************************************************************************************************/
static UINT vir_date_get(INT *year, INT *month, INT *day)
{
    UINT date;

    /* Check for an invalid *day, *month, *year */
    if ((year == NULL) || (month == NULL) || (day == NULL))
    {
        return(PTR_ERROR);
    }

    /* Get the current system date */
    date =  system_date;

    /* Pickup the year if it's valid */
    if (year)
    {
       *year =  ((date >> YEAR_SHIFT) & YEAR_MASK) + BASE_YEAR;
    }
    /* Pickup the month if it's valid */
    if (month)
    {
       *month =  (date >> MONTH_SHIFT) & MONTH_MASK;
    }
    /* Pickup the day if it's valid */
    if (day)
    {
        *day =  date & DAY_MASK;
    }

    /* Return successful status */
    return(SUCCESS);
}
/**********************************************************************************************************************
 * End of function vir_date_get
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: vir_time_get
 * Description  : This function gets the time.
 * Arguments    : second, minute, hour
 * Return value : SUCCESS   Upon time get successfully
 *                Any other Error code apart from SUCCESS
 *********************************************************************************************************************/
static UINT vir_time_get(INT *hour, INT *minute, INT *second)
{
    UINT time;

    /* Check for an invalid *second, *minute, *hour */
    if ((hour == NULL) || (minute == NULL) || (second == NULL))
    {
        return(PTR_ERROR);
    }

    /* Get the current system time */
    time = system_time;

    /* Pickup the hour if it's valid */
    if (hour)
    {
        *hour =  (time >> HOUR_SHIFT) & HOUR_MASK;
    }
    /* Pickup the minute if it's valid */
    if (minute)
    {
        *minute =  (time >> MINUTE_SHIFT) & MINUTE_MASK;
    }
    /* Pickup the second if it's valid */
    if (second)
    {
        *second =  (time & SECOND_MASK);
    }

    /* Return successful status */
    return(SUCCESS);
}
/**********************************************************************************************************************
 * End of function vir_time_get
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: assign_month_value
 * Description  : This function assigns month value based on network returned month.
 * Arguments    : time                    date and time to be modified
 *                read_buffer             Buffer that contains user input time values
 * Return value : None
 *********************************************************************************************************************/
static void assign_month_value(vtime_t * time, char * read_buffer)
{
    /* This compiler MACRO that had date value is processed using the following conversion, where ASCII value is converted to
     * integer value and month value is assigned */
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
/**********************************************************************************************************************
 * End of function assign_month_value
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: update_time_stamp_to_system_block
 * Description  : This function updates network time stamp to system time format.
 * Arguments    : time_data    Network time data
 * Return value : SUCCESS   Upon successful update network time
 *                Any other Error code apart from SUCCESS
 *********************************************************************************************************************/
UINT update_time_stamp_to_system_block(char * const time_data)
{
    UINT err = SUCCESS;
    /* Variable to store time */
    vtime_t set_time         = {RESET_VALUE};
    vtime_t disp_time        = {RESET_VALUE};
    time_t  epoch_val        = (time_t)RESET_VALUE;

    /* SNTP Client query Date and Time are converted to time format so that,
     * this can be used to add time stamping and comparison with current system time */
    convert_network_timestamp_to_system_block(&set_time, &time_data[0]);

    /* Convert returned month name to month value */
    assign_month_value(&set_time, &time_data[0]);

    /* Modify the month and year in standard time format */
    sys_month_year_time_format_update(&set_time);

    /* Convert sntp client time to epoch value */
    epoch_val = mktime(&set_time);

    /* Update epoch value as per time zone */
    epoch_val += TIME_ZONE_OFFSET;

    /* Modify the month and year in UTC time format */
    sys_month_year_readability_update(&set_time);

    /* Print the UTC Time readable format */
    PRINT_INFO_STR("UTC time info:");
    app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_PRINT_DATE_TIME, sizeof(vtime_t), &set_time);

    /* Convert the epoch value to user readable format */
    disp_time = *localtime(&epoch_val);

    /* Modify the month and year in user readable format */
    sys_month_year_readability_update(&disp_time);

    /* Print the current Network Date and Time in user readable format */
    PRINT_INFO_STR("Acquired network date and time successfully, Current Time Zone Offset Adjusted Network time:");
    app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_PRINT_DATE_TIME, sizeof(vtime_t), &disp_time);

    /* Get current Year, Month, Date from system timer */
    err = vir_date_get(&g_time.tm_year, &g_time.tm_mon, &g_time.tm_mday);
    if (SUCCESS != err)
    {
        PRINT_ERR_STR ("\r\nGet current system date failed.");
        return err;
    }

    /* Get current Hour, Minute, Second from system timer */
    err = vir_time_get(&g_time.tm_hour, &g_time.tm_min, &g_time.tm_sec);
    if (SUCCESS != err)
    {
        PRINT_ERR_STR ("\r\nGet current system time failed.");
        return err;
    }

    /* Compare network time and current system time */
    if(RESET_VALUE == memcmp(&g_time, &disp_time, sizeof(vtime_t)))
    {
        PRINT_INFO_STR("No Mismatch observed between network time and current system time, so time update to system time is not required.");
    }
    else
    {
        PRINT_INFO_STR("Mismatch observed between network time and current system time, so updating current system time...");

        /* Set new network Year, Month, Date to system timer */
        err = vir_date_set(disp_time.tm_year, disp_time.tm_mon, disp_time.tm_mday);
        if (SUCCESS != err)
        {
            PRINT_ERR_STR ("Set network date to system time failed.");
            return err;
        }

        /* Set new network Hour, Minute, Second to system timer */
        err = vir_time_set(disp_time.tm_hour, disp_time.tm_min, disp_time.tm_sec);
        if (SUCCESS != err)
        {
            PRINT_ERR_STR ("Set network time to system time failed.");
            return err;
        }

        /* Get new Year, Month, Date for system timer */
        err = vir_date_get(&g_time.tm_year, &g_time.tm_mon, &g_time.tm_mday);
        if (SUCCESS != err)
        {
            PRINT_ERR_STR ("Get system date based on network time failed.");
            return err;
        }

        /* Get new Hour, Minute, Second for system timer */
        err = vir_time_get(&g_time.tm_hour, &g_time.tm_min, &g_time.tm_sec);
        if (SUCCESS != err)
        {
            PRINT_ERR_STR ("Get system time based on network time failed.");
            return err;
        }

        /* Print the updated system Date and Time in user readable format */
        PRINT_INFO_STR("Updated system time:");
        app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_PRINT_DATE_TIME, sizeof(vtime_t), &g_time);
    }
    return err;
}
/**********************************************************************************************************************
 * End of function update_time_stamp_to_system_block
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: convert_network_timestamp_to_system_block
 * Description  : This function converts network date and time value into system block format.
 * Arguments    : *time                    variable to store time values
 *                *read_buffer             Buffer that contains user input time values
 * Return value : None
 *********************************************************************************************************************/
static void convert_network_timestamp_to_system_block(vtime_t * time, char * read_buffer)
{
    char * delimeter = NULL;
    int time_data[TIME_BUFF_SIZE] = {RESET_VALUE};
    uint8_t cnt = RESET_VALUE;

    /* Extract all delimiters */
    while((delimeter = (char *)strtok_r(read_buffer," ,:.", &read_buffer)))
    {
        time_data[cnt++] = atoi(delimeter);
    }

    /* Update the extracted time stamps to system time blocks */
    time->tm_mday = time_data[1];
    time->tm_year = time_data[2];
    time->tm_hour = time_data[3];
    time->tm_min  = time_data[4];
    time->tm_sec  = time_data[5];
}
/**********************************************************************************************************************
 * End of function convert_network_timestamp_to_system_block
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: sys_month_year_readability_update
 * Description  : This function modifies the month and year in readable format to the user.
 * Arguments    : time      month and year will be modified
 * Return value : None
 *********************************************************************************************************************/
static void sys_month_year_readability_update(vtime_t * time)
{
    time->tm_mon  +=  MON_ADJUST_VALUE;
    time->tm_year +=  YEAR_ADJUST_VALUE;
}
/**********************************************************************************************************************
 * End of function sys_month_year_readability_update
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: sys_month_year_time_format_update
 * Description  : This function modifies the month and year to time format.
 * Arguments    : time      month and year will be modified
 * Return value : None
 *********************************************************************************************************************/
static void sys_month_year_time_format_update(vtime_t * time)
{
    time->tm_mon  -=  MON_ADJUST_VALUE;
    time->tm_year -=  YEAR_ADJUST_VALUE;
}
/**********************************************************************************************************************
 * End of function sys_month_year_time_format_update
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * @} (end addtogroup system_ep)
 *********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
/**********************************************************************************************************************
 * File Name    : menu_slcd.c
 * Version      : .
 * Description  : The next steps screen display.
 *********************************************************************************************************************/

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "semphr.h"
#include "queue.h"
#include "task.h"
#include "jlink_console.h"
#include "update_slcd.h"

#include "common_init.h"
#include "common_utils.h"
#include "menu_slcd.h"
#include "slcd.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>


#define CONNECTION_ABORT_CRTL    (0x00)
#define MENU_EXIT_CRTL           (0x20)

#define MODULE_NAME     "\r\n%d. SEGMENT LCD DEMONSTRATION\r\n"

#define SUB_OPTIONS     "\r\n Select from the options in the menu below:" \

#define LIMIT (30)

static void string_from_console (char_t * p_out, size_t out_length, char_t * rtc_output);

static int8_t is_number_valid (char_t * p_data, uint8_t check);

static test_fn set_time_option (void);
static test_fn set_alarm_option (void);
static test_fn get_time_input (rtc_time_t * rtc_input, char_t * type);
static test_fn get_alarm_string (char_t * alarm_string);

static char_t s_print_buffer[BUFFER_LINE_LENGTH] = { };

static bool_t s_alarm_set = false;

static bool_t s_return_to_menu = false;

rtc_alarm_time_t g_rtc_alarm =
{ };

static st_menu_fn_tbl_t s_menu_items[] =
{
    {"Set Time", false, true, set_time_option}, \
    {"Set Alarm (Not Set)", false, true, set_alarm_option}, \
    {"", false, true, NULL }
};

/**********************************************************************************************************************
 * Function Name: slcd_display_menu
 * Description  : Displays menu for SLCD Demonstration and allows the user to choose an option
 * Return Value : The next steps screen.
 *********************************************************************************************************************/
test_fn slcd_display_menu(void)
{
    int8_t c = -1;
    int8_t menu_limit;
    char_t alarm_str[20];

    while ((MENU_EXIT_CRTL != c) && (CONNECTION_ABORT_CRTL != c) && (false == s_return_to_menu))
    {
        c = -1;

        menu_limit = 0;

        sprintf (s_print_buffer, "%s%s", gp_clear_screen, gp_cursor_home);
        print_to_console (s_print_buffer);

        sprintf (s_print_buffer, MODULE_NAME, g_selected_menu);
        print_to_console (s_print_buffer);

        sprintf (s_print_buffer, SUB_OPTIONS);
        print_to_console (s_print_buffer);

        for (int8_t i = 0; (NULL != s_menu_items[i].p_func); i++)
        {
            if ((1 == i) && (true == s_alarm_set))
            {

                get_alarm_string (alarm_str); //  returns formatted alarm as '(d hh:mmap)'
                sprintf (s_print_buffer, "\r\n 2. Set Alarm %s", alarm_str);
                menu_limit++;

            }
            else
            {
                sprintf (s_print_buffer, "\r\n %d. %s", (i + 1), s_menu_items[menu_limit++].p_name);
            }

            print_to_console (s_print_buffer);
        }

        print_to_console ("\r\n");

        sprintf (s_print_buffer, MENU_RETURN_INFO);
        print_to_console (s_print_buffer);

        while ((0 != c) || (false == s_return_to_menu))
        {
            c = input_from_console ();
            if ((MENU_EXIT_CRTL == c) || (CONNECTION_ABORT_CRTL == c))
            {
                break;
            }
            if (0 != c)
            {
                /* Cast, as compiler will assume calc is int */
                c = (int8_t) (c - '0');

                g_selected_menu = c;

                if ((c > 0) && (c <= menu_limit))
                {
                    s_menu_items[c - 1].p_func ();
                    break;
                }
            }

        }
    }
    s_return_to_menu = false;
    return (0);
}
/**********************************************************************************************************************
 End of function slcd_display_menu
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: set_time_option
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
static test_fn set_time_option(void)
{
    rtc_time_t set_time = { };

    char_t c = '0';

    fsp_err_t rtc_err = FSP_SUCCESS;

    sprintf (s_print_buffer, "\r\n SET TIME");
    print_to_console (s_print_buffer);

    g_s_alarm_reached = false;
    get_time_input (&set_time, "TIME");
    if (false == s_return_to_menu)
    {
        rtc_err = R_RTC_CalendarTimeSet (&g_rtc_ctrl, &set_time);
        set_time_on_slcd (&set_time);
        sprintf (s_print_buffer, SLCD_MENU_RETURN_INFO);
        print_to_console (s_print_buffer);
    }

    while ((0 != c))
    {
        if (true == s_return_to_menu)
        {
            break;
        }
        c = input_from_console ();
        if ((MENU_EXIT_CRTL == c) || (CONNECTION_ABORT_CRTL == c))
        {
            break;
        }
    }

    return (test_fn)rtc_err; /* Cast to test_fn */
}
/**********************************************************************************************************************
 End of function set_time_option
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: set_alarm_option
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
static test_fn set_alarm_option(void)
{
    char_t c = '0';



    sprintf (s_print_buffer, "\r\n SET ALARM");
    print_to_console (s_print_buffer);

    g_s_alarm_reached = false;
    get_time_input (&g_rtc_alarm.time, "ALARM");

    g_rtc_alarm.dayofweek_match = true;
    g_rtc_alarm.channel         = RTC_ALARM_CHANNEL_0;
    g_rtc_alarm.hour_match      = true;
    g_rtc_alarm.min_match       = true;


    if (false == s_return_to_menu)
    {
        R_RTC_CalendarAlarmSet (&g_rtc_ctrl, &g_rtc_alarm);
        set_segment_icon_named (ALARM_ICON, ICON_ON);
        s_alarm_set = true;
        sprintf (s_print_buffer, SLCD_MENU_RETURN_INFO);
        print_to_console (s_print_buffer);
    }
    while ((0 != c))
    {
        if (true == s_return_to_menu)
        {
            break;
        }
        c = input_from_console ();
        if ((MENU_EXIT_CRTL == c) || (CONNECTION_ABORT_CRTL == c))
        {
            break;
        }
        vTaskDelay (1);
    }

    return 0;
}
/**********************************************************************************************************************
 End of function set_alarm_option
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: get_time_input
 * Description  : .
 * Arguments    : rtc_input
 *              : type
 * Return Value : .
 *********************************************************************************************************************/
static test_fn get_time_input(rtc_time_t *rtc_input, char_t *type)
{
    int8_t is_valid;
    char_t data[8];
    char_t rtc_user_input[4][30] = { };
    bool_t previous_mode         = false;

    char_t c = '0';

    char_t * p_rtc_user_outputs[2] = { " Enter Time in Hours (ENTER 01-12 on console): ", \
                                        " Enter Time in Minutes (ENTER 0-59 on console): "};

    char_t * p_meridian_output = "Enter Meridian (A)M or (P)M: ";

    char_t * p_day_options[] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", NULL };

    R_RTC_CalendarTimeGet (&g_rtc_ctrl, rtc_input);

    while ((0 != c))
    {
        for (uint8_t i = 0; i < ((sizeof(p_rtc_user_outputs)) / (sizeof(p_rtc_user_outputs[0]))); i++)
        {
            strcpy (data, "");
            sprintf (s_print_buffer, "\r\n\n%s", (p_rtc_user_outputs[i]));
            print_to_console (s_print_buffer);
            string_from_console (&data[0], 8, p_rtc_user_outputs[i]);
            if (true == s_return_to_menu)
            {
                break;
            }
            is_valid = is_number_valid (data, i);

            switch (is_valid)
            {
                case -1:
                    sprintf (s_print_buffer, "\r\n Please enter up to two digits");
                    print_to_console (s_print_buffer);
                    i--;
                    break;

                case -2:
                    sprintf (s_print_buffer, "\r\n Please enter digits only");
                    print_to_console (s_print_buffer);
                    i--;
                    break;
                case -3:
                    sprintf (s_print_buffer, "\r\n Please enter a value within bounds");
                    print_to_console (s_print_buffer);
                    i--;
                    break;
                default:
                    strcpy (rtc_user_input[i], data);

                    switch (i)
                    {
                        case 0:
                        {
                            rtc_input->tm_hour = atoi (rtc_user_input[i]);
                            break;
                        }
                        case 1:
                        {
                            rtc_input->tm_min = atoi (rtc_user_input[i]);
                            break;
                        }
                        default:
                    }
            }
        }
        if (true == s_return_to_menu)
        {
            break;
        }
        sprintf (s_print_buffer, "\r\n\n %s", p_meridian_output);
        print_to_console (s_print_buffer);

        /* Store echo mode allowing user input for meridian */
        previous_mode = jlink_set_echo_mode(true);

        while ('0' == c)
        {
            c = input_from_console ();

            if ((MENU_EXIT_CRTL == c) || (CONNECTION_ABORT_CRTL == c))
            {
                s_return_to_menu = true;
                break;
            }
            else if ('a' == tolower(c)) /* Cast to lower case */
            {
                strcpy (rtc_user_input[2], "am");
                if (atoi (rtc_user_input[0]) == 12)
                {
                    rtc_input->tm_hour = 0;
                }
            }
            else if ('p' == tolower(c)) /* Cast to lower case */
            {
                strcpy (rtc_user_input[2], "pm");
                if (atoi (rtc_user_input[0]) != 12)
                {
                    rtc_input->tm_hour += 12;
                }
            }
            else
            {
                c = '0';
                sprintf (s_print_buffer, "\r\n Please enter a valid input");
                print_to_console (s_print_buffer);
                sprintf (s_print_buffer, "\r\n\n %s", p_meridian_output);
                print_to_console (s_print_buffer);
            }
        }

        if (true == s_return_to_menu)
        {
            break;
        }

        sprintf (s_print_buffer, "\r\n\n Select Day: ");
        print_to_console (s_print_buffer);

        for (int8_t i = 0; (NULL != p_day_options[i]); i++)
        {
            sprintf (s_print_buffer, "\r\n %d. %s", (i + 1), p_day_options[i]);
            print_to_console (s_print_buffer);
        }

        sprintf (s_print_buffer, "\r\n Enter Day (1-7): ");
        print_to_console (s_print_buffer);

        int8_t val;
        c = '0';
        while ('0' == c)
        {
            c = input_from_console ();
            if ((MENU_EXIT_CRTL == c) || (CONNECTION_ABORT_CRTL == c))
            {
                s_return_to_menu = true;
                break;
            }
            val = c - '0';

            if ((val > 0) && (val < 8))
            {
                strcpy (rtc_user_input[3], p_day_options[val - 1]);
                if (strcmp(type, "TIME") == 0)
                {
                    rtc_input->tm_mday = val;
                }
                else
                {
                    rtc_input->tm_wday = val - 1;
                }
            }
            else
            {
                c = '0';
                sprintf (s_print_buffer, "\r\n Please enter a valid input: ");
                print_to_console (s_print_buffer);
                sprintf (s_print_buffer, "\r\n Enter Day (1-7): ");
                print_to_console (s_print_buffer);
            }
        }
        if (true == s_return_to_menu)
        {
            break;
        }

        rtc_input->tm_sec = 0;
        sprintf (s_print_buffer, "\r\n\n %s SET: %s %02d:%02d%s", type, rtc_user_input[3], \
                atoi (rtc_user_input[0]), atoi (rtc_user_input[1]), rtc_user_input[2]);
        print_to_console (s_print_buffer);
        jlink_restore_echo_mode (previous_mode);
        return 0;

    }
    jlink_restore_echo_mode (previous_mode);
    return 0;
}
/**********************************************************************************************************************
 End of function get_time_input
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: get_alarm_string
 * Description  : .
 * Argument     : alarm_str
 * Return Value : .
 *********************************************************************************************************************/
static test_fn get_alarm_string(char_t *alarm_str)
{
    int8_t hour;

    char_t * p_day_options[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
    char_t   day[4];
    char_t   meridian[3];
    char_t   alarm_str_test[13];

    R_RTC_CalendarAlarmGet (&g_rtc_ctrl, &g_rtc_alarm);

    strcpy (day, p_day_options[g_rtc_alarm.time.tm_wday]);
    hour = (int8_t) g_rtc_alarm.time.tm_hour; /* Cast to int8_t */

    if (0 == g_rtc_alarm.time.tm_hour)
    {
        hour += 12;
        strcpy (meridian, "AM");
    }
    else if (g_rtc_alarm.time.tm_hour < 12)
    {
        strcpy (meridian, "AM");
    }
    else if (12 == g_rtc_alarm.time.tm_hour)
    {
        strcpy (meridian, "PM");
    }
    else
    {
        hour -= 12;
        strcpy (meridian, "PM");
    }

    sprintf (alarm_str, "(%s %02d:%02d%s)", day, hour, g_rtc_alarm.time.tm_min, meridian);
    strcpy (alarm_str_test, alarm_str);

    return 0;
}
/**********************************************************************************************************************
 End of function get_alarm_string
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: is_number_valid
 * Description  : .
 * Arguments    : p_data
 *              : hours_or_minutes
 * Return Value : .
 *********************************************************************************************************************/
static int8_t is_number_valid(char_t * p_data, uint8_t hours_or_minutes)
{
    volatile int8_t val_0;
    volatile int8_t val_1;
    volatile int8_t val;
    val_0 = 1;
    val_1 = 1;

    if (strlen (p_data) > 2)
    {
        return -1;
    }

    val_0 = (int8_t)atoi((char*) &p_data[0]); /* Cast to int8_t */

    if (strlen (p_data) == 2)
    {
        val_1 = (int8_t)atoi((char*) &p_data[1]); /* Cast to int8_t */
    }

    if (((0 == val_0) && ('0' != p_data[0])) || ((0 == val_1) && ('0' != p_data[1])))
    {
        return -2;
    }

    val = (int8_t)atoi((char*)p_data); /* Cast to char* */

    switch (hours_or_minutes)
    {
        case 0:
            if ((val < 1) || (val > 12))
            {
                return -3;
            }
            break;

        case 1:
            if ((val < 0) || (val > 59))
            {
                return -3;
            }
            break;
        default:
            return val;
    }
    return val;
}
/**********************************************************************************************************************
 End of function is_number_valid
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: string_from_console
 * Description  : .
 * Arguments    : p_out
 *              : out_length
 *              : rtc_output
 * Return Value : .
 *********************************************************************************************************************/
static void string_from_console(char_t * p_out, size_t out_length, char_t * rtc_output)
{
    char_t   user_input;
    char_t   user_string[LIMIT +1] = "";
    uint32_t current_loc           = 0;

    start_key_check ();

    while (true)
    {
        if (key_pressed () == true)
        {
            user_input = (char_t)get_detected_key(); /* Cast to char_t*/
            if (MENU_ENTER_RESPONSE_CRTL != user_input)
            {
                if ((CARRAGE_RETURN == user_input) || (LIMIT == current_loc) || (out_length == current_loc))
                {
                    break;
                }
                if ((MENU_EXIT_CRTL == user_input) || (CONNECTION_ABORT_CRTL == user_input))
                {
                    s_return_to_menu = true;
                    break;
                }

                if ((BACK_SPACE == user_input) && (current_loc > 0))
                {
                    user_string[current_loc - 1] = '\0';
                    current_loc--;
                }

                if ((BACK_SPACE != user_input) && (current_loc < LIMIT))
                {
                    user_string[current_loc] = user_input;
                    current_loc++;
                }
                print_to_console ("\x1b[2K\r");
                sprintf (s_print_buffer, "%s%s", rtc_output, user_string);

                print_to_console (s_print_buffer);
            }
            start_key_check ();
        }
    }
    if (current_loc > 0)
    {
        user_string[current_loc] = '\0';
        memcpy (p_out, user_string, current_loc + 1);
    }
    return;
}
/**********************************************************************************************************************
 End of function string_from_console
 *********************************************************************************************************************/



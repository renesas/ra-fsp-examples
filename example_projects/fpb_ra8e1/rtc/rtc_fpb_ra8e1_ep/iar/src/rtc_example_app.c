/***********************************************************************************************************************
 * File Name    : rtc_example_app.c
 * Description  : Example application layer for the RTC EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - hal_entry.c calls a single function: rtc_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "rtc_example_app.h"
#include "console/uart_console.h"

/* Private functions */
static void handle_user_input(void);
static void user_input(char *buf, int len);
static void handle_error(fsp_err_t err, const char *err_str);

/* Variables to store time */
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
static rtc_alarm_time_t alarm_time =
{
#if (BSP_PERIPHERAL_RTC_C_PRESENT)
    .time_minute     = RESET_VALUE,
    .time_hour       = RESET_VALUE,
    .weekday_match   = RESET_VALUE,
#else
    .sec_match       = RESET_VALUE,
    .min_match       = RESET_VALUE,
    .hour_match      = RESET_VALUE,
    .mday_match      = RESET_VALUE,
    .mon_match       = RESET_VALUE,
    .year_match      = RESET_VALUE,
    .dayofweek_match = RESET_VALUE,
#endif /* BSP_PERIPHERAL_RTC_C_PRESENT */
};

/* Global variables */
volatile uint32_t g_alarm_irq_flag = RESET_FLAG;    /* Flag to check occurrence of alarm interrupt */
volatile uint32_t g_periodic_irq_flag = RESET_FLAG; /* Flag to check occurrence of periodic interrupt */
volatile uint32_t g_invalid_sequence = RESET_FLAG;  /* Flag to check if the user provides options in proper sequence */

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the RTC example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void rtc_example_app_run(void)
{
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version = {RESET_VALUE};

    /* Initialize UART console before any printing */
    uint32_t status = uart_console_init();
    if (FSP_SUCCESS != status)
    {
        return;     /* UART failed to open — cannot print anything */
    }

    /* Get FLEX pack version information */
    R_FSP_VersionGet(&version);

    /* Example project information printed on the terminal */
    printf(BANNER_INFO, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    printf(EP_INFO);

    /* Initialize RTC module */
    err = rtc_init();
    handle_error(err, "rtc_init failed\r\n");

    /* Menu for user selection */
    printf(EP_MENU);

    while (true)
    {
        /* Process input only when the user has provided one */
        if (uart_console_has_char())
        {
            handle_user_input();
        }

        /* Check the occurrence of alarm interrupt */
        if (SET_FLAG == g_alarm_irq_flag)
        {
            g_alarm_irq_flag = RESET_FLAG;

            printf("\r\nRTC Alarm Interrupt\r\n");

            /* Get current RTC Calendar Date and Time */
            err = get_rtc_calendar_time(&g_present_time);
            handle_error(err, "get_rtc_calendar_time failed\r\n");

            printf("\r\nRTC Date: %d/%d/%d\r\nTime: %d:%d:%d\r\n", g_present_time.tm_mday,
                   g_present_time.tm_mon, g_present_time.tm_year, g_present_time.tm_hour,
                   g_present_time.tm_min, g_present_time.tm_sec);

            printf("\r\nLED will be turned ON for 2 seconds\r\n");

            /* Turn ON LED on occurrence of alarm */
            led_level_set(LED_ON);

            /* Delay 2 seconds */
            R_BSP_SoftwareDelay(LED_DELAY, BSP_DELAY_UNITS_SECONDS);

            /* Turn OFF LED */
            led_level_set(LED_OFF);
            printf(EP_GUIDE);
        }

        if (SET_FLAG == g_periodic_irq_flag)
        {
            g_periodic_irq_flag = RESET_FLAG;

            /* Toggle LED on occurrence of periodic interrupt */
            err = led_level_toggle();
            handle_error(err, "led_level_toggle failed\r\n");
        }
    }
}

/*******************************************************************************************************************//**
 * @brief     This function reads the command input from the user through terminal and processes the command.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void handle_user_input(void)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Variable to store user input */
    char input_buf[BUFFER_SIZE] = {NULL_CHAR};
    uint8_t user_option = RESET_VALUE;

    user_input(input_buf, (int)sizeof(input_buf));
    user_option = (uint8_t)atoi((char *)input_buf);

    switch (user_option)
    {
        case SET_TIME:
        {
            memset(input_buf, RESET_VALUE, sizeof(input_buf));
            printf("\r\nSetting RTC Date and Time\r\n");
            printf("\r\nEnter Date and Time in DD:MM:YYYY HH:MM:SS format.\nSample Input: '04:11:2019 15:23:05'\r\n");

            /* Wait until user data is available */
            while (BYTES_RECEIVED_ZERO == uart_console_has_char())
            {
                /* Do nothing */
            }

            /* Read Calendar time provided by the user */
            user_input(input_buf, (int)sizeof(input_buf));

            err = set_rtc_calendar_time(input_buf, &g_present_time);
            handle_error(err, "set_rtc_calendar_time failed\r\n");

            printf("\r\nRTC calendar set to Date: %d/%d/%d\r\nTime: %d:%d:%d\r\n",
                   g_present_time.tm_mday, g_present_time.tm_mon, g_present_time.tm_year,
                   g_present_time.tm_hour, g_present_time.tm_min, g_present_time.tm_sec);
            printf("\r\nRTC calendar time is set successfully");
            printf(EP_GUIDE);
            break;
        }

        case SET_ALARM:
        {
            memset(input_buf, RESET_VALUE, sizeof(input_buf));
            printf("\r\nSetting Calendar Alarm for RTC\r\n");
#if (BSP_PERIPHERAL_RTC_C_PRESENT)
            printf("\r\nEnter hours and minutes value (Format: HH:MM) for which recursive alarm has to be set."\
                      "\r\nSample Input: '15:24'. At 15:24:00, alarm occurred, and LED will be turned ON.\r\n");
#else
            printf("\r\nEnter seconds value (Format: SS) for which recursive alarm has to be set."\
                      "\r\nPlease input seconds value in the range between 00 and 59."\
                      "\r\nSample input: '35'. At HH:MM:35, alarm occurred, and LED will be turned ON.\r\n");
#endif /* BSP_PERIPHERAL_RTC_C_PRESENT */

            /* Wait until user data is available */
            while (BYTES_RECEIVED_ZERO == uart_console_has_char())
            {
                /* Do nothing */
            }

            /* Reading Calendar alarm time provided by the user */
            user_input(input_buf, (int)sizeof(input_buf));

            err = set_rtc_calendar_alarm(input_buf, &alarm_time);
            handle_error(err, "set_rtc_calendar_alarm failed\r\n");

            if (SET_FLAG == g_invalid_sequence)
            {
                g_invalid_sequence = RESET_FLAG;
                printf("\r\nPlease set RTC calendar Date and Time\r\n");
                printf(EP_GUIDE);
            }
            else
            {
#if (BSP_PERIPHERAL_RTC_C_PRESENT)
                printf("\r\nHours and minutes value for which alarm is set: %d:%d", alarm_time.time_hour,\
                       alarm_time.time_minute);
#else
                printf("\r\nSeconds value for which alarm is set: %d", alarm_time.time.tm_sec);
#endif /* BSP_PERIPHERAL_RTC_C_PRESENT */
                printf("\r\nRTC calendar alarm is set successfully");
                printf(EP_GUIDE);
            }
            break;
        }

        case SET_PERIODIC_IRQ:
        {
            printf("\r\nSetting Periodic IRQ rate for RTC\r\n");
            err = set_rtc_periodic_rate();
            handle_error(err, "set_rtc_periodic_rate failed\r\n");

            printf("\r\nPeriodic interrupt rate is set successfully");
            printf(EP_GUIDE);
            break;
        }

        case GET_CURRENT_TIME:
        {
            /* Get current RTC Calendar Date and Time */
            err = get_rtc_calendar_time(&g_present_time);
            handle_error(err, "get_rtc_calendar_time failed\r\n");

            printf("\r\nRTC Date: %d/%d/%d\r\nTime: %d:%d:%d\r\n", g_present_time.tm_mday,
                   g_present_time.tm_mon, g_present_time.tm_year, g_present_time.tm_hour,
                   g_present_time.tm_min, g_present_time.tm_sec);
            printf("\r\nGetting current RTC calendar time is successful");
            printf(EP_GUIDE);
            break;
        }

        default:
        {
            printf(EP_MENU);
            break;
        }
    }
}

/*******************************************************************************************************************//**
 * @brief         This function reads a line of user input from stdin and remove trailing newline characters.
 * @param[in,out] buf                   Pointer to buffer where the input string is stored.
 * @param[in]     len                   Maximum number of characters to read (buffer size).
 * @retval        None.
 **********************************************************************************************************************/
static void user_input(char *buf, int len)
{
    fgets(buf, len, stdin);
    buf[strcspn(buf, "\r\n")] = '\0';
}

/*******************************************************************************************************************//**
 * @brief     This function handles an error by printing a message and error code, de-initializing, and trapping.
 * @param[in] err                       Error code.
 * @param[in] err_str                   Error message.
 * @retval    None.
 **********************************************************************************************************************/
static void handle_error(fsp_err_t err, const char *err_str)
{
    if (FSP_SUCCESS != err)
    {
        printf("\r\n[ERR] %s", err_str);
        printf("Returned Error Code: 0x%x\r\n", (unsigned int)err);

        /* De-initialize RTC module */
        rtc_deinit();

        /* Trap here */
        __BKPT(0);
    }
}

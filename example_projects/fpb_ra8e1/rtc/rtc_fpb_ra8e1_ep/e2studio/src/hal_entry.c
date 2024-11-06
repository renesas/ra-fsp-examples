/***********************************************************************************************************************
 * File Name    : hal_entry.c
 * Description  : Contains data structures and functions used in hal_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#include "common_utils.h"
#include "rtc_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup rtc_ep
 * @{
 **********************************************************************************************************************/

void R_BSP_WarmStart(bsp_warm_start_event_t event);
/* Internal function to read input from RTT */
static void read_Input_from_RTT(void);
/* Internal function to toggle LED status */
static void led_level_set(bsp_io_level_t led_level);
extern bsp_leds_t g_bsp_leds;
/* Variable to store time */
rtc_time_t g_current_time =
{
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

volatile uint32_t g_alarm_irq_flag = RESET_FLAG;       //flag to check occurrence of alarm interrupt
volatile uint32_t g_periodic_irq_flag = RESET_FLAG;    //flag to check occurrence of periodic interrupt
volatile uint32_t g_invalid_sequence = RESET_FLAG;     //flag to check if user provides options in proper sequence

/*******************************************************************************************************************//**
 * The RA Configuration tool generates main() and uses it to generate threads if an RTOS is used.  This function is
 * called by main() when no RTOS is used.
 **********************************************************************************************************************/
void hal_entry(void)
{
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version = {RESET_VALUE};
    bsp_io_level_t Led_level = BSP_IO_LEVEL_LOW;

    /* version get API for FLEX pack information */
    R_FSP_VersionGet(&version);

    /* Example Project information printed on the Console */
    APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    APP_PRINT(EP_INFO);

    /* Initialize RTC driver */
    err = rtc_init();
    if (FSP_SUCCESS != err)
    {
        /* RTC module init failed */
        APP_ERR_PRINT("\r\n ** RTC INIT FAILED ** \r\n");
        APP_ERR_TRAP(err);
    }

    /* Menu for User Selection */
    APP_PRINT(MENU_INFO);

    while(true)
    {
        /* Process input only when User has provided one */
        if (APP_CHECK_DATA)
        {
            read_Input_from_RTT();
        }

        /* Check the occurrence of alarm interrupt */
        if (SET_FLAG == g_alarm_irq_flag)
        {
            APP_PRINT("\r\n RTC Alarm Interrupt \r\n");
            /* Get current RTC Calendar Date and Time */
#if defined (BOARD_RA0E1_FPB)
            err = R_RTC_C_CalendarTimeGet(&g_rtc_ctrl, &g_current_time);
#else
            err = R_RTC_CalendarTimeGet(&g_rtc_ctrl, &g_current_time);
#endif
            if (FSP_SUCCESS != err)
            {
                rtc_deinit();
                APP_ERR_PRINT("\r\n ** Getting RTC Calendar time failed ** \r\n");
                APP_ERR_TRAP(err);
            }
            /* Modify the date in standard format to user readable format */
            rtc_date_readability_update(&g_current_time);
            APP_PRINT("\r\n RTC Alarm occurred at  Date : %d/%d/%d \n Time  : %d : %d : %d \r\n\n", g_current_time.tm_mday,
                    g_current_time.tm_mon, g_current_time.tm_year, g_current_time.tm_hour,
                    g_current_time.tm_min, g_current_time.tm_sec);
            APP_PRINT("\r\n LED will be turned ON for 2 seconds \r\n");

            /* Turn ON LED on occurrence of alarm */
            led_level_set(BSP_IO_LEVEL_HIGH);
            /* Delay to glow LED for 2  seconds */
            R_BSP_SoftwareDelay(LED_DELAY, BSP_DELAY_UNITS_SECONDS);
            /* Turn OFF LED */
            led_level_set(BSP_IO_LEVEL_LOW);
            g_alarm_irq_flag = RESET_FLAG;

            APP_PRINT("\r\nEnter any key (other than 1,2,3,4) to go back to the menu \r\n");
        }

        if (SET_FLAG == g_periodic_irq_flag)
        {
            /* Toggle LED on occurrence of periodic interrupt */
            err = R_IOPORT_PinRead(&g_ioport_ctrl, (bsp_io_port_pin_t)(g_bsp_leds.p_leds[RESET_VALUE]) , &Led_level);
            if(FSP_SUCCESS != err)
            {
                rtc_deinit();
                APP_ERR_PRINT("\r\n ** R_IOPORT_PinRead FAILED ** \r\n");
                APP_ERR_TRAP(err);
            }

            Led_level ^= BSP_IO_LEVEL_HIGH;

            led_level_set(Led_level);
            g_periodic_irq_flag = RESET_FLAG;
        }
    }

}

/*******************************************************************************************************************//**
 * @brief This function reads the command(input) for RTT and processes the command.
 *
 * @param[in] None
 * @retval    None
 **********************************************************************************************************************/
static void read_Input_from_RTT(void)
{
    fsp_err_t err = FSP_SUCCESS;     // Error status

    /* Variable to store user input */
    uint8_t rByte[BUFFER_SIZE_DOWN] = {NULL_CHAR};
    uint8_t converted_rtt_input = RESET_VALUE;

    APP_READ(rByte);
    converted_rtt_input = (uint8_t)atoi((char *)rByte);

    switch (converted_rtt_input)
    {
        case SET_TIME:
        {
            err = set_rtc_calendar_time();
            if (FSP_SUCCESS != err)
            {
                rtc_deinit();
                APP_ERR_PRINT("\r\nRTC Calendar Time Set failed.\r\nClosing the driver. Restart the Application\r\n");
                APP_ERR_TRAP(err);
            }
            else
            {
                APP_PRINT("\r\nRTC Calendar Time is set successfully");
                APP_PRINT("\r\nEnter any key (other than 1,2,3,4) to go back to the menu \r\n");
            }
            break;
        }

        case SET_ALARM:
        {
            err = set_rtc_calendar_alarm();
            if (FSP_SUCCESS != err)
            {
                rtc_deinit();
                APP_ERR_PRINT("\r\nRTC Calendar Alarm Set failed.\r\nClosing the driver. Restart the Application\r\n");
                APP_ERR_TRAP(err);
            }
            else if (SET_FLAG == g_invalid_sequence)
            {
                APP_PRINT("\r\nPlease set RTC calendar Date and Time\r\n");
                APP_PRINT("\r\nEnter any key (other than 1,2,3,4) to go back to the menu \r\n");
                g_invalid_sequence = RESET_FLAG;
            }
            else
            {
                APP_PRINT("\r\nRTC Calendar Alarm is set successfully");
                APP_PRINT("\r\nEnter any key (other than 1,2,3,4) to go back to the menu \r\n");
            }
            break;
        }

        case SET_PERIODIC_IRQ:
        {
            err = set_rtc_periodic_rate();
            if (FSP_SUCCESS != err)
            {
                rtc_deinit();
                APP_ERR_PRINT("\r\nPeriodic interrupt rate setting failed.\r\nClosing the driver. Restart the Application\r\n");
                APP_ERR_TRAP(err);
            }
            else
            {
                APP_PRINT("\r\nPeriodic interrupt rate is set successfully");
                APP_PRINT("\r\nEnter any key (other than 1,2,3,4) to go back to the menu \r\n");
            }
            break;
        }

        case GET_CURRENT_TIME:
        {
            err = get_rtc_calendar_time();
            if (FSP_SUCCESS != err)
            {
                rtc_deinit();
                APP_ERR_PRINT("\r\nGetting current RTC Calendar time failed.\r\nClosing the driver. Restart the Application\r\n");
                APP_ERR_TRAP(err);
            }
            else
            {
                APP_PRINT("\r\nGetting current RTC Calendar time is successful");
                APP_PRINT("\r\nEnter any key (other than 1,2,3,4) to go back to the menu \r\n");
            }
            break;
        }

        default:
        {
            APP_PRINT(MENU_INFO);
            break;
        }
    }
}

/*******************************************************************************************************************//**
 * @brief This function sets the level of LED pins
 *
 * @param[IN]   led_level     level to be set
 * @retval      None
 **********************************************************************************************************************/
static void led_level_set(bsp_io_level_t led_level)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Set LED status */
    err = R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[RESET_VALUE], led_level);
    if(FSP_SUCCESS != err)
    {
        rtc_deinit();
        APP_ERR_PRINT("\r\n ** R_IOPORT_PinWrite FAILED ** \r\n");
        APP_ERR_TRAP(err);
    }
}

/*******************************************************************************************************************//**
 * This function is called at various points during the startup process.  This implementation uses the event that is
 * called right before main() to set up the pins.
 *
 * @param[in]  event    Where at in the start up process the code is currently at
 **********************************************************************************************************************/
void R_BSP_WarmStart(bsp_warm_start_event_t event) {
    if (BSP_WARM_START_POST_C == event) {
        /* C runtime environment and system clocks are setup. */

        /* Configure pins. */
        R_IOPORT_Open(&g_ioport_ctrl, &g_bsp_pin_cfg);
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup rtc_ep)
 **********************************************************************************************************************/

/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/**********************************************************************************************************************
 * File Name    : lp_mode.c
 * Version      : .
 * Description  : .
 *********************************************************************************************************************/



#include "common_utils.h"
#include "common_init.h"
#include "jlink_console.h"
#include "source_cancel_lpm.h"
#include "lp_mode.h"
#include "menu_main.h"
#include "board_leds.h"
#include "menu_slcd.h"
#include "slcd.h"
#include "agt_slcd_fresh.h"
#include "update_slcd.h"


#define  LPM_SLEEP                  (1)
#define  LPM_SW_STANDBY             (2)
#define  SUB_OPTIONS                "\r\n> Select from the options in the menu below (0 to %d):\r\n\r\nMENU"
#define  SECOND                     (1000)


/* set 1 and disable RTC period weak in FSP to make the weak up time longer (1 minute) but the alarm cannot be used*/
#define  MINTUES_WEAK               (1)

/**********************************************************************************************************************
* External variables
**********************************************************************************************************************/
extern bsp_leds_t g_bsp_leds;

extern rtc_alarm_time_t g_rtc_alarm;


bool_t g_s_alarm_reached = false;


volatile EventBits_t g_s_lp_bits;

static rtc_time_t s_current_time =
{ };
static rtc_time_t s_latest_update_time =
{ };
rtc_alarm_time_t g_mins_weak =
{ };

bool_t g_alarm_ring = false;

uint32_t g_alarm_cal = 0;

extern int16_t g_alarm_flash;

/**********************************************************************************************************************
* Private global variables declarations
**********************************************************************************************************************/
/* LPM instances */
static lpm_instance_t const * sp_g_lpm_instance[APP_LPM_MAX_STATE] =
{
    [APP_LPM_SLEEP_STATE] = &g_lpm_sleep,
#if (BSP_FEATURE_LPM_HAS_DEEP_SLEEP)
    [APP_LPM_DEEP_SLEEP_STATE] = &g_lpm_deep_sleep,
#endif
    [APP_LPM_SW_STANDBY_STATE] = &g_lpm_sw_standby,
#if (BSP_FEATURE_LPM_HAS_DEEP_STANDBY)
    [APP_LPM_DEEP_SW_STANDBY_STATE] = &g_lpm_deep_sw_standby,
#endif
};


/* LPM Mode name */
static const char_t * sp_g_lpm_mode_name[APP_LPM_MAX_STATE] =
{
    [APP_LPM_SLEEP_STATE]                   = NAME_LPM_SLEEP_MODE,
#if (BSP_FEATURE_LPM_HAS_DEEP_SLEEP)
    [APP_LPM_DEEP_SLEEP_STATE]              = NAME_LPM_DEEP_SLEEP_MODE,
#endif
    [APP_LPM_SW_STANDBY_STATE]              = NAME_LPM_SW_STANDBY_MODE,
#if (BSP_FEATURE_LPM_HAS_DEEP_STANDBY)
    [APP_LPM_DEEP_SW_STANDBY_STATE]         = NAME_LPM_DEEP_SW_STANDBY_MODE,
#endif
};


/**********************************************************************************************************************
* Private function declarations
**********************************************************************************************************************/

static test_fn lp_mode_demo (void);


static fsp_err_t lpm_enter_procedure (e_app_lpm_states_t lpm_mode);
static fsp_err_t lpm_exit_procedure (e_app_lpm_states_t lpm_mode);
static fsp_err_t lpm_wait_start (e_app_lpm_states_t lpm_mode);
static fsp_err_t lpm_operation (e_app_lpm_states_t lpm_mode);
static fsp_err_t lpm_ep_startup (void);

static void print_lpm_menu (void);
static void led_lpm_state_set (e_led_power_t state);
static void led_sleep_state_set (e_led_power_t state);
static void led_sw_standby_state_set (e_led_power_t state);
static void lpm_execute (void);
static void gpt_start (void);
static void gpt_stop (void);
static void lpm_sidefunction_stop (void);
static void lpm_sidefunction_start (void);

static char_t s_print_buffer[BUFFER_LINE_LENGTH] = {};






static int8_t  s_g_lpm_selected_menu;
static int8_t  s_g_menu_limit   = 1;
static uint8_t s_lpm_mode_case  = 0;

bool g_abort_demo = false;




static st_menu_fn_tbl_t s_lpm_menu_items[] =
{
    /* Human readable name             state                status lock management  function called  */
    {"Sleep Mode"           , false, true, lp_mode_demo},
    {"Software Standby Mode", false, true, lp_mode_demo},
    {""                     , false, true, NULL}
};

/**********************************************************************************************************************
 * Function Name: lpm_main_display_menu
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
test_fn lpm_main_display_menu(void)
{
    g_s_lp_bits = 0;
    gpt_stop();
    int8_t c = -1;
    while (MENU_EXIT_CRTL != c)
    {
        /* Turn off LEDs state at start up */
        led_lpm_state_set(LED_POWER_ON);
        led_sleep_state_set(LED_POWER_OFF);
        led_sw_standby_state_set(LED_POWER_OFF);
        if ((LPM_SWITCH_MODE == g_s_lp_bits) && (true == s_lpm_menu_items[LPM_SLEEP - 1].status))
        {
            /* Switch function from sleep mode to software standby mode*/
            s_lpm_menu_items[LPM_SLEEP - 1].status = false;
            s_g_lpm_selected_menu                  = LPM_SW_STANDBY;
        }
        else if ((LPM_SWITCH_MODE == g_s_lp_bits) && (true == s_lpm_menu_items[LPM_SW_STANDBY - 1].status))
        {
            /* Switch function from software standby mode to sleep mode*/
            s_lpm_menu_items[LPM_SW_STANDBY- 1].status = false;
            s_g_lpm_selected_menu                      = LPM_SLEEP;
        }
        else if (LPM_SWITCH_MODE == g_s_lp_bits)
        {
            /* Go into the sleep mode if button pressed*/
            s_g_lpm_selected_menu = LPM_SLEEP;
            c                     = -1;
        }else
        {
            /* Exit sleep/software standby mode when SW2 pressed*/
            s_lpm_menu_items[s_g_lpm_selected_menu].status = false;
            s_g_lpm_selected_menu                          = 0;
            c                                              = -1;
        }
        if (!(LPM_SWITCH_MODE == g_s_lp_bits))
        {
            print_lpm_menu();
            start_key_check();
        }

        while ((0 != c) || (LPM_SWITCH_MODE == g_s_lp_bits))
        {
            /* Check key press*/
            if (key_pressed () || (LPM_SWITCH_MODE == g_s_lp_bits))
            {
                /* Skip key detection when flag was up*/
                if (!(LPM_SWITCH_MODE == g_s_lp_bits))
                {
                    c     = (int8_t)get_detected_key(); /* Cast to int8_t */
                    if (' ' == c)
                    {
                        break;
                    }

                    /* Cast, as compiler will assume calc is int */
                    c                     = (int8_t) (c - '0');
                    s_g_lpm_selected_menu = c;
                }
                if ((-1) != c)
                {
                    g_s_lp_bits = 0;
                }
                if ((s_g_lpm_selected_menu > 0) && (s_g_lpm_selected_menu <= s_g_menu_limit))
                {
                    g_s_lp_bits = 0;

                    /* map key press 1 to option 0 */
                    s_g_lpm_selected_menu = s_g_lpm_selected_menu - 1;
                    if (true == s_lpm_menu_items[s_g_lpm_selected_menu].status)
                    {
                            s_lpm_menu_items[s_g_lpm_selected_menu].status = false;
                            break;
                    }
                    else
                    {
                            s_lpm_menu_items[s_g_lpm_selected_menu].status = true;
                            break;
                    }

                }
                else if ((LPM_SWITCH_MODE == g_s_lp_bits))
                {
                    break;
                }else
                {
                    sprintf(s_print_buffer, "\r\nPlease type a valid input\r\n");
                    print_to_console (s_print_buffer);
                    c                     = -1;
                    s_g_lpm_selected_menu = 0;
                    R_BSP_SoftwareDelay(SECOND*3, BSP_DELAY_UNITS_MILLISECONDS);
                    break;
                }
            }
        }
        lpm_execute();
    }
    gpt_start();
    return 0;
}
/**********************************************************************************************************************
 End of function lpm_main_display_menu
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: lpm_execute
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
static void lpm_execute(void)
{
    uint8_t array_size = (sizeof(s_lpm_menu_items)) /(sizeof(s_lpm_menu_items[0]));

    /* Check the status is true or not*/
    for (uint8_t test_active = 0; test_active < array_size; test_active++)
    {
        if (true == s_lpm_menu_items[test_active].status)
        {
            /* Run low power mode*/
            s_lpm_mode_case = test_active;
            lp_mode_demo();
        }
    }
}
/**********************************************************************************************************************
 End of function lpm_execute
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: gpt_stop
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
static void gpt_stop(void)
{
    R_GPT_Stop (g_blinker.p_ctrl);
    R_GPT_Stop (g_gpt_blue.p_ctrl);
}
/**********************************************************************************************************************
 End of function gpt_stop
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: gpt_start
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
static void gpt_start(void)
{
    R_GPT_Start (g_blinker.p_ctrl);
    R_GPT_Start (g_gpt_blue.p_ctrl);
}
/**********************************************************************************************************************
 End of function gpt_start
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: print_lpm_menu
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
static void print_lpm_menu(void)
{
    sprintf (s_print_buffer, "%s%s", gp_clear_screen, gp_cursor_home);
    print_to_console (s_print_buffer);

    s_g_menu_limit = ((sizeof(s_lpm_menu_items)) / sizeof(st_menu_fn_tbl_t)) - 1;

    sprintf(s_print_buffer, LPM_BANNER);
    print_to_console (s_print_buffer);

}
/**********************************************************************************************************************
 End of function print_lpm_menu
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: lpm_ep_startup
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
static fsp_err_t lpm_ep_startup(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* In case of internal reset */
    if (LPM_LEAVE_MODE == g_s_lp_bits)
    {
        /* Verify data stored in Standby SRAM */
#if (BSP_FEATURE_LPM_HAS_DEEP_STANDBY) && defined(SSRAM_START) && defined(SSRAM_LENGTH)
        if (false == standby_sram_verify(APP_SSRAM_OFFSET, (uint8_t *)APP_SSRAM_DATA, APP_SSRAM_LEN))
        {
            sprintf(s_print_buffer, "standby_sram_verify failed\r\n");
            print_to_console (s_print_buffer);
        }

        TERMINAL_PRINT("\r\nData stored in standby SRAM matches fixed data\r\n");
#endif
        /* Prints a new cycle start message */
        sprintf(s_print_buffer, "\r\nStart a new LPM operating cycle\r\n");
        print_to_console (s_print_buffer);
    }

    /* In case of normal reset */
    else
    {

        /* Store fixed data into standby SRAM */
#if (BSP_FEATURE_LPM_HAS_DEEP_STANDBY) && defined(SSRAM_START) && defined(SSRAM_LENGTH)
        if (false == standby_sram_write(APP_SSRAM_OFFSET, (uint8_t *)APP_SSRAM_DATA, APP_SSRAM_LEN))
        {
            sprintf(s_print_buffer, "standby_sram_write failed\r\n");
            print_to_console (s_print_buffer);

        }

        /* Print stored successful message */
        sprintf("\r\nStored fixed data into standby SRAM\r\n");
#else

#endif
    }

    /* Initialize all modules used to cancel LPM */
    err = source_cancel_lpm_init();
    if (FSP_SUCCESS != err)
    {
        sprintf(s_print_buffer, "source_cancel_lpm_init Failed\r\n");
        print_to_console (s_print_buffer);
    }

    return err;
}
/**********************************************************************************************************************
 End of function lpm_ep_startup
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: lp_mode_demo
 * Description  : .
 * Argument     : lpm_mode
 * Return Value : 0
 *********************************************************************************************************************/
static test_fn lp_mode_demo(void)
{
    fsp_err_t err = FSP_SUCCESS;
    lpm_ep_startup();
    if (FSP_SUCCESS != err)
    {
        sprintf(s_print_buffer, "lpm_ep_startup Failed\r\n");
        print_to_console (s_print_buffer);
    }

    switch (s_lpm_mode_case)
    {
        case 0:
            sprintf(s_print_buffer, LPM_SLEEP_MODE);
            print_to_console (s_print_buffer);
            lpm_operation(APP_LPM_SLEEP_STATE);
            break;
        case 1:
            sprintf(s_print_buffer, LPM_SOFTWARE_STANDBY_MODE);
            print_to_console (s_print_buffer);
            lpm_operation(APP_LPM_SW_STANDBY_STATE);
            break;
        default:
            break;
    }
    return 0;
}
/**********************************************************************************************************************
 End of function lp_mode_demo
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: lpm_operation
 * Description  : This function will perform a sample LPM operation
 * Argument     : lpm_mode
 * Return Value : FSP_SUCCESS    Upon successful operation
 *                Any Other Error code apart from FSP_SUCCES
 *********************************************************************************************************************/
static fsp_err_t lpm_operation(e_app_lpm_states_t lpm_mode)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Check valid LPM mode */
    if (APP_LPM_MAX_STATE <= lpm_mode)
    {
        sprintf(s_print_buffer, "This MCU does not support this LPM mode\r\n");
        print_to_console (s_print_buffer);
    }

    /* Open this LPM mode */
    err = R_LPM_Open(sp_g_lpm_instance[lpm_mode]->p_ctrl, sp_g_lpm_instance[lpm_mode]->p_cfg);
    if (FSP_SUCCESS != err)
    {
        sprintf(s_print_buffer, "R_LPM_Open API Failed\r\n");
        print_to_console (s_print_buffer);
    }

    /* Procedure before enter to LPM  */
    err = lpm_enter_procedure(lpm_mode);
    if (FSP_SUCCESS != err)
    {
        sprintf(s_print_buffer, "lpm_enter_procedure Failed\r\n");
        print_to_console (s_print_buffer);
    }
    led_lpm_state_set(LED_POWER_OFF);

    if (APP_LPM_SW_STANDBY_STATE == lpm_mode)
    {
        led_sw_standby_state_set(LED_POWER_ON);
    }

    /* Enter to LPM
     * Turn off the function to reduce power
     * */

    R_RTC_CalendarTimeGet (&g_rtc_ctrl, &s_latest_update_time);

    /* set alarm to weak the software standby mode*/
#ifdef MINTUES_WEAK

    /* Cast time from g_rtc_alarm to uint32_t for calculation */
    uint32_t g_rtc_alarm_mintues = (uint32_t)(((uint32_t)g_rtc_alarm.time.tm_wday * DAY_HOURS * HOUR_MINUTES) +
            (((uint32_t)g_rtc_alarm.time.tm_hour * HOUR_MINUTES)) + (uint32_t)g_rtc_alarm.time.tm_min);

    /* Cast time from g_rtc_alarm to uint32_t for calculation */
    uint32_t latest_update_time_mintues = (uint32_t)
            ((uint32_t)s_latest_update_time.tm_wday * DAY_HOURS * HOUR_MINUTES) +
            ((uint32_t)s_latest_update_time.tm_hour * HOUR_MINUTES) + (uint32_t)s_latest_update_time.tm_min;
    if (g_rtc_alarm_mintues > latest_update_time_mintues)
    {
        g_alarm_cal = g_rtc_alarm_mintues - latest_update_time_mintues;
    }else
    {
        g_alarm_cal = (WEEKDAYS * DAY_HOURS * HOUR_MINUTES) - (latest_update_time_mintues - g_rtc_alarm_mintues);
    }

    g_mins_weak.time        = s_latest_update_time;
    g_mins_weak.time.tm_min = g_mins_weak.time.tm_min + 1;
    g_mins_weak.min_match   = true;

    R_RTC_CalendarAlarmSet (&g_rtc_ctrl, &g_mins_weak);
#else
    R_RTC_PeriodicIrqRateSet(&g_rtc_ctrl, RTC_PERIODIC_IRQ_SELECT_2_SECOND);
#endif

    lpm_sidefunction_stop();
    err = R_LPM_LowPowerModeEnter(sp_g_lpm_instance[lpm_mode]->p_ctrl);
    if (FSP_SUCCESS != err)
    {
        sprintf(s_print_buffer, "R_LPM_LowPowerModeEnter API Failed\r\n");
        print_to_console (s_print_buffer);
    }

    lpm_wait_start(lpm_mode);
    R_RTC_PeriodicIrqRateSet(&g_rtc_ctrl, RTC_PERIODIC_IRQ_SELECT_1_SECOND);
    led_sleep_state_set(LED_POWER_OFF);
    led_sw_standby_state_set(LED_POWER_OFF);
    g_alarm_cal = 0;

    /* Procedure after exit LPM */
    err = lpm_exit_procedure(lpm_mode);
    if (FSP_SUCCESS != err)
    {
        sprintf(s_print_buffer, "lpm_exit_procedure Failed\r\n");
        print_to_console (s_print_buffer);
    }

    /* Close LPM */
    err = R_LPM_Close(sp_g_lpm_instance[lpm_mode]->p_ctrl);
    if (FSP_SUCCESS != err)
    {
        sprintf(s_print_buffer, "R_LPM_Close API Failed\r\n");
        print_to_console (s_print_buffer);
    }

    print_lpm_menu();
    return err;
}
/**********************************************************************************************************************
 End of function lpm_operation
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: lpm_sidefunction_stop
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
static void lpm_sidefunction_stop(void)
{
    R_ADC_Close(g_adc.p_ctrl);
    R_GPT_Stop (g_blinker.p_ctrl);
    R_GPT_Stop (g_gpt_blue.p_ctrl);
    if (!((g_alarm_flash > 0) || g_s_alarm_reached))
    {
        R_AGT_Stop(&g_timer0_ctrl);
    }
    cgc_clocks_cfg_t clk_set;


    clk_set.system_clock    = CGC_CLOCK_SUBCLOCK;
    clk_set.loco_state      = CGC_CLOCK_CHANGE_STOP;
    clk_set.moco_state      = CGC_CLOCK_CHANGE_STOP;
    clk_set.hoco_state      = CGC_CLOCK_CHANGE_STOP;
    clk_set.mainosc_state   = CGC_CLOCK_CHANGE_NONE;
    clk_set.pll_state       = CGC_CLOCK_CHANGE_STOP;
    clk_set.pll2_state      = CGC_CLOCK_CHANGE_STOP;
    clk_set.subosc_state    = CGC_CLOCK_CHANGE_NONE;
    g_cgc.p_api->clocksCfg(g_cgc.p_ctrl, &clk_set);
    bsp_prv_operating_mode_set(BSP_PRV_OPERATING_MODE_SUBOSC_SPEED);
}
/**********************************************************************************************************************
 End of function lpm_sidefunction_stop
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: lpm_enter_procedure
 * Description  : This function will perform a procedure before entering LPM mode
 * Argument     : lpm_mode
 * Return Value : .
 *********************************************************************************************************************/
static fsp_err_t lpm_enter_procedure(e_app_lpm_states_t lpm_mode)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Print LPM mode name */
    sprintf(s_print_buffer, (const char *)sp_g_lpm_mode_name[lpm_mode]);
    print_to_console (s_print_buffer);

    /* Turn off blue LED in case of LPM mode */
    led_lpm_state_set(LED_POWER_OFF);


    /* Start/enable hardware modules to cancel LPM mode */
    err = source_cancel_lpm_enable();
    if (FSP_SUCCESS != err)
    {
        sprintf(s_print_buffer, "source_cancel_lpm_enable Failed\r\n");
        print_to_console (s_print_buffer);
    }


#if defined(BOARD_RA8D1_EK) || defined(BOARD_RA8M1_EK) || defined(BOARD_RA8T1_MCK)
    /* SCI UART module Stop with Undefined in SW Standby and Deep SW Standby mode */
    if (APP_LPM_SW_STANDBY_STATE == lpm_mode)
    {
        uart_deinit();
    }
#endif

    /* Disable unused IO ports to reduce power */
    err = R_IOPORT_PinsCfg(&g_ioport_ctrl, &g_bsp_pin_cfg);
    if (FSP_SUCCESS != err)
    {
        sprintf(s_print_buffer, "R_IOPORT_PinsCfg API Failed\r\n");
        print_to_console (s_print_buffer);
    }

    return err;
}
/**********************************************************************************************************************
 End of function lpm_enter_procedure
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: lpm_exit_procedure
 * Description  : This function will perform a procedure after exiting LPM mode
 * Argument     : lpm_mode
 * Return Value : .
 *********************************************************************************************************************/
static fsp_err_t lpm_exit_procedure(e_app_lpm_states_t lpm_mode)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Put IO port configuration back to user's selections */
    err = R_IOPORT_PinsCfg(&g_ioport_ctrl, &g_bsp_pin_cfg);

    if (FSP_SUCCESS != err)
    {
        sprintf(s_print_buffer, "R_IOPORT_PinsCfg API Failed\r\n");
        print_to_console (s_print_buffer);
    }


    (void) lpm_mode;

    /* Turn on blue LED in case of normal mode */
    led_lpm_state_set(LED_POWER_ON);

    return err;
}
/**********************************************************************************************************************
 End of function lpm_exit_procedure
 *********************************************************************************************************************/



/**********************************************************************************************************************
 * Function Name: led_lpm_state_set
 * Description  : This function will set the state of the Blue LED
 * Argument     : state
 * Return Value : .
 *********************************************************************************************************************/
static void led_lpm_state_set(e_led_power_t state)
{
    /* Check board has this LED */
    if (LED_LPM_STATE < g_bsp_leds.led_count)
    {
        /* Cast to bsp_io_port_pin_t and bsp_io_level_t */
        R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t)g_bsp_leds.p_leds[LED_LPM_STATE], (bsp_io_level_t)state);
    }
}
/**********************************************************************************************************************
 End of function led_lpm_state_set
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: led_sw_standby_state_set
 * Description  : This function will set the state of the Red LED
 * Argument     : state
 * Return Value : .
 *********************************************************************************************************************/
static void led_sw_standby_state_set(e_led_power_t state)
{
    /* Check board has this LED */
    if (LED_SW_STANDBY_STATE < g_bsp_leds.led_count)
    {
        /* Cast to bsp_io_port_pin_t and bsp_io_level_t */
        R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t)g_bsp_leds.p_leds[LED_SW_STANDBY_STATE], \
                            (bsp_io_level_t)state);
    }
}
/**********************************************************************************************************************
 End of function led_sw_standby_state_set
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: led_sleep_state_set
 * Description  : This function will set the state of the green LED
 * Argument     : state
 * Return Value : .
 *********************************************************************************************************************/
static void led_sleep_state_set(e_led_power_t state)
{
    /* Check board has this LED */
    if (LED_SLEEP_STATE < g_bsp_leds.led_count)
    {
        /* Cast to bsp_io_port_pin_t and bsp_io_level_t */
        R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t)g_bsp_leds.p_leds[LED_SLEEP_STATE], (bsp_io_level_t)state);
    }
}
/**********************************************************************************************************************
 End of function led_sleep_state_set
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: lpm_wait_start
 * Description  : This function will wait for trigger to start LPM operation sequence
 * Argument     : lpm_mode
 * Return Value : FSP_SUCCESS    Upon successful operation
 *                Any Other Error code apart from FSP_SUCCESS
 *********************************************************************************************************************/
static fsp_err_t lpm_wait_start(e_app_lpm_states_t lpm_mode)
{
    fsp_err_t err = FSP_SUCCESS;

    if (APP_LPM_SLEEP_STATE == lpm_mode)
    {
        led_sleep_state_set(LED_POWER_ON);
    }

    /* Wait for an event */
    while (0 == g_s_lp_bits)
    {
        if (g_alarm_ring)
        {
            /*Update alarm time*/
            R_RTC_CalendarTimeGet (&g_rtc_ctrl, &s_current_time);
            set_time_on_slcd (&s_current_time);
            g_mins_weak.time        = s_current_time;
            g_mins_weak.time.tm_min = g_mins_weak.time.tm_min + 1;
            R_RTC_CalendarAlarmSet (&g_rtc_ctrl, &g_mins_weak);
            g_alarm_flash = 0;
            g_alarm_ring  = false;
        }
        if (g_s_alarm_reached)
        {
            /* Check if the alarm was activate or not */
            g_alarm_flash = (int16_t)(HOUR_MINUTES - s_current_time.tm_sec);
            agt_period_start();
            g_s_alarm_reached = false;
        }
        if (0 == g_alarm_cal)
        {
            /*Check if the alarm was set when device is in low power mode*/

            g_alarm_flash = 60;
            agt_period_start();

            /*reset the alarm for a week after*/
            g_alarm_cal += (HOUR_MINUTES * DAY_HOURS * WEEKDAYS);
        }

        if (APP_LPM_SW_STANDBY_STATE == lpm_mode)
        {
            /*re-enter the software standby mode once alarm wakes device up*/
            R_RTC_CalendarTimeGet (&g_rtc_ctrl, &s_current_time);
            led_lpm_state_set(LED_POWER_OFF);
            led_sw_standby_state_set(LED_POWER_ON);
            set_time_on_slcd (&s_current_time);
            s_latest_update_time.tm_min = s_current_time.tm_min;
            R_LPM_Close(sp_g_lpm_instance[lpm_mode]->p_ctrl);
            R_LPM_Open(sp_g_lpm_instance[lpm_mode]->p_ctrl, sp_g_lpm_instance[lpm_mode]->p_cfg);
            R_LPM_LowPowerModeEnter(sp_g_lpm_instance[lpm_mode]->p_ctrl);
        }else
        {
            led_lpm_state_set(LED_POWER_OFF);
            led_sleep_state_set(LED_POWER_ON);
        }
    }
    lpm_sidefunction_start();
    if (g_rtc_alarm.dayofweek_match)
    {
        set_segment_icon_named (ALARM_ICON, ICON_ON);
    }


    if (LPM_SWITCH_MODE == g_s_lp_bits)
    {
        sprintf(s_print_buffer, "MCU detect SW1 (IRQ6) was pressed.\r\n");
        print_to_console (s_print_buffer);
    }else
    {
        sprintf(s_print_buffer, "MCU detect SW2 (IRQ7) was pressed.\r\n");
        print_to_console (s_print_buffer);

        g_mins_weak.min_match = false;
        g_alarm_flash         = 0;

        R_RTC_CalendarAlarmSet(&g_rtc_ctrl, &g_rtc_alarm);

        /* reset LPM_LEAVE_MODE flag*/
        g_s_lp_bits = 0;
    }

    R_BSP_SoftwareDelay(SECOND*2, BSP_DELAY_UNITS_MILLISECONDS);

    return err;
}
/**********************************************************************************************************************
 End of function lpm_wait_start
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: lpm_sidefunction_start
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
static void lpm_sidefunction_start(void)
{
    cgc_clocks_cfg_t clk_set;

    bsp_prv_operating_mode_set(BSP_PRV_OPERATING_MODE_HIGH_SPEED);

    clk_set.system_clock                        = CGC_CLOCK_PLL;
    clk_set.divider_cfg.sckdivcr_b.iclk_div     = CGC_SYS_CLOCK_DIV_1;
    clk_set.divider_cfg.sckdivcr_b.pclka_div    = CGC_SYS_CLOCK_DIV_1;
    clk_set.divider_cfg.sckdivcr_b.pclkb_div    = CGC_SYS_CLOCK_DIV_2;
    clk_set.divider_cfg.sckdivcr_b.pclkc_div    = CGC_SYS_CLOCK_DIV_2;
    clk_set.divider_cfg.sckdivcr_b.pclkd_div    = CGC_SYS_CLOCK_DIV_1;
    clk_set.divider_cfg.sckdivcr_b.fclk_div     = CGC_SYS_CLOCK_DIV_2;
    clk_set.pll_cfg.source_clock                = CGC_CLOCK_MAIN_OSC;
    clk_set.pll_cfg.divider                     = CGC_PLL_DIV_1;
    clk_set.pll_cfg.multiplier                  = CGC_PLL_MUL_10_0;
    clk_set.loco_state                          = CGC_CLOCK_CHANGE_START;
    clk_set.moco_state                          = CGC_CLOCK_CHANGE_START;
    clk_set.hoco_state                          = CGC_CLOCK_CHANGE_START;
    clk_set.mainosc_state                       = CGC_CLOCK_CHANGE_NONE;
    clk_set.pll_state                           = CGC_CLOCK_CHANGE_START;
    clk_set.pll2_state                          = CGC_CLOCK_CHANGE_START;
    clk_set.subosc_state                        = CGC_CLOCK_CHANGE_NONE;
    g_cgc.p_api->clocksCfg(g_cgc.p_ctrl, &clk_set);
    jlink_console_init();
    common_init();
    R_GPT_Stop (g_blinker.p_ctrl);
    R_GPT_Stop (g_gpt_blue.p_ctrl);
}
/**********************************************************************************************************************
 End of function lpm_sidefunction_start
 *********************************************************************************************************************/


/***********************************************************************************************************************
 * File Name    : lpm_app_trigger_end_source.c
 * Description  : Contains function definition for the trigger and end sources and its supporting functions.
 **********************************************************************************************************************/
/**********************************************************************************************************************
* Copyright (c) 2019 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*********************************************************************************************************************/
#include <lpm_app_util.h>
#include "common_utils.h"

/* event_type keeps the latest event. By default it will be Power on Reset event*/
u_event_t event_type =  EV_POWER_ON_RESET;

/**********************************************************************************************************************
 * @brief       This function initializes and enables the ICU module for User Push Button Switch.
 * @param[IN]   None
 * @retval      FSP_SUCCESS                  Upon successful open and enable ICU module
 * @retval      Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
fsp_err_t user_switch_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Open external IRQ/ICU module */
    err = R_ICU_ExternalIrqOpen(&g_external_irq_user_sw_ctrl, &g_external_irq_user_sw_cfg);
    if(FSP_SUCCESS == err)
    {
    	/* Enable ICU module */
    	err = R_ICU_ExternalIrqEnable(&g_external_irq_user_sw_ctrl);
    }
    return err;
}

/***********************************************************************************************************************
 * @brief      User defined external irq callback for User Push button Switch.
 * @param[IN]  p_args
 * @retval     None
 **********************************************************************************************************************/
void external_irq_user_sw_cb(external_irq_callback_args_t *p_args)
{
	fsp_err_t err = FSP_SUCCESS;
	rtc_time_t rtc_time = {RESET_VALUE};
	uint32_t rtc_time_in_seconds = RESET_VALUE;
	static  uint32_t prv_rtc_time_in_seconds  = RESET_VALUE;
	static volatile switch_state_t  switch_state = SW_INVALID;

	/* Make sure it's the right interrupt*/
	if (USER_SW_IRQ_NUMBER == p_args->channel)
	{
		/* RTC Time is used for measuring the Hold time of the Switch.
		 * This helps to identify Short press vs Long Press
		 */
		err = R_RTC_CalendarTimeGet(&g_rtc_ctrl, &rtc_time);
		if (FSP_SUCCESS != err)
		{
			APP_ERR_TRAP(err);
		}

		rtc_time_in_seconds = (uint32_t)((rtc_time.tm_hour * SECONDS_IN_A_HOUR)  +
				              (rtc_time.tm_min * SECONDS_IN_A_MINUTE) + (rtc_time.tm_sec));

		/*
		 *  Need to keep the last state of the Switch. switch_state maintains this. Interrupt is configured
		 * for rising and falling edge. We will get interrupt for rising edge as well as falling edge.
		 * Falling edge is Switch pressed. Rising edge is Switch released. Time it is held gives the info
		 * on Short Press vs Long Press
		 */
		if (SW_RELEASED == switch_state)
		{
			switch_state = SW_PRESSED;
			prv_rtc_time_in_seconds = 0;
		} else if (SW_PRESSED == switch_state)
		{
			switch_state = SW_RELEASED;
		} else
		{
			switch_state = SW_PRESSED;
		}

		/* Check if the switch was pressed and held between 1-2 seconds or 3-6 seconds, and later released */
		if ((rtc_time_in_seconds > prv_rtc_time_in_seconds) && (SW_RELEASED == switch_state))
		{
			if ((rtc_time_in_seconds <= (prv_rtc_time_in_seconds + (USER_SW_RANGE_LONG_PRESS_HIGH_LIMT))) &&
				(rtc_time_in_seconds >= (prv_rtc_time_in_seconds + (USER_SW_RANGE_LONG_PRESS_LOW_LIMT))))
			{
				event_type = EV_PB_LONG_PR;
			}
			else if ((rtc_time_in_seconds <= (prv_rtc_time_in_seconds + USER_SW_RANGE_SHORT_PRESS)) ||
					 (rtc_time_in_seconds >= (prv_rtc_time_in_seconds + USER_SW_RANGE_SHORT_PRESS)))
			{
				event_type = EV_PB_SHORT_PR;
			}
			else
			{
                 ;
			}
		}
	}
	if (SW_PRESSED == switch_state)
	{
		/* Store it for now, we need this when the switch is released */
		prv_rtc_time_in_seconds = rtc_time_in_seconds;
	}
}

/***********************************************************************************************************************
 * @brief       This function starts the Periodic timer (AGT1 timer).
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
void startPeriodicTimer()
{
    fsp_err_t err = FSP_SUCCESS;

	err = R_AGT_Start(&agt_timer1_snooze_trg_source_ctrl);

	if(FSP_SUCCESS != err)
    {
		APP_ERR_TRAP(err);
    }
}

/***********************************************************************************************************************
 * @brief       This function stops AGT1 timer and Resets the Count.
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
void stopPeriodicTimer()
{
    fsp_err_t err = FSP_SUCCESS;

	err = R_AGT_Stop(&agt_timer1_snooze_trg_source_ctrl);
	if(FSP_SUCCESS != err)
    {
		APP_ERR_TRAP(err);
    }

	err = R_AGT_Reset(&agt_timer1_snooze_trg_source_ctrl);
	if(FSP_SUCCESS != err)
    {
		APP_ERR_TRAP(err);
    }
}

/***********************************************************************************************************************
 * @brief This function is the callback for AGT1 timer.
 * @param[in] (timer_callback_args_t *)
 * @retval None
 **********************************************************************************************************************/
void agt_timer1_periodic_cb(timer_callback_args_t *p_args)
{
    if(TIMER_EVENT_CYCLE_END == p_args->event)
    {
    	/* When the Timer Count expires, callback sets the event. This is an event for the LPM SM */
    	event_type = EV_PERIODIC_TIMER;
	}
}

/***********************************************************************************************************************
 * @brief       Function to toggle user LED. Turns ON if it was OFF, OFF if it was ON, with 500 msec delay
 * @param[IN]   pointer to the BSP LED structure
 * @retval      fsp_err_t
 **********************************************************************************************************************/
fsp_err_t user_led_toggle(bsp_leds_t *pleds)
{
    fsp_err_t err = FSP_SUCCESS;
	static bsp_io_level_t level = BSP_IO_LEVEL_HIGH;

	/* Turns OFF/ON the user LED based on the value of the level */
	err = R_IOPORT_PinWrite(&g_ioport_ctrl, pleds->p_leds[LED_NO_0], level);
	/* Handle error */
	if (FSP_SUCCESS == err)
	{
		/* Delay 500 milliseconds */
		delay_500ms();
    }

	if(BSP_IO_LEVEL_HIGH == level)
	{
       	level = BSP_IO_LEVEL_LOW;
	}
	else
	{
		level = BSP_IO_LEVEL_HIGH;
	}
    return err;
}

/***********************************************************************************************************************
 * @brief       Function to turn ON the user LED.
 * @param[IN]   leds
 * @retval      fsp_err_t
 **********************************************************************************************************************/
fsp_err_t user_led_on(bsp_leds_t *pleds)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Turn ON  user LED */
	err = R_IOPORT_PinWrite(&g_ioport_ctrl, pleds->p_leds[LED_NO_0], BSP_IO_LEVEL_HIGH);
    return err;
}

/***********************************************************************************************************************
 * @brief       Function to turn OFF the user LED.
 * @param[IN]   leds
 * @retval      fsp_err_t
 **********************************************************************************************************************/
fsp_err_t user_led_off(bsp_leds_t *pleds)
{
    fsp_err_t err = FSP_SUCCESS;

	/* Turn OFF the user LED */
	err = R_IOPORT_PinWrite(&g_ioport_ctrl, pleds->p_leds[LED_NO_0], BSP_IO_LEVEL_LOW);
    return err;
}

/***********************************************************************************************************************
 * @brief       Function to Initialize the RTC.
 * @param[IN]   void
 * @retval      fsp_err_t
 **********************************************************************************************************************/
fsp_err_t init_rtc()
{
	fsp_err_t err = FSP_SUCCESS;

	err = R_RTC_Open(&g_rtc_ctrl, &g_rtc_cfg);
	return err;
}

/***********************************************************************************************************************
 * @brief       This function reads the RTC time and prints the time, if the previous read time is incremented at least
 *              by 1 second.
 * @param[IN]   void
 * @retval      None
 **********************************************************************************************************************/
void read_and_print_rtc_time()
{
    fsp_err_t err = FSP_SUCCESS;
    static int32_t previous_rtc_time_in_seconds = RESET_VALUE;
    int32_t current_rtc_time_in_seconds = RESET_VALUE;
    rtc_time_t g_current_time = {RESET_VALUE};

    err = R_RTC_CalendarTimeGet(&g_rtc_ctrl, &g_current_time);
    if(FSP_SUCCESS != err)
    {
      APP_ERR_TRAP(err);
    }

    /* Add the adjust factor for Month and Year for User printing on to the console */
    g_current_time.tm_mon  +=  MON_ADJUST_VALUE;
    g_current_time.tm_year +=  YEAR_ADJUST_VALUE;

    current_rtc_time_in_seconds = (int32_t)((g_current_time.tm_hour * SECONDS_IN_A_HOUR)  +
			                      (g_current_time.tm_min * SECONDS_IN_A_MINUTE) + (g_current_time.tm_sec));

    if((current_rtc_time_in_seconds != previous_rtc_time_in_seconds))
    {
    	previous_rtc_time_in_seconds = g_current_time.tm_sec;

        APP_PRINT("\rRTC: Date (MM/DD/YYYY): %d/%d/%d, Time (HR:MIN:SEC): %d : %d : %d\r\n",
        		   g_current_time.tm_mday, g_current_time.tm_mon, g_current_time.tm_year,
				   g_current_time.tm_hour, g_current_time.tm_min, g_current_time.tm_sec);
    }
}

/***********************************************************************************************************************
 * @brief       This function is the callback for the RTC interrupts. Currently RTC is configured for Alarm interrupt
 *              only. RTC alarm event_type is updated here, when the RTC alarm interrupt triggers.
 * @param[IN]   void
 * @retval      None
 **********************************************************************************************************************/

void rtc_callback(rtc_callback_args_t *p_args)
{
    if(RTC_EVENT_ALARM_IRQ == p_args->event)
    {
    	event_type = EV_RTC_ALARM;
    }
}

/***********************************************************************************************************************
 * @brief       This functions enables the alarm seconds match flag and sets the alarm time for current RTC time
 *              +10 seconds.
 * @param[IN]   None
 * @retval      FSP_SUCCESS                  Upon successful Calendar alarm set
 * @retval      Any Other Error code apart from FSP_SUCCESS  Unsuccessful Calendar alarm set
 **********************************************************************************************************************/
fsp_err_t set_rtc_calendar_alarm(void)
{
    fsp_err_t err = FSP_SUCCESS;
    rtc_alarm_time_t alarm_time_set = {RESET_VALUE};
    rtc_time_t config_time = {RESET_VALUE};

    /* Set the flags for which the alarm has to be generated Setting it for seconds match*/
    alarm_time_set.hour_match  = false;
    alarm_time_set.min_match   = false;
    alarm_time_set.sec_match   = true;
    alarm_time_set.mday_match  = false;
    alarm_time_set.mon_match   = false;
    alarm_time_set.year_match  = false;

    /* Get the current Calendar time */
    err = R_RTC_CalendarTimeGet(&g_rtc_ctrl, &config_time);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nCalendarTime Get failed.\r\n");
        return err;
    }
    /* Adjust the desired Alarm time in seconds, so that seconds count rolls over from 60  */
    if((config_time.tm_sec + ALARM_TIME_IN_SECONDS) / SECONDS_IN_A_MINUTE)
    {
        alarm_time_set.time.tm_sec = ((config_time.tm_sec + ALARM_TIME_IN_SECONDS) % SECONDS_IN_A_MINUTE);
    }
    else
    {
        alarm_time_set.time.tm_sec = config_time.tm_sec + ALARM_TIME_IN_SECONDS;
    }
    /* Set the new alarm time calculated, to a specific second for the trigger to happen */
    err = R_RTC_CalendarAlarmSet(&g_rtc_ctrl, &alarm_time_set);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nCalendar alarm Set failed.\r\n");
        return err;
    }
    FSM_ALARM_PRINT
    return err;
}

/***********************************************************************************************************************
 * @brief       This functions resets the alarm time so that subsequent alarms are not triggered.
 * @param[IN]   None
 * @retval      FSP_SUCCESS                  Upon successful Calendar alarm set
 * @retval      Any Other Error code apart from FSP_SUCCESS  Unsuccessful Calendar alarm set
 **********************************************************************************************************************/
fsp_err_t reset_rtc_calendar_alarm(void)
{
    fsp_err_t err = FSP_SUCCESS;     // Error status
    rtc_alarm_time_t alarm_time_set = {RESET_VALUE};

	/* Set the flags to false for which the alarm has to be reset */
	alarm_time_set.hour_match = false;
	alarm_time_set.min_match  = false;
	alarm_time_set.sec_match  = false;
	alarm_time_set.mday_match = false;
	alarm_time_set.mon_match  = false;
	alarm_time_set.year_match = false;

	/* Reset  the alarm Flag, so that no alarm interrupt triggers from here onwards
	 * When needed, FSM Action Functions will be setting it back.
	 */
	err = R_RTC_CalendarAlarmSet(&g_rtc_ctrl, &alarm_time_set);
	/* Handle error */
	if (FSP_SUCCESS != err)
	{
		APP_ERR_PRINT("\r\nCalendar alarm Set failed.\r\n");
	}
    return err;
}

/***********************************************************************************************************************
 * @brief       This functions sets the Calendar time. This is done every time when the MCU boots up.
 * @param[IN]   None
 * @retval      FSP_SUCCESS  Upon successful Calendar time set
 * @retval      Any Other Error code apart from FSP_SUCCESS  Unsuccessful Calendar time set
 **********************************************************************************************************************/
fsp_err_t set_rtc_calendar_time(void)
{
    fsp_err_t err = FSP_SUCCESS;
    rtc_time_t config_time ={RESET_VALUE};

    /* RTC Calendar Time is set to start from 01/01/2020 00:00:00 */
    config_time.tm_sec  = USER_SECOND;
    config_time.tm_min  = USER_MINUTE;
    config_time.tm_hour = USER_HOUR;
    config_time.tm_mday = USER_DAY_OF_THE_MONTH;
    config_time.tm_mon  = USER_MONTH;
    config_time.tm_year = USER_YEAR;
    R_RTC_ClockSourceSet(&g_rtc_ctrl);
    /* Set the Calendar time  */
    err = R_RTC_CalendarTimeSet(&g_rtc_ctrl, &config_time);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nCalendarTime Set failed.\r\n");
        return err;
    }

    /* Get the current Calendar time */
    err = R_RTC_CalendarTimeGet(&g_rtc_ctrl, &config_time);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nCalendarTime Get failed.\r\n");
        return err;
    }

    /* Add the adjustment factor for Month and Year for User Readability*/
    config_time.tm_mon  +=  MON_ADJUST_VALUE;
    config_time.tm_year +=  YEAR_ADJUST_VALUE;

    APP_PRINT("\r\n RTC calendar Time set to  Date : %d/%d/%d \n Time  : %d : %d : %d \r\n\n",
    		   config_time.tm_mday,config_time.tm_mon, config_time.tm_year,
			   config_time.tm_hour,config_time.tm_min, config_time.tm_sec);
    return err;
}

fsp_err_t init_cgc(void)
{
    fsp_err_t err = FSP_SUCCESS;

    cgc_clock_t       	 lsys_clock_source = {RESET_VALUE};
	cgc_divider_cfg_t 	 sys_divider_cf    = {RESET_VALUE};

    /* Open CGC module */
    err = R_CGC_Open(&g_cgc0_ctrl, &g_cgc0_cfg);
    /* Handle error */
    if(FSP_SUCCESS == err)
    {
    	/* Get system clock source */
    	err =  R_CGC_SystemClockGet (&g_cgc0_ctrl, &lsys_clock_source, &sys_divider_cf);
    	/* Handle error */
    	if(FSP_SUCCESS == err)
    	{
  	        APP_PRINT("\r\nMCU Running with Clock Source = %s, \t", clk_to_string(lsys_clock_source));
   		}
    }//if FSP_SUCCESS == err
	return err;
}
/*******************************************************************************************************************//**
 * @} (end addtogroup lpm_app)
 **********************************************************************************************************************/

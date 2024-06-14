/***********************************************************************************************************************
 * File Name    : tml_ep.c
 * Description  : Contains data structures and functions used in hal_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2024 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#include "tml_ep.h"

extern bsp_leds_t g_bsp_leds;

/* Timer callback flags */
static volatile bool g_timer_flag [APP_TML_MAX_MODE] =
{
	[APP_TML_8_BIT]  = false,
	[APP_TML_16_BIT] = false,
	[APP_TML_32_BIT] = false,
	[APP_TML_16_BIT_CAPTURE] = false,
};

/* TML instances */
static timer_instance_t const * g_tml_instance [APP_TML_MAX_MODE] =
{
	[APP_TML_8_BIT]  = &g_timer_8bit,
	[APP_TML_16_BIT] = &g_timer_16bit,
	[APP_TML_32_BIT] = &g_timer_32bit,
	[APP_TML_16_BIT_CAPTURE] = &g_timer_16bit_capture,
	[APP_TML_32_BIT_COUNTER_LPM] = &g_timer_32bit_counter_lpm,
};

/* TML instances control */
static tml_instance_ctrl_t const * g_tml_instance_ctrl [APP_TML_MAX_MODE] =
{
	[APP_TML_8_BIT]  = &g_timer_8bit_ctrl,
	[APP_TML_16_BIT] = &g_timer_16bit_ctrl,
	[APP_TML_32_BIT] = &g_timer_32bit_ctrl,
	[APP_TML_16_BIT_CAPTURE] = &g_timer_16bit_capture_ctrl,
	[APP_TML_32_BIT_COUNTER_LPM] = &g_timer_32bit_counter_lpm_ctrl,
};

/* TML periods in milliseconds */
static uint16_t const g_tml_period_in_milli [APP_TML_MAX_MODE] =
{
	[APP_TML_8_BIT]  = PERIOD_8BIT_MILLI,
	[APP_TML_16_BIT] = PERIOD_16BIT_MILLI,
	[APP_TML_32_BIT] = PERIOD_32BIT_MILLI,
};

/* TML modules name */
static const uint8_t * g_tml_mode_name [APP_TML_MAX_MODE] =
{
	[APP_TML_8_BIT]  = (uint8_t *) NAME_8BIT_MODE,
	[APP_TML_16_BIT] = (uint8_t *) NAME_16BIT_MODE,
	[APP_TML_32_BIT] = (uint8_t *) NAME_32BIT_MODE,
	[APP_TML_16_BIT_CAPTURE] = (uint8_t *) NAME_16BIT_CAPTURE_MODE,
};

/* LPM instances */
static lpm_instance_t const * g_lpm_instance [APP_LPM_MAX_STATE] =
{
	 [APP_LPM_SLEEP_STATE] = &g_lpm_sleep,
	 [APP_LPM_SW_STANDBY_STATE] = &g_lpm_sw_standby,
};

/* LPM modes name */
static const uint8_t * g_lpm_mode_name [APP_LPM_MAX_STATE] =
{
	[APP_LPM_SLEEP_STATE]                   = (uint8_t *)NAME_LPM_SLEEP_MODE,
	[APP_LPM_SW_STANDBY_STATE]              = (uint8_t *)NAME_LPM_SW_STANDBY_MODE,
};

/* function declarations */
static fsp_err_t tml_open (app_tml_mode_t tml_mode);
static fsp_err_t tml_close (app_tml_mode_t tml_mode);
static fsp_err_t tml_start (app_tml_mode_t tml_mode);
static fsp_err_t tml_stop (app_tml_mode_t tml_mode);
static uint16_t tml_get_count_duration (void);
static fsp_err_t tml_counter_operation (uint16_t duration_time, app_tml_mode_t tml_mode);
static fsp_err_t tml_capture_operation (void);
static fsp_err_t tml_counter_operation_in_lpm(app_lpm_states_t lpm_mode);
static fsp_err_t lpm_enter_procedure(app_lpm_states_t lpm_mode);
static fsp_err_t lpm_exit_procedure(void);
static uint8_t get_user_input(void);
static void tml_deinit (void);
static void elc_deinit (void);
static void lpm_deinit (void);
static void handle_error(fsp_err_t err, uint8_t * err_str);

/* Global variables */
static uint32_t g_captured_time = RESET_VALUE;
static uint16_t g_number_cycles = RESET_VALUE;
static uint16_t g_capture_overflows = RESET_VALUE;

/* The buffer contains user input */
char g_rx_buffer [BUFFER_SIZE_DOWN] = {RESET_VALUE};

#if (USE_VIRTUAL_COM == 1)
/* Flag to detect the Enter key is Pressed, in the Callback function "serial_callback" */
extern volatile _Bool g_rx_complete_flag;
#endif /* USE_VIRTUAL_COM */

/*******************************************************************************************************************//**
 * @brief      This function initializes the TML modules and starts TML operation.
 * @param[in]  none
 * @retval     none
 **********************************************************************************************************************/
void tml_entry(void)
{
	fsp_err_t err = FSP_SUCCESS;
	fsp_pack_version_t version      = { RESET_VALUE };

	uint16_t duration_time = RESET_VALUE;
	uint8_t user_input = RESET_VALUE;

#if (USE_VIRTUAL_COM == 1)
	/* Initialize UARTA module first to print log to serial terminal */
	err = uart_init();
	if (FSP_SUCCESS != err)
	{
		/* Error trap */
		__asm("BKPT #0\n");
	}
#endif /* USE_VIRTUAL_COM */

	/* Version get API for FLEX pack information */
	R_FSP_VersionGet (&version);

	/* Example Project information printed on the Console */
	APP_PRINT(BANNER_INFO,EP_VERSION,version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch );
	APP_PRINT(EP_INFO);

	while (true)
	{
		APP_PRINT(PRINT_MENU);
		user_input = get_user_input();
		switch (user_input)
		{
			case TML_COUNTER:
			{
				APP_PRINT("\r\nDemonstrate TML counter mode\r\n");
				/* Get duration time for each TML counter mode */
				duration_time = tml_get_count_duration();

				/* The TML operates in 8-bit counter mode */
				err = tml_counter_operation(duration_time, APP_TML_8_BIT);
				APP_ERR_HANDLE(err, "\r\n**tml_8bit_operation failed**\r\n");

				/* The TML operates in 16-bit counter mode */
				err = tml_counter_operation(duration_time, APP_TML_16_BIT);
				APP_ERR_HANDLE(err, "\r\n**tml_16bit_operation failed**\r\n");

				/* The TML operates in 32-bit counter mode */
				err = tml_counter_operation(duration_time, APP_TML_32_BIT);
				APP_ERR_HANDLE(err, "\r\n**tml_32bit_operation failed**\r\n");
			}
			break;
			case TML_CAPTURE:
			{
				APP_PRINT("\r\nDemonstrate TML capture mode\r\n");
				/* The TML operates in 16-bit capture mode */
				err = tml_capture_operation();
				APP_ERR_HANDLE(err, "\r\n**tml_16bit_capture_operation failed**\r\n");
			}
			break;
			case TML_COUNTER_IN_LPM:
			{
				APP_PRINT("\r\nDemonstrate TML counter mode operates in LPM mode\r\n");
				/* The 32-bit counter TML operates in LPM sleep mode */
				err = tml_counter_operation_in_lpm(APP_LPM_SLEEP_STATE);
				APP_ERR_HANDLE(err, "\r\n**tml_counter_operates_in_LPM_sleep_mode failed**\r\n");
				/* The 32-bit counter TML operates in LPM SW standby mode */
				err = tml_counter_operation_in_lpm(APP_LPM_SW_STANDBY_STATE);
				APP_ERR_HANDLE(err, "\r\n**tml_counter_operates_in_LPM_sw_standby_mode failed**\r\n");
			}
			break;
			default:
			APP_PRINT("\r\nInvalid Input.\r\n");
		}
	}
}

/*******************************************************************************************************************//**
 * @brief       This function showcases how the TML operates in counter modes.
 * @param[IN]   time_period 	Time to check number cycles end
 * @param[IN]   tml_mode	 	selected mode for TML module
 * @retval      FSP_SUCCESS 	TML counter modes operate successfully
 * @retval      Any Other 		Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
static fsp_err_t tml_counter_operation (uint16_t duration_time, app_tml_mode_t tml_mode)
{
	fsp_err_t err = FSP_SUCCESS;
	uint16_t max_number_cycles    	= duration_time / g_tml_period_in_milli[tml_mode];
	led_power_t led_state = (led_power_t) RESET_VALUE;

	/* Reset number cycles end */
	g_number_cycles = RESET_VALUE;

	/* Open the timer */
	err = tml_open(tml_mode);
	APP_ERR_RETURN(err, "\r\n**tml_open for the %s failed**\r\n",g_tml_mode_name[tml_mode]);

	APP_PRINT("\r\n------------%s is running------------\r\n",g_tml_mode_name[tml_mode]);
	if (APP_TML_16_BIT == tml_mode || APP_TML_32_BIT == tml_mode)
	{
		APP_PRINT("\r\nLED will be toggled after end of each cycle\r\n");
	}

	/* Start the timer */
	err = tml_start(tml_mode);
	APP_ERR_RETURN(err, "\r\n**tml_start for the %s failed**\r\n", g_tml_mode_name[tml_mode]);

	while (true)
	{
		if (true == g_timer_flag[tml_mode])
		{
			/* Clear g_timer_flag flag */
			g_timer_flag[tml_mode] = false;

			if (APP_TML_16_BIT == tml_mode || APP_TML_32_BIT == tml_mode)
			{
				/* Toggle User LED */
				R_IOPORT_PinRead(&g_ioport_ctrl, g_bsp_leds.p_leds[LED_COUNTER_MODE], (bsp_io_level_t *)led_state);
				led_state = led_state ^ BSP_IO_LEVEL_HIGH;
				R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[LED_COUNTER_MODE], (bsp_io_level_t)led_state);
			}

			if (g_number_cycles >= max_number_cycles)
			{
				APP_PRINT("\r\nNumber cycles end of %s: %d \r\n", g_tml_mode_name[tml_mode], g_number_cycles);

				/* Stop the timer */
				err = tml_stop(tml_mode);
				APP_ERR_RETURN(err, "\r\n**tml_stop for the %s failed**\r\n", g_tml_mode_name[tml_mode]);

				/* Close the timer */
				if (tml_mode == APP_TML_8_BIT || tml_mode == APP_TML_16_BIT)
				{
					APP_PRINT("\r\nClose %s and open %s\r\n",g_tml_mode_name[tml_mode],g_tml_mode_name[tml_mode + 1]);
				}
				else
				{
					APP_PRINT("\r\nClose %s\r\n",g_tml_mode_name[tml_mode]);
				}
				err = tml_close(tml_mode);
				APP_ERR_RETURN(err, "\r\n**tml_close for the %s failed**\r\n", g_tml_mode_name[tml_mode]);

				break;
			}
		}
	}
	return err;
}

/*******************************************************************************************************************//**
 * @brief       This function showcases how the TML operates in capture mode.
 * @retval      FSP_SUCCESS 	16-bit capture mode operates successfully
 * @retval      Any Other 		Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
static fsp_err_t tml_capture_operation (void)
{
	fsp_err_t err = FSP_SUCCESS;
	uint8_t user_input = RESET_VALUE;

	/* Initialize timer 16-bit capture */
	err = tml_open(APP_TML_16_BIT_CAPTURE);
	APP_ERR_RETURN(err, "\r\n**tml_open for the %s failed**\r\n", g_tml_mode_name[APP_TML_16_BIT_CAPTURE]);

	/* Start timer 16-bit capture */
	err = tml_start(APP_TML_16_BIT_CAPTURE);
	APP_ERR_RETURN(err, "\r\n**tml_start for the %s failed**\r\n", g_tml_mode_name[APP_TML_16_BIT_CAPTURE]);

	/* Initialize ELC module */
	err = R_ELC_Open(&g_elc_ctrl, &g_elc_cfg);
	APP_ERR_RETURN(err, "\r\n**R_ELC_Open API failed**\r\n");

	/* Enable ELC module */
	err = R_ELC_Enable(&g_elc_ctrl);
	APP_ERR_RETURN(err, "\r\n**R_ELC_Enable API failed**\r\n");

	while (true)
	{
		APP_PRINT("\r\nEnter 1 to capture the raw counts value of 16-bit timer, or enter 2 to back to the main menu\r\n");
		user_input = get_user_input();
		if (CAPTURE == user_input)
		{
			/* Generate an ELC software event to trigger capture */
			err = R_ELC_SoftwareEventGenerate(&g_elc_ctrl, ELC_SOFTWARE_EVENT_0);
			APP_ERR_RETURN(err, "\r\n**R_ELC_SoftwareEventGenerate API failed**\r\n");

			while (false == g_timer_flag[APP_TML_16_BIT_CAPTURE])
			{
				/* Wait for capture flag */
			}
			g_timer_flag[APP_TML_16_BIT_CAPTURE] = false;
			APP_PRINT("\r\nCapture times:%d\r\n", g_captured_time);
		}
		else if (RETURN_MAIN_MENU == user_input)
		{
			/* De-initialize ELC module */
			err = R_ELC_Close(&g_elc_ctrl);
			APP_ERR_RETURN(err, "\r\n**R_ELC_Close API failed**\r\n");

			/* De-initialize timer 16-bit capture */
			err = tml_close(APP_TML_16_BIT_CAPTURE);
			APP_ERR_RETURN(err, "\r\n**tml_close timer 16-bit capture mode failed**\r\n");
			break;
		}
		else
		{
			APP_PRINT("\r\nInvalid input\r\n");
		}
	}
	return err;
}

/*******************************************************************************************************************//**
 * @brief       This function showcases how the TML operates in counter modes with low power modes.
 * @retval      FSP_SUCCESS 	32-bit counter operates in LPM mode successfully
 * @retval      Any Other 		Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
static fsp_err_t tml_counter_operation_in_lpm(app_lpm_states_t lpm_mode)
{
	fsp_err_t err = FSP_SUCCESS;

	/* Open this LPM instance */
	err = R_LPM_Open(g_lpm_instance[lpm_mode]->p_ctrl, g_lpm_instance[lpm_mode]->p_cfg);
	APP_ERR_RETURN(err, "\r\n**R_LPM_Open API Failed**\r\n");

	/* Procedure before enter to LPM */
	err = lpm_enter_procedure(lpm_mode);
	APP_ERR_RETURN(err, "\r\n**lpm_enter_procedure Failed**\r\n");

	/* Enter to LPM */
	err = R_LPM_LowPowerModeEnter(g_lpm_instance[lpm_mode]->p_ctrl);
	APP_ERR_RETURN(err, "\r\n**R_LPM_LowPowerModeEnter API Failed**\r\n");

	/* Procedure after exit LPM */
	err = lpm_exit_procedure();
	APP_ERR_RETURN(err, "\r\n**lpm_exit_procedure Failed**\r\n");

	/* Close this LPM instance */
	err = R_LPM_Close(g_lpm_instance[lpm_mode]->p_ctrl);
	APP_ERR_RETURN(err, "\r\n**R_LPM_Close API Failed**\r\n");

	return err;
}

/*******************************************************************************************************************//**
 *  @brief      This function will perform a procedure before entering LPM mode
 *  @param[in]  lpm_mode	LPM mode prepares to enter
 *  @retval     FSP_SUCCESS	Upon successful operation
 *  @retval     Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
static fsp_err_t lpm_enter_procedure(app_lpm_states_t lpm_mode)
{
	fsp_err_t err = FSP_SUCCESS;

	/* Print LPM mode name */
	APP_PRINT("\r\n%s. Please wait 10 seconds to return to the normal mode\r\n",g_lpm_mode_name[lpm_mode]);

	/* Turn off LPM LED in case of LPM mode */
	R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[LED_LPM_STATE], (bsp_io_level_t) LED_OFF);

	/* Software delay to print LPM mode name */
	R_BSP_SoftwareDelay(LPM_NAME_PRINT_TIME, LPM_TIME_TRANSITION_UNIT);

	/* Disable IO port before going to LPM mode*/
	err = R_IOPORT_PinsCfg(&g_ioport_ctrl, &g_bsp_pin_lpm_cfg);
	APP_ERR_RETURN(err, "\r\n**R_IOPORT_PinsCfg API failed**\r\n");

	/* Initialize 32-bit counter TML operates in LPM modes */
	err = tml_open(APP_TML_32_BIT_COUNTER_LPM);
	APP_ERR_RETURN(err, "\r\n**tml_open for 32-bit counter operates in LPM failed**\r\n");

	/* Start 32-bit counter operates in LPM mode */
	err = tml_start(APP_TML_32_BIT_COUNTER_LPM);
	APP_ERR_RETURN(err, "\r\n**tml_start for 32-bit counter operates in LPM failed**\r\n");

	return err;
}

/*******************************************************************************************************************//**
 *  @brief      This function will perform a procedure after exiting LPM mode
 *  @retval     FSP_SUCCESS    Upon successful operation
 *  @retval     Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
static fsp_err_t lpm_exit_procedure(void)
{
	fsp_err_t err = FSP_SUCCESS;

	/* Stop 32-bit timer counting in LPM mode */
	err = tml_stop(APP_TML_32_BIT_COUNTER_LPM);
	APP_ERR_RETURN(err, "\r\n**tml_stop for 32-bit counter operates in LPM failed**\r\n");

	/* De-initialize TML 32-bit counter operates in LPM modes */
	err = tml_close(APP_TML_32_BIT_COUNTER_LPM);
	APP_ERR_RETURN(err, "\r\n**tml_close for 32-bit counter operates in LPM failed**\r\n");

	/* Put IO port configuration back to user's selections */
	err = R_IOPORT_PinsCfg(&g_ioport_ctrl, &g_bsp_pin_cfg);
	APP_ERR_RETURN(err, "\r\n**R_IOPORT_PinsCfg API failed**\r\n");

	APP_PRINT("\r\nMCU returned to the normal mode\r\n");

	/* Turn on LPM state LED in case of normal mode */
	R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[LED_LPM_STATE], (bsp_io_level_t) LED_ON);

	/* Software delay to next LPM mode */
	R_BSP_SoftwareDelay(LPM_TIME_TRANSITION_VALUE, LPM_TIME_TRANSITION_UNIT);

	return err;
}

/*******************************************************************************************************************//**
 * @brief       This function is used to get duration time for each TML counter mode.
 * @param[IN]   None
 * @retval      Duration time value in milliseconds
 **********************************************************************************************************************/
static uint16_t tml_get_count_duration (void)
{
	uint16_t duration_time = RESET_VALUE;

	while(true)
	{
		APP_PRINT("\r\nPlease enter time period values in seconds\r\n"
				  "Valid range: 1 to 60\r\n");
		duration_time = get_user_input();
		if ((MAX_DURATION >= duration_time) && (MIN_DURATION < duration_time))
		{
			APP_PRINT("\r\nDuration time: %d seconds\r\n", duration_time);
			break;
		}
		else
		{
			APP_PRINT("\r\nInvalid input. Please enter valid input\r\n");
		}
	}
	/* Return duration time value in milliseconds */
	return duration_time * SECOND_TO_MILLISECOND;
}

/*********************************************************************************************************************
 * @brief  This function is used to get the user inputs.
 * @param[in]   None
 * @retval      input_value    Returns input buffer after receive enter key
 *********************************************************************************************************************/
static uint8_t get_user_input(void)
 {
	uint8_t input_value = RESET_VALUE;

	/* Clean buffer */
	memset(g_rx_buffer, NULL_CHAR, sizeof(g_rx_buffer));
#if (USE_VIRTUAL_COM == 1)
	g_rx_complete_flag = false;
	/* Wait for enter key */
	while (!g_rx_complete_flag)
	{
		;
	}
#else
		/* Wait until has any user input */
		while (!APP_CHECK_DATA)
		{
			;
		}
		/* Get user input and stored in g_rx_buffer */
		APP_READ(g_rx_buffer);
#endif /* USE_VIRTUAL_COM */
	/* Convert data in g_rx_buffer to integer */
	input_value = (uint8_t) atoi(g_rx_buffer);
	return input_value;
}

/*******************************************************************************************************************//**
 * @brief       This function opens TML modules.
 * @param[IN]   tml_mode 		Timer mode to open
 * @retval      FSP_SUCCESS 	Timer opened successfully
 * @retval      Any Other 		Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
static fsp_err_t tml_open (app_tml_mode_t tml_mode)
{
	fsp_err_t err = FSP_SUCCESS;

	/* Open timer */
	err = R_TML_Open(g_tml_instance[tml_mode]->p_ctrl, g_tml_instance[tml_mode]->p_cfg);
	APP_ERR_RETURN(err, "\r\n**R_TML_Open API failed**\r\n");

	return err;
}

/*******************************************************************************************************************//**
 * @brief       This function closes TML modules.
 * @param[IN]   tml_mode 		Timer mode to close
 * @retval      FSP_SUCCESS 	Timer closed successfully
 * @retval      Any Other 		Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
static fsp_err_t tml_close (app_tml_mode_t tml_mode)
{
	fsp_err_t err = FSP_SUCCESS;
	/* Close timer */
	if (MODULE_CLOSED != g_tml_instance_ctrl[tml_mode]->open)
	{
		err = R_TML_Close(g_tml_instance[tml_mode]->p_ctrl);
		APP_ERR_RETURN(err, "\r\n**R_TML_Close API failed**\r\n");
	}
	return err;
}

/*******************************************************************************************************************//**
 * @brief       This function starts TML modules.
 * @param[IN]   tml_mode 		Timer mode to start
 * @retval      FSP_SUCCESS 	Timer started successfully
 * @retval      Any Other 		Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
static fsp_err_t tml_start (app_tml_mode_t tml_mode)
{
	fsp_err_t err = FSP_SUCCESS;
	/* Start timer */
	err = R_TML_Start(g_tml_instance[tml_mode]->p_ctrl);
	APP_ERR_RETURN(err, "\r\n**R_TML_Start API failed**\r\n");

	return err;
}

/*******************************************************************************************************************//**
 * @brief       This function stops TML modules.
 * @param[IN]   tml_mode 		Timer mode to stop
 * @retval      FSP_SUCCESS 	Timer stopped successfully
 * @retval      Any Other 		Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
static fsp_err_t tml_stop (app_tml_mode_t tml_mode)
{
	fsp_err_t err = FSP_SUCCESS;
	timer_status_t timer_status;

	/* Get status of count timer */
	err= R_TML_StatusGet(g_tml_instance[tml_mode]->p_ctrl, &timer_status);
	APP_ERR_RETURN(err, "\r\n**R_TML_StatusGet API failed**\r\n");

	if(TIMER_STATE_STOPPED != timer_status.state)
	{
		/* Stop timer */
		err = R_TML_Stop(g_tml_instance[tml_mode]->p_ctrl);
		APP_ERR_RETURN(err, "\r\n**R_TML_Stop API failed**\r\n");
	}
	return err;
}

/*******************************************************************************************************************//**
 * @brief This function closes opened TML modules before the project ends up in an Error Trap.
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
static void tml_deinit (void)
{
	if (FSP_SUCCESS != tml_close(APP_TML_8_BIT))
	{
		APP_ERR_PRINT("tml_close for the 8-bit failed\r\n");
	}

	if (FSP_SUCCESS != tml_close(APP_TML_16_BIT))
	{
		APP_ERR_PRINT("tml_close for the 16-bit failed\r\n");
	}

	if (FSP_SUCCESS != tml_close(APP_TML_32_BIT))
	{
		APP_ERR_PRINT("tml_close for the 32-bit failed\r\n");
	}

	if (FSP_SUCCESS != tml_close(APP_TML_16_BIT_CAPTURE))
	{
		APP_ERR_PRINT("tml_close for the 16-bit capture failed\r\n");
	}

	if (FSP_SUCCESS != tml_close(APP_TML_32_BIT_COUNTER_LPM))
	{
		APP_ERR_PRINT("tml_close for the LPM 32-bit timer failed\r\n");
	}
}

/*******************************************************************************************************************//**
 * @brief This function closes opened ELC module before the project ends up in an Error Trap.
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
static void elc_deinit (void)
{
	/* De-initialize ELC module if it opened */
	if (MODULE_CLOSE != g_elc_ctrl.open)
	{
		if(FSP_SUCCESS != R_ELC_Close(&g_elc_ctrl))
		{
			APP_ERR_PRINT("\r\n**R_ELC_Close API failed**\r\n");
		}
	}
}

/*******************************************************************************************************************//**
 * @brief This function closes opened LPM modules before the project ends up in an Error Trap.
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
static void lpm_deinit (void)
{
	/* De-initialize LPM sleep mode module if it opened */
	if (MODULE_CLOSE != g_lpm_sleep_ctrl.lpm_open)
	{
		if(FSP_SUCCESS != R_LPM_Close(&g_lpm_sleep_ctrl))
		{
			APP_ERR_PRINT("\r\n**Close LPM sleep mode failed**\r\n");
		}
	}

	/* De-initialize LPM SW standby mode module if it opened */
	if (MODULE_CLOSE != g_lpm_sw_standby_ctrl.lpm_open)
	{
		if(FSP_SUCCESS != R_LPM_Close(&g_lpm_sw_standby_ctrl))
		{
			APP_ERR_PRINT("\r\n**Close LPM sw standby mode failed**\r\n");
		}
	}

}
/*******************************************************************************************************************//**
 *  @brief       This function handles error if error occurred, closes all opened modules, prints and traps error.
 *  @param[in]   err       error status
 *  @param[in]   err_str   error string
 *  @retval      None
 **********************************************************************************************************************/
void handle_error(fsp_err_t err, uint8_t * err_str)
{
		/* Print error information and error code */
		APP_ERR_PRINT(err_str);
		/* De-initialize all opened TML modules */
		tml_deinit();
		/* De-initialize opened ELC module */
		elc_deinit();
		/* De-initialize all opened LPM modules */
		lpm_deinit();
		/* Trap the error */
		APP_ERR_TRAP(err);
}

/*******************************************************************************************************************//**
 * @brief This function is callback for 8-bit counter TML.
 * @param[in]  p_args
 * @retval     None
 **********************************************************************************************************************/
void timer_8bit_callback(timer_callback_args_t *p_args)
{
	if (TIMER_EVENT_CYCLE_END == p_args->event)
	{
		g_timer_flag [APP_TML_8_BIT] = true;
		g_number_cycles ++;
	}
}

/*******************************************************************************************************************//**
 * @brief This function is callback for 16-bit counter TML.
 * @param[in]  p_args
 * @retval     None
 **********************************************************************************************************************/
void timer_16bit_callback(timer_callback_args_t *p_args)
{
	if (TIMER_EVENT_CYCLE_END == p_args->event)
	{
		g_timer_flag [APP_TML_16_BIT] = true;
		g_number_cycles ++;
	}
}

/*******************************************************************************************************************//**
 * @brief This function is callback for 32-bit counter TML.
 * @param[in]  p_args
 * @retval     None
 **********************************************************************************************************************/
void timer_32bit_callback(timer_callback_args_t *p_args)
{
	if (TIMER_EVENT_CYCLE_END == p_args->event)
	{
		g_timer_flag [APP_TML_32_BIT] = true;
		g_number_cycles ++;
	}
}

/*******************************************************************************************************************//**
 * @brief This function is callback for 16-bit capture TML.
 * @param[in]  p_args
 * @retval     None
 **********************************************************************************************************************/
void timer_16bit_capture_callback(timer_callback_args_t *p_args)
{
	if (TIMER_EVENT_CYCLE_END == p_args->event)
	{
		g_capture_overflows++;
	}
	if (TIMER_EVENT_CAPTURE_EDGE == p_args->event)
	{
		g_captured_time = (g_timer_16bit_capture_cfg.period_counts * g_capture_overflows) + p_args->capture;
		g_timer_flag [APP_TML_16_BIT_CAPTURE] = true;
	}

}


/*******************************************************************************************************************//**
 * @} (end addtogroup tml_ep)
 **********************************************************************************************************************/

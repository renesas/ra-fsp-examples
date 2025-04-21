/***********************************************************************************************************************
 * File Name    : tml_counter_in_lpm.c
 * Description  : Contains data structures and functions used in tml_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "tml_ep.h"

extern bsp_leds_t g_bsp_leds;

/* LPM instances */
lpm_instance_t const * g_lpm_instance [APP_LPM_MAX_STATE] =
{
    [APP_LPM_SLEEP_STATE] = &g_lpm_sleep,
    [APP_LPM_SW_STANDBY_STATE] = &g_lpm_sw_standby,
};

/* LPM modes name */
uint8_t * g_lpm_mode_name [APP_LPM_MAX_STATE] =
{
    [APP_LPM_SLEEP_STATE]                   = (uint8_t *)NAME_LPM_SLEEP_MODE,
    [APP_LPM_SW_STANDBY_STATE]              = (uint8_t *)NAME_LPM_SW_STANDBY_MODE,
};

static fsp_err_t tml_set_timer_cancel_lpm (uint8_t* g_time_cancel_lpm);
static fsp_err_t lpm_enter_procedure(app_lpm_states_t lpm_mode);
static fsp_err_t lpm_exit_procedure(void);

/*******************************************************************************************************************//**
 * @brief       This function is used to set the period time for counter in LPM operation.
 * @param[IN]   None
 * @retval      Duration time value in milliseconds
 **********************************************************************************************************************/
static fsp_err_t tml_set_timer_cancel_lpm (uint8_t* g_time_cancel_lpm)
{
    fsp_err_t err = FSP_SUCCESS;
    uint32_t period_counts = RESET_VALUE;
    timer_info_t info = {RESET_VALUE};

    while(true)
    {
        APP_PRINT("\r\nPlease enter the period time for counter in LPM operation in seconds"
	              "\r\nValid range: 1 to 60"
		          "\r\nUser Input: ");

        uint8_t user_input = get_user_input();

        if ((MAX_DURATION >= user_input) && (MIN_DURATION < user_input))
        {
            *g_time_cancel_lpm = (uint16_t) user_input;
            break;
        }
        else
        {
            APP_PRINT("\r\nInvalid input. Please enter valid input\r\n");
        }
    }

    /* Get the clock frequency of timer */
    err = R_TML_InfoGet(&g_timer_32bit_counter_lpm_ctrl, &info);
    APP_ERR_RETURN(err, "\r\n**R_TML_InfoGet API failed**\r\n");

    /* Calculate the period counts */
    period_counts = (*g_time_cancel_lpm) * info.clock_frequency;

    /* The timer must be stopped before updating the period */
    err = tml_stop(APP_TML_32_BIT_COUNTER_LPM);
    APP_ERR_RETURN(err, "\r\n**tml_stop for 32-bit counter operates in LPM failed**\r\n");

    /* Set the period value */
    err = R_TML_PeriodSet(&g_timer_32bit_counter_lpm_ctrl, period_counts);
    APP_ERR_RETURN(err, "\r\n**R_TML_PeriodSet API failed**\r\n");
    
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function showcases how the TML operates in counter modes with low power modes.
 * @retval      FSP_SUCCESS 	32-bit counter operates in LPM mode successfully
 * @retval      Any Other 		Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
fsp_err_t tml_counter_operation_in_lpm(app_lpm_states_t lpm_mode)
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
    uint8_t g_time_cancel_lpm = RESET_VALUE;

    /* Set the timer period to cancel from LPM mode */
    err = tml_set_timer_cancel_lpm(&g_time_cancel_lpm);
    APP_ERR_RETURN(err, "\r\n**tml_set_timer_cancel_lpm failed**\r\n");

    /* Print LPM mode name */
    APP_PRINT("\r\n\r\n%s. Please wait %d seconds to return to normal mode\r\n", g_lpm_mode_name[lpm_mode], \
              g_time_cancel_lpm);

    /* Turn off the LPM LED in case of LPM mode */
    R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[LED_LPM_STATE], (bsp_io_level_t) LED_OFF);

    /* Software delay to print LPM mode name */
    R_BSP_SoftwareDelay(LPM_NAME_PRINT_TIME, LPM_TIME_TRANSITION_UNIT);

    /* Disable IO port before entering LPM mode */
    err = R_IOPORT_PinsCfg(&g_ioport_ctrl, &g_bsp_pin_lpm_cfg);
    APP_ERR_RETURN(err, "\r\n**R_IOPORT_PinsCfg API failed**\r\n");

    /* Start 32-bit counter operating in LPM mode */
    err = tml_start(APP_TML_32_BIT_COUNTER_LPM);
    APP_ERR_RETURN(err, "\r\n**tml_start for 32-bit counter in LPM mode failed**\r\n");

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

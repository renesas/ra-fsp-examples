/***********************************************************************************************************************
 * File Name    : tml_counter.c
 * Description  : Contains data structures and functions used in tml_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#include "tml_ep.h"

extern bsp_leds_t g_bsp_leds;
uint16_t g_number_cycles = RESET_VALUE;

extern volatile bool g_timer_flag [APP_TML_MAX_MODE];
extern const uint8_t * g_tml_mode_name [APP_TML_MAX_MODE];

/* TML periods in milliseconds */
uint16_t g_tml_period_in_milli [APP_TML_MAX_MODE] =
{
    [APP_TML_8_BIT]  = PERIOD_8BIT_MILLI,
    [APP_TML_16_BIT] = PERIOD_16BIT_MILLI,
    [APP_TML_32_BIT] = PERIOD_32BIT_MILLI,
};

/*******************************************************************************************************************//**
 * @brief       This function is used to get duration time for each TML counter mode.
 * @param[IN]   None
 * @retval      Duration time value in milliseconds
 **********************************************************************************************************************/
uint16_t tml_get_count_duration (void)
{
    uint16_t duration_time = RESET_VALUE;

    while(true)
    {
        APP_PRINT("\r\nPlease enter time period values in seconds"
                  "\r\nValid range: 1 to 60"
                  "\r\nUser Input: ");
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

/*******************************************************************************************************************//**
 * @brief       This function showcases how the TML operates in counter modes.
 * @param[IN]   time_period 	Time to check number cycles end
 * @param[IN]   tml_mode	 	selected mode for TML module
 * @retval      FSP_SUCCESS 	TML counter modes operate successfully
 * @retval      Any Other 		Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
fsp_err_t tml_counter_operation (uint16_t duration_time, app_tml_mode_t tml_mode)
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
	APP_PRINT("\r\nLED will be toggled after end of each cycle\r\n");
    
    /* Start the timer */
    err = tml_start(tml_mode);
    APP_ERR_RETURN(err, "\r\n**tml_start for the %s failed**\r\n", g_tml_mode_name[tml_mode]);
    
    while (true)
    {
        if (true == g_timer_flag[tml_mode])
        {
            /* Clear g_timer_flag flag */
            g_timer_flag[tml_mode] = false;
            
            /* Toggle User LED */
            /* In the TML 8-bit timer counter operation, LED may be blink so quickly so we cannot see LED operation  */
            R_IOPORT_PinRead(&g_ioport_ctrl, g_bsp_leds.p_leds[LED_COUNTER_MODE], (bsp_io_level_t *)led_state);
            led_state = led_state ^ BSP_IO_LEVEL_HIGH;
            R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[LED_COUNTER_MODE], (bsp_io_level_t)led_state);
            
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

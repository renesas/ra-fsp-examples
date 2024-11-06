/***********************************************************************************************************************
 * File Name    : ulpt_ep.c
 * Description  : Contains data structures and functions used in ulpt_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "ulpt_ep.h"
#include "common_utils.h"

/*******************************************************************************************************************//**
 * @addtogroup ulpt_ep
 * @{
 **********************************************************************************************************************/

extern bsp_leds_t g_bsp_leds;

static volatile bool g_one_shot_timer_flag = false;
static volatile bool g_periodic_timer_flag = false;

/*******************************************************************************************************************//**
 * @brief This function is callback for one-shot mode timer.
 * @param[in]  p_args
 * @retval     None
 **********************************************************************************************************************/
void one_shot_timer_callback(timer_callback_args_t *p_args)
{
    if(TIMER_EVENT_CYCLE_END == p_args->event)
    {
        g_one_shot_timer_flag = true;
    }
}

/*******************************************************************************************************************//**
 * @brief This function is callback for periodic timer.
 * @param[in]  p_args
 * @retval     None
 **********************************************************************************************************************/
void periodic_timer_callback(timer_callback_args_t *p_args)
{
    if(TIMER_EVENT_CYCLE_END == p_args->event)
    {
        g_periodic_timer_flag = true;
    }
}

/*******************************************************************************************************************//**
 * @brief       This function initializes the necessary hardware for the example project.
 * @param[in]   None
 * @retval      FSP_SUCCESS Upon successful operation
 * @retval      Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
fsp_err_t hw_module_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Initialize LPM driver */
    err = R_LPM_Open(&g_lpm_deep_sw_standby_ctrl, &g_lpm_deep_sw_standby_cfg);
    APP_ERR_RETURN(err, "R_LPM_Open API failed\r\n");

    /* Initialize ULPT0 in One Shot Mode */
    err = R_ULPT_Open (&g_timer_one_shot_ctrl, &g_timer_one_shot_cfg);
    APP_ERR_RETURN(err, "R_ULPT_Open API for one-shot timer failed\r\n");

    /* Initialize ULPT1 in Periodic mode */
    err = R_ULPT_Open(&g_timer_periodic_ctrl, &g_timer_periodic_cfg);
    APP_ERR_RETURN(err, "R_ULPT_Open API for periodic timer failed\r\n");

    return err;
}


/*******************************************************************************************************************//**
 * @brief       This function gets the period value from the RTT and sets this period value for the one-shot timer.
 * @param[in]   None
 * @retval      FSP_SUCCESS Upon successful operation
 * @retval      Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
fsp_err_t ulpt_set_period(void)
{
    fsp_err_t       err                             = FSP_SUCCESS;
    uint8_t         rtt_buffer[BUFFER_SIZE_DOWN]    = {RESET_VALUE};
    uint32_t        period_in_second                = RESET_VALUE;
    uint32_t        period_in_raw                   = RESET_VALUE;
    bool            select_flag                     = false;
    timer_info_t    timer_info                      = {
                                                        .clock_frequency = RESET_VALUE,
                                                        .count_direction = (timer_direction_t) RESET_VALUE,
                                                        .period_counts = RESET_VALUE,
                                                     };

    /* Print menu */
    APP_PRINT(MENU_SELECT);

    /* RTT input handle */
    while (false == select_flag)
    {
        if (APP_CHECK_DATA)
        {
            /* Clean RTT input buffer */
            memset(rtt_buffer, NULL_CHAR, BUFFER_SIZE_DOWN);

            /* Read data from RTT */
            APP_READ(rtt_buffer);

            /* Check user input */
            switch (rtt_buffer[SELECT_INDEX])
            {
                case SELECT_1S:
                    period_in_second = PERIOD_1S;
                    select_flag = true;
                    APP_PRINT("\r\nThe selected period is 1 second\r\n");
                    break;

                case SELECT_2S:
                    period_in_second = PERIOD_2S;
                    select_flag = true;
                    APP_PRINT("\r\nThe selected period is 2 seconds\r\n");
                    break;

                case SELECT_3S:
                    period_in_second = PERIOD_3S;
                    select_flag = true;
                    APP_PRINT("\r\nThe selected period is 3 seconds\r\n");
                    break;

                default:
                    APP_PRINT("\r\nSelected period is invalid !\r\n");
                    APP_PRINT(MENU_SELECT);
                    break;
            }
        }
    }

    /* Get the clock frequency of the one shot timer */
    err = R_ULPT_InfoGet(&g_timer_one_shot_ctrl, &timer_info);
    APP_ERR_RETURN(err,"R_ULPT_InfoGet API for one-shot timer failed\r\n");

    /* Convert the period value in seconds to the raw value for a one-shot timer */
    period_in_raw = (uint32_t)((period_in_second * timer_info.clock_frequency ));

    /* Set a period value for the one-shot timer */
    err = R_ULPT_PeriodSet(&g_timer_one_shot_ctrl, period_in_raw);
    APP_ERR_RETURN(err,"R_ULPT_PeriodSet API for one-shot timer failed\r\n");

    /* Get the clock frequency of the periodic timer */
    err = R_ULPT_InfoGet(&g_timer_periodic_ctrl, &timer_info);
    APP_ERR_RETURN(err,"R_ULPT_InfoGet API for periodic timer failed\r\n");

    /* Convert the period value in second to the raw value for a periodic timer*/
    period_in_raw = (uint32_t)((period_in_second * timer_info.clock_frequency ));

    /* Set a period value for the periodic timer */
    err = R_ULPT_PeriodSet(&g_timer_periodic_ctrl, period_in_raw);
    APP_ERR_RETURN(err,"R_ULPT_PeriodSet API for periodic timer failed\r\n");

    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function demonstrates a periodic timer operation.
 * @param[in]   None
 * @retval      FSP_SUCCESS Upon successful operation
 * @retval      Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
fsp_err_t ulpt_periodic_operation(void)
{
    fsp_err_t   err         = FSP_SUCCESS;
    uint8_t     count_value = RESET_VALUE;
    char *      count_str[] = { [0] = NULL,
                                [1] = "1st",
                                [2] = "2nd",
                                [3] = "3rd", };
    led_power_t led_state   = (led_power_t)RESET_VALUE;

    /* Start periodic timer */
    err = R_ULPT_Start(&g_timer_periodic_ctrl);
    APP_ERR_RETURN(err,"R_ULPT_Start API for periodic timer failed\r\n");

    /* Print the ULPT1 has started message */
    APP_PRINT("\r\nULPT1 in periodic mode started\r\n");

    /* Wait until ULPT1 timer underflow three times*/
    while (TIMES_MAX > count_value)
    {
        if (true == g_periodic_timer_flag)
        {
            g_periodic_timer_flag = false;
            count_value ++;

            APP_PRINT("\r\nULPT1 underflow for the %s time\r\n", count_str[count_value]);

            if (LED_USE < g_bsp_leds.led_count)
            {
                led_state = led_state ^ (led_power_t)BSP_IO_LEVEL_HIGH;
                R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[LED_USE], (bsp_io_level_t)led_state);

                APP_PRINT("\r\nLED is toggled for the %s time\r\n", count_str[count_value]);
            }
        }
    }

    /* Stop ULPT1 in periodic mode */
    err = R_ULPT_Stop(&g_timer_periodic_ctrl);
    APP_ERR_RETURN(err,"R_ULPT_Stop API for periodic timer failed\r\n");

    /* Print the ULPT1 has stopped message */
    APP_PRINT("\r\nULPT1 in period mode stopped\r\n");

    return err;
}

/*******************************************************************************************************************//**
 * @brief       TThis function demonstrates a one-shot timer operation in low power mode.
 * @param[in]   None
 * @retval      FSP_SUCCESS Upon successful operation
 * @retval      Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
fsp_err_t ulpt_one_shot_operation(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Start one-shot timer */
    err = R_ULPT_Start(&g_timer_one_shot_ctrl);
    APP_ERR_RETURN(err,"R_ULPT_Start API for one-shot timer failed\r\n");

    /* Print the ULPT0 has started message */
    APP_PRINT("\r\nULPT0 in one-shot mode started\r\n");

    /* Print the Enter LPM mode notice and wait until the transfer to RTT is complete */
    APP_PRINT("\r\nMCU enter to Deep SW Standby mode\r\n");
    APP_PRINT_WAIT;

    /* Delay to ensure data has been transmitted before entering LPM mode */
    R_BSP_SoftwareDelay(1U, BSP_DELAY_UNITS_MILLISECONDS);

    /* Enter to deep sw standby mode */
    err = R_LPM_LowPowerModeEnter(&g_lpm_deep_sw_standby_ctrl);
    APP_ERR_RETURN(err,"R_LPM_LowPowerModeEnter API failed\r\n");

    /* MCU in Deep SW Standby here, ULPT0 interrupt causes MCU to reset internally. */
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function de-initialize all opened hardware modules.
 * @param[in]   None
 * @retval      Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
void hw_module_deinit(void)
{
    /* Close ULPT0 module */
    if (MODULE_CLOSED != g_timer_one_shot_ctrl.open)
    {
        if (FSP_SUCCESS != R_ULPT_Close(&g_timer_one_shot_ctrl))
        {
            APP_ERR_PRINT("R_ULPT_Close API for one-shot timer failed\r\n");
        }
    }

    /* Close ULPT1 module */
    if (MODULE_CLOSED != g_timer_periodic_ctrl.open)
    {
        if (FSP_SUCCESS != R_ULPT_Close(&g_timer_periodic_ctrl))
        {
            APP_ERR_PRINT("R_ULPT_Close API for periodic timer failed\r\n");
        }
    }

    /* Close LPM driver */
    if(MODULE_CLOSED != g_lpm_deep_sw_standby_ctrl.lpm_open)
    {
        if (FSP_SUCCESS != R_LPM_Close(&g_lpm_deep_sw_standby_ctrl))
        {
            APP_ERR_PRINT("R_LPM_Close API failed\r\n");
        }
    }
}

/*******************************************************************************************************************//**
 *  @brief       This function handle error if error occurred, closes all opened modules, print and traps error.
 *  @param[in]   status    error status
 *  @param[in]   err_str   error string
 *  @retval      None
 **********************************************************************************************************************/
void handle_error (fsp_err_t err, const char * err_func, const char * err_str)
{
    if (FSP_SUCCESS != err)
    {

        /* De-initialize all opened hardware modules */
        hw_module_deinit();

        /* Print the error */
        APP_PRINT("[ERR] In Function: %s(), %s", err_func, err_str);

        /* Trap the error */
        APP_ERR_TRAP(err);
    }
}


/*******************************************************************************************************************//**
 * @} (end addtogroup ulpt_ep)
 **********************************************************************************************************************/

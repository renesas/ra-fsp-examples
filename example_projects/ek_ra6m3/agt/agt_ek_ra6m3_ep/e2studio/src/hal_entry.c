/***********************************************************************************************************************
 * File Name    : hal_entry.c
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
 * Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/#include "common_utils.h"

#include "agt_ep.h"
#include "icu_ep.h"

void R_BSP_WarmStart(bsp_warm_start_event_t event);

static fsp_err_t timer_status_check (void);

volatile uint8_t g_one_shot_timer_flag = RESET_FLAG;    //flag to check timer is enable or not
volatile uint8_t g_periodic_timer_flag = RESET_FLAG;    //flag to check timer1 is enable or not
volatile uint32_t g_error_flag = RESET_FLAG;            //flag to capture error in ISR's
volatile uint32_t g_status_check_flag = RESET_FLAG;     //flag to capture the status of timers

/*******************************************************************************************************************//**
 * @brief     The RA Configuration tool generates main() and uses it to generate threads if an RTOS is used.  This function is
 *            called by main() when no RTOS is used.
 * @param[in] None
 **********************************************************************************************************************/
void hal_entry(void)
{
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version = {RESET_VALUE};
    unsigned char rByte[BUFFER_SIZE_DOWN] = {RESET_VALUE};
    uint32_t time_period_ms_oneshot = RESET_VALUE;
    uint32_t time_period_ms_periodic = RESET_VALUE;
    uint32_t raw_counts_oneshot = RESET_VALUE;
    uint32_t raw_counts_periodic = RESET_VALUE;
    timer_info_t  one_shot_info =
    {
     .clock_frequency = RESET_VALUE,
     .count_direction = RESET_VALUE,
     .period_counts = RESET_VALUE,
    };
    timer_info_t periodic_info =
    {
     .clock_frequency = RESET_VALUE,
     .count_direction = RESET_VALUE,
     .period_counts = RESET_VALUE,
    };
    uint32_t clock_freq = RESET_VALUE;

    /* version get API for FLEX pack information */
    R_FSP_VersionGet(&version);

    /* Example Project information printed on the Console */
    APP_PRINT(BANNER_1);
    APP_PRINT(BANNER_2);
    APP_PRINT(BANNER_3,EP_VERSION);
    APP_PRINT(BANNER_4,version.major, version.minor, version.patch );
    APP_PRINT(BANNER_5);
    APP_PRINT(BANNER_6);
    APP_PRINT(BANNER_7);

    /* Initialize AGT driver */
    err = agt_init();
    /* Handle error */
    if (FSP_SUCCESS != err)
    {   /* AGT module init failed */
        APP_ERR_PRINT("\r\n ** AGT INIT FAILED ** \r\n");
        APP_ERR_TRAP(err);
    }

    /* Initialize ICU driver */
    err = ext_irq_init();
    /* Handle error */
    if (FSP_SUCCESS != err)
    {   /* ICU module init failed */
        agt_deinit();
        APP_ERR_PRINT("\r\n ** ICU INIT FAILED ** \r\n");
        APP_ERR_TRAP(err);
    }

    /* Enable Push button */
    err = ext_irq_enable();
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        ext_irq_deinit();
        agt_deinit();
        APP_ERR_PRINT("\r\nEnabling external IRQ failed.Closing all drivers. Restart the Application\r\n");
        APP_ERR_TRAP(err);
    }

    APP_PRINT("\r\nPlease enter time period values for one-shot and periodic mode timers in milliseconds\n"
            " Valid range: 1 to 2000\r\n");
    APP_PRINT("\r\nOne-shot mode:");

    while(true)
    {
        if (APP_CHECK_DATA)
        {
            /* Cleaning buffer */
            memset(&rByte[0], NULL_CHAR, BUFFER_SIZE_DOWN);
            APP_READ(rByte);
            time_period_ms_oneshot = (uint32_t)atoi((char *)rByte);
            if ((TIME_PERIOD_MAX >= time_period_ms_oneshot) && (TIME_PERIOD_MIN < time_period_ms_oneshot))
            {
                APP_PRINT("\r\nTime period for one-shot mode timer: %d\r\n",time_period_ms_oneshot);
                break;
            }
            else
            {
                APP_PRINT("\r\nInvalid input. Please enter valid input\r\n");
            }
        }
    }

    APP_PRINT("\r\nPeriodic mode:");

    while(true)
    {
        if (APP_CHECK_DATA)
        {
            /* Cleaning buffer */
            memset(&rByte[0], NULL_CHAR, BUFFER_SIZE_DOWN);
            APP_READ(rByte);
            time_period_ms_periodic = (uint32_t)atoi((char *)rByte);
            if ((TIME_PERIOD_MAX >= time_period_ms_periodic) && (TIME_PERIOD_MIN < time_period_ms_periodic))
            {
                APP_PRINT("\r\nTime period for periodic mode timer: %d\r\n",time_period_ms_periodic);
                break;
            }
            else
            {
                APP_PRINT("\r\nInvalid input. Please enter valid input\r\n");
            }
        }
    }

    /* Calculation of raw counts value for given milliseconds value */
    err = R_AGT_InfoGet(&g_timer_one_shot_ctrl, &one_shot_info);
    /* Handle error */
    if(FSP_SUCCESS != err)
    {
        ext_irq_deinit();
        agt_deinit();
        APP_ERR_PRINT("\r\nInfoGet API failed.Closing all drivers. Restart the Application\r\n");
        APP_ERR_TRAP(err);

    }
    /* Depending on the user selected clock source, raw counts value can be calculated
     * for the user given time-period values */
    clock_freq = one_shot_info.clock_frequency;
    raw_counts_oneshot = (uint32_t)((time_period_ms_oneshot * clock_freq ) / 1000);
    /* Set period value */
    err = R_AGT_PeriodSet(&g_timer_one_shot_ctrl, raw_counts_oneshot);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        ext_irq_deinit();
        agt_deinit();
        APP_ERR_PRINT("\r\nPeriodSet API failed.Closing all drivers. Restart the Application\r\n");
        APP_ERR_TRAP(err);
    }

    /* Calculation of raw counts value for given milliseconds value */
    err = R_AGT_InfoGet(&g_timer_periodic_ctrl, &periodic_info);
    /* Handle error */
    if(FSP_SUCCESS != err)
    {
        ext_irq_deinit();
        agt_deinit();
        APP_ERR_PRINT("\r\nInfoGet API failed.Closing all drivers. Restart the Application\r\n");
        APP_ERR_TRAP(err);

    }
    /* Depending on the user selected clock source, raw counts value can be calculated
     * for the user given time-period values */
    clock_freq = periodic_info.clock_frequency;
    raw_counts_periodic = (uint32_t)((time_period_ms_periodic * clock_freq ) / 1000);
    /* Set period value */
    err = R_AGT_PeriodSet(&g_timer_periodic_ctrl, raw_counts_periodic);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        ext_irq_deinit();
        agt_deinit();
        APP_ERR_PRINT("\r\nPeriodSet API failed. Closing all drivers.Restart the Application\r\n");
        APP_ERR_TRAP(err);
    }

    APP_PRINT("\r\nPress user push-button to start or stop the timers");


    while(true)
    {
        if (SET_FLAG == g_status_check_flag)
        {
            /* Check the status of timers and perform operation accordingly */
            g_status_check_flag = RESET_FLAG;
            err = timer_status_check();
            /* Handle error */
            if (FSP_SUCCESS != err)
            {
                agt_deinit();
                ext_irq_deinit();
                APP_ERR_PRINT("\r\nTimer start/stop failed");
                APP_ERR_TRAP(err);
            }
        }

        /* Check if AGT0 is enabled in OneShot mode */
        if (SET_FLAG == g_one_shot_timer_flag)
        {
            g_one_shot_timer_flag = RESET_FLAG;
            APP_PRINT("\r\nAGT0 is Enabled in OneShot mode");
        }

        /* Check if AGT1 is enabled in Periodic mode */
        if (SET_FLAG == g_periodic_timer_flag)
        {
            g_periodic_timer_flag = RESET_FLAG;
            APP_PRINT ("\r\n\r\nOne-shot mode AGT timer elapsed");
            APP_PRINT ("\r\n\r\nAGT1 is Enabled in Periodic mode");
            APP_PRINT ("\r\nLED will toggle for set time period");
            APP_PRINT ("\r\nPress user push-button to stop timers\r\n");
        }

        /* Check if AGT1 is already running in Periodic mode */
        if (ALREADY_RUNNING == g_periodic_timer_flag)
        {
            g_periodic_timer_flag = RESET_FLAG;
            APP_PRINT ("\r\n\r\nOne-shot mode AGT timer elapsed\n");
            APP_PRINT ("\r\n\r\nAGT1 is already running in Periodic mode");
            APP_PRINT ("\r\nPress 1 to stop the timer\r\n");
        }
    }
}

/*******************************************************************************************************************//**
 * @brief This function checks the status of both timers and starts/stops the timers.
 *
 * @param[in] None
 **********************************************************************************************************************/
static fsp_err_t timer_status_check (void)
{
    fsp_err_t err = FSP_SUCCESS;
    timer_status_t periodic_timer_status =
    {
     .counter = RESET_VALUE,
     .state = RESET_VALUE,
    };
    timer_status_t oneshot_timer_status =
    {
     .counter = RESET_VALUE,
     .state = RESET_VALUE,
    };

    /* Retrieve the status of timer running in periodic mode */
    err = R_AGT_StatusGet(&g_timer_periodic_ctrl, &periodic_timer_status);
    /* Handle error */
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nStatusGet API failed");
        return err;
    }


    /* Retrieve the status of timer running in one-shot mode */
    err = R_AGT_StatusGet(&g_timer_one_shot_ctrl, &oneshot_timer_status);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nStatusGet API failed");
        return err;
    }

    if (TIMER_STATE_STOPPED != oneshot_timer_status.state)
    {
        err = R_AGT_Stop(&g_timer_one_shot_ctrl);
        if (FSP_SUCCESS != err)
        {
            APP_PRINT("\r\nAGT Stop API failed");
            return err;
        }
        else
        {
            APP_PRINT("\r\nOne-shot timer stopped. "
                    "Please press the push button to start timers.\r\n");
        }
    }
    else if (TIMER_STATE_STOPPED != periodic_timer_status.state)
    {
        err = R_AGT_Stop(&g_timer_periodic_ctrl);
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\nAGT Stop API failed");
            return err;
        }
        else
        {
            APP_PRINT("\r\nPeriodic timer stopped. "
                    "Please press the push button to start timers.\r\n");
        }
    }
    else
    {
        err = agt_start_oneshot_timer();
        /* Handle error */
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\n AGT start failed");
            return err;
        }
        else
        {
            g_one_shot_timer_flag = SET_FLAG;        // Set Timer Flag as timer is started
        }
    }
    return err;
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

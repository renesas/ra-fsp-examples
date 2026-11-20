/***********************************************************************************************************************
 * File Name    : cac_ep.c
 * Description  : Contains data structures and functions used in cac_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "cac_ep.h"

/* Variables to capture CAC events */
static volatile cac_event_t s_cac_event = (cac_event_t)CAC_EVENT_VALUE;
static volatile _Bool s_b_cac_status_flag = false;

uint32_t g_cac_counter_val = RESET_VALUE;

/* Supported clocks in this EP */
/* The CAC and CGC modules use different enumeration values for the same clock sources. */
/* Convert CAC clock source selection to the match CGC clock source */
static const cgc_clock_t target_cgc_clk_src[TARGET_CGC_CLK_CNT] =
{
    CGC_CLOCK_MAIN_OSC,  /* The target clock is CAC_CLOCK_SOURCE_MAIN_OSC */
    CGC_CLOCK_SUBCLOCK,  /* The target clock is CAC_CLOCK_SOURCE_SUBCLOCK */
    CGC_CLOCK_HOCO,      /* The target clock is CAC_CLOCK_SOURCE_HOCO */
    CGC_CLOCK_MOCO,      /* The target clock is CAC_CLOCK_SOURCE_MOCO */
    CGC_CLOCK_LOCO,      /* The target clock is CAC_CLOCK_SOURCE_LOCO */
    CGC_CLOCK_PLL        /* The target clock is CAC_CLOCK_SOURCE_PLL */
};

/* Private function */
static fsp_err_t cgc_clock_start_and_verify(cgc_clock_t clock);

/*******************************************************************************************************************//**
 * @brief     This function initializes CGC module.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t cgc_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Open CGC module */
    err = R_CGC_Open(&g_cgc_ctrl, &g_cgc_cfg);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function initializes CAC module, then verifies the selected clocks.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t cac_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Open CAC module */
    err = R_CAC_Open(&g_cac_ctrl, &g_cac_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Get the reference clock */
    cac_clock_source_t ref_clk = g_cac_cfg.cac_ref_clock.clock;

    /* Start and verify reference clock if it is not IWDT, External, or PCLKB (bus clock, always running) */
    if ((CAC_CLOCK_SOURCE_IWDT != ref_clk) && (CAC_CLOCK_SOURCE_EXTERNAL != ref_clk) &&\
        (CAC_CLOCK_SOURCE_PCLKB != ref_clk))
    {
        err = cgc_clock_start_and_verify(target_cgc_clk_src[ref_clk]);
        if (FSP_SUCCESS != err)
        {
            return err;
        }
    }

    /* Get the measurement clock */
    cac_clock_source_t target_clk = g_cac_cfg.cac_meas_clock.clock;

    /* If target clock is not IWDT clock or PCLKB (bus clock, always running), then start and check stability of
     * target clock selected in configurator */
    if ((CAC_CLOCK_SOURCE_IWDT != target_clk) && (CAC_CLOCK_SOURCE_PCLKB != target_clk))
    {
        err = cgc_clock_start_and_verify(target_cgc_clk_src[target_clk]);
        if (FSP_SUCCESS != err)
        {
            return err;
        }
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function performs CAC measurement operation.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Performed measurement successfully.
 * @retval    Any other error codes     Failed to perform measurement.
 **********************************************************************************************************************/
fsp_err_t cac_measurement_process(void)
{
    fsp_err_t err                  = FSP_SUCCESS;
    volatile uint16_t cac_time_out = UINT16_MAX;

    /* Reset flags before starting CAC measurement */
    s_cac_event         = (cac_event_t)CAC_EVENT_VALUE;
    s_b_cac_status_flag = false;
    g_cac_counter_val   = RESET_VALUE;

    /* Begin measurement */
    err = R_CAC_StartMeasurement(&g_cac_ctrl);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Wait for measurement complete or error generation event */
    do
    {
        --cac_time_out;
        if (RESET_VALUE == cac_time_out)
        {
            return FSP_ERR_TIMEOUT;
        }
    } while (true != s_b_cac_status_flag);

    /* Process event received from CAC callback */
    switch (s_cac_event)
    {
        case CAC_EVENT_MEASUREMENT_COMPLETE:
        {
            err = R_CAC_Read(&g_cac_ctrl, &g_cac_counter_val);
            if (FSP_SUCCESS != err)
            {
                return err;
            }
            err = R_CAC_StopMeasurement(&g_cac_ctrl);
            break;
        }

        case CAC_EVENT_FREQUENCY_ERROR:
        {
            err = FSP_ERR_INVALID_CAC_REF_CLOCK;
            break;
        }

        default:
        {
            err = FSP_ERR_OVERFLOW;
            break;
        }
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function starts a CGC clock and polls until it stabilizes or a timeout occurs.
 * @param[in] clock                     CGC clock to start and verify.
 * @retval    FSP_SUCCESS               Started and stabilized successfully.
 * @retval    Any other error codes     Failed to start or stabilize.
 **********************************************************************************************************************/
static fsp_err_t cgc_clock_start_and_verify(cgc_clock_t clock)
{
    fsp_err_t err      = FSP_SUCCESS;
    uint16_t  time_out = UINT16_MAX;

    /* Start selected target clock */
    err = R_CGC_ClockStart(&g_cgc_ctrl, clock, NULL);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Check selected target clock for stability */
    do
    {
        /* Start checking for timeout to avoid infinite loop */
        --time_out;
        err = R_CGC_ClockCheck(&g_cgc_ctrl, clock);

        /* Check for time elapse */
        if ((RESET_VALUE == time_out) || (FSP_ERR_CLOCK_INACTIVE == err))
        {
            return (RESET_VALUE == time_out) ? FSP_ERR_TIMEOUT : err;
        }
    } while (FSP_ERR_NOT_STABILIZED == err);

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief     This function closes the opened CAC module.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void cac_deinit(void)
{
    if (g_cac_ctrl.open)
    {
        R_CAC_Close(&g_cac_ctrl);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function closes the opened CGC module.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void cgc_deinit(void)
{
    if (g_cgc_ctrl.open)
    {
        R_CGC_Close(&g_cgc_ctrl);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function is the user-defined CAC callback.
 * @param[in] p_args                    Callback argument.
 * @retval    None.
 **********************************************************************************************************************/
void cac_callback(cac_callback_args_t *p_args)
{
    if (NULL != p_args)
    {
        s_cac_event = p_args->event;
        s_b_cac_status_flag = true;
    }
}

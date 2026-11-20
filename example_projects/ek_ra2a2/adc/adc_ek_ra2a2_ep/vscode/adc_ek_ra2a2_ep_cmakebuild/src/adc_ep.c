/***********************************************************************************************************************
 * File Name    : adc_ep.c
 * Description  : Contains variables and functions used in adc_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdbool.h>
#include "adc_ep.h"

/* Global variables */
volatile bool g_ready_to_read = false;
uint16_t g_adc_data = RESET_VALUE;
float g_adc_volt = RESET_VALUE;
uint8_t g_volt_str[5] = {RESET_VALUE};

#if (BSP_PERIPHERAL_ADC_PRESENT)
bool g_window_comp_event = false;
#endif /* BSP_PERIPHERAL_ADC_PRESENT */

/* Private variables */
#ifdef BOARD_RA2A1_EK
static uint16_t g_prev_adc_data = RESET_VALUE;
#endif /* BOARD_RA2A1_EK */

/* Private functions */
#if (defined(BOARD_RA2A1_EK)) || (BSP_PERIPHERAL_ADC_B_PRESENT)
static fsp_err_t adc_start_calibration(void);
#endif /* BOARD_RA2A1_EK || BSP_PERIPHERAL_ADC_B_PRESENT */

#ifdef BOARD_RA2A1_EK
static fsp_err_t adc_deviation_in_output(void);
#endif /* BOARD_RA2A1_EK */

/*******************************************************************************************************************//**
 * @brief     This function starts the ADC scan.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Started successfully.
 * @retval    Any other error codes     Failed to start.
 **********************************************************************************************************************/
fsp_err_t adc_scan_start(void)
{
    fsp_err_t err = FSP_SUCCESS;

#if (BSP_PERIPHERAL_ADC_PRESENT)
    g_window_comp_event = false;
#endif /* BSP_PERIPHERAL_ADC_PRESENT */

    /* Open ADC */
    err = ADC_OPEN(&g_adc_ctrl, &g_adc_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

#ifdef BOARD_RA2A1_EK
    /* Set Reference Voltage Circuit Control register */
    R_ADC0->VREFAMPCNT |= ((VREFADCG_VALUE << SHIFT_BY_ONE) | (VREFADCG_ENABLE << SHIFT_BY_THREE));
#endif /* BOARD_RA2A1_EK */

    /* Configure scan parameters */
    err = ADC_SCANCFG(&g_adc_ctrl, &ADC_CHANNEL_CONFIG);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

#if (defined(BOARD_RA2A1_EK)) || (BSP_PERIPHERAL_ADC_B_PRESENT)
    err = adc_start_calibration();
    if (FSP_SUCCESS != err)
    {
        return err;
    }
#endif /* BOARD_RA2A1_EK || BSP_PERIPHERAL_ADC_B_PRESENT */

    /* Start scan */
    err = ADC_SCANSTART(&g_adc_ctrl);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

#if (BSP_PERIPHERAL_ADC_PRESENT)
    /* Disable scan-complete interrupt */
    R_BSP_IrqDisable((IRQn_Type)VECTOR_NUMBER_ADC0_SCAN_END);
#endif /* BSP_PERIPHERAL_ADC_PRESENT */

    g_ready_to_read = true;
    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function stops scan ADC.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Stopped successfully.
 * @retval    Any other error codes     Failed to stop.
 **********************************************************************************************************************/
fsp_err_t adc_scan_stop(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = ADC_SCANSTOP(&g_adc_ctrl);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    err = ADC_CLOSE(&g_adc_ctrl);
    if (FSP_SUCCESS == err)
    {
        g_ready_to_read = false;
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function reads ADC data.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Read successfully.
 * @retval    Any other error codes     Failed to read.
 **********************************************************************************************************************/
fsp_err_t adc_read_data(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Read raw ADC counts */
    err = ADC_READ(&g_adc_ctrl, ADC_CHANNEL, &g_adc_data);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

#ifndef BOARD_RA6T2_MCK
    g_adc_volt = (float)(g_adc_data * V_REF) / ADC_COUNTS_FULL_SCALE;  /* Voltage conversion */
#endif /* BOARD_RA6T2_MCK */

    /* Single-scan or one-shot auto-close after first read */
    if (ADC_IS_SINGLE_SCAN() && g_ready_to_read)
    {
        err = adc_scan_stop();
        if (FSP_SUCCESS != err)
        {
            return err;
        }
    }
#if (BSP_PERIPHERAL_ADC_PRESENT)
    else if (g_window_comp_event && ((ADC_L_LMT > g_adc_data) || (ADC_H_LMT < g_adc_data)) && g_ready_to_read)
    {
        err = adc_scan_stop();
        if (FSP_SUCCESS != err)
        {
            return err;
        }
    }
    else
    {
        /* Do nothing */
    }
#endif /* BSP_PERIPHERAL_ADC_PRESENT */

#ifdef BOARD_RA2A1_EK
    err = adc_deviation_in_output();
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    g_prev_adc_data = g_adc_data;
#endif /* BOARD_RA2A1_EK */

    R_BSP_SoftwareDelay(ADC_READ_DELAY, BSP_DELAY_UNITS_SECONDS);
    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function de-initializes the opened ADC module.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void deinit_adc_module(void)
{
    /* Close the opened ADC module */
    if (g_adc_ctrl.opened)
    {
        ADC_CLOSE(&g_adc_ctrl);
    }
}

#if (defined(BOARD_RA2A1_EK)) || (BSP_PERIPHERAL_ADC_B_PRESENT)
/*******************************************************************************************************************//**
 * @brief     This function starts ADC calibration.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Started successfully.
 * @retval    Any other error codes     Failed to start.
 **********************************************************************************************************************/
static fsp_err_t adc_start_calibration(void)
{
    fsp_err_t err = FSP_SUCCESS;
    adc_status_t adc_status;

    err = ADC_CALIBRATE(&g_adc_ctrl);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Poll until calibration finishes (24 ms – 780 ms depending on clock) */
    do
    {
        err = ADC_STATUSGET(&g_adc_ctrl, &adc_status);
        if (FSP_SUCCESS != err)
        {
            return err;
        }
    }
    while (ADC_STATE_IDLE != adc_status.state);

    return err;
}
#endif /* BOARD_RA2A1_EK || BSP_PERIPHERAL_ADC_B_PRESENT */

#ifdef BOARD_RA2A1_EK
/*******************************************************************************************************************//**
 * @brief     This function re-calibrates ADC value.
 * @param[in] None.
 * @retval    FSP_SUCCESS              Re-calibrated successfully.
 * @retval    Any other error codes    Failed to re-calibrate.
 **********************************************************************************************************************/
static fsp_err_t adc_deviation_in_output(void)
{
    static uint8_t count = 0U;
    fsp_err_t err = FSP_SUCCESS;

    if (0U == g_prev_adc_data)
    {
        return FSP_SUCCESS;     /* No previous sample yet */
    }

    uint16_t diff = (g_prev_adc_data > g_adc_data)
                    ? (g_prev_adc_data - g_adc_data)
                    : (g_adc_data - g_prev_adc_data);

    if (TEMPERATURE_DEVIATION_LIMIT < diff)
    {
        count++;
        if (TEMPERATURE_DEVIATION_LIMIT < count)
        {
            count = 0U;

            /* Stop scan before re-calibrating */
            err = adc_scan_stop();
            if (FSP_SUCCESS != err)
            {
                return err;
            }

            err = adc_start_calibration();
            if (FSP_SUCCESS != err)
            {
                return err;
            }

            g_adc_data = 0U;
        }
    }
    else
    {
        count = 0U;
    }

    return err;
}
#endif /* BOARD_RA2A1_EK */

#if (BSP_PERIPHERAL_ADC_PRESENT)
/***********************************************************************************************************************
 *  @brief     The callback function for when window A compare event occurs.
 *  @param[in] p_args   Callback function parameter data.
 *  @retval    None.
 **********************************************************************************************************************/
void adc_callback(adc_callback_args_t * p_args)
{
    if (ADC_EVENT_WINDOW_COMPARE_A == p_args->event)
    {
        g_window_comp_event = true;
        IRQn_Type irq = R_FSP_CurrentIrqGet();
        R_BSP_IrqDisable(irq);
    }
}
#endif /* BSP_PERIPHERAL_ADC_PRESENT */

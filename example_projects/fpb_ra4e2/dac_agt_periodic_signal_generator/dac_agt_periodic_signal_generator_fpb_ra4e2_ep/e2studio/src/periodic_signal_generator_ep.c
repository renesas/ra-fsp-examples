/***********************************************************************************************************************
 * File Name    : periodic_signal_generator_ep.c
 * Description  : Contains data structures and functions used in periodic_signal_generator_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <math.h>
#include "periodic_signal_generator_ep.h"

/* Sine wave lookup table */
static uint16_t s_look_up_table[SPP_MAX_VAL];

/* Variable to store the DAC's maximum sample rate in Hz (board-specific) */
static double s_dac_max_samp_rate;

/* Variable to store AGT clock frequency */
static uint32_t s_timer_freq_hz;

static void generate_lut(uint16_t input_spp);

/*******************************************************************************************************************//**
 * @brief     This function initializes the DAC module, then calculates the maximum sample rate.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t dac_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Accounting for the 'Pin Configuration' limitation in the FSP Users' Manual for r_dac */
#ifdef BOARD_RA2A1_EK
    R_IOPORT_PinCfg(g_ioport.p_ctrl, BSP_IO_PORT_05_PIN_00,
                    ((uint32_t)IOPORT_CFG_ANALOG_ENABLE | IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_CAC_AD));
#endif /* BOARD_RA2A1_EK */

    /* Initialize and start the DAC module */
    err = DAC_OPEN(&g_dac0_ctrl, &g_dac0_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    err = DAC_START(&g_dac0_ctrl);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Calculate the DAC's maximum sample rate in Hz. This is calculated by the formula: 1/DAC_MAX_CONVERSION_PERIOD
     * DAC_MAX_CONVERSION_PERIOD for each MCU is specified in the Electrical Characteristics
     * section of the RA User's Manual */
    s_dac_max_samp_rate = 1.0 / (DAC_MAX_CONVERSION_PERIOD * pow(10.0, -6.0));

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function initializes the DTC module.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t dtc_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Initialize the DTC module */
    err = R_DTC_Open(&g_transfer0_ctrl, &g_transfer0_cfg);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function initializes AGT module and gets the AGT clock frequency.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t agt_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Initialize and start the AGT module */
    err = R_AGT_Open(&g_timer0_ctrl, &g_timer0_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    err = R_AGT_Start(&g_timer0_ctrl);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Get the AGT clock frequency */
    s_timer_freq_hz = R_FSP_SystemClockHzGet(FSP_PRIV_CLOCK_PCLKB) >> g_timer0_cfg.source_div;

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function generates the sine LUT for the given samples per period, then configures the DTC and
 *            AGT for the requested output frequency.
 * @param[in] input_spp                 Number of samples per period.
 * @param[in] input_freq                Output frequency in Hz.
 * @retval    FSP_SUCCESS               Configured successfully.
 * @retval    Any other error codes     Failed to configure.
 **********************************************************************************************************************/
fsp_err_t dac_output_set(uint16_t input_spp, uint32_t input_freq)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Calculate look up table */
    generate_lut(input_spp);

    /* Reconfigure the DTC:
     * Destination: Address of DADRn register
     * Source: Base address of the look up table
     * Length: Length of the look up table */
#if defined (BOARD_RA8T1_MCK)
    g_transfer0_cfg.p_info->p_dest = (void *)DADR1;
#else
    g_transfer0_cfg.p_info->p_dest = (void *)DADR0;
#endif /* BOARD_RA8T1_MCK */
    g_transfer0_cfg.p_info->p_src = s_look_up_table;
    g_transfer0_cfg.p_info->length = input_spp;

    err = R_DTC_Reconfigure(&g_transfer0_ctrl, g_transfer0_cfg.p_info);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Calculate the output sample frequency of the DAC for the specified wave */
    double sample_freq = (double)input_spp * (double)input_freq;

    /* Calculate and set the AGT counts per period, based on the relation between output sample frequency
     * and the AGT clock source frequency */
    uint32_t period_counts = (uint32_t)(s_timer_freq_hz / sample_freq);

    return R_AGT_PeriodSet(&g_timer0_ctrl, period_counts);
}

/*******************************************************************************************************************//**
 * @brief      This function calculates the minimum and maximum output frequency limits.
 * @param[in]  input_spp                Number of samples per period.
 * @param[out] p_min_hz                 Minimum output frequency in Hz.
 * @param[out] p_max_hz                 Maximum output frequency in Hz.
 * @retval     None.
 **********************************************************************************************************************/
void dac_freq_limits_get(uint16_t input_spp, uint32_t *p_min_hz, uint32_t *p_max_hz)
{
    /* Calculate the maximum possible frequency, determined by the relation:
     * dac_max_samp_rate = freq_max_val * input_spp */
    *p_max_hz = (uint32_t)(s_dac_max_samp_rate / input_spp);

    /* Calculate the minimum value allowed output frequency, based on:
     *   - AGT timer resolution (16-bit: AGT_MAX_PERIOD_16BIT or 32-bit: AGT_MAX_PERIOD_32BIT)
     *   - Number of samples per waveform cycle (input_spp)
     *   - Clock frequency supplied to AGT (timer_freq_hz)
     *
     * For 16-bit AGT: freq_min_val = (timer_freq_hz / (input_spp * AGT_MAX_PERIOD_16BIT)) + 1U
     * For 32-bit AGT: freq_min_val = (timer_freq_hz / (input_spp * AGT_MAX_PERIOD_32BIT)) + 1U
     */
#if (BSP_PERIPHERAL_AGTW_PRESENT)
    *p_min_hz = (s_timer_freq_hz / ((uint32_t)input_spp * AGT_MAX_PERIOD_32BIT)) + 1U;
#else
    *p_min_hz = (s_timer_freq_hz / ((uint32_t)input_spp * AGT_MAX_PERIOD_16BIT)) + 1U;
#endif /* BSP_PERIPHERAL_AGTW_PRESENT */
}

/*******************************************************************************************************************//**
 * @brief     This function closes the opened DAC module.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void dac_deinit(void)
{
    if (g_dac0_ctrl.channel_opened)
    {
        DAC_CLOSE(&g_dac0_ctrl);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function closes the opened AGT module.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void agt_deinit(void)
{
    if (g_timer0_ctrl.open)
    {
        R_AGT_Close(&g_timer0_ctrl);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function closes the opened DTC module.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void dtc_deinit(void)
{
    if (g_transfer0_ctrl.open)
    {
        R_DTC_Close(&g_transfer0_ctrl);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function computes one period of the cosine wave into the look-up table,
 *            scaled between 0 and the DAC full-scale range.
 * @param[in] input_spp                 Number of samples per period.
 * @retval    None.
 **********************************************************************************************************************/
static void generate_lut(uint16_t input_spp)
{
    for (uint16_t i = 0U; i < input_spp; i++)
    {
        s_look_up_table[i] = (uint16_t)(DAC_MID_VAL * (cos(2.0 * M_PI * i / input_spp) + 1.0));
        /* Check LUT values if requested by the user */
        /* printf("\r\n  %d", s_look_up_table[i]); */
    }
}

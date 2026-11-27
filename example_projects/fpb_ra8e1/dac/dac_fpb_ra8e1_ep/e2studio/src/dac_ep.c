/***********************************************************************************************************************
 * File Name    : dac_ep.c
 * Description  : Contains data structures and functions used in dac_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "dac_ep.h"

/* Flag to notify that ADC read is complete */
volatile bool g_is_read_complete = false;

/* Flag to notify any error in reading ADC data */
volatile bool g_error_flag = false;

/* Variable to store scanned ADC data */
uint16_t g_adc_data = RESET_VALUE;

#if defined(BOARD_RA2A1_EK) || (BSP_PERIPHERAL_ADC_B_PRESENT)
/* Flag to notify successful calibration completion */
static volatile bool s_is_calibration_complete = false;
#endif /* BOARD_RA2A1_EK || BSP_PERIPHERAL_ADC_B_PRESENT */

/*******************************************************************************************************************//**
 * @brief     This function opens DAC module.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Opened successfully.
 * @retval    Any other error codes     Failed to open.
 **********************************************************************************************************************/
fsp_err_t dac_open(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Configure DAC12 output pin for EK-RA2A1 */
#if defined(BOARD_RA2A1_EK)
    err = R_IOPORT_PinCfg(&g_ioport_ctrl, DAC12_OUTPUT_PIN,
                          IOPORT_CFG_ANALOG_ENABLE | IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_CAC_AD);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
#endif /* BOARD_RA2A1_EK */

    /* Open DAC module */
    err = R_DAC_OPEN(&g_dac_ctrl, &g_dac_cfg);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function opens ADC module.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Opened successfully.
 * @retval    Any other error codes     Failed to open.
 **********************************************************************************************************************/
fsp_err_t adc_open(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Open ADC module */
    err = R_ADC_OPEN(&g_adc_ctrl, &g_adc_cfg);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function configures the ADC scan parameters.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Configured successfully.
 * @retval    Any other error codes     Failed to configure.
 **********************************************************************************************************************/
fsp_err_t adc_scan_cfg(void)
{
    fsp_err_t err = FSP_SUCCESS;

#if (BSP_PERIPHERAL_ADC_B_PRESENT)
    err = R_ADC_SCAN_CFG(&g_adc_ctrl, &g_adc_scan_cfg);
#else
    err = R_ADC_SCAN_CFG(&g_adc_ctrl, &g_adc_channel_cfg);
#endif /* BSP_PERIPHERAL_ADC_B_PRESENT */

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function reads ADC conversion results from a single channel.
 * @param[in] p_ctrl                    Pointer to the control structure.
 * @param[in] reg_id                    ADC channel.
 * @param[in] p_data                    Data to store ADC conversion results.
 * @retval    FSP_SUCCESS               Read successfully.
 * @retval    Any other error codes     Failed to read.
 **********************************************************************************************************************/
fsp_err_t adc_read(adc_ctrl_t * p_ctrl, adc_channel_t const reg_id, uint16_t * const p_data)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Read ADC conversion result */
    err = R_ADC_READ(p_ctrl, reg_id, p_data);

    return err;
}

#if defined(BOARD_RA2A1_EK) || (BSP_PERIPHERAL_ADC_B_PRESENT)
/*******************************************************************************************************************//**
 * @brief     This function sets the ADC reference voltage and initiate ADC calibration.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Calibrated successfully.
 * @retval    Any other error codes     Failed to calibrate.
 **********************************************************************************************************************/
fsp_err_t adc_calibrate(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Set reference voltage circuit control register */
#if !(defined (BOARD_RA8P1_EK) || defined (BOARD_RA8T2_MCK) || defined (BOARD_RA8D2_EK)) || defined(BOARD_RA8M2_EK)
    R_ADC0->VREFAMPCNT |= ((VREFADCG_VALUE) | (VREFADCG_ENABLE));
#endif /* BOARD_RA8P1_EK || BOARD_RA8T2_MCK || BOARD_RA8D2_EK || BOARD_RA8M2_EK */

    /* Initiate ADC calibration */
    err = R_ADC_CALIBRATE(&g_adc_ctrl, NULL);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Wait for calibration to complete */
    while (!s_is_calibration_complete)
    {
        /* Do nothing */
    }

    return err;
}
#endif /* defined(BOARD_RA2A1_EK) || (BSP_PERIPHERAL_ADC_B_PRESENT) */

/*******************************************************************************************************************//**
 * @brief     This function performs DAC conversion and ADC Scan operation.
 * @param[in] input                     The user input for DAC conversion.
 * @retval    FSP_SUCCESS               Operated successfully.
 * @retval    Any other error codes     Failed to operate.
 **********************************************************************************************************************/
fsp_err_t dac_adc_operations(int32_t *input)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Write value to DAC */
    err = R_DAC_WRITE(&g_dac_ctrl, (uint16_t)(*input));
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Start DAC conversion */
    err = R_DAC_START(&g_dac_ctrl);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* As per mention in section 35.3.1 of EK-RA4W1 hardware user manual, DAC can't be synchronized with ADC
     * because both are sharing the same bus.
     * Re-open ADC for boards where DAC and ADC share the same bus */
#if defined(BOARD_RA4W1_EK) || defined(BOARD_RA6T1_RSSK)
    err = R_ADC_OPEN(&g_adc_ctrl, &g_adc_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    err = R_ADC_SCAN_CFG(&g_adc_ctrl, &g_adc_channel_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
#endif /* BOARD_RA4W1_EK || BOARD_RA6T1_RSSK */

    /* As mentioned in DAC properties and Electrical characteristics of RA8E2 User Manual (Section 51.6),
     * a software delay has to be given for stabilizing the voltage on DAC output pin.
     * Also referred as DAC conversion time delay */
    /* Delay for DAC output voltage to stabilize */
    R_BSP_SoftwareDelay(STABILIZATION_DELAY, BSP_DELAY_UNITS_MILLISECONDS);

    /* Start the ADC scan */
#if (BSP_PERIPHERAL_ADC_B_PRESENT)
    err = R_ADC_SCAN_GROUP_START(&g_adc_ctrl, ADC_GROUP_MASK_0);
#else
    err = R_ADC_SCAN_START(&g_adc_ctrl);
#endif /* BSP_PERIPHERAL_ADC_B_PRESENT */
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Stop DAC output for next conversion cycle */
    err = R_DAC_STOP(&g_dac_ctrl);
    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function handles the ADC callback.
 * @param[in] p_args                    Pointer to the callback function parameter data.
 * @retval    None.
 **********************************************************************************************************************/
void adc_callback(adc_callback_args_t *p_args)
{
    if (ADC_EVENT_SCAN_COMPLETE == p_args->event)
    {
        fsp_err_t err = FSP_SUCCESS;

        /* Read the result after ADC Scan is complete */
        err = adc_read(&g_adc_ctrl, ADC_CHANNEL, &g_adc_data);
        if (FSP_SUCCESS != err)
        {
            g_error_flag = true;
        }
        else
        {
            g_is_read_complete = true;
        }
    }

#if defined(BOARD_RA2A1_EK) || (BSP_PERIPHERAL_ADC_B_PRESENT)
    /* Check if calibration is complete and set flag */
    else if (ADC_EVENT_CALIBRATION_COMPLETE == p_args->event)
    {
        s_is_calibration_complete = true;
    }
    else
    {
        /* Do nothing */
    }
#endif /* BOARD_RA2A1_EK || BSP_PERIPHERAL_ADC_B_PRESENT */
}

/*******************************************************************************************************************//**
 * @brief     This function closes the opened DAC module.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void dac_deinit(void)
{
    /* Close the opened DAC module */
    if (g_dac_ctrl.channel_opened)
    {
        R_DAC_CLOSE(&g_dac_ctrl);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function closes the opened ADC module.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void adc_deinit(void)
{
    /* Close the opened ADC module */
    if (g_adc_ctrl.opened)
    {
        R_ADC_CLOSE(&g_adc_ctrl);
    }
}

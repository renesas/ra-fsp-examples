/***********************************************************************************************************************
 * File Name    : io_ep.c
 * Description  : Contains function definition.
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
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/
#include "common_utils.h"
#include <io_ep.h>
/*******************************************************************************************************************//**
 * @addtogroup io_ep
 * @{
 **********************************************************************************************************************/

/* Boolean flag to determine switches is pressed or not.*/
volatile bool g_sw1_press                     = false;
volatile bool g_sw2_press                     = false;
volatile uint8_t g_output_delay_overfow_flag = 0;

/*******************************************************************************************************************//**
 * @brief       This functions initializes ICU module.
 * @param[IN]   None
 * @retval      FSP_SUCCESS                  Upon successful open of ICU module
 * @retval      Any Other Error code apart from FSP_SUCCESS  Unsuccessful open
 **********************************************************************************************************************/
fsp_err_t icu_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Open ICU module */
    err = R_ICU_ExternalIrqOpen(&g_external_irq_ctrl, &g_external_irq_cfg);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Enable ICU module */
    err = R_ICU_ExternalIrqEnable(&g_external_irq_ctrl);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Open ICU module */
    err = R_ICU_ExternalIrqOpen(&g_external_irq_s2_ctrl, &g_external_irq_s2_cfg);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Enable ICU module */
    err = R_ICU_ExternalIrqEnable(&g_external_irq_s2_ctrl);

    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function closes opened ICU module before the project ends up in an Error Trap.
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
void icu_deinit(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Close ICU module */
    err = R_ICU_ExternalIrqClose(&g_external_irq_ctrl);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        /* ICU Close failure message */
        APP_ERR_PRINT("\r\n**R_ICU_ExternalIrqClose API FAILED**\r\n");
    }
    /* Close ICU module */
    err = R_ICU_ExternalIrqClose(&g_external_irq_s2_ctrl);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        /* ICU Close failure message */
        APP_ERR_PRINT("\r\n**R_ICU_ExternalIrqClose API FAILED**\r\n");
    }
}

/*******************************************************************************************************************//**
 * @brief      User defined external irq callback.
 * @param[IN]  p_args
 * @retval     None
 **********************************************************************************************************************/
void irq_ep_callback(external_irq_callback_args_t *p_args)
{
    /* Make sure it's the right interrupt*/
    if(USER_SW_IRQ_NUMBER == p_args->channel)
    {
        g_sw1_press = true;
    }
}

/*******************************************************************************************************************//**
 * @brief      User defined external irq callback.
 * @param[IN]  p_args
 * @retval     None
 **********************************************************************************************************************/
void irq_s2_ep_callback(external_irq_callback_args_t *p_args)
{
    /* Make sure it's the right interrupt*/
    if(USER_SW2_IRQ_NUMBER == p_args->channel)
    {
        g_sw2_press = true;
    }
}

/*******************************************************************************************************************//**
 * @brief This function is setting up GPT/PWM timer
 * @param[in] None
 * @retval    fsp_err_t
 **********************************************************************************************************************/
fsp_err_t gpt_ref_timer_PWM_init(void)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Open GPT */
    err = R_GPT_Open(&g_ref_timer_ctrl, &g_ref_timer_cfg);
    if(FSP_SUCCESS != err)
    {
        return err;
    }
    /* Enable GPT Timer */
    err = R_GPT_Enable(&g_ref_timer_ctrl);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    /* Reset GPT timer */
    err = R_GPT_Reset(&g_ref_timer_ctrl);;

    return err;
}

/*******************************************************************************************************************//**
 * @brief This function is setting up GPT/PWM timer
 * @param[in] None
 * @retval    fsp_err_t
 **********************************************************************************************************************/
fsp_err_t gpt_output_delay_timer_PWM_init(void)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Open GPT */
    err = R_GPT_Open(&g_output_delay_timer_ctrl, &g_output_delay_timer_cfg);
    if(FSP_SUCCESS != err)
    {
        return err;
    }
    /* Enable GPT Timer */
    err = R_GPT_Enable(&g_output_delay_timer_ctrl);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    /* Reset GPT timer */
    err = R_GPT_Reset(&g_output_delay_timer_ctrl);;

    return err;
}

/*******************************************************************************************************************//**
 * @brief This function is configuring ODC and start GPT0 (Ref PWM) & GPT1 (PWM Output Delay) simultaneously
 * @param[in] None
 * @retval    None
 **********************************************************************************************************************/
void odc_init(void)
{
    /* Initialize the PWM Delay Generation Circuit */
    R_GPT_PwmOutputDelayInitialize();

    /* GPT0 (Ref PWM) & GPT1 (PWM Output Delay) signals both pass via the Delay Circuit for synchronization */

    /* GTDLYCR2.DLYBSn = 1 : PWM delay generation circuit bypass off */
    R_GPT_ODC->GTDLYCR2_b.DLYBS1 = 1;
    /* GTDLYCR2.DLYBSn = 1 : PWM delay generation circuit bypass off */
    R_GPT_ODC->GTDLYCR2_b.DLYBS0 = 1;

    /* Should be 0 by default - enable the delay circuit */
    R_GPT_ODC->GTDLYCR2_b.DLYEN1 = 0;

    /* GPT0 (Ref PWM) & GPT1 (PWM Output Delay) simultaneously */
    R_GPT0->GTSTR = 0x00000003;
}

/*
 * There is this usage note when using the Output Delay Function (R01UH0886EJ0110 Rev.1.10)
 *
 * 24.4.2 Notes on Delay Settings for PWM Delay Generation Circuit
 *   When the PWM Delay Generation Circuit generates delays for a PWM output waveform and the waveform is toggled in
 *   response to compare-matches, do not change the settings for delay while the compare-match value is within the ranges
 *   listed in Table 24.4. This constraint applies to the GTDLYFnA, GTDLYRnA, GTDLFnB, and GTDLYRnB
 *   registers.
 *
 *   An example of how the restrictions apply to the timing of setting GTDLYFnA in Saw-wave waveform one-shot pulse
 *   mode (counting up) is shown as Figure 24.5 (in R01UH0886EJ0110 Rev.1.10). Do not change the value set in GTDLYFnA while GTCCRD ≥ GTPR - 2.
 *   Table 24.4 Constraints on delay settings
 *
 *   Mode               |   Direction of counting     |     Compare-match value
 *   -------------------+-----------------------------+----------------------------
 *   Saw-wave mode      |   Up                        |     GTPR - 2 or above
 *                      |   Down                      |     2 or below
 *   -------------------+-----------------------------+----------------------------
 *   Triangle-wave mode |   Down                      |     2 or below
 *
 *   Figure 24.5 (in R01UH0886EJ0110 Rev.1.10) shows an example of how the constraints apply to the timing of setting GTDLYFnA in saw-wave waveform
 *   one-shot pulse mode (counting up). Do not change the value set in GTDLYFnA while GTCCRD ≥ GTPR - 2.
 *
 *   Changing the values in the GTDLYFnA, GTDLYRnA, GTDLYFnB, and GTDLYRnB registers during periods where
 *   changes to settings are not allowed, might lead to faulty output waveforms such as shifts in the timing of output
 *   waveform transitions from the expected values.
 *
 *   Therefore, due to the interrupt latency when the callback function is executed, the Compare Match value
 *   of the GPT will be outside the ranges stated above.
 */
void g_output_delay_overflow_callback(timer_callback_args_t *p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);

    /* The delay values can be changed now, if required */
    g_output_delay_overfow_flag = 1;
}
/*******************************************************************************************************************//**
 * @} (end addtogroup io_ep)
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * File Name    : poeg_ep.c
 * Description  : Contains functions
 ***********************************************************************************************************************/
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
 * Copyright (C) 2022 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/
#include "common_utils.h"
#include "poeg_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup poeg_ep
 * @{
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief     This function de-initialize the ACMPHS module
 * @param[in] None
 * @retval    None
 **********************************************************************************************************************/
void deinit_acmphs(void)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Close ACMPHS module */
    err = R_ACMPHS_Close(&g_comparator_ctrl);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nR_ACMPHS_Close API failed\r\n")
    }
    else
    {
        APP_PRINT("\r\nR_ACMPHS_Close API success\r\n")
    }
}


/*******************************************************************************************************************//**
 * @brief     This function de-initialize the DAC module
 * @param[in] dac_instance_ctrl_t * p_ctr
 * @retval    None
 **********************************************************************************************************************/
void deinit_dac(dac_instance_ctrl_t * p_ctrl)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Close DAC */
    err = R_DAC_Close(p_ctrl);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nR_DAC_Close API failed\r\n");
    }
    else
    {
        APP_PRINT("\r\nR_DAC_Close API success\r\n");
    }
}


/*******************************************************************************************************************//**
 * @brief     This function de-initialize the DAC0 ,DAC1 and ACMPHS modules
 * @param[in] None
 * @retval    None
 **********************************************************************************************************************/
void deinit_dac_and_acmphs(void)
{
    deinit_dac(&g_dac0_ctrl);
    deinit_dac(&g_dac1_ctrl);
    deinit_acmphs();
}


/*******************************************************************************************************************//**
 * @brief     This function de-initialize the Timer module
 * @param[in] timer_ctrl_t * const p_timer_ctl
 * @retval    None
 **********************************************************************************************************************/
void deinit_gpt_timer(timer_ctrl_t * const p_timer_ctl)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Timer Close API call*/
    err = R_GPT_Close(p_timer_ctl);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        /* GPT Close failure message */
        APP_ERR_PRINT ("\r\nR_GPT_Close failed\r\n");
    }
    else
    {
        APP_PRINT ("\r\nR_GPT_Close success\r\n");
    }
}


/*******************************************************************************************************************//**
 * @brief init_gpt_timer_pwm1
 * This function initialize the GPT PWM1 module
 * @param[in] None
 * @retval      FSP_SUCCESS                  Upon successful GPT Open
 * @retval      Any Other Error code apart from FSP_SUCCESS  Upon Unsuccessful
 **********************************************************************************************************************/
fsp_err_t init_gpt_timer_pwm1(void)
{
    fsp_err_t err = FSP_SUCCESS;
    err = R_GPT_Open(&g_timer_pwm1_ctrl, &g_timer_pwm1_cfg);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\nR_GPT_Open failed\r\n");
    }
    else
    {
        APP_PRINT ("\r\nR_GPT_Open success\r\n");
    }
    return (err);
}


/*******************************************************************************************************************//**
 * @brief     This function initialize the GPT PWM2 module
 * @param[in] None
 * @retval      FSP_SUCCESS                  Upon successful GPT Open
 * @retval      Any Other Error code apart from FSP_SUCCESS  Upon Unsuccessful
 **********************************************************************************************************************/
fsp_err_t init_gpt_timer_pwm2(void)
{
    fsp_err_t err = FSP_SUCCESS;
    err = R_GPT_Open(&g_timer_pwm2_ctrl, &g_timer_pwm2_cfg);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\nR_GPT_Open failed\r\n");
    }
    else
    {
        APP_PRINT ("\r\nR_GPT_Open success\r\n");
    }
    return (err);
}


/*******************************************************************************************************************//**
 * @brief     This function initialize the GPT PWM3 module
 * @param[in] None
 * @retval      FSP_SUCCESS                  Upon successful GPT Open
 * @retval      Any Other Error code apart from FSP_SUCCESS  Upon Unsuccessful
 **********************************************************************************************************************/
fsp_err_t init_gpt_timer_pwm3(void)
{
    fsp_err_t err = FSP_SUCCESS;
    err = R_GPT_Open(&g_timer_pwm3_ctrl, &g_timer_pwm3_cfg);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\nR_GPT_Open failed\r\n");
    }
    else
    {
        APP_PRINT ("\r\nR_GPT_Open success\r\n");
    }
    return (err);
}



/*******************************************************************************************************************//**
 * @brief     This function initialize the POEG0 module
 * @param[in] None
 * @retval      FSP_SUCCESS                  Upon successful POEG0 Open
 * @retval      Any Other Error code apart from FSP_SUCCESS  Upon Unsuccessful
 **********************************************************************************************************************/
fsp_err_t init_poeg0_module(void)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Initializes the POEG. */
    err = R_POEG_Open(&g_poeg0_ctrl, &g_poeg0_cfg);
    /* Handle any errors */
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("POEG 0 initialization failed\r\n");
        return (err);
    }
    else
    {
        APP_PRINT("\r\nPOEG 0 initialized successfully");
    }
    NVIC_DisableIRQ(VECTOR_NUMBER_POEG0_EVENT);
    return err;
}


/*******************************************************************************************************************//**
 * @brief     This function de-initialize the POEG0 module
 * @param[in] None
 * @retval      FSP_SUCCESS                  Upon successful POEG0 Close
 * @retval      Any Other Error code apart from FSP_SUCCESS  Upon Unsuccessful
 **********************************************************************************************************************/
fsp_err_t deinit_poeg0_module(void)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Initializes the POEG. */
    err = R_POEG_Close(&g_poeg0_ctrl);
    /* Handle error */
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("POEG 0 de-initialization failed\r\n");
    }
    else
    {
        APP_PRINT("POEG 0 de-initialization success\r\n");
    }
    return err;
}


/*******************************************************************************************************************//**
 * @brief     This function initialize the POEG1 module
 * @param[in] None
 * @retval      FSP_SUCCESS                  Upon successful POEG1 Open
 * @retval      Any Other Error code apart from FSP_SUCCESS  Upon Unsuccessful
 **********************************************************************************************************************/
fsp_err_t init_poeg1_module(void)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Initializes the POEG. */
    err = R_POEG_Open(&g_poeg1_ctrl, &g_poeg1_cfg);
    /* Handle error */
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("POEG 1 initialization Failed\r\n");
        return (err);
    }
    else
    {
        APP_PRINT("\r\nPOEG 1 initialized successfully");
    }
    NVIC_DisableIRQ(VECTOR_NUMBER_POEG1_EVENT);
    return err;
}


/*******************************************************************************************************************//**
 * @brief     This function de-initialize the POEG1 module
 * @param[in] None
 * @retval      FSP_SUCCESS                  Upon successful POEG1 Close
 * @retval      Any Other Error code apart from FSP_SUCCESS  Upon Unsuccessful
 **********************************************************************************************************************/
fsp_err_t deinit_poeg1_module(void)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Initializes the POEG. */
    err = R_POEG_Close(&g_poeg1_ctrl);
    /* Handle error */
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("POEG 1 de-initialization failed\r\n");
    }
    else
    {
        APP_PRINT("POEG 1 de-initialization success\r\n");
    }
    return err;
}


/*******************************************************************************************************************//**
 * @brief     This function Starts conversions on DAC0,DAC1 modules
 * @param[in] None
 * @retval      FSP_SUCCESS                  Upon successful DAC and ACMPHS open
 * @retval      Any Other Error code apart from FSP_SUCCESS  Upon Unsuccessful
 **********************************************************************************************************************/
fsp_err_t enable_acmphs_mode_modules(void)
{
    fsp_err_t err = FSP_SUCCESS;
    //comparator_info_t stabilize_time = {RESET_VALUE};
    /* Open DAC0 module */
    err = R_DAC_Open(&g_dac0_ctrl, &g_dac0_cfg);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\nR_DAC_Open API failed for DAC0\r\n");
        return(err);
    }
    else
    {
        APP_PRINT ("\r\nR_DAC_Open API success for DAC0\r\n");
    }

    /* Open DAC1 module */
    err = R_DAC_Open(&g_dac1_ctrl, &g_dac1_cfg);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\nR_DAC_Open API FAILED FOR DAC1\r\n");
        deinit_dac(&g_dac0_ctrl);
        return(err);
    }
    else
    {
        APP_PRINT ("\r\nR_DAC_Open API success FOR DAC1\r\n");
    }


    /* Open ACMPHS module */
    err  = R_ACMPHS_Open(&g_comparator_ctrl, &g_comparator_cfg);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\nR_ACMPHS_Open API failed\r\n");
        deinit_dac(&g_dac0_ctrl);
        deinit_dac(&g_dac1_ctrl);
        return(err);
    }
    else
    {
        APP_PRINT ("\r\nR_ACMPHS_Open API success\r\n");
    }


    /* Write 1024 on DAC0 module for reference voltage to comparator */
    err = R_DAC_Write(&g_dac0_ctrl, DAC_REF_VAL);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\nR_DAC_Write API failed for DAC0\r\n");
        deinit_dac_and_acmphs();
        return(err);
    }
    else
    {
        APP_PRINT ("\r\nR_DAC_Write API success for DAC0\r\n");
    }


    /* Write 0 on DAC1 module for providing analog input voltage to comparator
     * and avoid garbage to ACMPHS input */
    err = R_DAC_Write(&g_dac1_ctrl, RESET_VALUE);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\nR_DAC_Write API failed for DAC1\r\n");
        deinit_dac_and_acmphs();
        return(err);
    }
    else
    {
        APP_PRINT ("\r\nR_DAC_Write API success for DAC1\r\n");
    }

    /* Start conversion on DAC0 module */
    err = R_DAC_Start(&g_dac0_ctrl);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\nR_DAC_Start API failed for DAC0\r\n");
        deinit_dac_and_acmphs();
        return(err);
    }
    else
    {
        APP_PRINT ("\r\nR_DAC_Start API success for DAC0\r\n");
    }

    /* Start conversion on DAC1 module  */
    err = R_DAC_Start(&g_dac1_ctrl);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\nR_DAC_Start API failed for DAC1\r\n");
        deinit_dac_and_acmphs();
        return(err);
    }
    else
    {
        APP_PRINT ("\r\nR_DAC_Start API success for DAC1\r\n");
    }

    /* Get the minimum stabilization wait time */
    err = R_ACMPHS_InfoGet(&g_comparator_ctrl, &g_stabilize_time);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\nR_ACMPHS_InfoGet API failed\r\n");
        deinit_dac_and_acmphs();
        return(err);
    }
    else
    {
        APP_PRINT ("\r\nR_ACMPHS_InfoGet API success\r\n");
    }

    /* Enable the comparator output */
    err = R_ACMPHS_OutputEnable(&g_comparator_ctrl);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\nR_ACMPHS_OutputEnable API failed\r\n");
        deinit_dac_and_acmphs();
        return(err);
    }
    else
    {
        APP_PRINT ("\r\nR_ACMPHS_OutputEnable API success\r\n");
    }
    return err;
}


/*******************************************************************************************************************//**
 * @} (end addtogroup poeg_ep)
 **********************************************************************************************************************/

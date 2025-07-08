/***********************************************************************************************************************
 * File Name    : poeg_ep.c
 * Description  : Contains functions
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2022 - 2025 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "poeg_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup poeg_ep
 * @{
 **********************************************************************************************************************/

#if (BSP_PERIPHERAL_ACMPHS_PRESENT) || (BSP_PERIPHERAL_ACMPHS_B_PRESENT)
/*******************************************************************************************************************//**
 * @brief       This function de-initializes the ACMPHS module
 * @param[in]   None
 * @retval      None
 **********************************************************************************************************************/
void deinit_acmphs(void)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Close ACMPHS module */
#if BSP_PERIPHERAL_ACMPHS_B_PRESENT
    err = R_ACMPHS_B_Close(&g_comparator_ctrl);
#else
    err = R_ACMPHS_Close(&g_comparator_ctrl);
#endif /* BSP_PERIPHERAL_ACMPHS_B_PRESENT */
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nR_"ACMPHS_TYPE"_Close API failed\r\n");
    }
    else
    {
        APP_PRINT("\r\nR_"ACMPHS_TYPE"_Close API success\r\n");
    }
}

#if (BSP_PERIPHERAL_DAC_PRESENT)
/*******************************************************************************************************************//**
 * @brief       This function de-initializes the DAC module
 * @param[in]   dac_instance_ctrl_t * p_ctr
 * @retval      None
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
 * @brief       This function de-initializes the DAC0, DAC1 and ACMPHS modules
 * @param[in]   None
 * @retval      None
 **********************************************************************************************************************/
void deinit_dac_and_acmphs(void)
{
    deinit_dac(&g_dac0_ctrl);
#if !(defined (BOARD_RA8E1_FPB) || defined (BOARD_RA8E2_EK))
    deinit_dac(&g_dac1_ctrl);
#endif /* BOARD_RA8E1_FPB, BOARD_RA8E2_EK */
    deinit_acmphs();
}
#endif /* BSP_PERIPHERAL_DAC_PRESENT */
#endif /* BSP_PERIPHERAL_ACMPHS_PRESENT, BSP_PERIPHERAL_ACMPHS_B_PRESENT */

/*******************************************************************************************************************//**
 * @brief       This function de-initializes the Timer module
 * @param[in]   timer_ctrl_t * const p_timer_ctl
 * @retval      None
 **********************************************************************************************************************/
void deinit_gpt_timer(timer_ctrl_t *const p_timer_ctl)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Timer Close API call */
    err = R_GPT_Close(p_timer_ctl);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        /* GPT Close failure message */
        APP_ERR_PRINT("\r\nR_GPT_Close API failed\r\n");
    }
    else
    {
        APP_PRINT("\r\nR_GPT_Close API success\r\n");
    }
}

/*******************************************************************************************************************//**
 * @brief       This function initializes the GPT PWM1 module
 * @param[in]   None
 * @retval      FSP_SUCCESS                  Upon successful GPT Open
 * @retval      Any other error code apart from FSP_SUCCESS  Upon Unsuccessful
 **********************************************************************************************************************/
fsp_err_t init_gpt_timer_pwm1(void)
{
    fsp_err_t err = FSP_SUCCESS;
    err = R_GPT_Open(&g_timer_pwm1_ctrl, &g_timer_pwm1_cfg);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nR_GPT_Open API failed\r\n");
    }
    else
    {
        APP_PRINT("\r\nR_GPT_Open API success\r\n");
    }
    return (err);
}

/*******************************************************************************************************************//**
 * @brief       This function initializes the GPT PWM2 module
 * @param[in]   None
 * @retval      FSP_SUCCESS                  Upon successful GPT Open
 * @retval      Any other error code apart from FSP_SUCCESS  Upon Unsuccessful
 **********************************************************************************************************************/
fsp_err_t init_gpt_timer_pwm2(void)
{
    fsp_err_t err = FSP_SUCCESS;
    err = R_GPT_Open(&g_timer_pwm2_ctrl, &g_timer_pwm2_cfg);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nR_GPT_Open failed\r\n");
    }
    else
    {
        APP_PRINT("\r\nR_GPT_Open API success\r\n");
    }
    return (err);
}

/*******************************************************************************************************************//**
 * @brief       This function initializes the POEG0 module
 * @param[in]   None
 * @retval      FSP_SUCCESS                  Upon successful POEG0 Open
 * @retval      Any other error code apart from FSP_SUCCESS  Upon Unsuccessful
 **********************************************************************************************************************/
fsp_err_t init_poeg0_module(void)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Initialize the POEG */
    err = R_POEG_Open(&g_poeg0_ctrl, &g_poeg0_cfg);
    /* Handle any errors */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nR_POEG_Open API failed\r\n");
        return (err);
    }
    else
    {
        APP_PRINT("\r\nR_POEG_Open API success");
    }
    NVIC_DisableIRQ(VECTOR_NUMBER_POEG0_EVENT);
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function de-initializes the POEG0 module
 * @param[in]   None
 * @retval      FSP_SUCCESS                  Upon successful POEG0 Close
 * @retval      Any other error code apart from FSP_SUCCESS  Upon Unsuccessful
 **********************************************************************************************************************/
fsp_err_t deinit_poeg0_module(void)
{
    fsp_err_t err = FSP_SUCCESS;
    /* De-initialize the POEG */
    err = R_POEG_Close(&g_poeg0_ctrl);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nR_POEG_Close API failed\r\n");
    }
    else
    {
        APP_PRINT("\r\nR_POEG_Close API success\r\n");
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function initializes the POEG1 module
 * @param[in]   None
 * @retval      FSP_SUCCESS                  Upon successful POEG1 Open
 * @retval      Any other error code apart from FSP_SUCCESS  Upon Unsuccessful
 **********************************************************************************************************************/
fsp_err_t init_poeg1_module(void)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Initialize the POEG */
    err = R_POEG_Open(&g_poeg1_ctrl, &g_poeg1_cfg);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nR_POEG_Open API failed\r\n");
        return (err);
    }
    else
    {
        APP_PRINT("\r\nR_POEG_Open API success");
    }
    NVIC_DisableIRQ(VECTOR_NUMBER_POEG1_EVENT);
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function de-initializes the POEG1 module
 * @param[in]   None
 * @retval      FSP_SUCCESS                  Upon successful POEG1 Close
 * @retval      Any other error code apart from FSP_SUCCESS  Upon Unsuccessful
 **********************************************************************************************************************/
fsp_err_t deinit_poeg1_module(void)
{
    fsp_err_t err = FSP_SUCCESS;
    /* De-initialize the POEG */
    err = R_POEG_Close(&g_poeg1_ctrl);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nR_POEG_Close API failed\r\n");
    }
    else
    {
        APP_PRINT("\r\nR_POEG_Close API success\r\n");
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function de-initializes the POEG0 and POEG1 modules
 * @param[in]   None
 * @retval      None
 **********************************************************************************************************************/
void deinit_poeg_module(void)
{
    /* De-initialize the POEG0 */
    deinit_poeg0_module();

    /* De-initialize the POEG1 */
    deinit_poeg1_module();
}

#if (BSP_PERIPHERAL_ACMPHS_PRESENT) || (BSP_PERIPHERAL_ACMPHS_B_PRESENT)
/*******************************************************************************************************************//**
 * @brief       This function starts conversions on the DAC0, DAC1 modules
 * @param[in]   None
 * @retval      FSP_SUCCESS                  Upon successful DAC and ACMPHS open
 * @retval      Any other error code apart from FSP_SUCCESS  Upon Unsuccessful
 **********************************************************************************************************************/
fsp_err_t enable_acmphs_mode_modules(void)
{
    fsp_err_t err = FSP_SUCCESS;
#if (BSP_PERIPHERAL_DAC_PRESENT)
    /* Open DAC0 module */
    err = R_DAC_Open(&g_dac0_ctrl, &g_dac0_cfg);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nR_DAC_Open API failed for DAC0\r\n");
        return(err);
    }
    else
    {
        APP_PRINT("\r\nR_DAC_Open API success for DAC0\r\n");
    }

#if !(defined (BOARD_RA8E1_FPB) || defined (BOARD_RA8E2_EK))
    /* Open DAC1 module */
    err = R_DAC_Open(&g_dac1_ctrl, &g_dac1_cfg);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nR_DAC_Open API failed for DAC1\r\n");
        deinit_dac(&g_dac0_ctrl);
        return(err);
    }
    else
    {
        APP_PRINT("\r\nR_DAC_Open API success for DAC1\r\n");
    }
#endif /* BOARD_RA8E1_FPB, BOARD_RA8E2_EK */
#endif /* BSP_PERIPHERAL_DAC_PRESENT */

    /* Open ACMPHS module */
#if BSP_PERIPHERAL_ACMPHS_B_PRESENT
    err  = R_ACMPHS_B_Open(&g_comparator_ctrl, &g_comparator_cfg);
#else
    err  = R_ACMPHS_Open(&g_comparator_ctrl, &g_comparator_cfg);
#endif /* BSP_PERIPHERAL_ACMPHS_B_PRESENT */
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nR_"ACMPHS_TYPE"_Open API failed\r\n");
#if BSP_PERIPHERAL_DAC_PRESENT
        deinit_dac(&g_dac0_ctrl);
#if !(defined (BOARD_RA8E1_FPB) || defined (BOARD_RA8E2_EK))
        deinit_dac(&g_dac1_ctrl);
#endif /* BOARD_RA8E1_FPB, BOARD_RA8E2_EK */
#endif /* BSP_PERIPHERAL_DAC_PRESENT */
        return(err);
    }
    else
    {
        APP_PRINT("\r\nR_"ACMPHS_TYPE"_Open API success\r\n");
    }

#if BSP_PERIPHERAL_DAC_PRESENT
#if defined (BOARD_RA8E1_FPB) || defined (BOARD_RA8E2_EK)
    /* Write 0 on DAC0 module for providing analog input voltage to comparator and avoid garbage to ACMPHS input */
    err = R_DAC_Write(&g_dac0_ctrl, RESET_VALUE);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nR_DAC_Write API failed for DAC0\r\n");
        deinit_dac_and_acmphs();
        return(err);
    }
    else
    {
        APP_PRINT("\r\nR_DAC_Write API success for DAC0\r\n");
    }
#else
    /* Write 2048 (≈ 1.65V) on DAC0 module for reference voltage to comparator */
    err = R_DAC_Write(&g_dac0_ctrl, DAC_REF_VAL);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nR_DAC_Write API failed for DAC0\r\n");
        deinit_dac_and_acmphs();
        return(err);
    }
    else
    {
        APP_PRINT("\r\nR_DAC_Write API success for DAC0\r\n");
    }

    /* Write 0 on DAC1 module for providing analog input voltage to comparator
     * and avoid garbage to ACMPHS input */
    err = R_DAC_Write(&g_dac1_ctrl, RESET_VALUE);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nR_DAC_Write API failed for DAC1\r\n");
        deinit_dac_and_acmphs();
        return(err);
    }
    else
    {
        APP_PRINT("\r\nR_DAC_Write API success for DAC1\r\n");
    }
#endif /* BOARD_RA8E1_FPB, BOARD_RA8E2_EK */

    /* Start conversion on DAC0 module */
    err = R_DAC_Start(&g_dac0_ctrl);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nR_DAC_Start API failed for DAC0\r\n");
        deinit_dac_and_acmphs();
        return(err);
    }
    else
    {
        APP_PRINT("\r\nR_DAC_Start API success for DAC0\r\n");
    }

    /* Start conversion on DAC1 module */
#if !(defined (BOARD_RA8E1_FPB) || defined (BOARD_RA8E2_EK))
    err = R_DAC_Start(&g_dac1_ctrl);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nR_DAC_Start API failed for DAC1\r\n");
        deinit_dac_and_acmphs();
        return(err);
    }
    else
    {
        APP_PRINT("\r\nR_DAC_Start API success for DAC1\r\n");
    }
#endif /* BOARD_RA8E1_FPB, BOARD_RA8E2_EK */
#endif /* BSP_PERIPHERAL_DAC_PRESENT */

    /* Get the minimum stabilization wait time */
#if BSP_PERIPHERAL_ACMPHS_B_PRESENT
    err = R_ACMPHS_B_InfoGet(&g_comparator_ctrl, &g_stabilize_time);
#else
    err = R_ACMPHS_InfoGet(&g_comparator_ctrl, &g_stabilize_time);
#endif /* BSP_PERIPHERAL_ACMPHS_B_PRESENT */
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nR_"ACMPHS_TYPE"_InfoGet API failed\r\n");
#if BSP_PERIPHERAL_DAC_PRESENT
        deinit_dac_and_acmphs();
#endif /* BSP_PERIPHERAL_DAC_PRESENT*/
        return(err);
    }
    else
    {
        APP_PRINT("\r\nR_"ACMPHS_TYPE"_InfoGet API success\r\n");
    }

    /* Enable the comparator output */
#if BSP_PERIPHERAL_ACMPHS_B_PRESENT
    err = R_ACMPHS_B_OutputEnable(&g_comparator_ctrl);
#else
    err = R_ACMPHS_OutputEnable(&g_comparator_ctrl);
#endif /* BSP_PERIPHERAL_ACMPHS_B_PRESENT */

    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nR_"ACMPHS_TYPE"_OutputEnable API failed\r\n");
#if BSP_PERIPHERAL_DAC_PRESENT
        deinit_dac_and_acmphs();
#endif /* BSP_PERIPHERAL_DAC_PRESENT*/
        return(err);
    }
    else
    {
        APP_PRINT("\r\nR_"ACMPHS_TYPE"_OutputEnable API success\r\n");
    }
    return err;
}
#endif /* BSP_PERIPHERAL_ACMPHS_PRESENT,  BSP_PERIPHERAL_ACMPHS_B_PRESENT */

/*******************************************************************************************************************//**
 * @} (end addtogroup poeg_ep)
 **********************************************************************************************************************/

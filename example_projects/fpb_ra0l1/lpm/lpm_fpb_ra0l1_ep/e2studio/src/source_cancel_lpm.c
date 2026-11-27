/***********************************************************************************************************************
 * File Name    : source_cancel_lpm.c
 * Description  : Contains data structures and functions used in source_cancel_lpm.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2023 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stddef.h>
#include "source_cancel_lpm.h"

/* Global variables */
volatile bool g_source_flag = false;
volatile bool g_external_irq_flag = false;
volatile bool g_timer_flag = false;

#if (BSP_FEATURE_LPM_HAS_SNOOZE)
volatile bool g_snooze_flag = false;
uint16_t g_dtc_dest_data = DTC_DATA_RESET;
uint16_t g_dtc_src_data = DTC_DATA_VALUE;
#endif /* BSP_FEATURE_LPM_HAS_SNOOZE */

/*******************************************************************************************************************//**
 * @brief     This function initializes the modules used to cancel LPM mode.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t source_cancel_lpm_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Initialize ICU External IRQ used to cancel LPM */
    err = R_ICU_ExternalIrqOpen(&g_external_irq_cancel_lpm_ctrl, &g_external_irq_cancel_lpm_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Initialize the Timer used to cancel LPM */
#if (BSP_PERIPHERAL_ULPT_PRESENT)
    err = R_ULPT_Open(&g_timer_cancel_lpm_ctrl, &g_timer_cancel_lpm_cfg);
#elif (BSP_PERIPHERAL_TML_PRESENT)
    err = R_TML_Open(&g_timer_cancel_lpm_ctrl, &g_timer_cancel_lpm_cfg);
#else
    err = R_AGT_Open(&g_timer_cancel_lpm_ctrl, &g_timer_cancel_lpm_cfg);
#endif /* Specific timer */
    if (FSP_SUCCESS != err)
    {
        return err;
    }

#if (BSP_FEATURE_LPM_HAS_SNOOZE)
    /* Initialize the Timer used to automatically trigger DTC transfers in case of Snooze mode */
#if (BSP_PERIPHERAL_TML_PRESENT)
    if (!(g_timer_cancel_lpm_ctrl.open))
    {
        err = R_TML_Open(&g_timer_cancel_lpm_ctrl, &g_timer_cancel_lpm_cfg);
        if (FSP_SUCCESS != err)
        {
            return err;
        }
    }
#else
    err = R_AGT_Open(&g_timer_trigger_dtc_ctrl, &g_timer_trigger_dtc_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
#endif /* BSP_PERIPHERAL_TML_PRESENT */

    /* Set source and destination addresses for DTC transfer information */
    g_dtc_cancel_snooze_info.p_src = &g_dtc_src_data;
    g_dtc_cancel_snooze_info.p_dest = &g_dtc_dest_data;

    /* Initialize DTC module used to cancel Snooze mode */
    err = R_DTC_Open(&g_dtc_cancel_snooze_ctrl, &g_dtc_cancel_snooze_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
#endif /* BSP_FEATURE_LPM_HAS_SNOOZE */

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function enables the modules to cancel LPM mode.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Enabled successfully.
 * @retval    Any other error codes     Failed to enable.
 **********************************************************************************************************************/
fsp_err_t source_cancel_lpm_enable(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Enable External IRQ channel interrupt */
    err = R_ICU_ExternalIrqEnable(&g_external_irq_cancel_lpm_ctrl);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Start the Timer */
#if (BSP_PERIPHERAL_ULPT_PRESENT)
    err = R_ULPT_Start(&g_timer_cancel_lpm_ctrl);
#elif (BSP_PERIPHERAL_TML_PRESENT)
    err = R_TML_Start(&g_timer_cancel_lpm_ctrl);
#else
    err = R_AGT_Start(&g_timer_cancel_lpm_ctrl);
#endif /* Specific timer */

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function disables the modules to cancel LPM mode.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Disabled successfully.
 * @retval    Any other error codes     Failed to disable.
 **********************************************************************************************************************/
fsp_err_t source_cancel_lpm_disable(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Disable External IRQ channel interrupt */
    err = R_ICU_ExternalIrqDisable(&g_external_irq_cancel_lpm_ctrl);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

#if (BSP_PERIPHERAL_ULPT_PRESENT)
    /* Stop the Timer */
    err = R_ULPT_Stop(&g_timer_cancel_lpm_ctrl);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Reset the Timer counter */
    err = R_ULPT_Reset(&g_timer_cancel_lpm_ctrl);
#elif (BSP_PERIPHERAL_TML_PRESENT)
    /* Stop the Timer  */
    err = R_TML_Stop(&g_timer_cancel_lpm_ctrl);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Reset the Timer counter */
    err = R_TML_Reset(&g_timer_cancel_lpm_ctrl);
#else
    /* Stop the Timer */
    err = R_AGT_Stop(&g_timer_cancel_lpm_ctrl);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Reset the Timer counter */
    err = R_AGT_Reset(&g_timer_cancel_lpm_ctrl);
#endif /* Specific timer */

    return err;
}

#if (BSP_FEATURE_LPM_HAS_SNOOZE)
/*******************************************************************************************************************//**
 * @brief     This function enables the modules to cancel the Snooze mode.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Enabled successfully.
 * @retval    Any other error codes     Failed to enable.
 **********************************************************************************************************************/
fsp_err_t source_cancel_snooze_enable(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Start the Timer */
#if (BSP_PERIPHERAL_TML_PRESENT)
    err = R_TML_Start(&g_timer_cancel_lpm_ctrl);
#else
    err = R_AGT_Start(&g_timer_trigger_dtc_ctrl);
#endif /* BSP_PERIPHERAL_TML_PRESENT */
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Enable DTC transfer */
    err = R_DTC_Enable(&g_dtc_cancel_snooze_ctrl);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Set flag to confirm Snooze mode and clean destination data */
    g_snooze_flag = true;
    g_dtc_dest_data = DTC_DATA_RESET;

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function disables the modules to cancel the Snooze mode.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Disabled successfully.
 * @retval    Any other error codes     Failed to disable.
 **********************************************************************************************************************/
fsp_err_t source_cancel_snooze_disable(void)
{
    fsp_err_t err = FSP_SUCCESS;

#if (BSP_PERIPHERAL_TML_PRESENT)
    /* Stop the Timer */
    err = R_TML_Stop(&g_timer_cancel_lpm_ctrl);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Reset the Timer */
    err = R_TML_Reset(&g_timer_cancel_lpm_ctrl);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
#else
    /* Stop the Timer */
    err = R_AGT_Stop(&g_timer_trigger_dtc_ctrl);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Reset the Timer */
    err = R_AGT_Reset(&g_timer_trigger_dtc_ctrl);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
#endif /* BSP_PERIPHERAL_TML_PRESENT */

    /* Disable DTC transfer */
    err = R_DTC_Disable(&g_dtc_cancel_snooze_ctrl);

    return err;
}
#endif /* BSP_FEATURE_LPM_HAS_SNOOZE */

/*******************************************************************************************************************//**
 * @brief     This function de-initializes the opened modules used to cancel LPM mode.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void source_cancel_lpm_deinit(void)
{
    /* Close the opened Timer used to cancel LPM */
    if (g_timer_cancel_lpm_ctrl.open)
    {
#if (BSP_PERIPHERAL_ULPT_PRESENT)
        R_ULPT_Close(&g_timer_cancel_lpm_ctrl);
#elif (BSP_PERIPHERAL_TML_PRESENT)
        R_TML_Close(&g_timer_cancel_lpm_ctrl);
#else
        R_AGT_Close(&g_timer_cancel_lpm_ctrl);
#endif /* Specific timer */
    }

    /* Close the opened ICU External IRQ module */
    if (g_external_irq_cancel_lpm_ctrl.open)
    {
        R_ICU_ExternalIrqClose(&g_external_irq_cancel_lpm_ctrl);
    }

#if (BSP_FEATURE_LPM_HAS_SNOOZE)
#if (BSP_PERIPHERAL_TML_PRESENT)
    if (g_timer_cancel_lpm_ctrl.open)
    {
        R_TML_Close(&g_timer_cancel_lpm_ctrl);
    }
#else
    /* Close the opened Timer used to automatically trigger DTC transfers in case of Snooze mode */
    if (g_timer_trigger_dtc_ctrl.open)
    {
        R_AGT_Close(&g_timer_trigger_dtc_ctrl);
    }
#endif /* BSP_PERIPHERAL_TML_PRESENT */

    /* Close the opened DTC module use to cancel Snooze mode */
    if (g_dtc_cancel_snooze_ctrl.open)
    {
        R_DTC_Close(&g_dtc_cancel_snooze_ctrl);
    }
#endif /* BSP_FEATURE_LPM_HAS_SNOOZE */
}

/*******************************************************************************************************************//**
 * @brief     This function clears all source detection flags.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void source_cancel_lpm_flag_clear(void)
{
    g_external_irq_flag = false;
    g_timer_flag = false;
#if (BSP_FEATURE_LPM_HAS_SNOOZE)
    g_snooze_flag = false;
#endif /* BSP_FEATURE_LPM_HAS_SNOOZE */
    g_source_flag = false;
}

#if (BSP_FEATURE_LPM_HAS_DEEP_STANDBY)
/*******************************************************************************************************************//**
 *  @brief      This function performs a procedure after exiting Deep SW Standby mode.
 *  @param[in]  None.
 *  @retval     None.
 **********************************************************************************************************************/
void source_internal_reset_handle(void)
{
    uint16_t external_irq_channel = RESET_VALUE;

    /* Get Deep Standby Interrupt flags from IRQ0 to IRQ15 */
    external_irq_channel = (uint16_t)((R_SYSTEM->DPSIFR1) << 8U);
    external_irq_channel |= (uint16_t)(R_SYSTEM->DPSIFR0);

    /* Check External IRQ flag */
    if (external_irq_channel & (1U << (g_external_irq_cancel_lpm_cfg.channel)))
    {
        g_timer_flag = false;
        g_external_irq_flag = true;
    }

    /* Check Deep Standby Interrupt flag for ULPT0 or AGT1 based on specific board */
#if defined(BOARD_RA8D1_EK) || defined(BOARD_RA8M1_EK) || defined(BOARD_RA8T1_MCK) || defined(BOARD_RA8E1_FPB) ||\
    defined(BOARD_RA8E2_EK) || defined(BOARD_RA8P1_EK) || defined(BOARD_RA8T2_MCK)
    else if (R_SYSTEM->DPSIFR3_b.DULPT0IF)
#else
    else if (R_SYSTEM->DPSIFR3_b.DAGT1IF)
#endif /* Specific board */
    {
        g_timer_flag = true;
        g_external_irq_flag = false;
    }

    /* The source is not detected */
    else
    {
        g_timer_flag = false;
        g_external_irq_flag = false;
    }

    /* Clear the IOKEEP bit after exiting Deep Software Standby mode */
    R_LPM_IoKeepClear(NULL);

#if (BSP_FEATURE_LPM_HAS_SNOOZE)
    /* Clear Snooze flag */
    g_snooze_flag = false;
#endif /* BSP_FEATURE_LPM_HAS_SNOOZE */

    /* Set source flag to notify internal reset event */
    g_source_flag = true;
}
#endif /* BSP_FEATURE_LPM_HAS_DEEP_STANDBY */

/*******************************************************************************************************************//**
 * @brief     This function is a callback for the timer used to cancel LPM.
 * @param[in] p_args                    Pointer to the callback argument.
 * @retval    None.
 **********************************************************************************************************************/
void timer_cancel_lpm_callback(timer_callback_args_t * p_args)
{
    /* Check null pointer */
    if (NULL == p_args)
    {
        return;
    }

    /* Check source flag state */
    if (true == g_source_flag)
    {
        return;
    }

#if (BSP_FEATURE_LPM_HAS_SNOOZE)
    /* In case of Snooze mode, timer is used to change between SW Standby mode and Snooze mode */
    if (true == g_snooze_flag)
    {
        return;
    }
#endif /* BSP_FEATURE_LPM_HAS_SNOOZE */

    /* Set detection flags in other LPM modes */
    g_source_flag = true;
    g_timer_flag = true;
}

/*******************************************************************************************************************//**
 * @brief     This function is a callback for External IRQ used to cancel LPM.
 * @param[in] p_args                    Pointer to the callback argument.
 * @retval    None.
 **********************************************************************************************************************/
void external_irq_cancel_lpm_callback(external_irq_callback_args_t * p_args)
{
    /* Check null pointer */
    if (NULL == p_args)
    {
        return;
    }

    /* Check External IRQ channel */
    if (g_external_irq_cancel_lpm_cfg.channel != p_args->channel)
    {
        return;
    }

    /* Check source flag state */
    if (true == g_source_flag)
    {
        return;
    }

    /* Set detection flags */
    g_source_flag = true;
    g_external_irq_flag = true;
}

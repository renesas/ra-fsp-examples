/***********************************************************************************************************************
 * File Name    : source_cancel_lpm.c
 * Description  : Contains macros, data structures and functions used  common to the EP
 ***********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "source_cancel_lpm.h"
#include "lpm_ep.h"

/**********************************************************************************************************************
 * Public global variables declarations
 **********************************************************************************************************************/
volatile bool g_source_flag = false;

/**********************************************************************************************************************
 * Private global variables declarations
 **********************************************************************************************************************/
static volatile bool g_external_irq_flag = false;
static volatile bool g_timer_flag        = false;

#if (BSP_FEATURE_LPM_HAS_SNOOZE)
extern transfer_info_t g_dtc_cancel_snooze_info;
static volatile bool   g_snooze_flag   = false;
static uint16_t        g_dtc_dest_data = DTC_DATA_RESET;
static uint16_t        g_dtc_src_data  = DTC_DATA_VALUE;
#endif

/**********************************************************************************************************************
 * Function implementations
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 *  @brief      Timer callback function
 *  @param[in]  p_args
 *  @retval     None
 **********************************************************************************************************************/
void timer_cancel_lpm_callback (timer_callback_args_t * p_args)
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

    /* In case of snooze mode, timer is used to change between SW standby mode and Snooze mode */
    if (true == g_snooze_flag)
    {
        return;
    }
#endif

    /* Set detection flags in other LPM modes */
    g_source_flag = true;
    g_timer_flag  = true;
}

/*******************************************************************************************************************//**
 *  @brief      External IRQ callback function
 *  @param[in]  p_args
 *  @retval     None
 **********************************************************************************************************************/
void external_irq_cancel_lpm_callback (external_irq_callback_args_t * p_args)
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
    g_source_flag       = true;
    g_external_irq_flag = true;
}

/*******************************************************************************************************************//**
 *  @brief      This function will initializes modules used to cancel LPM mode
 *  @param      None
 *  @retval     FSP_SUCCESS    Upon successful operation
 *  @retval     Any Other Error code apart from FSP_SUCCES
 **********************************************************************************************************************/
fsp_err_t source_cancel_lpm_init (void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Initialize ICU External IRQ used to cancel LPM */
    err = R_ICU_ExternalIrqOpen(&g_external_irq_cancel_lpm_ctrl, &g_external_irq_cancel_lpm_cfg);
    TERMINAL_ERR_RETURN(err, "R_ICU_ExternalIrqOpen API Failed\r\n");

    /* Initialize Timer module used to cancel LPM */
#if defined(BOARD_RA8D1_EK) || defined(BOARD_RA8M1_EK)
    err = R_ULPT_Open(&g_timer_cancel_lpm_ctrl, &g_timer_cancel_lpm_cfg);
    TERMINAL_ERR_RETURN(err, "R_ULPT_Open API Failed\r\n");
#else
    err = R_AGT_Open(&g_timer_cancel_lpm_ctrl, &g_timer_cancel_lpm_cfg);
    TERMINAL_ERR_RETURN(err, "R_AGT_Open API Failed\r\n");
#endif

#if (BSP_FEATURE_LPM_HAS_SNOOZE)

    /* Initialize Timer module used to automatically trigger DTC transfers in case of snooze mode */
    err = R_AGT_Open(&g_timer_trigger_dtc_ctrl, &g_timer_trigger_dtc_cfg);
    TERMINAL_ERR_RETURN(err, "R_AGT_Open API Failed\r\n");

    /* Set source and destination addresses for DTC transfer information */
    g_dtc_cancel_snooze_info.p_src  = &g_dtc_src_data;
    g_dtc_cancel_snooze_info.p_dest = &g_dtc_dest_data;

    /* Initialize DTC module used to cancel Snooze mode */
    err = R_DTC_Open(&g_dtc_cancel_snooze_ctrl, &g_dtc_cancel_snooze_cfg);
    TERMINAL_ERR_RETURN(err, "R_DTC_Open API Failed\r\n");
#endif

    return err;
}

/*******************************************************************************************************************//**
 *  @brief      This function will de-initializes modules used to cancel LPM mode
 *  @param      None
 *  @retval     FSP_SUCCESS    Upon successful operation
 *  @retval     Any Other Error code apart from FSP_SUCCES
 **********************************************************************************************************************/
void source_cancel_lpm_deinit (void)
{
    /* Close Timer module used to cancel LPM if it is already open */
    if (g_timer_cancel_lpm_ctrl.open)
    {
#if defined(BOARD_RA8D1_EK) || defined(BOARD_RA8M1_EK)
        R_ULPT_Close(&g_timer_cancel_lpm_ctrl);
#else
        R_AGT_Close(&g_timer_cancel_lpm_ctrl);
#endif
    }

    /* Close ICU External IRQ module used to cancel LPM if it is already open */
    if (g_external_irq_cancel_lpm_ctrl.open)
    {
        R_ICU_ExternalIrqClose(&g_external_irq_cancel_lpm_ctrl);
    }

#if (BSP_FEATURE_LPM_HAS_SNOOZE)

    /* Close Timer module used to automatically trigger DTC transfers in case of Snooze mode if it is already open */
    if (g_timer_trigger_dtc_ctrl.open)
    {
        R_AGT_Close(&g_timer_trigger_dtc_ctrl);
    }

    /* Close DTC module use to cancel Snooze mode if it is already open */
    if (g_dtc_cancel_snooze_ctrl.open)
    {
        R_DTC_Close(&g_dtc_cancel_snooze_ctrl);
    }
#endif
}

/*******************************************************************************************************************//**
 *  @brief      This function will enable modules to cancel LPM mode
 *  @param      None
 *  @retval     FSP_SUCCESS    Upon successful operation
 *  @retval     Any Other Error code apart from FSP_SUCCES
 **********************************************************************************************************************/
fsp_err_t source_cancel_lpm_enable (void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Enable External IRQ channel interrupt */
    err = R_ICU_ExternalIrqEnable(&g_external_irq_cancel_lpm_ctrl);
    TERMINAL_ERR_RETURN(err, "R_ICU_ExternalIrqEnable API Failed\r\n");

    /* Start Timer  */
#if defined(BOARD_RA8D1_EK) || defined(BOARD_RA8M1_EK)
    err = R_ULPT_Start(&g_timer_cancel_lpm_ctrl);
    TERMINAL_ERR_RETURN(err, "R_ULPT_Start API Failed\r\n");
#else
    err = R_AGT_Start(&g_timer_cancel_lpm_ctrl);
    TERMINAL_ERR_RETURN(err, "R_AGT_Start API Failed\r\n");
#endif

    return err;
}

/*******************************************************************************************************************//**
 *  @brief      This function will disable modules to cancel LPM mode
 *  @param      None
 *  @retval     FSP_SUCCESS    Upon successful operation
 *  @retval     Any Other Error code apart from FSP_SUCCES
 **********************************************************************************************************************/
fsp_err_t source_cancel_lpm_disable (void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Disable External IRQ channel interrupt */
    err = R_ICU_ExternalIrqDisable(&g_external_irq_cancel_lpm_ctrl);
    TERMINAL_ERR_RETURN(err, "R_ICU_ExternalIrqEnable API Failed\r\n");

#if defined(BOARD_RA8D1_EK) || defined(BOARD_RA8M1_EK)

    /* Stop Timer  */
    err = R_ULPT_Stop(&g_timer_cancel_lpm_ctrl);
    TERMINAL_ERR_RETURN(err, "R_ULPT_Stop API Failed\r\n");

    /* Reset Timer counter */
    err = R_ULPT_Reset(&g_timer_cancel_lpm_ctrl);
    TERMINAL_ERR_RETURN(err, "R_ULPT_Reset API Failed\r\n");
#else

    /* Stop Timer */
    err = R_AGT_Stop(&g_timer_cancel_lpm_ctrl);
    TERMINAL_ERR_RETURN(err, "R_AGT_Stop API Failed\r\n");

    /* Reset Timer counter */
    err = R_AGT_Reset(&g_timer_cancel_lpm_ctrl);
    TERMINAL_ERR_RETURN(err, "R_AGT_Reset API Failed\r\n");
#endif

    return err;
}

#if (BSP_FEATURE_LPM_HAS_SNOOZE)
fsp_err_t source_cancel_snooze_enable (void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Start Timer module */
    err = R_AGT_Start(&g_timer_trigger_dtc_ctrl);
    TERMINAL_ERR_RETURN(err, "R_AGT_Start API Failed\r\n");

    /* Enable DTC transfer */
    err = R_DTC_Enable(&g_dtc_cancel_snooze_ctrl);
    TERMINAL_ERR_RETURN(err, "R_DTC_Enable API Failed\r\n");

    /* Set flag to confirm snooze mode and clean destination data */
    g_snooze_flag   = true;
    g_dtc_dest_data = DTC_DATA_RESET;

    return err;
}

fsp_err_t source_cancel_snooze_disable (void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Stop Timer module */
    err = R_AGT_Stop(&g_timer_trigger_dtc_ctrl);
    TERMINAL_ERR_RETURN(err, "R_AGT_Stop API Failed\r\n");

    /* Reset Timer counter */
    err = R_AGT_Reset(&g_timer_trigger_dtc_ctrl);
    TERMINAL_ERR_RETURN(err, "R_AGT_Reset API Failed\r\n");

    /* Disable DTC transfer */
    err = R_DTC_Disable(&g_dtc_cancel_snooze_ctrl);
    TERMINAL_ERR_RETURN(err, "R_DTC_Disable API Failed\r\n");

    return err;
}

#endif

/*******************************************************************************************************************//**
 *  @brief      This function will detect which module has canceled LPM mode
 *  @param      None
 *  @retval     None
 **********************************************************************************************************************/
void source_cancel_lpm_detect (void)
{
    if (true == g_timer_flag)
    {
        TERMINAL_PRINT(NAME_LPM_CANCEL_SOURCE_TIMER);
    }
    else if (true == g_external_irq_flag)
    {
        TERMINAL_PRINT(NAME_LPM_CANCEL_SOURCE_IRQ);
    }

#if (BSP_FEATURE_LPM_HAS_SNOOZE)
    else if ((true == g_snooze_flag) && (g_dtc_src_data == g_dtc_dest_data))
    {
        TERMINAL_PRINT(NAME_LPM_CANCEL_SOURCE_DTC);
    }
#endif

    else
    {
        TERMINAL_PRINT(NAME_LPM_CANCEL_SOURCE_NOT_DETECTED);
    }
}

/*******************************************************************************************************************//**
 *  @brief      This function will clears all source detection flags
 *  @param      None
 *  @retval     None
 **********************************************************************************************************************/
void source_cancel_lpm_flag_clear (void)
{
    g_external_irq_flag = false;
    g_timer_flag        = false;
#if (BSP_FEATURE_LPM_HAS_SNOOZE)
    g_snooze_flag = false;
#endif
    g_source_flag = false;
}

#if (BSP_FEATURE_LPM_HAS_DEEP_STANDBY)

/*******************************************************************************************************************//**
 *  @brief      This function will perform a procedure after Deep SW Standby
 *  @param      None
 *  @retval     None
 **********************************************************************************************************************/
void source_internal_reset_handle (void)
{
    uint16_t external_irq_channel = RESET_VALUE;

    /* Get Deep Standby Interrupt flags from IRQ0 to IRQ15 */
    external_irq_channel  = (uint16_t) (R_SYSTEM->DPSIFR1) << 8U;
    external_irq_channel |= (uint16_t) (R_SYSTEM->DPSIFR0);

    /* Check External IRQ Flag */
    if (external_irq_channel & (1U << (g_external_irq_cancel_lpm_cfg.channel)))
    {
        g_timer_flag        = false;
        g_external_irq_flag = true;
    }

    /* Check Deep Standby Interrupt flag for ULPT0 or AGT1 depending on board type */
 #if defined(BOARD_RA8D1_EK) || defined(BOARD_RA8M1_EK)
    else if (R_SYSTEM->DPSIFR3_b.DULPT0IF)
 #else
    else if (R_SYSTEM->DPSIFR3_b.DAGT1IF)
 #endif
    {
        g_timer_flag        = true;
        g_external_irq_flag = false;
    }

    /* Source not detected */
    else
    {
        g_timer_flag        = false;
        g_external_irq_flag = false;
    }

    /* Clear the IOKEEP bit after deep software standby */
    R_LPM_IoKeepClear(NULL);

 #if (BSP_FEATURE_LPM_HAS_SNOOZE)

    /* Clear snooze flag */
    g_snooze_flag = false;
 #endif

    /* Set source flag to notify internal reset event */
    g_source_flag = true;
}

#endif

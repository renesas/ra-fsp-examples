/***********************************************************************************************************************
 * File Name    : ext_irq_setup.c
 * Description  : Contains data structures and functions used in ext_irq_setup.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "ext_irq_setup.h"
#include "timer_setup.h"
#include "wdt_ep.h"

/* Variable to capture IRQ callback */
volatile uint8_t g_user_button = RESET_FLAG;

/*******************************************************************************************************************//**
 * @brief     This function initializes the ICU module.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t init_icu_module(void)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Open ICU module */
    err = R_ICU_ExternalIrqOpen(&g_external_irq_ctrl, &g_external_irq_cfg);
    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function enables the user push button.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Enabled successfully.
 * @retval    Any other error codes     Failed to enable.
 **********************************************************************************************************************/
fsp_err_t enable_icu_module(void)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Enable external IRQ */
    err = R_ICU_ExternalIrqEnable(&g_external_irq_ctrl);
    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function closes the opened ICU driver.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void deinit_icu_module(void)
{
    /* Close the opened ICU module */
    if (g_external_irq_ctrl.open)
    {
        R_ICU_ExternalIrqClose(&g_external_irq_ctrl);
    }
}

/*******************************************************************************************************************//**
 * @brief     The callback function stops the GPT timer when pressing the push button.
 * @param[in] p_args    External IRQ callback argument.
 * @retval    None.
 **********************************************************************************************************************/
void ext_irq_callback(external_irq_callback_args_t *p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);

    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Stop GPT module */
    err = timer_stop();
    if (FSP_SUCCESS != err)
    {
        return;
    }

    /* Set g_user_button flag */
    g_user_button = SET_FLAG;
}

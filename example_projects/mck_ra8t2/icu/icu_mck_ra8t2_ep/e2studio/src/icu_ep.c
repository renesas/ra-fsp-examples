/***********************************************************************************************************************
 * File Name    : icu_ep.c
 * Description  : Contains data structures and functions used in icu_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "icu_ep.h"

/* Boolean flag to determine switch is pressed or not */
volatile bool g_sw_press = false;

/*******************************************************************************************************************//**
 * @brief     This function initializes ICU module.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t icu_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Open ICU module */
    err = R_ICU_ExternalIrqOpen(&g_external_irq_ctrl, &g_external_irq_cfg);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function enables external interrupt for specified channel.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Enabled successfully.
 * @retval    Any other error codes     Failed to enable.
 **********************************************************************************************************************/
fsp_err_t icu_enable(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Enable ICU module */
    err = R_ICU_ExternalIrqEnable(&g_external_irq_ctrl);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function closes the opened ICU module.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void icu_deinit(void)
{
    /* Close the opened ICU module */
    if (g_external_irq_ctrl.open)
    {
        R_ICU_ExternalIrqClose(&g_external_irq_ctrl);
    }
}

/*******************************************************************************************************************//**
 * @brief     User defined external IRQ callback.
 * @param[in] p_args                    Pointer to the callback argument.
 * @retval    None.
 **********************************************************************************************************************/
void irq_ep_callback(external_irq_callback_args_t *p_args)
{
    /* Make sure it's the right interrupt */
    if (USER_SW_IRQ_NUMBER == p_args->channel)
    {
        g_sw_press = true;
    }
}

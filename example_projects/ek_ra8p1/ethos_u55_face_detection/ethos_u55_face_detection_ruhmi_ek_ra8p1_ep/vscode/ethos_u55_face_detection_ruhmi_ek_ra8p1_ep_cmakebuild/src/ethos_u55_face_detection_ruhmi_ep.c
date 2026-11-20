/***********************************************************************************************************************
 * File Name    : ethos_u55_face_detection_ruhmi_ep.c
 * Description  : Contains data structures and functions used in ethos_u55_face_detection_ruhmi_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2025 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "ethos_u55_face_detection_ruhmi_ep.h"

#ifdef ARM_NPU
/*******************************************************************************************************************//**
 * @brief     This function initializes the Ethos-U NPU module.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Opened successfully.
 * @retval    Any other error codes     Failed to open.
 **********************************************************************************************************************/
fsp_err_t ethos_u55_ruhmi_npu_open(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = RM_ETHOSU_Open(&g_rm_ethosu0_ctrl, &g_rm_ethosu0_cfg);

    return err;
}
#endif /* ARM_NPU */

#if (INFERENCE_TIME_MEASUREMENT_ENABLED == 1)
/*******************************************************************************************************************//**
 * @brief     This function initializes the GPT module.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Opened successfully.
 * @retval    Any other error codes     Failed to open.
 **********************************************************************************************************************/
fsp_err_t ethos_u55_ruhmi_timer_open(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_GPT_Open(&g_timer_ctrl, &g_timer_cfg);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function resets and starts the GPT module.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Reset and started successfully.
 * @retval    Any other error codes     Failed to reset or start.
 **********************************************************************************************************************/
fsp_err_t ethos_u55_ruhmi_timer_start(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Clear timer counter */
    err = R_GPT_Reset(&g_timer_ctrl);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Start timer */
    err = R_GPT_Start(&g_timer_ctrl);

    return err;
}

/*******************************************************************************************************************//**
 * @brief      This function gets the elapsed time by reading the timer counter.
 * @param[out] p_time                   Pointer to store the measured time in microseconds.
 * @retval     FSP_SUCCESS              Got successfully.
 * @retval     Any other error codes    Failed to get.
 **********************************************************************************************************************/
fsp_err_t ethos_u55_ruhmi_timer_get_measure(uint32_t * p_time)
{
    fsp_err_t err = FSP_SUCCESS;
    timer_status_t timer_status = {RESET_VALUE, RESET_VALUE};
    timer_info_t timer_info = {RESET_VALUE, RESET_VALUE, RESET_VALUE};

    /* Get status of timer */
    err = R_GPT_StatusGet(&g_timer_ctrl, &timer_status);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Get info of timer */
    err = R_GPT_InfoGet(&g_timer_ctrl, &timer_info);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Convert count value to microseconds */
    *p_time = (uint32_t) (((uint64_t) timer_status.counter * 1000000U) / timer_info.clock_frequency);

    return err;
}
#endif /* INFERENCE_TIME_MEASUREMENT_ENABLED */

/*******************************************************************************************************************//**
 * @brief     This function closes all the opened modules.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void ethos_u55_ruhmi_deinit(void)
{
#ifdef ARM_NPU
    /* Close the opened ETHOSU module */
    if (g_rm_ethosu0_ctrl.open)
    {
        RM_ETHOSU_Close(&g_rm_ethosu0_ctrl);
    }
#endif /* ARM_NPU */

#if (INFERENCE_TIME_MEASUREMENT_ENABLED == 1)
    /* Close the opened GPT module */
    if (g_timer_ctrl.open)
    {
        R_GPT_Close(&g_timer_ctrl);
    }
#endif /* INFERENCE_TIME_MEASUREMENT_ENABLED */
}

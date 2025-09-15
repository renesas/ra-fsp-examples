/***********************************************************************************************************************
 * File Name    : g_timer_periodic_guard.c
 * Description  : Contain functions used in the EP.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "guard.h"

BSP_CMSE_NONSECURE_ENTRY fsp_err_t g_timer_periodic_close_guard(timer_ctrl_t *const p_ctrl)
{
    /* TODO: add your own security checks here */

    FSP_PARAMETER_NOT_USED(p_ctrl);

    return R_AGT_Close(&g_timer_periodic_ctrl);
}

BSP_CMSE_NONSECURE_ENTRY fsp_err_t g_timer_periodic_period_set_guard(timer_ctrl_t *const p_ctrl, uint32_t const period_counts)
{
    /* TODO: add your own security checks here */

    FSP_PARAMETER_NOT_USED(p_ctrl);

    return R_AGT_PeriodSet(&g_timer_periodic_ctrl, period_counts);
}

BSP_CMSE_NONSECURE_ENTRY fsp_err_t g_timer_periodic_duty_cycle_set_guard(timer_ctrl_t *const p_ctrl,\
	uint32_t const duty_cycle_counts, uint32_t const pin)
{
    /* TODO: add your own security checks here */

    FSP_PARAMETER_NOT_USED(p_ctrl);

    return R_AGT_DutyCycleSet(&g_timer_periodic_ctrl, duty_cycle_counts, pin);
}

BSP_CMSE_NONSECURE_ENTRY fsp_err_t g_timer_periodic_reset_guard(timer_ctrl_t *const p_ctrl)
{
    /* TODO: add your own security checks here */

    FSP_PARAMETER_NOT_USED(p_ctrl);

    return R_AGT_Reset(&g_timer_periodic_ctrl);
}

BSP_CMSE_NONSECURE_ENTRY fsp_err_t g_timer_periodic_start_guard(timer_ctrl_t *const p_ctrl)
{
    /* TODO: add your own security checks here */

    FSP_PARAMETER_NOT_USED(p_ctrl);

    return R_AGT_Start(&g_timer_periodic_ctrl);
}

BSP_CMSE_NONSECURE_ENTRY fsp_err_t g_timer_periodic_enable_guard(timer_ctrl_t *const p_ctrl)
{
    /* TODO: add your own security checks here */

    FSP_PARAMETER_NOT_USED(p_ctrl);

    return R_AGT_Enable(&g_timer_periodic_ctrl);
}

BSP_CMSE_NONSECURE_ENTRY fsp_err_t g_timer_periodic_disable_guard(timer_ctrl_t *const p_ctrl)
{
    /* TODO: add your own security checks here */

    FSP_PARAMETER_NOT_USED(p_ctrl);

    return R_AGT_Disable(&g_timer_periodic_ctrl);
}

BSP_CMSE_NONSECURE_ENTRY fsp_err_t g_timer_periodic_info_get_guard(timer_ctrl_t *const p_ctrl,\
	timer_info_t *const p_info)
{
    /* Verify all pointers are in non-secure memory. */
    timer_info_t *const p_info_checked = cmse_check_pointed_object(p_info, CMSE_AU_NONSECURE);
    FSP_ASSERT(p_info_checked != NULL);

    /* TODO: add your own security checks here */

    FSP_PARAMETER_NOT_USED(p_ctrl);

    return R_AGT_InfoGet(&g_timer_periodic_ctrl, p_info_checked);
}

BSP_CMSE_NONSECURE_ENTRY fsp_err_t g_timer_periodic_status_get_guard(timer_ctrl_t *const p_ctrl,\
	timer_status_t *const p_status)
{
    /* Verify all pointers are in non-secure memory. */
    timer_status_t *const p_status_checked = cmse_check_pointed_object(p_status, CMSE_AU_NONSECURE);
    FSP_ASSERT(p_status_checked != NULL);

    /* TODO: add your own security checks here */

    FSP_PARAMETER_NOT_USED(p_ctrl);

    return R_AGT_StatusGet(&g_timer_periodic_ctrl, p_status_checked);
}

BSP_CMSE_NONSECURE_ENTRY fsp_err_t g_timer_periodic_stop_guard(timer_ctrl_t *const p_ctrl)
{
    /* TODO: add your own security checks here */

    FSP_PARAMETER_NOT_USED(p_ctrl);

    return R_AGT_Stop(&g_timer_periodic_ctrl);
}

BSP_CMSE_NONSECURE_ENTRY fsp_err_t g_timer_periodic_open_guard(timer_ctrl_t *const p_ctrl,\
	timer_cfg_t const *const p_cfg)
{
    /* TODO: add your own security checks here */

    FSP_PARAMETER_NOT_USED(p_ctrl);
    FSP_PARAMETER_NOT_USED(p_cfg);

    return R_AGT_Open(&g_timer_periodic_ctrl, &g_timer_periodic_cfg);
}

BSP_CMSE_NONSECURE_ENTRY fsp_err_t g_timer_periodic_callback_set_guard(timer_ctrl_t *const p_api_ctrl,\
	void(*p_callback)(timer_callback_args_t *), void *const p_context, timer_callback_args_t *const p_callback_memory)
{
    /* Verify all pointers are in non-secure memory. */
    void(*p_callback_checked)(timer_callback_args_t *) = (void(*)(timer_callback_args_t *)) \
    	 cmse_check_address_range((void *) p_callback, sizeof(void *), CMSE_AU_NONSECURE);
    FSP_ASSERT(p_callback_checked != NULL);
    timer_callback_args_t *const p_callback_memory_checked = cmse_check_pointed_object(p_callback_memory,\
    	 CMSE_AU_NONSECURE);
    FSP_ASSERT(p_callback_memory_checked != NULL);

    /* TODO: add your own security checks here */

    FSP_PARAMETER_NOT_USED(p_api_ctrl);
    FSP_PARAMETER_NOT_USED(p_context);

    return R_AGT_CallbackSet(&g_timer_periodic_ctrl, p_callback_checked, p_context, p_callback_memory_checked);
}

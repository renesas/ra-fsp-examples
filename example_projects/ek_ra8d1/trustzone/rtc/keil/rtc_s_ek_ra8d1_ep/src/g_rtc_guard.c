/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "guard.h"


BSP_CMSE_NONSECURE_ENTRY fsp_err_t g_rtc_open_guard (rtc_ctrl_t *const p_ctrl, rtc_cfg_t const *const p_cfg)
{
    /* TODO: add your own security checks here */

    FSP_PARAMETER_NOT_USED(p_ctrl);
    FSP_PARAMETER_NOT_USED(p_cfg);

    return R_RTC_Open(&g_rtc_ctrl, &g_rtc_cfg);
}

BSP_CMSE_NONSECURE_ENTRY fsp_err_t g_rtc_close_guard (rtc_ctrl_t *const p_ctrl)
{
    /* TODO: add your own security checks here */

    FSP_PARAMETER_NOT_USED(p_ctrl);

    return R_RTC_Close(&g_rtc_ctrl);
}

BSP_CMSE_NONSECURE_ENTRY fsp_err_t g_rtc_clock_source_set_guard (rtc_ctrl_t * const p_ctrl)
{
    /* TODO: add your own security checks here */

    FSP_PARAMETER_NOT_USED(p_ctrl);

    return R_RTC_ClockSourceSet(&g_rtc_ctrl);
}

BSP_CMSE_NONSECURE_ENTRY fsp_err_t g_rtc_calendar_time_set_guard (rtc_ctrl_t *const p_ctrl, rtc_time_t *const p_time)
{
    /* Verify all pointers are in non-secure memory. */
    rtc_time_t *const p_time_checked = cmse_check_pointed_object(p_time, CMSE_AU_NONSECURE);
    FSP_ASSERT(p_time_checked != NULL);

    /* TODO: add your own security checks here */

    FSP_PARAMETER_NOT_USED(p_ctrl);

    return R_RTC_CalendarTimeSet(&g_rtc_ctrl, p_time_checked);
}

BSP_CMSE_NONSECURE_ENTRY fsp_err_t g_rtc_calendar_time_get_guard (rtc_ctrl_t *const p_ctrl, rtc_time_t *const p_time)
{
    /* Verify all pointers are in non-secure memory. */
    rtc_time_t *const p_time_checked = cmse_check_pointed_object(p_time, CMSE_AU_NONSECURE);
    FSP_ASSERT(p_time_checked != NULL);

    /* TODO: add your own security checks here */

    FSP_PARAMETER_NOT_USED(p_ctrl);

    return R_RTC_CalendarTimeGet(&g_rtc_ctrl, p_time_checked);
}

BSP_CMSE_NONSECURE_ENTRY fsp_err_t g_rtc_calendar_alarm_set_guard (rtc_ctrl_t *const p_ctrl, rtc_alarm_time_t *const p_alarm)
{
    /* Verify all pointers are in non-secure memory. */
    rtc_alarm_time_t *const p_alarm_checked = cmse_check_pointed_object(p_alarm, CMSE_AU_NONSECURE);
    FSP_ASSERT(p_alarm_checked != NULL);

    /* TODO: add your own security checks here */

    FSP_PARAMETER_NOT_USED(p_ctrl);

    return R_RTC_CalendarAlarmSet(&g_rtc_ctrl, p_alarm_checked);
}

BSP_CMSE_NONSECURE_ENTRY fsp_err_t g_rtc_calendar_alarm_get_guard (rtc_ctrl_t *const p_ctrl, rtc_alarm_time_t *const p_alarm)
{
    /* Verify all pointers are in non-secure memory. */
    rtc_alarm_time_t *const p_alarm_checked = cmse_check_pointed_object(p_alarm, CMSE_AU_NONSECURE);
    FSP_ASSERT(p_alarm_checked != NULL);

    /* TODO: add your own security checks here */

    FSP_PARAMETER_NOT_USED(p_ctrl);

    return R_RTC_CalendarAlarmGet(&g_rtc_ctrl, p_alarm_checked);
}

BSP_CMSE_NONSECURE_ENTRY fsp_err_t g_rtc_periodic_irq_rate_set_guard (rtc_ctrl_t *const p_ctrl, rtc_periodic_irq_select_t const rate)
{
    /* TODO: add your own security checks here */

    FSP_PARAMETER_NOT_USED(p_ctrl);

    return R_RTC_PeriodicIrqRateSet(&g_rtc_ctrl, rate);
}

BSP_CMSE_NONSECURE_ENTRY fsp_err_t g_rtc_error_adjustment_set_guard (rtc_ctrl_t *const p_ctrl, rtc_error_adjustment_cfg_t const *const err_adj_cfg)
{
    /* Verify all pointers are in non-secure memory. */
    rtc_error_adjustment_cfg_t const *const err_adj_cfg_checked = cmse_check_pointed_object((rtc_error_adjustment_cfg_t *) err_adj_cfg, CMSE_AU_NONSECURE);
    FSP_ASSERT(err_adj_cfg_checked != NULL);

    /* TODO: add your own security checks here */

    FSP_PARAMETER_NOT_USED(p_ctrl);

    return R_RTC_ErrorAdjustmentSet(&g_rtc_ctrl, err_adj_cfg_checked);
}

BSP_CMSE_NONSECURE_ENTRY fsp_err_t g_rtc_info_get_guard (rtc_ctrl_t *const p_ctrl, rtc_info_t *const p_rtc_info)
{
    /* Verify all pointers are in non-secure memory. */
    rtc_info_t *const p_rtc_info_checked = cmse_check_pointed_object(p_rtc_info, CMSE_AU_NONSECURE);
    FSP_ASSERT(p_rtc_info_checked != NULL);

    /* TODO: add your own security checks here */

    FSP_PARAMETER_NOT_USED(p_ctrl);

    return R_RTC_InfoGet(&g_rtc_ctrl, p_rtc_info_checked);
}

BSP_CMSE_NONSECURE_ENTRY fsp_err_t g_rtc_callback_set_guard (rtc_ctrl_t *const p_ctrl, void(*p_callback)(rtc_callback_args_t *), void *const p_context, rtc_callback_args_t *const p_callback_memory)
{
    /* Verify all pointers are in non-secure memory. */
    void(*p_callback_checked)(rtc_callback_args_t *) = (void(*)(rtc_callback_args_t *)) cmse_check_address_range((void *) p_callback, sizeof(void *), CMSE_AU_NONSECURE);
    FSP_ASSERT(p_callback_checked != NULL);
    rtc_callback_args_t *const p_callback_memory_checked = cmse_check_pointed_object(p_callback_memory, CMSE_AU_NONSECURE);
    FSP_ASSERT(p_callback_memory_checked != NULL);

    /* TODO: add your own security checks here */

    FSP_PARAMETER_NOT_USED(p_ctrl);
    FSP_PARAMETER_NOT_USED(p_context);

    return R_RTC_CallbackSet(&g_rtc_ctrl, p_callback_checked, p_context, p_callback_memory_checked);
}

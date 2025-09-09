/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/***********************************************************************************************************************
 * File Name    : dwt.c
 * Description  : Contains function implementations for using the DWT.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "hal_data.h"
#include "time_counter.h"

// ############### USER SETTING ###############
#define TIMER_TYPE (1) // 0: CPU DWT, 1: Peripheral timer
// ############ END OF USER SETTING ###########

#if (TIMER_TYPE == 1)
static volatile uint32_t timer_count = 0;
#endif

/*********************************************************************************************************************
 *  Enable the time counter
 *  @param[IN]   None
 *  @retval      None
***********************************************************************************************************************/
void TimeCounter_Init(void)
{
#if (TIMER_TYPE == 0)
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk |CoreDebug_DEMCR_MON_EN_Msk;
    DWT->CTRL |= (DWT_CTRL_CYCCNTENA_Msk << DWT_CTRL_CYCCNTENA_Pos);
#elif (TIMER_TYPE == 1)
    g_time_counter.p_api->open(g_time_counter.p_ctrl, g_time_counter.p_cfg);
    g_time_counter.p_api->start(g_time_counter.p_ctrl);
    timer_count = 0;
#endif
}

/*********************************************************************************************************************
 *  @brief       Disable the counte timer
 *  @param[IN]   None
 *  @retval      None
***********************************************************************************************************************/
void TimeCounter_Disable(void)
{
#if (TIMER_TYPE == 0)
    CoreDebug->DEMCR &= ~(CoreDebug_DEMCR_TRCENA_Msk |CoreDebug_DEMCR_MON_EN_Msk);
#elif (TIMER_TYPE == 1)
    g_time_counter.p_api->stop(g_time_counter.p_ctrl);
    g_time_counter.p_api->close(g_time_counter.p_ctrl);
#endif
}

/*********************************************************************************************************************
 *  Reset the time counter
 *  @param[IN]   None
 *  @retval      None
***********************************************************************************************************************/
void TimeCounter_CountReset(void)
{
#if (TIMER_TYPE == 0)
    DWT->CYCCNT = 0;
#elif (TIMER_TYPE == 1)
    g_time_counter.p_api->stop(g_time_counter.p_ctrl);

    timer_count = 0;

    g_time_counter.p_api->reset(g_time_counter.p_ctrl);
    g_time_counter.p_api->start(g_time_counter.p_ctrl);
#endif
}

/*********************************************************************************************************************
 *  @brief       Get current cycle count
 *  @param[IN]   None
 *  @retval      None
***********************************************************************************************************************/
uint32_t TimeCounter_CurrentCountGet(void)
{
#if (TIMER_TYPE == 0)
    return DWT->CYCCNT;
#elif (TIMER_TYPE == 1)
    return timer_count;
#endif
}

/*********************************************************************************************************************
 *  @brief       Convert count value to ms
 *  @param[IN]   None
 *  @retval      Count value in milliseconds
***********************************************************************************************************************/
uint32_t TimeCounter_CountValueConvertToMs(uint32_t t1, uint32_t t2)
{
#if (TIMER_TYPE == 0)
#if BSP_FEATURE_CGC_HAS_CPUCLK
    return (t2 - t1) * 1000 / R_FSP_SystemClockHzGet(FSP_PRIV_CLOCK_CPUCLK);
#else
    return (t2 - t1) * 1000 / R_FSP_SystemClockHzGet(FSP_PRIV_CLOCK_ICLK);
#endif
#elif (TIMER_TYPE == 1)
    return (t2 - t1) / 10;
#endif
}

/*********************************************************************************************************************
 *  @brief       Convert count value in ms to fps
 *  @param[IN]   None
 *  @retval      Count value in fps
***********************************************************************************************************************/
uint32_t TimeCounter_ConvertFromMsToFps(uint32_t ms)
{
    return (uint32_t)(1000 / ms);
}

#if (TIMER_TYPE == 1)
void time_counter_callback(timer_callback_args_t *p_args)
{
    if(TIMER_EVENT_CYCLE_END == p_args->event)
    {
        timer_count++;
    }
}
#endif

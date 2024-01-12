/*
 * timer.c
 *
 *  Created on: Jan 12, 2023
 *      Author: a5139312
 */
#include "hal_data.h"
#include "timer.h"

#define MICROSECONDS_TO_SECONDS    1000000

static uint64_t timer_overflow_times;

static inline void set_timer_overflow_times (uint64_t value) {
    timer_overflow_times = value;
}

static inline uint64_t get_timer_overflow_times () {
    return timer_overflow_times;
}

static inline uint64_t calculate_cycle_to_microsecond_ratio () {
    timer_info_t info;
    (void) R_GPT_InfoGet(&g_timer_ctrl, &info);

    return info.clock_frequency / MICROSECONDS_TO_SECONDS;
}

static inline uint64_t get_timer_count () {
    timer_status_t status;
    R_GPT_StatusGet(&g_timer_ctrl, &status);

    return status.counter;
}

void timer_init () {
    fsp_err_t err = FSP_SUCCESS;
    err = R_GPT_Open(&g_timer_ctrl, &g_timer_cfg);
    assert(FSP_SUCCESS == err);
    (void) R_GPT_Start(&g_timer_ctrl);
    set_timer_overflow_times(0);
}

void g_timer_overflow_callback (timer_callback_args_t * p_args)
{
    if (TIMER_EVENT_CYCLE_END == p_args->event)
    {
        set_timer_overflow_times(get_timer_overflow_times() + 1);
    }
}

uint64_t get_timestamp () {
    uint64_t div           = calculate_cycle_to_microsecond_ratio();
    uint64_t overflow_time = ((uint64_t) 1 << 32) / div;

    return (get_timer_overflow_times() * overflow_time) +
           (get_timer_count() / div);
}

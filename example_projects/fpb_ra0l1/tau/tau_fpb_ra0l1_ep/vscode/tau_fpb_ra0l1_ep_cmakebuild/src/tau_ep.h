/***********************************************************************************************************************
 * File Name    : tau_ep.h
 * Description  : Contains macros, variables, data structures, and functions used in tau_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef TAU_EP_H_
#define TAU_EP_H_

#include <stdbool.h>
#include "hal_data.h"

/* Macro for the reset value */
#define RESET_VALUE                     (0x00)

/* Macro for the null char */
#define NULL_CHAR                       ('\0')

/* Macro for the buffer size */
#define BUFFER_SIZE                     (16U)

/* Macros for LED */
#define LED_SEQUENCE1                   (0U)
#define LED_SEQUENCE2                   (1U)

/* Macro for maximum capture value supported by the TAU overflow handling */
#define CAPTURE_VALUE_MAX               (65536 * 2)

/* Macros for the minimum value of user input */
#define TIME_PERIOD_MIN                 (1U)
#define SQUARE_WAVE_FREQUENCY_MIN       (1U)
#define DIVIDER_MIN                     (1U)
#define SQUARE_WAVE_PERIOD_MIN          (1U)

/* Macro for the demonstration times */
#define DEMO_TIMES                      (5U)

/* Macros for the conversion */
#define CONVERT_SECOND_TO_MILLISECOND   (1000U)
#define CONVERT_HALF_TO_ONE_CYCLE       (2U)

/* Macros for the period count limits */
#define MAX_PERIOD                      (65536U)
#define MIN_PERIOD                      (1U)

/* Extern variables - Defined in tau_ep.c */
extern volatile bool g_led1_status_flag_cur;
extern volatile bool g_input_capture_detect_flag;
extern volatile bool g_external_counter_flag;
extern volatile bool g_measure_pulse_width_flag;
extern volatile bool g_delay_timer_end_flag;
extern volatile uint32_t g_capture_value;
extern volatile uint32_t g_interrupt_counter;
extern timer_info_t g_periodic_info;

/* Extern variable - Used for LED */
extern bsp_leds_t g_bsp_leds;

/* Enumeration of TAU operation modes */
typedef enum e_tau_mode
{
    INTERVAL_TIMER = 0,
    DELAY_COUNTER,
    SQUARE_WAVE_SEQUENCE2,
    DIVIDER,
    INPUT_CAPTURE,
    EXTERNAL_EVENT_COUNT,
    SQUARE_WAVE_SEQUENCE3,
    LOW_LEVEL_MEASUREMENT,
    TAU_MAX_MODE
} tau_mode_t;

/* Function declarations */
fsp_err_t tau_init(timer_ctrl_t * const p_ctrl, timer_cfg_t const * const p_cfg);
fsp_err_t tau_start(timer_ctrl_t * const p_ctrl);
fsp_err_t tau_enable(timer_ctrl_t * const p_ctrl);
fsp_err_t tau_info_get(timer_ctrl_t * const p_ctrl, timer_info_t * const p_info);
fsp_err_t tau_reset(timer_ctrl_t * const p_ctrl);
fsp_err_t tau_period_set(timer_ctrl_t * const p_ctrl, uint32_t const period_counts);
void tau_close(timer_ctrl_t * const p_ctrl);
void tau_deinit(void);

#endif /* TAU_EP_H_ */

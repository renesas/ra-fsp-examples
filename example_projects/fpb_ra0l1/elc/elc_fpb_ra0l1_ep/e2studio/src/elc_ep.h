/***********************************************************************************************************************
 * File Name    : elc_ep.h
 * Description  : Contains declarations of data structures and functions used in elc_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2024 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef ELC_EP_H_
#define ELC_EP_H_

#include <stdio.h>
#include <stdbool.h>
#include "hal_data.h"

/* Macros for used function based on specific timer version
 *   - TAU timer supports Delay Counter and One-Shot Pulse Output modes
 *   - GPT timer supports One-Shot and Saw-wave PWM modes */
#if (BSP_PERIPHERAL_TAU_PRESENT)
#define OS_TIMER_CTRL                   (g_timer1_ctrl)
#define OS_TIMER_CFG                    (g_timer1_cfg)
#define PWM_TIMER_CTRL                  (g_timer0_ctrl)
#define PWM_TIMER_CFG                   (g_timer0_cfg)
#define OS_TIMER_INIT(ctrl, cfg)        (R_TAU_Open((ctrl), (cfg)))
#define OS_TIMER_DEINIT(ctrl)           (R_TAU_Close(ctrl))
#define OS_TIMER_ENABLE(ctrl)           (R_TAU_Enable(ctrl))
#define PWM_TIMER_INIT(ctrl, cfg)       (R_TAU_PWM_Open((ctrl), (cfg)))
#define PWM_TIMER_DEINIT(ctrl)          (R_TAU_PWM_Close(ctrl))
#define PWM_TIMER_ENABLE(ctrl)          (R_TAU_PWM_Enable(ctrl))
#elif (BSP_PERIPHERAL_GPT_PRESENT)
#define OS_TIMER_CTRL                   (g_timer_oneshot_ctrl)
#define OS_TIMER_CFG                    (g_timer_oneshot_cfg)
#define PWM_TIMER_CTRL                  (g_timer_pwm_ctrl)
#define PWM_TIMER_CFG                   (g_timer_pwm_cfg)
#define OS_TIMER_INIT(ctrl, cfg)        (R_GPT_Open((ctrl), (cfg)))
#define OS_TIMER_DEINIT(ctrl)           (R_GPT_Close(ctrl))
#define OS_TIMER_ENABLE(ctrl)           (R_GPT_Enable(ctrl))
#define PWM_TIMER_INIT(ctrl, cfg)       (R_GPT_Open((ctrl), (cfg)))
#define PWM_TIMER_DEINIT(ctrl)          (R_GPT_Close(ctrl))
#define PWM_TIMER_ENABLE(ctrl)          (R_GPT_Enable(ctrl))
#endif /* Timer version */

/* External variable - Defined in elc_ep.c */
extern volatile bool g_external_irq_flag;

/* Function declarations */
fsp_err_t elc_init(void);
fsp_err_t elc_enable(void);
fsp_err_t elc_event_gen(void);
void elc_deinit(void);
fsp_err_t oneshot_timer_init(void);
fsp_err_t pwm_timer_init(void);
fsp_err_t oneshot_timer_enable(void);
fsp_err_t pwm_timer_enable(void);
void oneshot_timer_deinit(void);
void pwm_timer_deinit(void);
fsp_err_t icu_init(void);
void icu_deinit(void);

#endif /* ELC_EP_H_ */

/***********************************************************************************************************************
 * File Name    : gpt_timer.h
 * Description  : Contains macros and function declarations.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef GPT_TIMER_H_
#define GPT_TIMER_H_

#include "hal_data.h"

/* Macro for the max duty cycle percentage */
#define GPT_MAX_PERCENT             (100U)

/* Macro for the size of buffer for console input data */
#define BUF_SIZE                    (16U)

/* Macro for GPT timer in Periodic mode */
#define PERIODIC_MODE_TIMER         (1U)

/* Macro for GPT timer in PWM mode */
#define PWM_MODE_TIMER              (2U)

/* Macro for GPT timer in One-Shot mode */
#define ONE_SHOT_MODE_TIMER         (3U)

/* Macro for the initial value */
#define INITIAL_VALUE               ('\0')

/* Macro for timer unit in millisecond */
#define TIMER_UNITS_MILLISECONDS    (1000U)

/* Macro for the type specifier */
#define CLOCK_TYPE_SPECIFIER        (1ULL)

/* Macro for the reset value */
#define RESET_VALUE                 (0x00)

/* Macro for the closed module */
#define MODULE_CLOSE                (0U)

/* Macro for GPT timer pin for specific board */
#if defined (BOARD_RA6E2_EK) || defined (BOARD_RA2E3_FPB) || defined (BOARD_RA2T1_FPB) || defined (BOARD_RA8T2_MCK)\
    || defined (BOARD_RA4E2_FPB)
#define TIMER_PIN                   (GPT_IO_PIN_GTIOCA)
#else
#define TIMER_PIN                   (GPT_IO_PIN_GTIOCB)
#endif

#if defined (BOARD_RA2A1_EK) || defined (BOARD_RA2A2_EK) || defined (BOARD_RA2E2_EK) || defined (BOARD_RA4W1_EK)\
    || defined (BOARD_RA4E2_EK) || defined (BOARD_RA4L1_EK) || defined (BOARD_RA6E2_EK) || defined (BOARD_RA6E1_FPB)\
    || defined (BOARD_RA4T1_MCK) || defined (BOARD_RA6T3_MCK) || defined (BOARD_RA2L2_EK) || defined (BOARD_RA2T1_FPB)\
    || defined (BOARD_RA4C1_EK) || defined (BOARD_RA4E2_FPB)
/* Macro for the max period count for 16-bit timer */
#define GPT_MAX_PERIOD_COUNT        (0XFFFF)
#else
/* Macro for the max period count for 32-bit timer */
#define GPT_MAX_PERIOD_COUNT        (0XFFFFFFFF)
#endif

/* Macro for GPT timer period range */
#if defined (BOARD_RA6M1_EK) || defined (BOARD_RA6M2_EK) || defined (BOARD_RA6M3_EK) || defined (BOARD_RA6M3G_EK) ||\
    defined (BOARD_RA6T1_RSSK) || defined (BOARD_RA6T2_MCK) || defined (BOARD_RA8M1_EK) || defined (BOARD_RA8D1_EK) ||\
    defined (BOARD_RA8T1_MCK) || defined (BOARD_RA8E1_FPB) || defined (BOARD_RA8E2_EK)
#define PERIOD_RANGE                "(0 to 35791)"
#elif defined (BOARD_RA4M1_EK) || defined (BOARD_RA2L1_EK) || defined (BOARD_RA2E1_EK) || defined (BOARD_RA2E3_FPB)
#define PERIOD_RANGE                "(0 to 89478)"
#elif defined (BOARD_RA2A1_EK) || defined (BOARD_RA2E2_EK) || defined (BOARD_RA2A2_EK) || defined (BOARD_RA2L2_EK)
#define PERIOD_RANGE                "(0 to 1398)"
#elif defined (BOARD_RA4W1_EK)
#define PERIOD_RANGE                "(0 to 2097)"
#elif defined (BOARD_RA6M5_EK) || defined (BOARD_RA6M4_EK) || defined (BOARD_RA4M3_EK) || defined (BOARD_RA4M2_EK) ||\
      defined (BOARD_RA4E1_FPB)
#define PERIOD_RANGE                "(0 to 42949)"
#elif defined (BOARD_RA6E1_FPB) || defined (BOARD_RA4E2_EK) || defined (BOARD_RA6E2_EK) || defined (BOARD_RA4T1_MCK) ||\
      defined (BOARD_RA6T3_MCK) || defined (BOARD_RA4E2_FPB)
#define PERIOD_RANGE                "(0 to 671)"
#elif defined (BOARD_RA4L1_EK)
#define PERIOD_RANGE                "(0 to 838)"
#elif defined (BOARD_RA8P1_EK) || defined (BOARD_RA8T2_MCK)
#define PERIOD_RANGE                "(0 to 17179)"
#elif defined (BOARD_RA8D2_EK) || defined (BOARD_RA8M2_EK)
#if (BSP_PERIPHERAL_GPT_GTCLK_PRESENT && !GPT_CFG_GPTCLK_BYPASS)
/* GPTCLK = GTLCK (300 MHz) */
#define PERIOD_RANGE                "(0 to 14316)"
#else
/* GPTCLK = PCLKD (250 MHz) */
#define PERIOD_RANGE                "(0 to 17179)"
#endif /* BSP_PERIPHERAL_GPT_GTCLK_PRESENT && !GPT_CFG_GPTCLK_BYPASS */
#elif defined (BOARD_RA2T1_FPB) || defined (BOARD_RA4C1_EK)
#define PERIOD_RANGE                "(0 to 1048)"
#endif

/* Macro for check status of GPT timer in Periodic mode */
#define PERIODIC_MODE	            (1U)

/* Macro for check status of GPT timer in PWM mode */
#define PWM_MODE	                (2U)

/* Macro for check status of GPT timer in One-Shot mode */
#define ONE_SHOT_MODE               (3U)

/* Extern variable - defined in gpt_timer.c */
extern gpt_instance_ctrl_t * gp_active_timer_ctrl;

/* Function declarations */
fsp_err_t init_gpt_timer(timer_ctrl_t * const p_timer_ctl, timer_cfg_t const * const p_timer_cfg);
fsp_err_t start_gpt_timer(timer_ctrl_t * const p_timer_ctl);
fsp_err_t set_timer_duty_cycle(uint8_t duty_cycle_percent);
fsp_err_t set_period_gpt_timer(timer_ctrl_t * const p_ctrl, uint32_t const period_counts);
void deinit_gpt_timer(timer_ctrl_t * const p_timer_ctl);

#endif /* GPT_TIMER_H_ */

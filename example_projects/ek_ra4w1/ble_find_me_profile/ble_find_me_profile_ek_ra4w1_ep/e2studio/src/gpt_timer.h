/***********************************************************************************************************************
 * File Name    : gpt_timer.h
 * Description  : Contains Macros and function declarations.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#ifndef GPT_TIMER_H_
#define GPT_TIMER_H_

/* Macros definitions */
#define PERIODIC_MODE_TIMER      (1U)            /* To perform GPT Timer in Periodic mode */
#define TIMER_UNITS_MILLISECONDS  (1000U)           /* timer unit in millisecond */
#define CLOCK_TYPE_SPECIFIER      (1ULL)         /* type specifier */

/* GPT Timer Pin for boards */
#define TIMER_PIN           GPT_IO_PIN_GTIOCB

#if defined (BOARD_RA2A1_EK) || defined (BOARD_RA4W1_EK)
#define GPT_MAX_PERIOD_COUNT     (0XFFFF)        /* Max Period Count for 16-bit Timer*/
#endif

/* Function declaration */
fsp_err_t init_gpt_timer(timer_ctrl_t * const p_timer_ctl, timer_cfg_t const * const p_timer_cfg, uint8_t timer_mode);
fsp_err_t stop_gpt_timer (timer_ctrl_t * const p_timer_ctl);
fsp_err_t start_gpt_timer (timer_ctrl_t * const p_timer_ctl);
void deinit_gpt_timer(timer_ctrl_t * const p_timer_ctl);
void gpt_timer();

#endif /* GPT_TIMER_H_ */

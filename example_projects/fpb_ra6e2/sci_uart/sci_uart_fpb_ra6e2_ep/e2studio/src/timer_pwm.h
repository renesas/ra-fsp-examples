/***********************************************************************************************************************
 * File Name    : timer_pwm.h
 * Description  : Contains function declarations and macros of timer_pwm.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef TIMER_PWM_H_
#define TIMER_PWM_H_

#include "hal_data.h"

/* Board specific macros for conditional compilation */
#if defined(BOARD_RA2E3_FPB) || defined(BOARD_RA6E2_EK) || defined(BOARD_RA8E1_FPB) || defined(BOARD_RA4L1_EK) ||\
	defined(BOARD_RA8E2_EK) || defined(BOARD_RA2T1_FPB) || defined(BOARD_RA8T2_MCK) || defined(BOARD_RA6E2_FPB)
#define TIMER_PIN           (GPT_IO_PIN_GTIOCA)
#else
#define TIMER_PIN           (GPT_IO_PIN_GTIOCB)
#endif

/* Macros definition */
#define MAX_INTENSITY       (100U)        /* Maximum intensity 100 */
#define STEP                (10U)         /* Step increment/decrement */
#define MAX_DUTY_CYCLE      (1000U)       /* Max duty cycle count */

/* Function declarations */
fsp_err_t gpt_init(void);
fsp_err_t gpt_start(void);
fsp_err_t set_intensity(uint32_t raw_count, uint8_t pin);
void gpt_deinit(void);

#endif /* TIMER_PWM_H_ */

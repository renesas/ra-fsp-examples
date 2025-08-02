/***********************************************************************************************************************
 * File Name    : timer_pwm.h
 * Description  : Contains function declaration and macros of timer_pwm.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef TIMER_PWM_H_
#define TIMER_PWM_H_

/* Board specific macros for conditional compilation */
#if defined (BOARD_RA2E3_FPB) || defined(BOARD_RA6E2_EK) || defined(BOARD_RA8E1_FPB) || defined(BOARD_RA4L1_EK) ||\
	defined (BOARD_RA8E2_EK) || defined (BOARD_RA2T1_FPB)
#define TIMER_PIN           (GPT_IO_PIN_GTIOCA)
#else
#define TIMER_PIN           (GPT_IO_PIN_GTIOCB)
#endif

/* Macros definition */
#define MAX_INTENISTY       (100u)        /* Maximum intensity 100 */
#define STEP                (10u)         /* Step increment/decrement */
#define MAX_DUTY_CYCLE      (1000u)       /* Max duty cycle count */

/* Function declarations */
fsp_err_t gpt_initialize(void);
fsp_err_t gpt_start(void);
fsp_err_t set_intensity(uint32_t raw_count, uint8_t pin);
void timer_gpt_deinit(void);

#endif /* TIMER_PWM_H_ */

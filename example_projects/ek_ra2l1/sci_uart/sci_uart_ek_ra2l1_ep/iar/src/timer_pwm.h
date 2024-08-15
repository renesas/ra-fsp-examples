/***********************************************************************************************************************
 * File Name    : timer_pwm.h
 * Description  : Contains function declaration and macros of timer_pwm.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef TIMER_PWM_H_
#define TIMER_PWM_H_

/* Board specific macros for conditional compilation */
#define TIMER_PIN          GPT_IO_PIN_GTIOCB

/* Macros definition */
#define MAX_INTENISTY       (100u)        /* Maximum intensity 100 */
#define STEP                (10u)         /* Step increment/decrement */
#define MAX_DUTY_CYCLE      (1000u)       /* Max duty cycle count */

/* Function declaration */
fsp_err_t gpt_initialize(void);
fsp_err_t gpt_start(void);
fsp_err_t set_intensity(uint32_t raw_count, uint8_t pin);
void timer_gpt_deinit(void);

#endif /* TIMER_PWM_H_ */

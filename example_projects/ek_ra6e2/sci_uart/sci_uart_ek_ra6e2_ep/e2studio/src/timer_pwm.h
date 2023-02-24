/***********************************************************************************************************************
 * File Name    : timer_pwm.h
 * Description  : Contains function declaration and macros of timer_pwm.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#ifndef TIMER_PWM_H_
#define TIMER_PWM_H_

/* Board specific macros for conditional compilation */

#if defined (BOARD_RA6E2_EK)
#define TIMER_PIN          GPT_IO_PIN_GTIOCA
#elif
#define TIMER_PIN          GPT_IO_PIN_GTIOCB
#endif

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

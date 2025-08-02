/***********************************************************************************************************************
 * File Name    : io_ep.h
 * Description  : Contains Macros and function declarations.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2022 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#ifndef IO_EP_H_
#define IO_EP_H_

/* External IRQ channel for specific boards*/

#define USER_SW_IRQ_NUMBER        (0x0D)
#define USER_SW2_IRQ_NUMBER        (0x0C)
/* Function declaration */
fsp_err_t icu_init(void);
void icu_deinit(void);
fsp_err_t gpt_ref_timer_PWM_init(void);
fsp_err_t gpt_output_delay_timer_PWM_init(void);
void odc_init(void);
#endif /* IO_EP_H_ */

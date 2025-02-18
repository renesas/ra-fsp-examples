/***********************************************************************************************************************
 * File Name    : arm_dsp_example.h
 * Description  : Contains declarations of data structures and functions used in hal_entry.c.
 **********************************************************************************************************************/
/**********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*********************************************************************************************************************/

#ifndef ARM_DSP_EXAMPLE_H_
#define ARM_DSP_EXAMPLE_H_

#include "SEGGER_RTT/SEGGER_RTT.h"

#define SEGGER_INDEX            (0)

#define APP_PRINT(fn_, ...)      SEGGER_RTT_printf (SEGGER_INDEX,(fn_), ##__VA_ARGS__);

/* Function Prototype */
int32_t main_arm_fir_example_f32(void);
int32_t main_arm_fft_bin_example_f32(void);
int32_t main_arm_sin_cos_example_f32(void);
int32_t main_arm_signal_converge_example_f32(void);
int32_t main_arm_dotproduct_example_f32(void);
int32_t main_arm_linear_interp_example_f32(void);
int32_t main_arm_graphic_equalizer_example_q31(void);
int32_t main_arm_variance_example_f32(void);
int32_t main_arm_convolution_example_f32(void);
int32_t main_arm_class_marks_example_f32(void);
int32_t main_arm_matrix_example_f32(void);
int32_t main_arm_svm_example_f32(void);
int32_t main_arm_bayes_example_f32(void);
#endif /* ARM_DSP_EXAMPLE_H_ */

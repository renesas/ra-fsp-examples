/***********************************************************************************************************************
 * File Name    : arm_dsp_example.h
 * Description  : Contains declarations of data structures and functions used in hal_entry.c.
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
 * Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

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

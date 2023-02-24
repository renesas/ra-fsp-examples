/***********************************************************************************************************************
 * File Name    : arm_dsp_example_config.h
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

#ifndef ARM_DSP_EXAMPLE_CONFIG_H_
#define ARM_DSP_EXAMPLE_CONFIG_H_

/*
 * DSP Instruction Benchmark  macro is disabled by default (0U),
 * set it to (1U) to enable the benchmark
 */
#define DSP_INSTRUCTION_BENCH            (1U)
/*
 * Set macro definition to 1U to enable a specific example
 */
#define ARM_FIR_EXAMPLE                  (1U)
#define ARM_FFT_BIN_EXAMPLE              (0U)
#define ARM_SIN_COS_EXAMPLE              (0U)
#define ARM_SIGNAL_CONVERGE_EXAMPLE      (0U)
#define ARM_DOTPRODUCT_EXAMPLE           (0U)
#define ARM_LINEAR_INTERP_EXAMPLE        (0U)
#define ARM_GRAPHIC_EQUALIZER_EXAMPLE    (0U)
#define ARM_VARIANCE_EXAMPLE             (0U)
#define ARM_CONVOLUTION_EXAMPLE          (0U)
#define ARM_CLASS_MARKS_EXAMPLE          (0U)
#define ARM_MATRIX_EXAMPLE               (0U)
#define ARM_SVM_EXAMPLE                  (0U)
#define ARM_BAYES_EXAMPLE                (0U)
#endif /* ARM_DSP_EXAMPLE_CONFIG_H_ */

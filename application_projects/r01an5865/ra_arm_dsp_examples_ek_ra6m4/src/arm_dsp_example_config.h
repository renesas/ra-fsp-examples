/***********************************************************************************************************************
 * File Name    : arm_dsp_example_config.h
 * Description  : Contains declarations of data structures and functions used in hal_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2019 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
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

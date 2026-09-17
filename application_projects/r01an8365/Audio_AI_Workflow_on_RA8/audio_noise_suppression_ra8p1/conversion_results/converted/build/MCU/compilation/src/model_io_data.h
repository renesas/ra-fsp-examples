/*
 * This file is developed by EdgeCortix Inc. to be used with certain Renesas Electronics Hardware only.
 *
 * Copyright © 2025 EdgeCortix Inc. Licensed to Renesas Electronics Corporation with the
 * right to sublicense under the Apache License, Version 2.0.
 *
 * This file also includes source code originally developed by the Renesas Electronics Corporation.
 * The Renesas disclaimer below applies to any Renesas-originated portions for usage of the code.
 *
 * The Renesas Electronics Corporation
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED 'AS IS' AND RENESAS MAKES NO WARRANTIES REGARDING
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
 * Changed from original python code to C source code.
 * Copyright (C) 2017 Renesas Electronics Corporation. All rights reserved.
 *
 * This file also includes source codes originally developed by the TensorFlow Authors which were distributed under the following conditions.
 *
 * The TensorFlow Authors
 * Copyright 2023 The Apache Software Foundation
 *
 * This product includes software developed at
 * The Apache Software Foundation (http://www.apache.org/).
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef MODEL_MODEL_DATA_H
#define MODEL_MODEL_DATA_H

#include <stdint.h>

extern int8_t model_main_input_int8[];
#define model_main_input_int8_SIZE (42)
#define model_main_input_int8_COUNT (42)

extern int8_t model_vad_gru_prev_state_int8[];
#define model_vad_gru_prev_state_int8_SIZE (24)
#define model_vad_gru_prev_state_int8_COUNT (24)

extern int8_t model_noise_gru_prev_state_int8[];
#define model_noise_gru_prev_state_int8_SIZE (48)
#define model_noise_gru_prev_state_int8_COUNT (48)

extern int8_t model_denoise_gru_prev_state_int8[];
#define model_denoise_gru_prev_state_int8_SIZE (96)
#define model_denoise_gru_prev_state_int8_COUNT (96)

extern int8_t model_Identity_int8[];
#define model_Identity_int8_SIZE (96)
#define model_Identity_int8_COUNT (96)

extern int8_t model_Identity_1_int8[];
#define model_Identity_1_int8_SIZE (22)
#define model_Identity_1_int8_COUNT (22)

extern int8_t model_Identity_2_int8[];
#define model_Identity_2_int8_SIZE (48)
#define model_Identity_2_int8_COUNT (48)

extern int8_t model_Identity_3_int8[];
#define model_Identity_3_int8_SIZE (24)
#define model_Identity_3_int8_COUNT (24)

extern int8_t model_Identity_4_int8[];
#define model_Identity_4_int8_SIZE (1)
#define model_Identity_4_int8_COUNT (1)


#endif // MODEL_MODEL_DATA_H

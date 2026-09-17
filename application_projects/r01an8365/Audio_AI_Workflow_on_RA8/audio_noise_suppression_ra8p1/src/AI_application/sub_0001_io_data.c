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

#include "sub_0001_io_data.h"

// Input tensor: Tensordot_Reshape_model_1_fc1_StatefulPartitionedCall_Tensordot_Reshape_70039
// Data Type: INT8
// Shape: [1, 42]
// Number of elements: 42
// Size in bytes: 42
int8_t sub_0001_Tensordot_Reshape_model_1_fc1_StatefulPartitionedCall_Tensordot_Reshape_70039[] = {
  12, 23, 55, 88, 26, 91, 11, 88, -20, 31,
  37, -30, -16, -52, 100, -114, 118, -59, -30, -6,
  74, 79, 7, -6, 17, -28, 108, 86, -110, -42,
  -106, 37, -123, -34, 85, 117, 71, -93, 94, 94,
  122, -7,
};

// Input tensor: denoise_gru_prev_state_int8
// Data Type: INT8
// Shape: [1, 96]
// Number of elements: 96
// Size in bytes: 96
int8_t sub_0001_denoise_gru_prev_state_int8[] = {
  76, 77, -10, 5, 71, 45, -98, 56, 35, 20,
  -92, 9, 113, 66, 5, -101, -22, -7, -61, -81,
  70, 60, -12, -73, 17, -94, -124, -46, 30, -90,
  28, -72, 29, -30, 113, 103, 46, -13, -36, 28,
  -17, 103, 50, -103, -113, 120, 42, 39, 43, -85,
  -75, -37, -95, 64, -48, 27, -35, -45, 17, -119,
  -16, 34, 125, 117, -102, 39, -75, 34, -87, 126,
  39, 20, -64, -22, -9, -7, -66, 31, -88, -42,
  -100, 44, 40, -47, -93, 71, -78, 115, -34, 41,
  82, -125, -104, 31, 86, 44,
};

// Input tensor: main_input_int8
// Data Type: INT8
// Shape: [1, 1, 42]
// Number of elements: 42
// Size in bytes: 42
int8_t sub_0001_main_input_int8[] = {
  -104, 120, 121, 96, -9, 2, 122, -114, 26, -13,
  61, -123, -118, -15, -56, 122, -98, -36, -53, -5,
  -98, 48, -47, 97, -22, 107, -112, -73, 49, 16,
  17, 93, -61, 2, 5, 106, -104, 107, 19, -107,
  109, -57,
};

// Input tensor: noise_gru_prev_state_int8
// Data Type: INT8
// Shape: [1, 48]
// Number of elements: 48
// Size in bytes: 48
int8_t sub_0001_noise_gru_prev_state_int8[] = {
  -47, -126, 42, 87, -95, 37, 55, 87, -54, -61,
  -82, -27, 22, 13, -123, -86, 84, -34, -127, -91,
  45, 17, -59, 52, 60, -55, 118, -18, -65, 65,
  19, -27, 23, 101, 18, 35, -71, 100, 115, 46,
  -14, -14, 88, 122, 51, -99, -52, 68,
};

// Input tensor: vad_gru_prev_state_int8
// Data Type: INT8
// Shape: [1, 24]
// Number of elements: 24
// Size in bytes: 24
int8_t sub_0001_vad_gru_prev_state_int8[] = {
  80, -23, -27, 44, 97, -65, 20, -48, 97, 119,
  49, 22, 57, 40, 0, 8, 116, -69, 36, -27,
  -20, 30, 27, -7,
};

// Output tensor: Identity_1_int8_70152
// Data Type: INT8
// Shape: [1, 1, 22]
// Number of elements: 22
// Size in bytes: 22
int8_t sub_0001_Identity_1_int8_70152[] = {
  50, 69, 105, 61, -28, -31, 28, 69, 71, 74,
  76, 71, 87, 97, 109, 114, 116, 119, 117, 115,
  105, 95,
};

// Output tensor: Identity_2_int8_70118
// Data Type: INT8
// Shape: [1, 1, 48]
// Number of elements: 48
// Size in bytes: 48
int8_t sub_0001_Identity_2_int8_70118[] = {
  -128, -126, -76, -128, -94, -15, -14, -14, 99, -108,
  -128, -128, -128, -15, -123, 127, 127, -123, -128, -87,
  127, 127, -122, -15, -128, -54, -14, -18, -64, -15,
  -70, 74, -128, -29, -15, 127, -71, -9, -108, -128,
  -128, 127, 49, 35, 127, -99, 127, 127,
};

// Output tensor: Identity_3_int8_70089
// Data Type: INT8
// Shape: [1, 1, 24]
// Number of elements: 24
// Size in bytes: 24
int8_t sub_0001_Identity_3_int8_70089[] = {
  99, 118, -35, -108, 89, -105, 8, -18, 72, 92,
  52, 25, 14, -41, -81, 123, 66, 122, 119, -72,
  107, 97, 64, -22,
};

// Output tensor: Identity_4_int8_70156
// Data Type: INT8
// Shape: [1, 1, 1]
// Number of elements: 1
// Size in bytes: 1
int8_t sub_0001_Identity_4_int8_70156[] = {
  125,
};

// Output tensor: Identity_int8_70148
// Data Type: INT8
// Shape: [1, 1, 96]
// Number of elements: 96
// Size in bytes: 96
int8_t sub_0001_Identity_int8_70148[] = {
  126, -128, -128, -128, -128, 46, -126, -128, 34, 21,
  -115, 126, -128, 67, 6, -102, -21, -8, -128, -80,
  69, -128, -128, -74, 16, -93, 126, -128, 126, 126,
  29, 126, 126, -128, 126, 104, 47, -12, -37, 29,
  -16, -128, 51, -102, -113, -128, 45, 126, -1, 126,
  125, 126, 126, 63, -47, 28, -128, 125, 24, -118,
  -15, 33, 124, 116, -128, 40, -76, 33, -128, 126,
  113, 20, -128, -21, -10, -128, -128, -119, -88, 51,
  -101, 43, -128, -128, -94, 126, -79, 105, 126, 42,
  81, -124, -128, 30, 87, 43,
};


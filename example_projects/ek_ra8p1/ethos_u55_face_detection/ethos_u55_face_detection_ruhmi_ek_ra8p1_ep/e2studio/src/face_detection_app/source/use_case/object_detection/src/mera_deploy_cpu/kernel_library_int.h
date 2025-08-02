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
 */

#ifndef __KERNEL_LIBRARY_INT__
#define __KERNEL_LIBRARY_INT__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <math.h>

#include "kernel_library_utils.h"


/****************************************************************************/
/*                               Integer Operators                          */
/****************************************************************************/

void AffineQuantizeFloatToInt8(const float* input, int8_t* output, uint32_t size, int8_t output_zp, float output_scale);
void AffineDequantizeUInt8ToFloat(const uint8_t* input, float* output, uint32_t size, int8_t input_zp, float input_scale);
void AffineDequantizeInt8ToFloat(const int8_t* input, float* output, uint32_t size, int8_t input_zp, float input_scale);
void Requantize(const uint8_t* input, int8_t* output, uint32_t size, int32_t multiplier, int32_t shift,
  int32_t input_zp, int32_t output_zp);
void RequantizeInt8ToUint8(const int8_t* input_data, uint8_t* output_data,
  uint32_t size, int32_t multiplier, int32_t shift, int32_t input_zp,
  int32_t output_zp);
void RequantizeInt8ToInt8(const int8_t* input_data, int8_t* output_data,
    uint32_t size, int32_t multiplier, int32_t shift, int32_t input_zp,
    int32_t output_zp);
void Sigmoid(const int8_t *input, int8_t *output, uint32_t size,
  int32_t input_zp, float input_scale, int32_t output_zp, float output_scale);
void Minimum(const int8_t* input_1, const int8_t* input_2,  int8_t* output,
  const int32_t* input1_shape, int32_t input1_dims, const int32_t* input2_shape,
  int32_t input2_dims, const int32_t* output_shape, int32_t output_dims);
void Tanh(const int8_t* input, int8_t* output, uint32_t size, float input_scale, int8_t input_zp,
  float output_scale, int8_t output_zp);

void Slice(const int8_t* input, int8_t* output, const int32_t* begin, const int32_t* size,
  const int32_t* in_shape, int32_t dims);
void StridedSlice(const int8_t* input, int8_t* output, const int32_t* begin_orig,
  const int32_t* end_orig, const int32_t* strides, const int32_t* in_shape, int32_t in_dims,
  const int32_t* out_shape, int32_t out_dims, TfLiteStridedSliceParams str_slc_params);

void Pack(const int8_t **inputs, uint32_t inp_count, int8_t* output, uint32_t size, uint32_t slice_size, uint32_t stride);
void Mean(const int8_t* input, int8_t* output, const int32_t* in_shape, const int32_t in_num_dims,
  int32_t input_zp, int32_t out_size, int32_t output_multiplier, int32_t output_shift, int32_t output_zp,
  const int32_t* axis, const int32_t num_axis);
void Sum(const int8_t* input, int8_t* output, const int32_t* in_shape, int32_t in_num_dims,
  int32_t input_zp, int32_t out_size, int32_t output_multiplier, int32_t output_shift,
  int32_t output_zp, const int32_t* axis, int32_t num_axis);
void ReduceMax(const int8_t* input, int8_t* output, const int32_t* in_shape, const int32_t in_num_dims,
   int32_t out_size, const int32_t* axis, const int32_t num_axis);
void Exp(const int8_t *input, int8_t *output, uint32_t size, 
  int32_t input_zp, float input_scale, int32_t output_zp, float output_scale);
void Log(const int8_t *input, int8_t *output, uint32_t size, 
  int32_t input_zp, float input_scale, int32_t output_zp, float output_scale);
void Neg(const float* input,  float* output, uint32_t size);
void LeakyRelu(const int8_t* input, int8_t* output, uint32_t size, int32_t input_zp, int32_t output_zp,
  int32_t output_multiplier_alpha, int32_t output_shift_alpha, int32_t output_multiplier_identity,
  int32_t output_shift_identity);
void LogSoftMax(const int8_t* input, int8_t* output, const int32_t* in_shape, const int32_t in_num_dims,
  int32_t input_zp, float input_scale, int32_t output_zp, float output_scale);
void SpaceToBatchNd(const int8_t* input1_data, int8_t* output_data, const int32_t* block_shape, const int32_t* pad,
  const int32_t* in_shape, const int32_t in_num_dims, const int32_t* out_shape, int32_t output_zp);
void BatchToSpaceNd(const int8_t* input1_data, int8_t* output_data, const int32_t* block_shape, const int32_t* crops,
  const int32_t* in_shape, const int32_t in_num_dims, const int32_t* out_shape, int32_t output_zp);
void Transpose(const int8_t *input, int8_t *output, int32_t size, int32_t rank, int32_t *strides, 
  int32_t *next_dim_sizes, int32_t *dim_sizes);
void ResizeBilinear(const int8_t* input_data, int8_t* output_data, const int32_t* input_shape,
  const int32_t* output_shape, bool half_pixel_centers, bool align_corners);
void MirrorPad(const int32_t* padding_matrix,
  const int* input_dims, int* output_dims_num_elements,
  int* input_dims_num_elements, const int8_t* input_data,
  int8_t* output_data, const int offset, const int num_dims,
  const int output_size);
void UpsamplingNearestNeighbor(const int8_t *input, int8_t *output, const int32_t *in_shape,
  const int32_t *out_shape, const bool align_corners,  const bool half_pixel_centers);

void PRelu(const int8_t* input, const int32_t *in_shape, int32_t in_dims, int32_t input_zp,
  int8_t* output, const int32_t *out_shape, int32_t out_dims, int32_t output_zp,
  const int8_t *alpha, const int32_t *alpha_shape, int32_t alpha_dims, int32_t alpha_zp,
  int32_t output_multiplier_alpha, int32_t output_shift_alpha, int32_t output_multiplier_identity,
  int32_t output_shift_identity);
void Relu(const int8_t *input, int8_t *output, uint32_t size, int32_t input_zp,
  float output_scale, int32_t output_zp, int32_t output_multiplier, int32_t output_shift);
void Relu6(const int8_t *input, int8_t *output, uint32_t size, int32_t input_zp,
  float input_scale, int32_t output_zp, float output_scale);

void Abs(const int8_t* input, int32_t input_zp, int32_t size, int8_t* output,
  int32_t output_zp, int32_t multiplier, int32_t shift);
void Mul(const int8_t* input1, const int32_t* input1_shape, int32_t input1_dims,
  const int8_t* input2, const int32_t* input2_shape, int32_t input2_dims, int8_t* output,
  const int32_t* output_shape, int32_t output_dims, ArithmeticParams quant_params);

void Sub(const int8_t* input1, const int8_t* input2, int8_t* output,
  const int32_t* input1_shape, int32_t input1_dims, const int32_t* input2_shape,
  int32_t input2_dims, const int32_t* output_shape, int32_t output_dims,
  ArithmeticParams quant_params);
void SubElementwise(const int8_t* input1_data, const int8_t* input2_data,
  int8_t* output_data, int32_t size, ArithmeticParams quant_params);
void BroadcastSubSlowInt8(const int32_t* input1_shape, const int8_t* input1_data,
  int32_t input1_dims, const int32_t* input2_shape, const int8_t* input2_data,
  int32_t input2_dims, const int32_t* output_shape, int8_t* output_data,
  int32_t output_dims, ArithmeticParams quant_params);
bool ReduceDimensionsForBroadcast(const int32_t* input1_dims, int32_t num_input1_dims,
  const int32_t* input2_dims, int32_t num_input2_dims, size_t* compressed_input1_stride,
  size_t* compressed_input2_stride, size_t* compressed_output_shape);
void BroadcastSubRecursiveDimensions(int dimension, int32_t* input1_offset_p, int32_t* input2_offset_p,
  int32_t* output_offset, size_t* compressed_input1_stride, size_t* compressed_input2_stride,
  size_t* compressed_output_shape, ArithmeticParams quant_params, const int8_t* input1_data,
  const int8_t* input2_data, int8_t* output_data);
void SubBroadcast(const int8_t* input_data, const int8_t* broadcast_data,
  int8_t* output_data, int32_t size, ArithmeticParams quant_params);

int8_t AddOp(int8_t input1, int8_t input2, ArithmeticParams quant_params);
void BroadcastInput1(const int8_t* input1, const int8_t* input2, int8_t* output,
  ArithmeticParams quant_params, int size);
void BroadcastInput2(const int8_t* input1, const int8_t* input2, int8_t* output,
  ArithmeticParams quant_params, int size);
void ElementWise(const int8_t* input1, const int8_t* input2, int8_t* output,
  ArithmeticParams quant_params, int size);
void BroadcastAddRecursiveDimensions(const int8_t* input1, size_t* input1_offset_p,
  size_t* compressed_input1_stride, const int8_t* input2, size_t* input2_offset_p,
  size_t* compressed_input2_stride, int8_t* output, size_t* output_offset,
  size_t* compressed_output_shape, ArithmeticParams quant_params, int dimension);
void BroadcastBinaryFunction6DSlow(const int8_t* input1, const int32_t* input1_shape,
  int32_t input1_dims, const int8_t* input2, const int32_t* input2_shape,
  int32_t input2_dims, int8_t* output, const int32_t* output_shape, int32_t output_dims,
  ArithmeticParams quant_params);
void Add(const int8_t* input1, const int32_t* input1_shape, int32_t input1_dims,
  const int8_t* input2, const int32_t* input2_shape, int32_t input2_dims, int8_t* output,
  const int32_t* output_shape, int32_t output_dims, ArithmeticParams quant_params);
void Pad(int32_t left_padding_count, int32_t *left_padding,
  int32_t right_padding_count, int32_t *right_padding,
  int32_t *ext_input_shape, const int8_t *input_data,
  const int8_t pad_value, const int32_t *ext_output_shape,
  int8_t *output_data);

int8_t SquaredDifferenceOp(int8_t x, int8_t y, const ArithmeticParams params);
void SquaredDifferenceBroadcast1(const int8_t* input1_data, const int8_t* input2_data,
  int8_t* output, int size, const ArithmeticParams params);
void SquaredDifferenceBroadcast2(const int8_t* input1_data, const int8_t* input2_data,
  int8_t* output, int size, const ArithmeticParams params);
void ElementWiseSquaredDifference(const int8_t* input1, const int8_t* input2,
  int8_t* output, int size, const ArithmeticParams params);
void BroadcastRecursiveDimensions(
  const int8_t* input1, size_t* input1_offset_p, size_t* compressed_input1_stride,
  const int8_t* input2, size_t* input2_offset_p, size_t* compressed_input2_stride,
  int8_t* output, size_t* output_offset, size_t* compressed_output_shape,
  int dimension, const ArithmeticParams params);
void BroadcastSquaredDifference6DSlow(
  const int8_t* input1_data, const int32_t* input1_shape, int32_t input1_dims,
  const int8_t* input2_data,  const int32_t* input2_shape, int32_t input2_dims,
  int8_t* output_data, const int32_t* output_shape, const ArithmeticParams params);
void SquaredDifference(const int8_t* input1, const int32_t* input1_shape,
  uint32_t input1_dims, uint32_t input1_size, const int8_t* input2,
  const int32_t* input2_shape, uint32_t input2_dims, int8_t* output,
  const int32_t* output_shape, uint32_t output_dims, const ArithmeticParams params);

void HardSwish(const int8_t* input, const int32_t* input_shape,
  int32_t input_dims, int8_t* output, int16_t input_zero_point,
  int16_t output_zero_point, int16_t reluish_multiplier_fixedpoint_int16,
  int reluish_multiplier_exponent, int16_t output_multiplier_fixedpoint_int16,
  int output_multiplier_exponent);

#endif /*  __KERNEL_LIBRARY_INT__ */

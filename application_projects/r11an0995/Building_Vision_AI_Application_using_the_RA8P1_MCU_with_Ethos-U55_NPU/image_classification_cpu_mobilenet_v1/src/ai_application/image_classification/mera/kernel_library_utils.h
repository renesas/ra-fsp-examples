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

#ifndef __KERNEL_LIBRARY_UTILS__
#define __KERNEL_LIBRARY_UTILS__

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

/****************************************************************************/
/*                      Generic/Common Data structures                   */
/****************************************************************************/

#define kMaxBroadcastDim 6
#define kMaxMulBroadcastDim 6

#define kMaxNumberOfAxis 5
#define kMaxNumberOfReducedAxis 2 + 1

#define PadKernelMaxDimensionCount 5

typedef enum {
    kNone = 0,
    kNonBroadcast,              // Matching input shapes.
    kFirstInputBroadcastsFast,  // Fivefold nested loops.
    kSecondInputBroadcastsFast, // Fivefold nested loops.
    kGenericBroadcast,          // Fall-back.
} BroadcastableOpCategory;

typedef struct {
    // The "extent" of each dimension. Indices along dimension d must be in the
    // half-open interval [0, extents[d]).
    int extents[kMaxMulBroadcastDim];
    // The number of *elements* (not bytes) between consecutive indices of each
    // dimension.
    int strides[kMaxMulBroadcastDim];
} NdArrayDesc;

typedef struct {
    int begin_mask;
    int end_mask;
    int ellipsis_mask;
    int new_axis_mask;
    int shrink_axis_mask;
} TfLiteStridedSliceParams;

typedef struct {
    int8_t start_indices_count;
    int32_t start_indices[5];
    int8_t stop_indices_count;
    int32_t stop_indices[5];
    int8_t strides_count;
    int32_t strides[5];

    uint16_t begin_mask;
    uint16_t ellipsis_mask;
    uint16_t end_mask;
    uint16_t new_axis_mask;
    uint16_t shrink_axis_mask;
    bool offset;
} StridedSliceParams;

typedef struct {
  int32_t input1_offset;
  int32_t input2_offset;
  int32_t output_offset;
  int32_t output_multiplier;
  int output_shift;
  // Add / Sub, not Mul, uint8_t inference params.
  int left_shift;
  int32_t input1_multiplier;
  int input1_shift;
  int32_t input2_multiplier;
  int input2_shift;

  int32_t quantized_activation_min;
  int32_t quantized_activation_max;
} ArithmeticParams;

// Possible fused activation functions.
typedef enum {
  ActNone = 0,
  ActRelu,
  ActReluN1To1,  // min(max(-1, x), 1)
  ActRelu6,      // min(max(0, x), 6)
  ActTanh,
  ActSignBit,
  ActSigmoid,
} FusedActivation;


/****************************************************************************/
/*                  Generic/Common Op Utility Functions                  */
/****************************************************************************/

bool ResolveAxis(const int num_dims, const int *axis, const int num_axis,
                 int *out_axis, int *out_num_axis);
void ExtendShapeSpaceToBatch(const int32_t* input_dims, int32_t input_num_dims,
                  int32_t* extended_dims);
void ExtendShapeBatchToSpace(const int32_t* input_dims, int32_t input_num_dims,
                  int32_t* extended_dims);
void ExtendedShape(const int32_t *input_dims, int32_t input_num_dims,
                   int32_t *extended_dims, int32_t extended_num_dims);
void CopyDimsToDesc(const int32_t *input_shape, NdArrayDesc *desc_out,
                    int32_t num_dims);
int FlatSize(const int32_t *shape, int32_t dims);
bool HaveSameShapes(const int32_t *input1, int32_t input1_dims,
                    const int32_t *input2, int32_t input2_dims);
int FlatSizeSkipDim(const int *dims, int dims_count, int skip_dim);
int64_t SignedIntMin(const int64_t a, const int64_t b);
int64_t SignedIntMax(const int64_t a, const int64_t b);
uint32_t ReducedOutputOffset(const int num_dims, const int *dims, const int *index,
                             const int num_axis, const int *axis);
bool NextIndex(const int num_dims, const int *dims, int *current);
int CountLeadingZeros(int64_t integer_input);
int32_t MultiplyByQuantizedMultiplier(int32_t x, int64_t quantized_multiplier,
                                      int shift);
int32_t Offset(const int32_t *dims, int i0, int i1, int i2, int i3);
void ComputeInterpolationValuesInteger(
    const int32_t value, const int32_t scale_10, const bool half_pixel_centers,
    int32_t input_size, int32_t *scaled_value, int32_t *lower_bound,
    int32_t *upper_bound);
void NdArrayDescsForElementwiseBroadcast(const int32_t *input0_data, int32_t input0_dims,
                                         const int32_t *input1_data, int32_t input1_dims, NdArrayDesc *desc0_out,
                                         NdArrayDesc *desc1_out, uint32_t num_dims);
int SubscriptToIndex(const NdArrayDesc desc, int i0, int i1, int i2, int i3);
int SubscriptToIndexArr5(const NdArrayDesc desc, const int* indexes);
bool lc(int end, int stride, int index);
int Clamp(const int v, const int lo, const int hi);

StridedSliceParams BuildStridedSliceParams(const int32_t* begin, const int32_t* end,
  const int32_t* strides, TfLiteStridedSliceParams str_slc_params, int32_t in_dims);
void StridedSlicePadIndices(StridedSliceParams* p, int in_dims,
                                   int dim_count);
int StridedSliceStartForAxis(const StridedSliceParams* params,
                                    const int32_t* input_shape,
                                    int32_t axis);
int StridedSliceEndForAxis(const StridedSliceParams* params,
                                  const int32_t* input_shape, int axis,
                                  int start);
                                  
int32_t Max(int32_t op1, int32_t op2);
int32_t Min(int32_t op1, int32_t op2);
float MaxFp(float op1, float op2);
float MinFp(float op1, float op2);
int32_t MultiplyByQuantizedMultiplierSmallerThanOneExp(int32_t x,
  int32_t quantized_multiplier, int shift);
int32_t GetNearestNeighbor(const int input_value, const int32_t input_size, const int32_t output_size,
  const bool align_corners, const bool half_pixel_centers);

void GetPadding(const int32_t* data, int offset, int64_t* left_pad,
                       int64_t* right_pad);
int GetInputDimension(int padded_dimension, int left_pad, int right_pad,
                             int input_dim_size, int offset);
int GetFlatIndex(int index, int num_dims,
                 const int32_t *padding_matrix,
                 const int32_t *input_dims,
                 int *output_dims_num_elements, int *input_dims_num_elements,
                 const int offset);

void DequantizeKernelInt8ToFp32BatchWise(const int8_t* kernel, uint32_t kernel_size,
  float *output, const int32_t* kernel_shape, const float* scale, uint32_t scale_dims,
  int32_t zero_point);
void DequantizeKernelInt8ToFp32ChannelWise(const int8_t* kernel, uint32_t kernel_size,
  float *output, const int32_t* kernel_shape, const float* scale, uint32_t scale_dims,
  int32_t zero_point);

#endif /*  __KERNEL_LIBRARY_UTILS__ */
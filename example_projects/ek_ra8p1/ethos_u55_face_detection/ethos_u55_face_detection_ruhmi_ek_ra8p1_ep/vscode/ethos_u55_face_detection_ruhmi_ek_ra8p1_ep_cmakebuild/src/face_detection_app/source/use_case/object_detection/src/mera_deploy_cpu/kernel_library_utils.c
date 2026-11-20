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

#include "kernel_library_utils.h"

/****************************************************************************/
/*                  Generic/Common Op Utility Functions                  */
/****************************************************************************/

// Ported from TF
extern inline bool ResolveAxis(const int num_dims, const int* axis, const int num_axis,
  int* out_axis, int* out_num_axis) {
  *out_num_axis = 0;  // Just in case.
  // Short-circuit axis resolution for scalars; the axis will go unused.
  if (num_dims == 0) {
    return true;
  }
  // o(n^2) is fine since out_num_axis should be really small, mostly <= 4
  for (int idx = 0; idx < num_axis; ++idx) {
    // Handle negative index. A positive index 'p_idx' can be represented as a
    // negative index 'n_idx' as: n_idx = p_idx-num_dims
    // eg: For num_dims=3, [0, 1, 2] is the same as [-3, -2, -1]  */
    int current = axis[idx] < 0 ? (axis[idx] + num_dims) : axis[idx];
    if (current < 0 || current >= num_dims) {
      return false;
    }
    bool is_dup = false;
    for (int j = 0; j < *out_num_axis; ++j) {
      if (out_axis[j] == current) {
        is_dup = true;
        break;
      }
    }
    if (!is_dup) {
      out_axis[*out_num_axis] = current;
      *out_num_axis += 1;
    }
  }
  return true;
}

// Copies dims to desc, calculating strides.
void CopyDimsToDesc(const int32_t* input_shape, NdArrayDesc* desc_out,
  int32_t num_dims) {
  int desc_stride = 1;
  for (int i = num_dims - 1; i >= 0; --i) {
    desc_out->extents[i] = input_shape[i];
    desc_out->strides[i] = desc_stride;
    desc_stride *= input_shape[i];
  }
}

extern inline void ExtendShapeSpaceToBatch(const int32_t* input_dims, int32_t input_num_dims,
  int32_t* extended_dims) {
  memcpy(extended_dims, input_dims, input_num_dims * sizeof(int32_t));
  if (input_num_dims != 4) {
    extended_dims[2] = 1;
    extended_dims[3] = input_dims[2];
  }
}

extern inline void ExtendShapeBatchToSpace(const int32_t* input_dims, int32_t input_num_dims,
  int32_t* extended_dims) {
  memcpy(extended_dims, input_dims, input_num_dims * sizeof(int32_t));
  if (input_num_dims != 4) {
    extended_dims[2] = 1;
    extended_dims[3] = input_dims[2];
  }
}

void ExtendedShape(const int32_t* input_dims, int32_t input_num_dims,
  int32_t* extended_dims, int32_t extended_num_dims) {
  int size_diff = extended_num_dims - input_num_dims;
  for (int i=0; i < extended_num_dims; i++) {
    if (i < size_diff)
      extended_dims[i] = 1;
    else
      extended_dims[i] = input_dims[i-size_diff];
  }
}

int FlatSize(const int32_t* shape, int32_t dims) {
    int buffer_size = 1;
    for (int i = 0; i < dims; i++) {
      buffer_size *= shape[i];
    }
    return buffer_size;
}

bool HaveSameShapes(const int32_t* input1, int32_t input1_dims,
  const int32_t* input2, int32_t input2_dims) {
  if (input1_dims != input2_dims)
    return false;
  for (int i=0; i<input1_dims; i++) {
    if (input1[i] != input2[i]) return false;
  }
  return true;
}

extern inline int FlatSizeSkipDim(const int* dims, int dims_count, int skip_dim) {
  int flat_size = 1;
  for (int i = 0; i < dims_count; ++i) {
    flat_size *= (i == skip_dim) ? 1 : dims[i];
  }
  return flat_size;
}

extern inline int64_t SignedIntMin(const int64_t a, const int64_t b) {
  return ((a < b) ? a : b);
}

extern inline int64_t SignedIntMax(const int64_t a, const int64_t b) {
  return ((a > b) ? a : b);
}

//Utility function for Mean
extern inline uint32_t ReducedOutputOffset(const int num_dims, const int* dims,
  const int* index, const int num_axis, const int* axis) {
  uint32_t offset = 0;
  for (int idx = 0; idx < num_dims; ++idx) {
    // if we need to skip this axis
    bool is_axis = false;
    if (axis != NULL) {
      for (int axis_idx = 0; axis_idx < num_axis; ++axis_idx) {
        if (idx == axis[axis_idx]) {
          is_axis = true;
          break;
        }
      }
    }
    if (!is_axis) {
      offset = offset * (uint32_t)dims[idx] + (uint32_t)index[idx];
    }
  }
  return offset;
}

//Utility function for Mean
extern inline bool NextIndex(const int num_dims, const int* dims, int* current) {
  int carry = 1;
  for (int idx = num_dims - 1; idx >= 0; --idx) {
    int current_val = current[idx] + carry;
    if (dims[idx] == current_val) {
      current[idx] = 0;
    } else {
      current[idx] = current_val;
      carry = 0;
      break;
    }
  }
  return (carry == 0);
}

//Utility function
int CountLeadingZeros(int64_t integer_input) {
  if (integer_input == 0) {
    return 64;
  }
  //63 is the number of numeric digits for a 64 bit integer
  int64_t one_in_leading_positive = (int64_t) 1 << (63 - 1);
  int leading_zeros = 0;
  while (integer_input < one_in_leading_positive) {
    integer_input <<= 1;
    ++leading_zeros;
  }
  return leading_zeros;
}

//Utility function
extern inline int32_t Offset(const int32_t* dims, int i0, int i1, int i2, int i3) {
  return ((i0 * dims[1] + i1) * dims[2] + i2) * dims[3] + i3;
}

//Utility function
int32_t MultiplyByQuantizedMultiplier(int32_t x, int64_t quantized_multiplier,
  int shift) {
  const int64_t total_shift = 31 - shift;
  const int64_t round = (int64_t)(1) << (total_shift - 1);
  int64_t result = x * (int64_t)(quantized_multiplier) + round;
  result = result >> total_shift;
  return (int32_t)(result);
}

//Utility function
extern inline void ComputeInterpolationValuesInteger(
    const int32_t value, const int32_t scale_10, const bool half_pixel_centers,
    int32_t input_size, int32_t* scaled_value, int32_t* lower_bound,
    int32_t* upper_bound) {
  if (half_pixel_centers) {
    *scaled_value = value * scale_10 + scale_10 / 2 - (1 << 9);
  } else {
    *scaled_value = value * scale_10;
  }

  int32_t zero = 0;
  int32_t lb_temp = (*scaled_value / (1 << 10));
  *lower_bound = (lb_temp > zero) ? lb_temp : zero;
  int32_t ub_temp = (*scaled_value + (1 << 10) - 1) / (1 << 10);
  *upper_bound = (ub_temp < input_size - 1) ? ub_temp : input_size - 1 ;
}

void NdArrayDescsForElementwiseBroadcast(const int32_t* input0_data, int32_t input0_dims,
  const int32_t* input1_data, int32_t input1_dims, NdArrayDesc* desc0_out,
  NdArrayDesc* desc1_out, uint32_t num_dims) {
  int32_t extended_input0_shape[num_dims];
  int32_t extended_input1_shape[num_dims];
  int32_t N = num_dims;

  ExtendedShape(input0_data, input0_dims, extended_input0_shape, N);
  ExtendedShape(input1_data, input1_dims, extended_input1_shape, N);

  // Copy dims to desc, calculating strides.
  CopyDimsToDesc(extended_input0_shape, desc0_out, N);
  CopyDimsToDesc(extended_input1_shape, desc1_out, N);

  // Walk over each dimension. If the extents are equal do nothing.
  // Otherwise, set the desc with extent 1 to have extent equal to the other and
  // stride 0.
  for (int i = 0; i < N; ++i) {
    const int extent0 = extended_input0_shape[i];
    const int extent1 = extended_input1_shape[i];
    if (extent0 != extent1) {
      if (extent0 == 1) {
        desc0_out->strides[i] = 0;
        desc0_out->extents[i] = extent1;
      } else {
        desc1_out->strides[i] = 0;
        desc1_out->extents[i] = extent0;
      }
    }
  }
}

int SubscriptToIndex(const NdArrayDesc desc, int i0, int i1, int i2,int i3) {
  return i0 * desc.strides[0] + i1 * desc.strides[1] + i2 * desc.strides[2] +
         i3 * desc.strides[3];
}

extern inline int SubscriptToIndexArr5(const NdArrayDesc desc, const int* indexes) {
  return indexes[0] * desc.strides[0] + indexes[1] * desc.strides[1] +
         indexes[2] * desc.strides[2] + indexes[3] * desc.strides[3] +
         indexes[4] * desc.strides[4];
}

extern inline bool lc(int end, int stride, int index) {
  if (stride < 0) {
    return index > end;
  } else {
    return index < end;
  }
}

extern inline int Clamp(const int v, const int lo, const int hi) {
  if (hi < v) return hi;
  if (v < lo) return lo;
  return v;
}

// Supporting function for Strided Slice op
StridedSliceParams BuildStridedSliceParams(const int32_t* begin, const int32_t* end,
  const int32_t* strides, TfLiteStridedSliceParams str_slc_params, int32_t in_dims) {

  StridedSliceParams op_params={0};
  op_params.start_indices_count = in_dims;
  op_params.stop_indices_count = in_dims;
  op_params.strides_count = in_dims;

  for (int i = 0; i < in_dims; ++i) {
    op_params.start_indices[i] = begin[i];
    op_params.stop_indices[i] = end[i];
    op_params.strides[i] = strides[i];
  }

  op_params.begin_mask = str_slc_params.begin_mask;
  op_params.ellipsis_mask = 0;
  op_params.end_mask = str_slc_params.end_mask;
  op_params.new_axis_mask = 0;
  op_params.shrink_axis_mask = str_slc_params.shrink_axis_mask;
  return op_params;
}

// Supporting function for Strided Slice op
extern inline void StridedSlicePadIndices(StridedSliceParams* p, int in_dims, int dim_count) {
  (void)in_dims;

  // Add indices and mask bits to fully include extra dimensions
  const int pad_count = dim_count - p->start_indices_count;

  // Pad indices at start, so move arrays by pad_count.
  for (int i = p->start_indices_count - 1; i >= 0; --i) {
    p->strides[i + pad_count] = p->strides[i];
    p->start_indices[i + pad_count] = p->start_indices[i];
    p->stop_indices[i + pad_count] = p->stop_indices[i];
  }
  for (int i = 0; i < pad_count; ++i) {
    p->start_indices[i] = 0;
    p->stop_indices[i] = 1;
    p->strides[i] = 1;
  }

  // Pad masks with 0s or 1s as required.
  p->shrink_axis_mask <<= pad_count;
  p->ellipsis_mask <<= pad_count;
  p->new_axis_mask <<= pad_count;
  p->begin_mask <<= pad_count;
  p->end_mask <<= pad_count;
  p->begin_mask |= (1 << pad_count) - 1;
  p->end_mask |= (1 << pad_count) - 1;

  p->start_indices_count = dim_count;
  p->stop_indices_count = dim_count;
  p->strides_count = dim_count;
}

// Supporting function for Strided Slice op
extern inline int StridedSliceStartForAxis(const StridedSliceParams* params, const int32_t* input_shape,
  int32_t axis) {
  const int32_t axis_size = input_shape[axis];
  int32_t start = params->start_indices[axis];
  const int32_t stride = params->strides[axis];
  const int32_t begin_mask = (params->begin_mask & 1 << axis);
  if (start < 0) {
    start += axis_size;
  }
  if (stride > 0) {
    start = Clamp(start, 0, axis_size);
  } else {
    start = Clamp(start, -1, axis_size - 1);
  }
  if (begin_mask) {
    if (stride > 0) {
      start = 0;
    } else {
      start = axis_size - 1;
    }
  }
  return start;
}

// Supporting function for Strided Slice op
extern inline int StridedSliceEndForAxis(const StridedSliceParams* params, const int32_t* input_shape,
  int axis, int start) {
  const uint16_t shrink_axis_mask = params->shrink_axis_mask;
  const bool shrink_axis = shrink_axis_mask & (1 << axis);
  const int axis_size = input_shape[axis];
  const bool offset = params->offset;
  if (shrink_axis) {
    if (start >= axis_size) {
      return start;
    } else {
      return start + 1;
    }
  }
  const int32_t* indices = params->stop_indices;
  int end = indices[axis];
  if (offset) {
    end += start;
  }
  const int32_t stride = params->strides[axis];
  const int32_t end_mask = (params->end_mask & 1 << axis);
  if (end < 0) {
    end += axis_size;
  }
  if (stride > 0) {
    end = Clamp(end, 0, axis_size);
  } else {
    end = Clamp(end, -1, axis_size - 1);
  }
  if (end_mask) {
    if (stride > 0) {
      end = axis_size;
    } else {
      end = -1;
    }
  }
  return end;
}

// Utility Function
int32_t Max(int32_t op1, int32_t op2) {
  return ( (op1 >= op2) ? (op1) : (op2) );
}

// Utility Function
int32_t Min(int32_t op1, int32_t op2) {
  return ( (op1 <= op2) ? (op1) : (op2) );
}

// Utility Function
float MaxFp(float op1, float op2) {
  return ( (op1 >= op2) ? (op1) : (op2) );
}

// Utility Function
float MinFp(float op1, float op2) {
  return ( (op1 <= op2) ? (op1) : (op2) );
}

extern inline int32_t MultiplyByQuantizedMultiplierSmallerThanOneExp(
    int32_t x, int32_t quantized_multiplier, int shift) {
  return MultiplyByQuantizedMultiplier(x, quantized_multiplier, shift);
}

extern inline int32_t GetNearestNeighbor(const int input_value, const int32_t input_size, const int32_t output_size,
  const bool align_corners, const bool half_pixel_centers) {
  const float scale =
    (align_corners && output_size > 1)
      ? (input_size - 1) / (float) (output_size - 1)
      : input_size / (float) output_size;
  const float offset = half_pixel_centers ? 0.5f : 0.0f;
  int32_t output_value = SignedIntMin(
    align_corners
      ? (int32_t)(roundf((input_value + offset) * scale))
      : (int32_t)(floor((input_value + offset) * scale)),
    input_size - 1);
  if (half_pixel_centers) {
    output_value = SignedIntMax((int32_t)0, output_value);
  }
  return output_value;
}

// Helper method for MirrorPad that fills the left and right pads.
extern inline void GetPadding(const int32_t* data, int offset, int64_t* left_pad,
                       int64_t* right_pad) {
  *left_pad = (int64_t)(*(data + offset * 2));
  *right_pad = (int64_t)(*(data + offset * 2 + 1));
}

// Helper method for MirrorPad
// Given dimension index and the left/right padding.
// Returns the corresponding dimension in the input array.
extern inline int GetInputDimension(int padded_dimension, int left_pad, int right_pad,
                             int input_dim_size, int offset) {
  (void)right_pad;
  if (padded_dimension < left_pad) {
    const int original_ind = left_pad + offset - 1;
    int diffindoffset = original_ind - offset;
    int min_paddim_diffindoffset = (padded_dimension < diffindoffset) ? padded_dimension : diffindoffset;
    return original_ind - min_paddim_diffindoffset;
  }
  padded_dimension -= left_pad;
  if (padded_dimension >= input_dim_size) {
    padded_dimension -= input_dim_size;
    const int original_ind = input_dim_size - (1 + offset);
    int min_paddim_origind = (padded_dimension < original_ind) ? padded_dimension : original_ind;
    return original_ind - min_paddim_origind;
  }
  return padded_dimension;
}

// Helper method for MirrorPad
// Given and index in output array, returns the index of the value
// in input array.
extern inline int GetFlatIndex(int index, int num_dims,
                 const int32_t* padding_matrix,
                 const int32_t* input_dims,
                 int* output_dims_num_elements, int* input_dims_num_elements,
                 const int offset) {
  int flat_index = 0;
  int64_t left_pad = 0, right_pad = 0, dimension_index, index_in_input;

  for (int i = 0; i < num_dims; ++i) {
    GetPadding(padding_matrix, i, &left_pad, &right_pad);

    dimension_index = index / output_dims_num_elements[i];

    index_in_input = GetInputDimension(dimension_index, left_pad, right_pad,
                                       input_dims[i], offset);

    flat_index += index_in_input * (input_dims_num_elements)[i];
    index %= output_dims_num_elements[i];
  }

  return flat_index;
}

// Helper function for ConvFp, FullyconnectedFp
void DequantizeKernelInt8ToFp32BatchWise(const int8_t* kernel, uint32_t kernel_size,
  float *output, const int32_t* kernel_shape, const float* scale, uint32_t scale_dims,
  int32_t zero_point) {

  if(scale_dims > 1) {
    int batches = kernel_shape[0], height = kernel_shape[1], width = kernel_shape[2], channels = kernel_shape[3];
    for (int n = 0; n < batches; n++) {
        for (int h = 0; h < height; h++) {
            for (int w = 0; w < width; w++) {
                for (int c = 0; c < channels; c++) {
                    int idx = ((n * height + h) * width + w) * channels + c;
                    output[idx] = scale[n] * (kernel[idx] - zero_point);
                }
            }
        }
    }
  }
  else {
    for (uint32_t i = 0; i < kernel_size; i++) {
      const int32_t val = kernel[i];
      output[i] = scale[0] * (val - zero_point);
    }
  }
}

// Helper function for DepthwiseconvFp
void DequantizeKernelInt8ToFp32ChannelWise(const int8_t* kernel, uint32_t kernel_size,
  float *output, const int32_t* kernel_shape, const float* scale, uint32_t scale_dims,
  int32_t zero_point) {

  if(scale_dims > 1) {
    int batches = kernel_shape[0], height = kernel_shape[1], width = kernel_shape[2], channels = kernel_shape[3];
    for (int n = 0; n < batches; n++) {
        for (int h = 0; h < height; h++) {
            for (int w = 0; w < width; w++) {
                for (int c = 0; c < channels; c++) {
                    int idx = ((n * height + h) * width + w) * channels + c;
                    output[idx] = scale[c] * (kernel[idx] - zero_point);
                }
            }
        }
    }
  }
  else {
    for (uint32_t i = 0; i < kernel_size; i++) {
      const int32_t val = kernel[i];
      output[i] = scale[0] * (val - zero_point);
    }
  }
}


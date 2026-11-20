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

#include "kernel_library_int.h"

/****************************************************************************/
/*                               Integer Operators                          */
/****************************************************************************/

extern inline void AffineQuantizeFloatToInt8(const float* input, int8_t* output, uint32_t size,
  int8_t output_zp, float output_scale) {
  const int32_t int8_min = INT8_MIN;
  const int32_t int8_max = INT8_MAX;
  for (uint32_t i = 0; i < size; i++) {
    int32_t res = (int32_t) roundf(input[i] / output_scale) + output_zp;
    res = (res > int8_min ? res : int8_min);
    res = (res < int8_max ? res : int8_max);
    output[i] = res;
  }
}

extern inline void AffineDequantizeUInt8ToFloat(const uint8_t* input, float* output, uint32_t size,
  int8_t input_zp, float input_scale) {
  for (uint32_t i = 0; i < size; i++) {
    const int32_t val = input[i];
    output[i] = input_scale * (val - input_zp);
  }
}

extern inline void AffineDequantizeInt8ToFloat(const int8_t* input, float* output, uint32_t size,
  int8_t input_zp, float input_scale) {
  for (uint32_t i = 0; i < size; i++) {
    const int32_t val = input[i];
    output[i] = input_scale * (val - input_zp);
  }
}

void Requantize(const uint8_t* input, int8_t* output, uint32_t size, int32_t multiplier, int32_t shift,
  int32_t input_zp, int32_t output_zp) {
  (void)multiplier;
  (void)shift;
  (void)input_zp;
  (void)output_zp;
  for (uint32_t i = 0; i < size; ++i) {
    output[i] = input[i] ^ 0x80;
  }
}

void RequantizeInt8ToUint8(const int8_t* input_data, uint8_t* output_data,
  uint32_t size, int32_t multiplier, int32_t shift, int32_t input_zp,
  int32_t output_zp) {

  const bool same_scale =  (multiplier == 1 << 30 && shift == 1);
  if (same_scale) {
    int32_t zero_point_diff = input_zp - output_zp;
    // Fast path to do requantization for the case when just a shift of 128 is
    // needed.
    if ((zero_point_diff == -128) || (zero_point_diff == 128)) {
      for (uint32_t i = 0; i < size; ++i) {
        output_data[i] = input_data[i] ^ 0x80;
      }
      return;
    }
  }
  for (uint32_t i = 0; i < size; ++i) {
    int32_t input = input_data[i] - input_zp;
    int32_t output = MultiplyByQuantizedMultiplier(input, multiplier,
                               shift) + output_zp;
    output = (output < UINT8_MAX ) ? output : UINT8_MAX;
    output = (output > 0 ) ? output : 0;
    output_data[i] = (uint8_t)output;
  }
}

void RequantizeInt8ToInt8(const int8_t* input_data, int8_t* output_data,
  uint32_t size, int32_t multiplier, int32_t shift, int32_t input_zp,
  int32_t output_zp) {

  for (uint32_t i = 0; i < size; ++i) {
    int32_t input = input_data[i] - input_zp;
    int32_t output = MultiplyByQuantizedMultiplier(input, multiplier,
                               shift) + output_zp;
    output = (output < INT8_MAX ) ? output : INT8_MAX;
    output = (output > INT8_MIN ) ? output : INT8_MIN;
    output_data[i] = (int8_t)output;
  }
}

void Sigmoid(const int8_t *input, int8_t *output, uint32_t size, 
  int32_t input_zp, float input_scale, int32_t output_zp, float output_scale) {
  const float cutoff_upper = 16.619047164916992188f;
  const float cutoff_lower = -9.f;
  float dq_buffer[size];

  AffineDequantizeInt8ToFloat(input, dq_buffer, size, input_zp, input_scale);

  for (uint32_t i = 0; i < size; i++) {
    float val = dq_buffer[i];
    float result;
    if (val > cutoff_upper) {
      result = 1.0f;
    } else if (val < cutoff_lower) {
      result = expf(val);
    } else {
      result = 1.f / (1.f + expf(-val));
    }
    dq_buffer[i] = result;
  }

  AffineQuantizeFloatToInt8(dq_buffer, output, size, output_zp, output_scale);
}

void Minimum(const int8_t* input_1, const int8_t* input_2,  int8_t* output,
  const int32_t* input1_shape, int32_t input1_dims, const int32_t* input2_shape,
  int32_t input2_dims, const int32_t* output_shape, int32_t output_dims) {
  bool simple_mode = false;

  // Broadcast if all the dimensions of the input 1 & 2 don't match
  if (input1_dims == input2_dims) {
    for (int i=0; i < input1_dims; i++) {
      if (input1_shape[i] != input2_shape[i]) {
        simple_mode = false;
      } else {
        simple_mode = true;
      }
    }
  }

  if (simple_mode) {
    int flat_size = FlatSize(input1_shape, input1_dims);
    for (int i = 0; i < flat_size; ++i) {
      output[i] = (input_1[i] < input_2[i]) ? input_1[i] : input_2[i];
    }
  } else {
    NdArrayDesc desc1, desc2, output_desc;
    int32_t extended_out_shape[5];

    NdArrayDescsForElementwiseBroadcast(input1_shape, input1_dims, input2_shape,
      input2_dims, &desc1, &desc2, 5);
    ExtendedShape(output_shape, output_dims, extended_out_shape, 5);
    CopyDimsToDesc(extended_out_shape, &output_desc, 5);

    int indexes[5] = {0};
    for (indexes[0] = 0; indexes[0] < output_desc.extents[0]; ++indexes[0]) {
      for (indexes[1] = 0; indexes[1] < output_desc.extents[1]; ++indexes[1]) {
        for (indexes[2] = 0; indexes[2] < output_desc.extents[2]; ++indexes[2]) {
          for (indexes[3] = 0; indexes[3] < output_desc.extents[3]; ++indexes[3]) {
            for (indexes[4] = 0; indexes[4] < output_desc.extents[4]; ++indexes[4]) {
              int8_t inp_1 = input_1[SubscriptToIndexArr5(desc1, indexes)];
              int8_t inp_2 = input_2[SubscriptToIndexArr5(desc2, indexes)];
              output[SubscriptToIndexArr5(output_desc, indexes)] =
                  inp_1 < inp_2 ? inp_1 : inp_2;
            }
          }
        }
      }
    }
  }
}

// Ported from Renesas library
void Tanh(const int8_t* input, int8_t* output, uint32_t size, float input_scale, int8_t input_zp,
  float output_scale, int8_t output_zp) {
  float transit_out[size], working_val, floor_val, diff;

  for (uint32_t i = 0; i < size; i++) {
    working_val = (input[i] - input_zp) * input_scale;
    transit_out[i] = (float) (1+exp(-2*working_val));
  }
  for (uint32_t i = 0; i < size; i++) {
    transit_out[i] = 2/transit_out[i]-1;
    working_val = ((transit_out[i]/output_scale)+output_zp);
    floor_val = (float)(floor(working_val));
    diff = working_val - floor_val;

    if (diff < 0.5f) {
      working_val = floor_val;
    } else {
      working_val = floor_val + 1.0f;
    }

    if (working_val < -128) {
      output[i] = -128;
    } else if(working_val > 127) {
      output[i] = 127;      // output
    } else {
      output[i] = (int8_t)working_val;
    }
  }
}

// Adaptation from Renesas library
void Slice(const int8_t* input, int8_t* output, const int32_t* begin, const int32_t* size,
  const int32_t* in_shape, int32_t dims) {
  if (dims == 4) {
    int n_offset = in_shape[1] * in_shape[2] * in_shape[3];
    int h_offset = in_shape[2] * in_shape[3];
    int w_offset = in_shape[3];
    int end[4] = {
      ((size[0] == -1) ? in_shape[0] : begin[0]+size[0]), 
      ((size[1] == -1) ? in_shape[1] : begin[1]+size[1]), 
      ((size[2] == -1) ? in_shape[2] : begin[2]+size[2]), 
      ((size[3] == -1) ? in_shape[3] : begin[3]+size[3])
    };

    for (int iItr0 = begin[0]; iItr0 < end[0]; iItr0++) {
      for (int iItr1 = begin[1]; iItr1 < end[1]; iItr1++) {
        for (int iItr2 = begin[2]; iItr2 < end[2]; iItr2++) {
          for (int iItr3 = begin[3]; iItr3 < end[3]; iItr3++) {
            *(output++) =  input[ (iItr0 * n_offset) + (iItr1 * h_offset) + (iItr2 * w_offset) + (iItr3)];
          }
        }
      }
    }
  } else if (dims == 3) {
    int h_offset = in_shape[1] * in_shape[2];
    int w_offset = in_shape[2];
    int end[3] = {
      ((size[0] == -1) ? in_shape[0] : begin[0]+size[0]), 
      ((size[1] == -1) ? in_shape[1] : begin[1]+size[1]), 
      ((size[2] == -1) ? in_shape[2] : begin[2]+size[2])
    };

    for (int iItr0 = begin[0]; iItr0 < end[0]; iItr0++) {
      for (int iItr1 = begin[1]; iItr1 < end[1]; iItr1++) {
        for (int iItr2 = begin[2]; iItr2 < end[2]; iItr2++) {
          *(output++) =  input[(iItr0 * h_offset) + (iItr1 * w_offset) + (iItr2)];
        }
      }
    }
  } else {
    int w_offset = in_shape[1];
    int end[2] = {
      ((size[0] == -1) ? in_shape[0] : begin[0]+size[0]), 
      ((size[1] == -1) ? in_shape[1] : begin[1]+size[1])
    };

    for (int iItr0 = begin[0]; iItr0 < end[0]; iItr0++) {
      for (int iItr1 = begin[1]; iItr1 < end[1]; iItr1++) {
        *(output++) =  input[(iItr0 * w_offset) + (iItr1 )];
      }
    }
  }
}

// Supporting function for Add, SquaredDifference ops
bool ReduceDimensionsForBroadcast(const int32_t* input1_dims, int32_t num_input1_dims,
  const int32_t* input2_dims, int32_t num_input2_dims, size_t* compressed_input1_stride,
  size_t* compressed_input2_stride, size_t* compressed_output_shape) {

  int num_compressed_dims = 0;
  int compressed_input1_shape[kMaxBroadcastDim] = {1, 1, 1, 1, 1, 1};
  int compressed_input2_shape[kMaxBroadcastDim] = {1, 1, 1, 1, 1, 1};
  bool broadcast_input1 = false;
  bool broadcast_input2 = false;
  bool first_nonunit = true;
  const int32_t num_common_dims = (num_input1_dims < num_input2_dims) ? num_input1_dims
                                      : num_input2_dims;
  for (int32_t i = 1; i <= num_common_dims; i++) {
    const int32_t input1_dim = input1_dims[num_input1_dims - i];
    const int32_t input2_dim = input2_dims[num_input2_dims - i];
    if (input1_dim == 0 || input2_dim == 0) {
      return false;
    }
    if (input1_dim == 1 && input2_dim == 1) {
      continue;
    }

    if (input1_dim == 1) {
      if (!broadcast_input1) {
        broadcast_input1 = true;
        broadcast_input2 = false;
        num_compressed_dims++;
      }
      compressed_input2_shape[num_compressed_dims - 1] *= input2_dim;
      compressed_output_shape[num_compressed_dims - 1] *= input2_dim;
    } else if (input2_dim == 1) {
      if (!broadcast_input2) {
        broadcast_input1 = false;
        broadcast_input2 = true;
        num_compressed_dims++;
      }
      compressed_input1_shape[num_compressed_dims - 1] *= input1_dim;
      compressed_output_shape[num_compressed_dims - 1] *= input1_dim;
    } else {
      if (broadcast_input1 || broadcast_input2 || first_nonunit) {
        broadcast_input1 = false;
        broadcast_input2 = false;
        num_compressed_dims++;
      }
      compressed_input1_shape[num_compressed_dims - 1] *= input1_dim;
      compressed_input2_shape[num_compressed_dims - 1] *= input1_dim;
      compressed_output_shape[num_compressed_dims - 1] *= input1_dim;
    }
    first_nonunit = false;
  }
  if (num_input1_dims > num_input2_dims) {
    if (!broadcast_input2) {
      num_compressed_dims++;
    }
    for (int32_t i = 0; i < num_input1_dims - num_input2_dims; i++) {
      const int32_t input1_dim = input1_dims[i];
      if (input1_dim == 0) {
        return false;
      }
      compressed_input1_shape[num_compressed_dims - 1] *= input1_dim;
      compressed_output_shape[num_compressed_dims - 1] *= input1_dim;
    }
  } else if (num_input2_dims > num_input1_dims) {
    if (!broadcast_input1) {
      num_compressed_dims++;
    }
    for (int32_t i = 0; i < num_input2_dims - num_input1_dims; i++) {
      const int32_t input2_dim = input2_dims[i];
      if (input2_dim == 0) {
        return false;
      }
      compressed_input2_shape[num_compressed_dims - 1] *= input2_dim;
      compressed_output_shape[num_compressed_dims - 1] *= input2_dim;
    }
  }
  num_compressed_dims = (num_compressed_dims > 1) ? num_compressed_dims : 1;

  int input1_stride = 1;
  int input2_stride = 1;
  for (int i = 0; i < kMaxBroadcastDim; ++i) {
    compressed_input1_stride[i] = input1_stride;
    input1_stride *= compressed_input1_shape[i];
    compressed_input2_stride[i] = input2_stride;
    input2_stride *= compressed_input2_shape[i];
  }
  for (int i = 0; i < kMaxBroadcastDim; ++i) {
    if (compressed_input1_shape[i] != compressed_input2_shape[i]) {
      if (compressed_input1_shape[i] == 1) {
        compressed_input1_stride[i] = 0;
      } else {
        compressed_input2_stride[i] = 0;
      }
    }
  }
  return true;
}

// Supporting function for Add op
int8_t AddOp(int8_t input1, int8_t input2, ArithmeticParams quant_params) {

  const int32_t input1_val = quant_params.input1_offset + input1;
  const int32_t input2_val = quant_params.input2_offset + input2;
  const int32_t shifted_input1_val = input1_val * (1 << quant_params.left_shift);
  const int32_t shifted_input2_val = input2_val * (1 << quant_params.left_shift);

  const int32_t scaled_input1_val = MultiplyByQuantizedMultiplier(shifted_input1_val,
                                    quant_params.input1_multiplier, quant_params.input1_shift);
  const int32_t scaled_input2_val = MultiplyByQuantizedMultiplier(shifted_input2_val,
                                    quant_params.input2_multiplier, quant_params.input2_shift);

  const int32_t raw_sum = scaled_input1_val + scaled_input2_val;
  const int32_t raw_output = MultiplyByQuantizedMultiplier(raw_sum, quant_params.output_multiplier,
                              quant_params.output_shift) + quant_params.output_offset;
  const int32_t clamped_output = Min(quant_params.quantized_activation_max,
                                     Max(quant_params.quantized_activation_min, raw_output));
  return (int8_t)(clamped_output);
}

// Supporting function for Add op
void BroadcastInput1(const int8_t* input1, const int8_t* input2, int8_t* output,
  ArithmeticParams quant_params, int size) {

  for (int i = 0; i < size; ++i) {
    output[i] = AddOp(input1[0], input2[i], quant_params);
  }
}

// Supporting function for Add op
void BroadcastInput2(const int8_t* input1, const int8_t* input2, int8_t* output,
  ArithmeticParams quant_params, int size) {

  for (int i = 0; i < size; ++i) {
    output[i] = AddOp(input1[i], input2[0], quant_params);
  }
}

// Supporting function for Add op
void ElementWise(const int8_t* input1, const int8_t* input2, int8_t* output,
  ArithmeticParams quant_params, int size) {

  for (int i = 0; i < size; ++i) {
    output[i] = AddOp(input1[i], input2[i], quant_params);
  }
}

// Supporting function for Add op
extern inline void BroadcastAddRecursiveDimensions(const int8_t* input1, size_t* input1_offset_p,
  size_t* compressed_input1_stride, const int8_t* input2, size_t* input2_offset_p,
  size_t* compressed_input2_stride, int8_t* output, size_t* output_offset,
  size_t* compressed_output_shape, ArithmeticParams quant_params, int dimension) {
  
  if (dimension > 0) {
    for (size_t c = 0; c < compressed_output_shape[dimension]; ++c) {
      size_t input1_offset_c = *input1_offset_p;
      size_t input2_offset_c = *input2_offset_p;
      BroadcastAddRecursiveDimensions(input1, &input1_offset_c, compressed_input1_stride, input2, &input2_offset_c,
        compressed_input2_stride, output, output_offset, compressed_output_shape, quant_params, dimension - 1);

      *input1_offset_p += compressed_input1_stride[dimension];
      *input2_offset_p += compressed_input2_stride[dimension];
    }
  } else {
    bool input1_is_broadcast = compressed_input1_stride[dimension] == 0;
    bool input2_is_broadcast = compressed_input2_stride[dimension] == 0;
    const int8_t* input1_ptr = input1 + *input1_offset_p;
    const int8_t* input2_ptr = input2 + *input2_offset_p;
    int8_t* output_ptr = output + *output_offset;
    if (input1_is_broadcast) {
      // input1 is broadcast.
      BroadcastInput1(input1_ptr, input2_ptr, output_ptr, quant_params, compressed_output_shape[dimension]);

      *input2_offset_p += compressed_output_shape[dimension];
    } else if (input2_is_broadcast) {
      // input2 is broadcast.
      BroadcastInput2(input1_ptr, input2_ptr, output_ptr, quant_params, compressed_output_shape[dimension]);
      *input1_offset_p += compressed_output_shape[dimension];
    } else {
      // Add element-wise.
      ElementWise(input1_ptr, input2_ptr, output_ptr, quant_params, compressed_output_shape[dimension]);
      *input1_offset_p += compressed_output_shape[dimension];
      *input2_offset_p += compressed_output_shape[dimension];
    }
    *output_offset += compressed_output_shape[dimension];
  }
} 

// Supporing function for Add op  
void BroadcastBinaryFunction6DSlow(const int8_t* input1, const int32_t* input1_shape, int32_t input1_dims,
  const int8_t* input2, const int32_t* input2_shape, int32_t input2_dims, int8_t* output, const int32_t* output_shape,
  int32_t output_dims, ArithmeticParams quant_params) {
  (void)output_shape;
  (void)output_dims;

  size_t compressed_input1_stride[kMaxBroadcastDim];
  size_t compressed_input2_stride[kMaxBroadcastDim];
  // size_t compressed_output_shape[kMaxBroadcastDim];
  size_t compressed_output_shape[kMaxBroadcastDim] = {1, 1, 1, 1, 1, 1};
  bool broadcastable_shape = ReduceDimensionsForBroadcast(input1_shape, input1_dims, input2_shape, input2_dims,
          compressed_input1_stride, compressed_input2_stride, compressed_output_shape);
  // Skip broadcasting for degenerate shapes.
  if (!broadcastable_shape) {
    return;
  }

  size_t input1_offset = 0;
  size_t input2_offset = 0;
  size_t output_offset = 0;
  BroadcastAddRecursiveDimensions(input1, &input1_offset, compressed_input1_stride,
        input2, &input2_offset, compressed_input2_stride, output, &output_offset,
        compressed_output_shape, quant_params, kMaxBroadcastDim - 1);
}

// Ported from TF
void StridedSlice(const int8_t* input, int8_t* output, const int32_t* begin_orig,
  const int32_t* end_orig, const int32_t* strides, const int32_t* in_shape, int32_t in_dims,
  const int32_t* out_shape, int32_t out_dims, TfLiteStridedSliceParams str_slc_params) {

  StridedSliceParams params_copy = BuildStridedSliceParams(begin_orig, end_orig, strides,
        str_slc_params, in_dims);
  params_copy.offset = 0;

  int32_t input_shape[5];
  int32_t output_shape[5];
  ExtendedShape(in_shape, in_dims, input_shape, 5);
  ExtendedShape(out_shape, out_dims, output_shape, 5);

  // Reverse and pad to 5 dimensions because that is what the runtime code
  // requires (ie. all shapes must be 5D and are given backwards).
  StridedSlicePadIndices(&params_copy, in_dims, 5);

  const int start_0 = StridedSliceStartForAxis(&params_copy, input_shape, 0);
  const int stop_0 = StridedSliceEndForAxis(&params_copy, input_shape, 0, start_0);
  const int start_1 = StridedSliceStartForAxis(&params_copy, input_shape, 1);
  const int stop_1 = StridedSliceEndForAxis(&params_copy, input_shape, 1, start_1);
  const int start_2 = StridedSliceStartForAxis(&params_copy, input_shape, 2);
  const int stop_2 = StridedSliceEndForAxis(&params_copy, input_shape, 2, start_2);
  const int start_3 = StridedSliceStartForAxis(&params_copy, input_shape, 3);
  const int stop_3 = StridedSliceEndForAxis(&params_copy, input_shape, 3, start_3);
  const int start_4 = StridedSliceStartForAxis(&params_copy, input_shape, 4);
  const int stop_4 = StridedSliceEndForAxis(&params_copy, input_shape, 4, start_4);

  // With a static_cast it is not possible to initialize
  // a variable of type 'const int *'
  // with an rvalue of type 'const int32_t *' (aka 'const long *').
  // reinterpret_cast is required to handle this casting.
  const int* shape = input_shape;
  const int* stride = params_copy.strides;
  const bool inner_stride_is_1 = params_copy.strides[4] == 1;
  int output_ptr=0;

  for (int offset_0 = start_0; lc(stop_0, stride[0], offset_0);
       offset_0 += stride[0]) {
    for (int offset_1 = start_1; lc(stop_1, stride[1], offset_1);
         offset_1 += stride[1]) {
      for (int offset_2 = start_2; lc(stop_2, stride[2], offset_2);
           offset_2 += stride[2]) {
        for (int offset_3 = start_3; lc(stop_3, stride[3], offset_3);
             offset_3 += stride[3]) {
          // When the stride is 1, the inner loop is equivalent to the
          // optimized slice inner loop. Otherwise, it is identical to the
          // strided_slice reference implementation inner loop.
          if (inner_stride_is_1) {
            const int len = stop_4 - start_4;
            int index = start_4 + offset_3 * shape[4] +
                        offset_2 * shape[3] * shape[4] +
                        offset_1 * shape[2] * shape[3] * shape[4] +
                        offset_0 * shape[1] * shape[2] * shape[3] * shape[4];
            if (len > 0) {
              memcpy(output+output_ptr, input+index, sizeof(int8_t) * len);
              output_ptr += len;
            }
          } else {
            for (int offset_4 = start_4; lc(stop_4, stride[4], offset_4);
                 offset_4 += stride[4]) {
              int index = offset_4 + offset_3 * shape[4] +
                          offset_2 * shape[3] * shape[4] +
                          offset_1 * shape[2] * shape[3] * shape[4] +
                          offset_0 * shape[1] * shape[2] * shape[3] * shape[4];
              output[output_ptr] = input[index];
              ++output_ptr;
            }
          }
        }
      }
    }
  }
}

void Pack(const int8_t **inputs, uint32_t inp_count, int8_t *output, uint32_t size, 
  uint32_t slice_size, uint32_t stride) {
  (void)inp_count;
  uint32_t out_ind = 0;
  uint32_t inp_ind = 0;
  uint32_t laps = 0;
  const int8_t* curr_inp;

  while (out_ind < size) {
    curr_inp = inputs[inp_ind/stride];

    if (slice_size == 1) {
      output[out_ind] = curr_inp[(inp_ind % stride)];
    } else {
      // TODO: Setup build to include "arm_nnsupportfunctions.h" for this
      #ifdef ARM_NNSUPPORTFUNCTIONS_H
      arm_memcpy_s8(output + out_ind, curr_inp + (inp_ind % stride), slice_size);
      #endif

      #ifndef ARM_NNSUPPORTFUNCTIONS_H
      memcpy(output + out_ind, curr_inp + (inp_ind % stride), slice_size);
      #endif
    }

    out_ind += slice_size;
    inp_ind += stride;
    if (inp_ind >= size) {
      laps++;
      inp_ind = laps * slice_size;
    }
  }
}

void Mean(const int8_t* input, int8_t* output, const int32_t* in_shape, const int32_t in_num_dims,
  int32_t input_zp, int32_t out_size, int32_t output_multiplier, int32_t output_shift, int32_t output_zp,
  const int32_t* axis, const int32_t num_axis) {

  int temp_index[5] = {0};
  int32_t temp_sum[out_size];
  int64_t output_multiplier_mean;

  memset(temp_sum, 0, sizeof(temp_sum));

  // Reduce
  do {
    int32_t input_offset =
        ReducedOutputOffset(in_num_dims, in_shape, temp_index, 0, NULL);
    int32_t output_offset = ReducedOutputOffset(in_num_dims, in_shape,
                                               temp_index, num_axis, axis);
    temp_sum[output_offset] = temp_sum[output_offset] + input[input_offset];
  } while (NextIndex(in_num_dims, in_shape, temp_index));

  // Calculate Mean
  int64_t num_elements_in_axis = 1;
  for (int idx = 0; idx < num_axis; ++idx) {
    int32_t current = in_shape[axis[idx]];
    num_elements_in_axis *= current;
  }

  int shift = 63 - CountLeadingZeros((int64_t)(num_elements_in_axis));
  shift = (shift < 32) ? shift : 32;
  shift = (shift < (31 + output_shift)) ? shift : (31 + output_shift);
  output_multiplier_mean = ((int64_t)output_multiplier << shift) / num_elements_in_axis;
  output_shift = output_shift - shift;

  for (int32_t idx = 0; idx < out_size; ++idx) {
    const int32_t shifted_sum = (int32_t)(temp_sum[idx] - input_zp * num_elements_in_axis);
    int32_t output_val = MultiplyByQuantizedMultiplier(shifted_sum, output_multiplier_mean,
                            output_shift) + output_zp;

    output_val = (output_val > INT8_MIN ) ? output_val : INT8_MIN;
    output_val = (output_val < INT8_MAX ) ? output_val : INT8_MAX;
    output[idx] = (int8_t)output_val;
  }
}

void Sum(const int8_t* input, int8_t* output, const int32_t* in_shape, int32_t in_num_dims,
  int32_t input_zp, int32_t out_size, int32_t output_multiplier, int32_t output_shift,
  int32_t output_zp, const int32_t* axis, int32_t num_axis) {

  int temp_index[kMaxNumberOfAxis]={0};
  int32_t resolved_axis[kMaxNumberOfReducedAxis]={0};
  int32_t temp_sum[out_size];

  memset(temp_sum, 0, out_size * sizeof(int32_t));
  memset(output, 0, out_size * sizeof(int8_t));

  int32_t num_resolved_axis = 0;
  if (!ResolveAxis(in_num_dims, axis, num_axis, resolved_axis,
                    &num_resolved_axis)) {
    return;
  }

  // Reduce
  do {
    uint32_t input_offset =
        ReducedOutputOffset(in_num_dims, in_shape, temp_index, 0, NULL);
    uint32_t output_offset = ReducedOutputOffset(in_num_dims, in_shape,
                              temp_index, num_resolved_axis, resolved_axis);
    temp_sum[output_offset] = temp_sum[output_offset] + input[input_offset];
  } while (NextIndex(in_num_dims, in_shape, temp_index));

  int64_t num_elements_in_axis = 1;
  for (int idx = 0; idx < num_resolved_axis; ++idx) {
    uint32_t current = (uint32_t) in_shape[resolved_axis[idx]];
    num_elements_in_axis *= current;
  }

  for (int32_t idx = 0; idx < out_size; ++idx) {
    const int32_t shifted_sum = (int32_t)(temp_sum[idx] - input_zp * num_elements_in_axis);
    int32_t output_val = MultiplyByQuantizedMultiplier(shifted_sum, (int64_t) output_multiplier,
                            output_shift) + output_zp;

    output_val = (output_val > INT8_MIN ) ? output_val : INT8_MIN;
    output_val = (output_val < INT8_MAX ) ? output_val : INT8_MAX;
    output[idx] = (int8_t)output_val;
  }
}

void ReduceMax(const int8_t* input, int8_t* output, const int32_t* in_shape, const int32_t in_num_dims,
  int32_t out_size, const int32_t* axis, const int32_t num_axis) {
  int resolved_axis[num_axis];

  int32_t num_resolved_axis = 0;
  ResolveAxis(in_num_dims, axis, num_axis, resolved_axis, &num_resolved_axis);

  int temp_index[5] = {0};

  memset(output, INT8_MIN, out_size * sizeof(int8_t));

  // Reduce
  do {
    int32_t input_offset =
        ReducedOutputOffset(in_num_dims, in_shape, temp_index, 0, NULL);
    int32_t output_offset = ReducedOutputOffset(in_num_dims, in_shape,
                                                temp_index, num_resolved_axis, resolved_axis);
    output[output_offset] = SignedIntMax(output[output_offset], input[input_offset]);
  } while (NextIndex(in_num_dims, in_shape, temp_index));
}

void Exp(const int8_t *input, int8_t *output, uint32_t size, 
  int32_t input_zp, float input_scale, int32_t output_zp, float output_scale) {
  float dq_buffer[size];

  AffineDequantizeInt8ToFloat(input, dq_buffer, size, input_zp, input_scale);
  for (uint32_t i = 0; i < size; i++) {
    dq_buffer[i] = expf(dq_buffer[i]);
  }
  AffineQuantizeFloatToInt8(dq_buffer, output, size, output_zp, output_scale);
}

void Log(const int8_t *input, int8_t *output, uint32_t size, 
  int32_t input_zp, float input_scale, int32_t output_zp, float output_scale) {
  float dq_buffer[size];

  AffineDequantizeInt8ToFloat(input, dq_buffer, size, input_zp, input_scale);
  for (uint32_t i = 0; i < size; i++) {
    if (dq_buffer[i] < 0) {
      // Setting it manually to -inf since a NaN is being interpreted as
      // a large +ve val in the MCU, but a large -ve val in the tflite impl
      dq_buffer[i] = -INFINITY;
    } else {
      dq_buffer[i] = logf(dq_buffer[i]);
    }
  }
  AffineQuantizeFloatToInt8(dq_buffer, output, size, output_zp, output_scale);
}

void Neg(const float* input,  float* output, uint32_t size) {
  for (uint32_t i = 0; i < size; ++i) {
    output[i] = -input[i];
  }
}

void LeakyRelu(const int8_t* input, int8_t* output, uint32_t size, int32_t input_zp, int32_t output_zp,
  int32_t output_multiplier_alpha, int32_t output_shift_alpha, int32_t output_multiplier_identity,
  int32_t output_shift_identity) {
  for (uint32_t i = 0; i < size; ++i) {
    const int32_t input_val = input[i] - input_zp;
    int32_t output_val;
    if (input_val >= 0) {
      output_val = output_zp + MultiplyByQuantizedMultiplier(input_val, (int64_t) output_multiplier_identity,
                            output_shift_identity);
    } else {
      output_val = output_zp + MultiplyByQuantizedMultiplier(input_val, (int64_t) output_multiplier_alpha,
                            output_shift_alpha);
    }
    output_val = (output_val > INT8_MIN ) ? output_val : INT8_MIN;
    output_val = (output_val < INT8_MAX ) ? output_val : INT8_MAX;
    output[i] = (int8_t)output_val;
  }
}

// Ported from Renesas library
// LogSoftMax is always performed on the last axis only.
void LogSoftMax(const int8_t* input, int8_t* output, const int32_t* in_shape, const int32_t in_num_dims,
  int32_t input_zp, float input_scale, int32_t output_zp, float output_scale) {

  int32_t outer_size, depth, total_size;

  if (in_num_dims == 4) {
    outer_size = in_shape[0] * in_shape[1] * in_shape[2];
    depth = in_shape[3];
  } else if (in_num_dims == 3) {
    outer_size = in_shape[0] * in_shape[1];
    depth = in_shape[2];
  } else{
    outer_size = in_shape[0];
    depth = in_shape[1];
  }
  total_size = outer_size * depth;

  float dMax_array[outer_size], dData_exp_array[total_size], input_temp[total_size], output_temp[total_size];

  for (int i = 0; i < total_size; i++) {
    input_temp[i] = (float)(input[i] - input_zp) * input_scale;
  }

  float dMax = 0;
  int inc = 0;
  for (int i = 0; i < outer_size; i++) {
    dMax = input_temp[i * depth];
    for (int j = 0; j < depth; j++) {
      if (dMax < input_temp[i * depth + j]) {
        dMax = input_temp[i * depth + j];
      }
    }
    dMax_array[inc++] = dMax;
  }
  for (int i = 0; i < outer_size; i++) {
    for (int j = 0; j < depth; j++) {
      int cur_offset = i * depth + j;
      input_temp[cur_offset] = input_temp[cur_offset] - dMax_array[i];
      dData_exp_array[cur_offset] = (float)exp(input_temp[cur_offset]);
    }
  }

  inc = 0;
  for (int i = 0; i < outer_size; i++) {
    dMax = 0;
    for (int j = 0; j < depth; j++) {
      dMax += dData_exp_array[i * depth + j];
    }
    dMax_array[inc++] = (float)log(dMax);
  }
  for (int i = 0; i < outer_size; i++) {
    for (int j = 0; j < depth; j++) {
      output_temp[i * depth + j] = input_temp[i * depth + j] - dMax_array[i];
    }
  }

  for (int i = 0; i < total_size; i++) {
    output[i] = (int8_t)roundf((output_temp[i] / output_scale + output_zp));
  }
}

// Ported from TF
void SpaceToBatchNd(const int8_t* input1_data, int8_t* output_data, const int32_t* block_shape, const int32_t* pad,
  const int32_t* unextended_input1_shape, const int32_t in_num_dims, const int32_t* unextended_output_shape,
  int32_t output_zp) {

  int32_t input_batch_size, input_height, input_width, depth, output_batch_size, output_height, output_width;
  int32_t block_shape_height, block_shape_width, padding_top, padding_left;

  int32_t in_shape[4] = {0};
  int32_t out_shape[4] = {0};

  ExtendShapeSpaceToBatch(unextended_input1_shape, in_num_dims, in_shape);
  ExtendShapeSpaceToBatch(unextended_output_shape, in_num_dims, out_shape);

  input_batch_size = in_shape[0];
  input_height = in_shape[1];
  input_width = in_shape[2];
  output_batch_size = out_shape[0];
  output_height = out_shape[1];
  output_width = out_shape[2];
  block_shape_height = block_shape[0];
  padding_top = pad[0];
  depth = in_shape[3];

  if (in_num_dims == 4) {
    block_shape_width = block_shape[1];
    padding_left = pad[2];
  } else { /* in_num_dims is 3 here */
    block_shape_width = 1;
    padding_left = 0;
  }

  // For uint8 quantized, the correct padding "zero value" is the output offset.
  const int32_t pad_value = output_zp;

  for (int out_b = 0; out_b < output_batch_size; ++out_b) {
    int input_batch = out_b % input_batch_size;
    int shift_w = (out_b / input_batch_size) % block_shape_width;
    int shift_h = (out_b / input_batch_size) / block_shape_width;
    for (int out_h = 0; out_h < output_height; ++out_h) {
      for (int out_w = 0; out_w < output_width; ++out_w) {
        int8_t* out = output_data + Offset(out_shape, out_b, out_h, out_w, 0);
        if (out_h * block_shape_height + shift_h < padding_top ||
            out_h * block_shape_height + shift_h >=
                padding_top + input_height ||
            out_w * block_shape_width + shift_w < padding_left ||
            out_w * block_shape_width + shift_w >= padding_left + input_width) {
          memset(out, pad_value, depth * sizeof(int8_t));
        } else {
          const int8_t* in =
              input1_data +
              Offset(in_shape, input_batch,
                     (out_h * block_shape_height + shift_h) - padding_top,
                     (out_w * block_shape_width + shift_w) - padding_left, 0);
          memcpy(out, in, depth * sizeof(int8_t));
        }
      }
    }
  }
}

// Ported from TF
void BatchToSpaceNd(const int8_t* input1_data, int8_t* output_data, const int32_t* block_shape,
  const int32_t* crops, const int32_t* unextended_input1_shape, const int32_t in_num_dims,
  const int32_t* unextended_output_shape, int32_t output_zp) {
  (void)output_zp;
  int32_t input_batch_size, input_height, input_width, depth, output_batch_size, output_height, output_width;
  int32_t block_shape_height, block_shape_width, crops_top, crops_left;

  int32_t in_shape[4] = {0};
  int32_t out_shape[4] = {0};

  ExtendShapeBatchToSpace(unextended_input1_shape, in_num_dims, in_shape);
  ExtendShapeBatchToSpace(unextended_output_shape, in_num_dims, out_shape);

  input_batch_size = in_shape[0];
  input_height = in_shape[1];
  input_width = in_shape[2];
  output_batch_size = out_shape[0];
  output_height = out_shape[1];
  output_width = out_shape[2];
  block_shape_height = block_shape[0];
  crops_top = crops[0];
  depth = in_shape[3];

  if (in_num_dims == 4) {
    block_shape_width = block_shape[1];
    crops_left = crops[2];
  } else { /* in_num_dims is 3 here */
    block_shape_width = 1;
    crops_left = 0;
  }

  for (int in_batch = 0; in_batch < input_batch_size; ++in_batch) {
    const int out_batch = in_batch % output_batch_size;
    const int spatial_offset = in_batch / output_batch_size;
    for (int in_h = 0; in_h < input_height; ++in_h) {
      const int out_h = in_h * block_shape_height +
                        spatial_offset / block_shape_width - crops_top;
      if (out_h < 0 || out_h >= output_height) {
        continue;
      }
      for (int in_w = 0; in_w < input_width; ++in_w) {
        const int out_w = in_w * block_shape_width +
                          spatial_offset % block_shape_width - crops_left;
        if (out_w < 0 || out_w >= output_width) {
          continue;
        }
        int8_t* out = output_data + Offset(out_shape, out_batch, out_h, out_w, 0);
        const int8_t* in =
            input1_data + Offset(in_shape, in_batch, in_h, in_w, 0);
        memcpy(out, in, depth * sizeof(int8_t));
      }
    }
  }
}

void Transpose(const int8_t *input, int8_t *output, int32_t size, int32_t rank, 
  int32_t *strides, int32_t *next_dim_sizes, int32_t *dim_sizes) {
  int inp_ind;
  int out_ind = 0;
  int local_offset;
  while (out_ind < size) {
    inp_ind = 0;
    for (int i = 0; i < rank; i++) {
      local_offset = out_ind % next_dim_sizes[i];
      inp_ind += strides[i] * (int)(local_offset / dim_sizes[i]);
    } 
    output[out_ind] = input[inp_ind];
    out_ind++;
  }
}

// Ported from TF
void ResizeBilinear(const int8_t* input_data, int8_t* output_data, const int32_t* input_shape,
  const int32_t* output_shape, bool half_pixel_centers, bool align_corners) {

  int32_t batches = input_shape[0];
  int32_t input_height = input_shape[1];
  int32_t input_width = input_shape[2];
  int32_t depth = input_shape[3];

  int32_t output_height = output_shape[1];
  int32_t output_width = output_shape[2];

  int32_t height_scale_10 =
      ((1 << 10) * input_height + output_height / 2) / output_height;
  int32_t width_scale_10 =
      ((1 << 10) * input_width + output_width / 2) / output_width;

  if (align_corners && output_height > 1) {
    height_scale_10 =
        ((1 << 10) * (input_height - 1) + (output_height - 1) / 2) /
        (output_height - 1);
  }
  if (align_corners && output_width > 1) {
    width_scale_10 = ((1 << 10) * (input_width - 1) + (output_width - 1) / 2) /
                     (output_width - 1);
  }

  for (int b = 0; b < batches; ++b) {
    for (int y = 0; y < output_height; ++y) {
      int32_t input_y, y0, y1;
      ComputeInterpolationValuesInteger(y, height_scale_10,
                                        half_pixel_centers,
                                        input_height, &input_y, &y0, &y1);
      for (int x = 0; x < output_width; ++x) {
        int32_t input_x, x0, x1;
        ComputeInterpolationValuesInteger(x, width_scale_10,
                                          half_pixel_centers,
                                          input_width, &input_x, &x0, &x1);
        for (int c = 0; c < depth; ++c) {
          const int64_t output_20_ll =
              (int64_t)(input_data[Offset(input_shape, b, y0, x0, c)]) *
              ((1 << 10) - (input_y - (1 << 10) * y0)) *
              ((1 << 10) - (input_x - (1 << 10) * x0));
          const int64_t output_20_lu =
              (int64_t)(input_data[Offset(input_shape, b, y1, x0, c)]) *
              (input_y - (1 << 10) * y0) *
              ((1 << 10) - (input_x - (1 << 10) * x0));
          const int64_t output_20_rl =
              (int64_t)(input_data[Offset(input_shape, b, y0, x1, c)]) *
              ((1 << 10) - (input_y - (1 << 10) * y0)) *
              (input_x - (1 << 10) * x0);
          const int64_t output_20_ru =
              (int64_t)(input_data[Offset(input_shape, b, y1, x1, c)]) *
              (input_y - (1 << 10) * y0) * (input_x - (1 << 10) * x0);
          const int64_t output_20 =
              output_20_ll + output_20_lu + output_20_rl + output_20_ru;

          const int64_t round = 1 << 19;
          const int8_t interpolation = (int8_t)((output_20 + round) >> 20);

          output_data[Offset(output_shape, b, y, x, c)] = interpolation;
        }
      }
    }
  }
}

void MirrorPad(const int32_t* padding_matrix,
  const int* input_dims, int* output_dims_num_elements,
  int* input_dims_num_elements, const int8_t* input_data,
  int8_t* output_data, const int offset, const int num_dims,
  const int output_size) {
  for (int i = 0; i < output_size; ++i) {
    output_data[i] = input_data[GetFlatIndex(
      i, num_dims, padding_matrix, input_dims, output_dims_num_elements,
      input_dims_num_elements, offset)];
  }
}

void UpsamplingNearestNeighbor(const int8_t *input, int8_t *output, const int32_t *in_shape, const int32_t *out_shape,
 const bool align_corners, const bool half_pixel_centers) {
  int32_t batches = out_shape[0];
  int32_t input_height = in_shape[1];
  int32_t input_width = in_shape[2];
  int32_t depth = out_shape[3];
  
  int32_t output_height = out_shape[1];
  int32_t output_width = out_shape[2];

  const int col_offset = in_shape[3];
  const int row_offset = in_shape[2] * col_offset;
  const int batch_offset = in_shape[1] * row_offset;

  const int8_t* input_ptr = input;
  int8_t* output_ptr = output;
  for (int b = 0; b < batches; ++b) {
    for (int y = 0; y < output_height; ++y) {
      int32_t in_y = GetNearestNeighbor(y, input_height, output_height,
                                        align_corners,
                                        half_pixel_centers);
      const int8_t* y_input_ptr = input_ptr + in_y * row_offset;
      for (int x = 0; x < output_width; ++x) {
        int32_t in_x = GetNearestNeighbor(x, input_width, output_width,
                                          align_corners,
                                          half_pixel_centers);
        const int8_t* x_input_ptr = y_input_ptr + in_x * col_offset;
        memcpy(output_ptr, x_input_ptr, depth * sizeof(int8_t));
        output_ptr += depth;
      }
    }
    input_ptr += batch_offset;
  }
}

void PRelu(const int8_t* input, const int32_t *in_shape, int32_t in_dims, int32_t input_zp,
  int8_t* output, const int32_t *out_shape, int32_t out_dims, int32_t output_zp, const int8_t *alpha,
  const int32_t *alpha_shape, int32_t alpha_dims, int32_t alpha_zp, int32_t output_multiplier_alpha,
  int32_t output_shift_alpha, int32_t output_multiplier_identity, int32_t output_shift_identity) {

  NdArrayDesc desc1,desc2;
  int32_t extended_shape[4]={0};

  ExtendedShape(out_shape, out_dims, extended_shape, 4);
  NdArrayDescsForElementwiseBroadcast(in_shape, in_dims, alpha_shape, alpha_dims, &desc1, &desc2, 4);

  for (int b = 0; b < extended_shape[0]; ++b) {
    for (int y = 0; y < extended_shape[1]; ++y) {
      for (int x = 0; x < extended_shape[2]; ++x) {
        for (int c = 0; c < extended_shape[3]; ++c) {
          int output_index = Offset(extended_shape, b, y, x, c);
          int input_index = SubscriptToIndex(desc1, b, y, x, c);
          const int32_t input_val = input[input_index] - input_zp;
          int32_t output_val;
          if (input_val >= 0) {
            output_val = MultiplyByQuantizedMultiplier(input_val, output_multiplier_identity,
                                  output_shift_identity);
          } else {
            int alpha_index = SubscriptToIndex(desc2, b, y, x, c);
            const int32_t alpha_value = alpha[alpha_index] - alpha_zp;
            output_val = MultiplyByQuantizedMultiplier(input_val * alpha_value, output_multiplier_alpha,
                                  output_shift_alpha);
          }
          output_val += output_zp;

          int32_t clamped_output = output_val;
          const int32_t quantized_min = INT8_MIN;
          const int32_t quantized_max = INT8_MAX;
          clamped_output = (clamped_output > quantized_min ) ? clamped_output : quantized_min;
          clamped_output = (clamped_output < quantized_max ) ? clamped_output : quantized_max;

          output[output_index] = (int8_t)clamped_output;
        }
      }
    }
  }
}

void Relu(const int8_t *input, int8_t *output, uint32_t size, int32_t input_zp,
  float output_scale, int32_t output_zp, int32_t output_multiplier, int32_t output_shift) {

  float act_min = 0.0;
  float act_max = HUGE_VALF;

  int32_t temp = output_zp + (int32_t)(roundf(act_min / output_scale));
  int32_t quantized_activation_min = (INT8_MIN > temp) ? (INT8_MIN) : (temp);
  
  temp = output_zp + (int32_t)(roundf(act_max / output_scale));
  temp = (temp < (int32_t)(INT8_MAX)) ? (temp) : ((int32_t)(INT8_MAX));

  int32_t quantized_activation_max = (act_max == HUGE_VALF) ? ((int32_t)INT8_MAX) : (temp);

  for (uint32_t i = 0; i < size; ++i) {
    const int32_t val = (input[i]);
    int32_t clamped = output_zp +
        MultiplyByQuantizedMultiplier(val - input_zp, output_multiplier, output_shift);

    clamped = (quantized_activation_min > clamped) ?  (quantized_activation_min) : (clamped);
    clamped = (quantized_activation_max < clamped) ? (quantized_activation_max) : (clamped);
    output[i] = (int8_t)(clamped);
  }
}

void Relu6(const int8_t *input, int8_t *output, uint32_t size, int32_t input_zp,
  float input_scale, int32_t output_zp, float output_scale) {

  float dq_buffer[size];
  AffineDequantizeInt8ToFloat(input, dq_buffer, size, input_zp, input_scale);

  for (uint32_t i = 0; i < size; i++) {
    dq_buffer[i] = (dq_buffer[i] > 6.0f) ? (6.0f) : ( (dq_buffer[i] < 0.0f) ? (0.0f) : (dq_buffer[i]));
  }
  AffineQuantizeFloatToInt8(dq_buffer, output, size, output_zp, output_scale);
}

void Abs(const int8_t* input, int32_t input_zp, int32_t size, int8_t* output,
  int32_t output_zp, int32_t multiplier, int32_t shift) {

  for (int i = 0; i < size; i++) {
    int32_t temp = (input[i] - input_zp);
    const int32_t value = (temp >= 0) ? (temp) : (-temp);
    const int8_t kMin = INT8_MIN;
    const int8_t kMax = INT8_MAX;

    const int val = MultiplyByQuantizedMultiplier(value, multiplier, shift) + output_zp;
    temp = (int8_t) ( (val > kMin) ? (val) : (kMin) );
    output[i] = (int8_t) ( (temp < kMax) ? (temp) : (kMax) );
  }
}

void Mul(const int8_t* input1, const int32_t* input1_shape, int32_t input1_dims,
  const int8_t* input2, const int32_t* input2_shape, int32_t input2_dims, int8_t* output,
  const int32_t* output_shape, int32_t output_dims, ArithmeticParams quant_params) {

  NdArrayDesc desc1, desc2;
  NdArrayDescsForElementwiseBroadcast(input1_shape, input1_dims, input2_shape, input2_dims, &desc1, &desc2, 6);

  int32_t extended_output_shape[6] = {0};
  ExtendedShape(output_shape, output_dims, extended_output_shape, 6);

  int32_t extended_output_shape_dims[6];
  memcpy(extended_output_shape_dims, extended_output_shape, sizeof(extended_output_shape_dims));

  size_t input1_offset_a = 0;
  size_t input2_offset_a = 0;
  size_t output_offset_a = 0;

  for (int a = 0; a < extended_output_shape_dims[0]; ++a) {
    size_t input1_offset_d = input1_offset_a;
    size_t input2_offset_d = input2_offset_a;
    size_t output_offset_d = output_offset_a;

    for (int d = 0; d < extended_output_shape_dims[1]; ++d) {
      size_t input1_offset_b = input1_offset_d;
      size_t input2_offset_b = input2_offset_d;
      size_t output_offset_b = output_offset_d;

      for (int b = 0; b < extended_output_shape_dims[2]; ++b) {
        size_t input1_offset_y = input1_offset_b;
        size_t input2_offset_y = input2_offset_b;
        size_t output_offset_y = output_offset_b;

        for (int y = 0; y < extended_output_shape_dims[3]; ++y) {
          size_t input1_offset_x = input1_offset_y;
          size_t input2_offset_x = input2_offset_y;
          size_t output_offset_x = output_offset_y;

          for (int x = 0; x < extended_output_shape_dims[4]; ++x) {
            size_t input1_offset_c = input1_offset_x;
            size_t input2_offset_c = input2_offset_x;
            size_t output_offset_c = output_offset_x;

            for (int c = 0; c < extended_output_shape_dims[5]; ++c) {
              const int32_t input1_val = quant_params.input1_offset + input1[input1_offset_c];
              const int32_t input2_val = quant_params.input2_offset + input2[input2_offset_c];
              const int32_t unclamped_result = quant_params.output_offset + MultiplyByQuantizedMultiplier(input1_val * input2_val,
                                                quant_params.output_multiplier, quant_params.output_shift);
              const int32_t clamped_output = Min(quant_params.quantized_activation_max,
                                                Max(quant_params.quantized_activation_min, unclamped_result) );
              
              output[output_offset_c] = (int8_t)(clamped_output);
              input1_offset_c += desc1.strides[5];
              input2_offset_c += desc2.strides[5];
              ++output_offset_c;
            }
            input1_offset_x += desc1.strides[4];
            input2_offset_x += desc2.strides[4];
            output_offset_x += extended_output_shape_dims[5];
          }
          input1_offset_y += desc1.strides[3];
          input2_offset_y += desc2.strides[3];
          output_offset_y += extended_output_shape_dims[4] * extended_output_shape_dims[5];
        }
        input1_offset_b += desc1.strides[2];
        input2_offset_b += desc2.strides[2];
        output_offset_b += extended_output_shape_dims[3] * extended_output_shape_dims[4] * extended_output_shape_dims[5];
      }
      input1_offset_d += desc1.strides[1];
      input2_offset_d += desc2.strides[1];
      output_offset_d += extended_output_shape_dims[2] * extended_output_shape_dims[3] *
                         extended_output_shape_dims[4] * extended_output_shape_dims[5];
    }
    input1_offset_a += desc1.strides[0];
    input2_offset_a += desc2.strides[0];
    output_offset_a += extended_output_shape_dims[1] * extended_output_shape_dims[2] *
                       extended_output_shape_dims[3] * extended_output_shape_dims[4] *
                       extended_output_shape_dims[5];
  }
}

// Ported from TF
void Sub(const int8_t* input1, const int8_t* input2, int8_t* output,
  const int32_t* input1_shape, int32_t input1_dims, const int32_t* input2_shape,
  int32_t input2_dims, const int32_t* output_shape, int32_t output_dims,
  ArithmeticParams quant_params) {

  if (HaveSameShapes(input1_shape, input1_dims, input2_shape, input2_dims)) {
    const int flat_size = FlatSize(input1_shape, input1_dims);
    SubElementwise(input1, input2, output, flat_size, quant_params);
  } else {
    BroadcastSubSlowInt8(input1_shape, input1, input1_dims, input2_shape,
      input2, input2_dims, output_shape, output, output_dims, quant_params);
  }
}

// Supporting function for Sub op
void SubElementwise(const int8_t* input1_data, const int8_t* input2_data, int8_t* output_data,
                    int32_t size, ArithmeticParams quant_params) {

  for (int i = 0; i < size; ++i) {
    const int32_t input1_val = quant_params.input1_offset + input1_data[i];
    const int32_t input2_val = quant_params.input2_offset + input2_data[i];
    const int32_t shifted_input1_val = input1_val * (1 << quant_params.left_shift);
    const int32_t shifted_input2_val = input2_val * (1 << quant_params.left_shift);
    const int32_t scaled_input1_val =
        MultiplyByQuantizedMultiplierSmallerThanOneExp(
          shifted_input1_val, quant_params.input1_multiplier, quant_params.input1_shift);
    const int32_t scaled_input2_val =
        MultiplyByQuantizedMultiplierSmallerThanOneExp(
          shifted_input2_val, quant_params.input2_multiplier, quant_params.input2_shift);
    const int32_t raw_sub = scaled_input1_val - scaled_input2_val;
    const int32_t raw_output =
        MultiplyByQuantizedMultiplierSmallerThanOneExp(
          raw_sub, quant_params.output_multiplier, quant_params.output_shift) +
        quant_params.output_offset;
    int32_t clamped_output = (raw_output > quant_params.quantized_activation_min) ?
              raw_output : quant_params.quantized_activation_min;
    clamped_output = (raw_output < quant_params.quantized_activation_max) ?
              raw_output : quant_params.quantized_activation_max;
    output_data[i] = (int8_t)clamped_output;
  }
}

// Supporting function for Sub op
void BroadcastSubSlowInt8(const int32_t* input1_shape, const int8_t* input1_data,
  int32_t input1_dims, const int32_t* input2_shape, const int8_t* input2_data,
  int32_t input2_dims, const int32_t* output_shape, int8_t* output_data,
  int32_t output_dims, ArithmeticParams quant_params) {
  (void)output_shape;
  (void)output_dims;
  size_t compressed_input1_stride[kMaxBroadcastDim];
  size_t compressed_input2_stride[kMaxBroadcastDim];
  size_t compressed_output_shape[kMaxBroadcastDim] = {1, 1, 1, 1, 1, 1};
  bool broadcastable_shape = ReduceDimensionsForBroadcast(
      input1_shape, input1_dims, input2_shape, input2_dims, compressed_input1_stride,
      compressed_input2_stride, compressed_output_shape);
  // Skip broadcasting for degenerate shapes.
  if (!broadcastable_shape) {
    return;
  }

  int32_t input1_offset = 0;
  int32_t input2_offset = 0;
  int32_t output_offset = 0;
  BroadcastSubRecursiveDimensions(kMaxBroadcastDim - 1, &input1_offset,
    &input2_offset, &output_offset, compressed_input1_stride, compressed_input2_stride,
    compressed_output_shape, quant_params, input1_data, input2_data, output_data);
}

// Supporting function for Sub op
extern inline void BroadcastSubRecursiveDimensions(
    int dimension, int32_t* input1_offset_p, int32_t* input2_offset_p,
    int32_t* output_offset, size_t* compressed_input1_stride,
    size_t* compressed_input2_stride, size_t* compressed_output_shape,
    ArithmeticParams quant_params, const int8_t* input1_data, const int8_t* input2_data,
    int8_t* output_data) {
  if (dimension > 0) {
    for (size_t c = 0; c < compressed_output_shape[dimension]; ++c) {
      int32_t input1_offset_c = *input1_offset_p;
      int32_t input2_offset_c = *input2_offset_p;
      BroadcastSubRecursiveDimensions(
          dimension - 1, &input1_offset_c, &input2_offset_c, output_offset,
          compressed_input1_stride, compressed_input2_stride,
          compressed_output_shape, quant_params, input1_data,
          input2_data, output_data);
      *input1_offset_p += compressed_input1_stride[dimension];
      *input2_offset_p += compressed_input2_stride[dimension];
    }
  } else {
    bool input1_is_broadcast = compressed_input1_stride[dimension] == 0;
    bool input2_is_broadcast = compressed_input2_stride[dimension] == 0;
    const int8_t* input1_data_ptr = input1_data + *input1_offset_p;
    const int8_t* input2_data_ptr = input2_data + *input2_offset_p;
    int8_t* output_data_ptr = output_data + *output_offset;
    if (input1_is_broadcast) {
      // input1 is broadcast.
      SubBroadcast(input2_data_ptr, input1_data_ptr, output_data_ptr,
                      compressed_output_shape[dimension], quant_params);
      *input2_offset_p += compressed_output_shape[dimension];
    } else if (input2_is_broadcast) {
      // input2 is broadcast.
      SubBroadcast(input1_data_ptr, input2_data_ptr, output_data_ptr,
                      compressed_output_shape[dimension], quant_params);
      *input1_offset_p += compressed_output_shape[dimension];
    } else {
      // Sub element-wise.
      SubElementwise(input1_data_ptr, input2_data_ptr, output_data_ptr,
                        compressed_output_shape[dimension], quant_params);
      *input1_offset_p += compressed_output_shape[dimension];
      *input2_offset_p += compressed_output_shape[dimension];
    }
    *output_offset += compressed_output_shape[dimension];
  }
}

// Supporting function for Sub op
extern inline void SubBroadcast(const int8_t* input_data, const int8_t* broadcast_data,
                         int8_t* output_data, int32_t size, ArithmeticParams quant_params) {

  for (int32_t c = 0; c < size; ++c) {
    const int32_t shifted_input1_val =
        (quant_params.input1_offset + input_data[c]) * (1 << quant_params.left_shift);
    const int32_t shifted_input2_val =
        (quant_params.input2_offset + broadcast_data[c]) * (1 << quant_params.left_shift);
    const int32_t scaled_input1_val =
        MultiplyByQuantizedMultiplierSmallerThanOneExp(
            shifted_input1_val, quant_params.input1_multiplier,
            quant_params.input1_shift);
    const int32_t scaled_input2_val =
        MultiplyByQuantizedMultiplierSmallerThanOneExp(
            shifted_input2_val, quant_params.input2_multiplier,
            quant_params.input2_shift);
    const int32_t raw_sub = scaled_input1_val - scaled_input2_val;
    const int32_t raw_output =
        MultiplyByQuantizedMultiplierSmallerThanOneExp(
            raw_sub, quant_params.output_multiplier, quant_params.output_shift) +
        quant_params.output_offset;
    int32_t clamped_output = (raw_output > quant_params.quantized_activation_min) ?
              raw_output : quant_params.quantized_activation_min;
    clamped_output = (raw_output < quant_params.quantized_activation_max) ?
              raw_output : quant_params.quantized_activation_max;
    output_data[c] = (int8_t)clamped_output;
  }
}

void Add(const int8_t* input1, const int32_t* input1_shape, int32_t input1_dims, const int8_t* input2,
  const int32_t* input2_shape, int32_t input2_dims, int8_t* output, const int32_t* output_shape,
  int32_t output_dims, ArithmeticParams quant_params) {

  BroadcastBinaryFunction6DSlow(input1, input1_shape, input1_dims, input2, input2_shape,
  input2_dims, output, output_shape, output_dims, quant_params);
}

// Ported from TF
void Pad(int32_t left_padding_count, int32_t *left_padding,
             int32_t right_padding_count, int32_t *right_padding, 
             int32_t *ext_input_shape, const int8_t *input_data,
             const int8_t pad_value, const int32_t *ext_output_shape,
             int8_t *output_data) {
  (void)ext_input_shape;
  // Runtime calls are currently fixed at 5 dimensions. Copy inputs so we can
  // pad them to 5 dims (yes, we are "padding the padding").
  int left_padding_copy[PadKernelMaxDimensionCount];
  for (int i = 0; i < PadKernelMaxDimensionCount; i++) {
    left_padding_copy[i] = 0;
  }
  for (int i = 0; i < left_padding_count; ++i) {
    left_padding_copy[i + PadKernelMaxDimensionCount -
                      left_padding_count] = left_padding[i];
  }
  int right_padding_copy[PadKernelMaxDimensionCount];
  for (int i = 0; i < PadKernelMaxDimensionCount; i++) {
    right_padding_copy[i] = 0;
  }
  for (int i = 0; i < right_padding_count; ++i) {
    right_padding_copy[i + PadKernelMaxDimensionCount -
                       right_padding_count] =
        right_padding[i];
  }

  const int output_batch = ext_output_shape[0];
  const int output_plane = ext_output_shape[1];
  const int output_height = ext_output_shape[2];
  const int output_width = ext_output_shape[3];
  const int output_depth = ext_output_shape[4];

  const int left_b_padding = left_padding_copy[0];
  const int left_p_padding = left_padding_copy[1];
  const int left_h_padding = left_padding_copy[2];
  const int left_w_padding = left_padding_copy[3];
  const int left_d_padding = left_padding_copy[4];

  const int right_b_padding = right_padding_copy[0];
  const int right_p_padding = right_padding_copy[1];
  const int right_h_padding = right_padding_copy[2];
  const int right_w_padding = right_padding_copy[3];
  const int right_d_padding = right_padding_copy[4];

  const int8_t* in_ptr = input_data;
  int8_t* out_ptr = output_data;
  for (int out_b = 0; out_b < output_batch; ++out_b) {
    for (int out_p = 0; out_p < output_plane; ++out_p) {
      for (int out_h = 0; out_h < output_height; ++out_h) {
        for (int out_w = 0; out_w < output_width; ++out_w) {
          for (int out_d = 0; out_d < output_depth; ++out_d) {
            if (out_b < left_b_padding ||
                out_b >= output_batch - right_b_padding ||
                out_p < left_p_padding ||
                out_p >= output_plane - right_p_padding ||
                out_h < left_h_padding ||
                out_h >= output_height - right_h_padding ||
                out_w < left_w_padding ||
                out_w >= output_width - right_w_padding ||
                out_d < left_d_padding ||
                out_d >= output_depth - right_d_padding) {
              *out_ptr++ = pad_value;
            } else {
              *out_ptr++ = *in_ptr++;
            }
          }
        }
      }
    }
  }
}

// Supporting function for Squared Difference op
int8_t SquaredDifferenceOp(int8_t x, int8_t y, const ArithmeticParams params) {
  const int32_t input1_val = params.input1_offset + x;
  const int32_t input2_val = params.input2_offset + y;
  const int32_t shifted_input1_val = input1_val * (1 << params.left_shift);
  const int32_t shifted_input2_val = input2_val * (1 << params.left_shift);
  const int32_t scaled_input1_val =
      MultiplyByQuantizedMultiplierSmallerThanOneExp(
          shifted_input1_val, params.input1_multiplier, params.input1_shift);
  const int32_t scaled_input2_val =
      MultiplyByQuantizedMultiplierSmallerThanOneExp(
          shifted_input2_val, params.input2_multiplier, params.input2_shift);
  const int32_t raw_diff = scaled_input1_val - scaled_input2_val;

  // Max of this is 32767^2 * (1 << 0), so won't overflow 32 bits.
  const int32_t squared_raw_diff = raw_diff * raw_diff;
  const int32_t raw_output =
      MultiplyByQuantizedMultiplier(squared_raw_diff, params.output_multiplier,
                                    params.output_shift) +
      params.output_offset;
  const int32_t clamped_output =
      Min(params.quantized_activation_max, Max(params.quantized_activation_min, raw_output));
  return (int8_t)(clamped_output);
}

// Supporting function for Squared Difference op
void SquaredDifferenceBroadcast1(const int8_t* input1_data, const int8_t* input2_data,
  int8_t* output, int size, const ArithmeticParams params) {

  for (int i = 0; i < size; ++i) {
    output[i] = SquaredDifferenceOp(input1_data[0], input2_data[i], params);
  }
}

// Supporting function for Squared Difference op
void SquaredDifferenceBroadcast2(const int8_t* input1_data, const int8_t* input2_data,
  int8_t* output, int size, const ArithmeticParams params) {

  for (int i = 0; i < size; ++i) {
    output[i] = SquaredDifferenceOp(input1_data[i], input2_data[0], params);
  }
}

// Supporting function for Squared Difference op
void ElementWiseSquaredDifference(const int8_t* input1, const int8_t* input2,
  int8_t* output, int size, const ArithmeticParams params) {

  for (int i = 0; i < size; ++i) {
    output[i] = SquaredDifferenceOp(input1[i], input2[i], params);
  }
}

// Supporting function for Squared Difference op
inline void BroadcastRecursiveDimensions(
  const int8_t* input1, size_t* input1_offset_p, size_t* compressed_input1_stride,
  const int8_t* input2, size_t* input2_offset_p, size_t* compressed_input2_stride,
  int8_t* output, size_t* output_offset, size_t* compressed_output_shape,
  int dimension, const ArithmeticParams params) {

  if (dimension > 0) {
    for (size_t c = 0; c < compressed_output_shape[dimension]; ++c) {
      size_t input1_offset_c = *input1_offset_p;
      size_t input2_offset_c = *input2_offset_p;
      BroadcastRecursiveDimensions(input1, &input1_offset_c, compressed_input1_stride, input2,
              &input2_offset_c, compressed_input2_stride, output, output_offset, 
              compressed_output_shape, dimension - 1, params);

      *input1_offset_p += compressed_input1_stride[dimension];
      *input2_offset_p += compressed_input2_stride[dimension];
    }
  } else {
    bool input1_is_broadcast = compressed_input1_stride[dimension] == 0;
    bool input2_is_broadcast = compressed_input2_stride[dimension] == 0;
    const int8_t* input1_ptr = input1 + *input1_offset_p;
    const int8_t* input2_ptr = input2 + *input2_offset_p;
    int8_t* output_ptr = output + *output_offset;
    if (input1_is_broadcast) {
      // input1 is broadcast.
      SquaredDifferenceBroadcast1(input1_ptr, input2_ptr, output_ptr, compressed_output_shape[dimension], params);
      *input2_offset_p += compressed_output_shape[dimension];
    } else if (input2_is_broadcast) {
      // input2 is broadcast.
      SquaredDifferenceBroadcast2(input1_ptr, input2_ptr, output_ptr, compressed_output_shape[dimension], params);
      *input1_offset_p += compressed_output_shape[dimension];
    } else { 
      ElementWiseSquaredDifference(input1_ptr, input2_ptr, output_ptr, compressed_output_shape[dimension], params);
      *input1_offset_p += compressed_output_shape[dimension];
      *input2_offset_p += compressed_output_shape[dimension];
    }
    *output_offset += compressed_output_shape[dimension];
  }
} 

// Supporting function for Squared Difference op
void BroadcastSquaredDifference6DSlow(
  const int8_t* input1_data, const int32_t* input1_shape, int32_t input1_dims,
  const int8_t* input2_data,  const int32_t* input2_shape, int32_t input2_dims,
  int8_t* output_data, const int32_t* output_shape, const ArithmeticParams params) {
  (void)output_shape;

  size_t compressed_input1_stride[kMaxBroadcastDim];
  size_t compressed_input2_stride[kMaxBroadcastDim];
  size_t compressed_output_shape[kMaxBroadcastDim] = {1, 1, 1, 1, 1, 1};
  bool broadcastable_shape = ReduceDimensionsForBroadcast(
      input1_shape, input1_dims, input2_shape, input2_dims, compressed_input1_stride,
      compressed_input2_stride, compressed_output_shape);
  // Skip broadcasting for degenerate shapes.
  if (!broadcastable_shape) {
    return;
  }

  size_t input1_offset = 0;
  size_t input2_offset = 0;
  size_t output_offset = 0;
  BroadcastRecursiveDimensions(input1_data, &input1_offset, compressed_input1_stride,
      input2_data, &input2_offset, compressed_input2_stride, output_data, &output_offset,
      compressed_output_shape, kMaxBroadcastDim - 1, params);
}

// Ported from TF
void SquaredDifference(const int8_t* input1, const int32_t* input1_shape,
  uint32_t input1_dims, uint32_t input1_size, const int8_t* input2,
  const int32_t* input2_shape, uint32_t input2_dims, int8_t* output,
  const int32_t* output_shape, uint32_t output_dims, const ArithmeticParams params) {
  (void)output_dims;

  if (!HaveSameShapes(input1_shape, input1_dims, input2_shape, input2_dims)) {
    BroadcastSquaredDifference6DSlow(input1, input1_shape, input1_dims, input2, input2_shape,
      input2_dims, output, output_shape, params);
  } else {
    ElementWiseSquaredDifference(input1, input2, output, input1_size, params);
  }
}

// Ported from TFLite Micro
static inline int16_t saturating_left_shift_int16(int16_t val, int amt) {
  int64_t res = ((int64_t)val) << amt;
  if (res > INT16_MAX) { return  INT16_MAX; }
  if (res < INT16_MIN) { return  INT16_MIN; }
  return (int16_t)res;
}

static inline int16_t saturating_doubling_high_mul_int16(int16_t a, int16_t b) {
  if (a == INT16_MIN && b == INT16_MIN) { return INT16_MAX; }
  int32_t prod = (int32_t) a * (int32_t) b;
  return (int16_t) (prod / 32768);
}

static inline int16_t saturating_rounding_doubling_high_mul_int16(int16_t a, int16_t b){
  if (a == INT16_MIN && b == INT16_MIN) { return INT16_MAX; }
  int32_t prod  = (int32_t) a * (int32_t) b;
  int32_t nudge = (prod >= 0) ?  (1 << 14) :  (1 - (1 << 14));
  return (int16_t) ((prod + nudge) / (1 << 15));
}

static inline int16_t rounding_divide_by_pot_int16(int16_t x, int32_t exponent) {
    int16_t mask = (int16_t) ((1U << exponent) - 1);
    int16_t remainder = x & mask;
    int16_t threshold = (mask >> 1) + (x < 0);
    return (x >> exponent) + (remainder > threshold);
}

void HardSwish(const int8_t* input, const int32_t* input_shape,
  int32_t input_dims, int8_t* output, int16_t input_zero_point,
  int16_t output_zero_point, int16_t reluish_multiplier_fixedpoint_int16,
  int reluish_multiplier_exponent, int16_t output_multiplier_fixedpoint_int16,
  int output_multiplier_exponent) {

  int flat_size = FlatSize(input_shape, input_dims);
  for (int i = 0; i < flat_size; ++i) {
    int16_t input_val  = (int16_t)(input[i] - input_zero_point);
    int16_t hi_res_in  = (int16_t)(input_val << 7);
    int16_t hi_res_out = saturating_rounding_doubling_high_mul_int16(hi_res_in,
      output_multiplier_fixedpoint_int16);

    int16_t reluish = hi_res_in;
    if (reluish_multiplier_exponent > 0) {
      reluish = saturating_left_shift_int16(reluish, reluish_multiplier_exponent - 1);
    }
    reluish = saturating_rounding_doubling_high_mul_int16(reluish, reluish_multiplier_fixedpoint_int16);

    if (reluish_multiplier_exponent > 0) {
      reluish = saturating_left_shift_int16(reluish, 1);
    }
    if (reluish_multiplier_exponent < 0) {
      reluish = rounding_divide_by_pot_int16(reluish, -reluish_multiplier_exponent);
    }

    reluish = (int16_t)((reluish + (1 << 15)) >> 1);
    int16_t preshift = saturating_doubling_high_mul_int16(reluish, hi_res_out);
    int16_t out_val = rounding_divide_by_pot_int16(preshift, -output_multiplier_exponent);

    out_val = (int16_t)(out_val + output_zero_point);
    output[i] = out_val;
  }
}


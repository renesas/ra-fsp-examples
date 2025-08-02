/*
 * SPDX-FileCopyrightText: Copyright 2022, 2024 Arm Limited and/or its
 * affiliates <open-source-office@arm.com>
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "log_macros.h"             /* Logging functions */
#include "DetectorPostProcessing.h"
#include "ImageUtils.hpp"
#include "sample_files.h"
#include "tensorflow/lite/micro/micro_time.h"
#include "mera_deploy_cpu/compute_sub_0000_net1.h"
#include "mera_deploy_npu/model.h"
#include "mera_deploy_npu/model_io_data.h"

extern "C" void MainLoop(void);

static uint8_t g_image_buffer[INPUT_IMAGE_WIDTH * INPUT_IMAGE_HEIGHT * FORMAT_MULTIPLY_FACTOR];
static uint8_t g_model_input[INPUT_IMAGE_WIDTH * INPUT_IMAGE_HEIGHT] = { 0 };

#ifndef ARM_NPU
static uint8_t g_main_buffer[kBufferSize_sub_0000_net1];
static int8_t g_image_input[INPUT_IMAGE_WIDTH * INPUT_IMAGE_HEIGHT];
#endif

void create_int8_tensor(TfLiteTensor *tensor, int8_t *input_ptr, float scale, int zero_point)
{
    // Set type to int8
    tensor->type = kTfLiteInt8;

    // Assign the existing input pointer
    tensor->data.int8 = input_ptr;

    // Set bytes
    tensor->bytes = 0;

    // Set dims to NULL as requested
    tensor->dims = NULL;
    // Allocate and set quantization parameters on the stack
    static TfLiteAffineQuantization quantization;  // Static to avoid malloc
    static TfLiteFloatArray scale_array;           // Static to avoid malloc
    static TfLiteIntArray zero_point_array;       // Static to avoid malloc

    // Set scale
    scale_array.size = 1;
    scale_array.data[0] = scale;
    quantization.scale = &scale_array;

    // Set zero point
    zero_point_array.size = 1;
    zero_point_array.data[0] = zero_point;
    quantization.zero_point = &zero_point_array;

    quantization.quantized_dimension = 0;  // Per-tensor quantization

    tensor->quantization.type = kTfLiteAffineQuantization;
    tensor->quantization.params = &quantization;

    // Set allocation type
    tensor->allocation_type = kTfLiteArenaRw;

    // Set other fields
    tensor->is_variable = false;
}

void MainLoop()
{
    /* Load input image: RGB format. */
    memcpy (g_image_buffer, get_sample_data_ptr (0), INPUT_IMAGE_WIDTH * INPUT_IMAGE_HEIGHT * FORMAT_MULTIPLY_FACTOR);

    /* Convert RGB to grayscale, then quantize to int8_t. */
    rgb_to_grayscale (g_image_buffer, g_model_input, INPUT_IMAGE_WIDTH, INPUT_IMAGE_HEIGHT);
    arm::app::image::ConvertImgToInt8 (g_model_input, INPUT_IMAGE_WIDTH * INPUT_IMAGE_HEIGHT);

#ifdef ARM_NPU
    /* Copy the preprocessed input data to the model input buffer */
    memcpy (GetModelInputPtr_image_input (), g_model_input, INPUT_IMAGE_WIDTH * INPUT_IMAGE_HEIGHT);
#else
    int8_t identity_70275[648] = { 0 };
    int8_t identity_1_70284[2592] = { 0 };

    /* Copy the preprocessed input data to the model input buffer */
    memcpy ((void*) &g_image_input[0], g_model_input, INPUT_IMAGE_WIDTH * INPUT_IMAGE_HEIGHT);
#endif

    /* Run inference over this image. */
    info("Running inference on image %" PRIu32 " => %s\n", 0, get_sample_data_filename (0));
    uint32_t cpuCyclesBegin = tflite::GetCurrentTimeTicks ();

#ifdef ARM_NPU
    /* Run the inference */
    RunModel (false);
#else
    compute_sub_0000_net1((uint8_t*)&g_main_buffer[0], g_image_input, identity_70275, identity_1_70284);
#endif

    uint32_t cpuCyclesStop = tflite::GetCurrentTimeTicks () - cpuCyclesBegin;
    info("Inference runtime: %" PRIu32 " microseconds\n\n", cpuCyclesStop);

#ifdef ARM_NPU
    /* Retrieve raw output pointers from hardware runtime */
    int8_t *output_identity_0 = GetModelOutputPtr_Identity_70275();
    int8_t *output_identity_1 = GetModelOutputPtr_Identity_1_70284();
#else
    /* Use software fallback buffers */
    int8_t *output_identity_0 = identity_70275;
    int8_t *output_identity_1 = identity_1_70284;
#endif

   /* Wrap outputs in TfLiteTensor structures */
   TfLiteTensor output_tensors[2];
   create_int8_tensor (&output_tensors[0], output_identity_0, 0.13408391177654266f, 47);
   create_int8_tensor (&output_tensors[1], output_identity_1, 0.18535925447940826f, 10);

   /* Provide tensor pointers for post-processing */
   TfLiteTensor *output_array[2] = { &output_tensors[0], &output_tensors[1] };

   /* Run post-processing to decode detections */
   std::vector<arm::app::DetectionResult> results;
   runPostProcessing (g_image_buffer, output_array, results);

   info("INFO - Final results:\n");
   info("INFO - Total number of inferences: 1\n");

   for (size_t i = 0; i < results.size (); ++i)
   {
       const auto &result = results[i];
       info("INFO - %zu)  (%.6f) -> Detection box: {x=%d, y=%d, w=%d, h=%d}\n", i, result.m_normalisedVal,
            result.m_x0, result.m_y0, result.m_w, result.m_h);
   }

   info("Main loop terminated successfully.\r\n");

}

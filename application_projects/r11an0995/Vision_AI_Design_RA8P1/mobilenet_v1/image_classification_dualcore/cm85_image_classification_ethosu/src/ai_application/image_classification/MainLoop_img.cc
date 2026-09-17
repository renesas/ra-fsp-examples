/*
 * SPDX-FileCopyrightText: Copyright 2022 Arm Limited and/or its affiliates <open-source-office@arm.com>
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
#include <ai_common/Classifier.hpp>
#include <ai_common/log_macros.h>
#include "ImgClassProcessing.hpp"
#include "common_util.h"
#include "Labels.h"

extern "C" {
#include "time_counter.h"
#include <image_classification/mera/wrapper.h>
void update_classification_result(unsigned index, unsigned short category, float probability);
}

#define IMAGE_DATA_SIZE  (AI_INPUT_IMAGE_WIDTH * AI_INPUT_IMAGE_HEIGHT * AI_INPUT_IMAGE_BYTE_PER_PIXEL)
extern int8_t model_buffer_int8[IMAGE_DATA_SIZE];

int CompareOutput(const uint8_t* reference, const uint8_t* actual_output, const uint64_t size_in_elements, const float epsilon) {
    int mismatches = 0;
    for (uint32_t i = 0; i < size_in_elements; i += 1) {
        float diff = (actual_output[i] - reference[i]);
        if (diff > epsilon) {
            mismatches += 1;
        }
    }
    return mismatches;
}


void create_int8_tensor(TfLiteTensor* tensor, int8_t* input_ptr, float scale, int zero_point) {
    // Set type to int8
    tensor->type = kTfLiteInt8;

    // Assign the existing input pointer
    tensor->data.int8 = input_ptr;

    // Set bytes
    tensor->bytes = 1000;

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

static bool PresentInferenceResult(const std::vector<arm::app::ClassificationResult> &results)
{
    for (uint32_t i = 0; i < results.size(); ++i) {
        update_classification_result((unsigned)i, (unsigned short)results[i].m_labelIdx, (float)results[i].m_normalisedVal);
    }

    return true;
}

void copy_data_to_mera(int8_t* ptr, uint8_t* src, uint32_t size) {
    for(volatile uint32_t i = 0; i < size; i++){
        ptr[i] = src[i] - 128;
    }
}


bool main_loop_image_classification()
{
    // Copy input data to the buffer expected by the Ethos-U NPU (via MERA)
    copy_data_to_mera((int8_t*)mera_input_ptr(), (uint8_t*)model_buffer_int8, (uint32_t)mera_input_size());
 
    // Clean (write-back) the data cache for the input buffer so the NPU sees up-to-date values in memory
    SCB_CleanDCache_by_Addr((int8_t*)mera_input_ptr(), (int32_t)mera_input_size());
    /* Run inference over this image. */
    volatile uint32_t old_counter =  TimeCounter_CurrentCountGet();
    mera_invoke();
    volatile uint32_t new_counter = TimeCounter_CurrentCountGet();

    application_processing_time.ai_inference_time_ms = TimeCounter_CountValueConvertToMs(old_counter, new_counter);

    int8_t* output = (int8_t*)mera_output_ptr();
    // Invalidate the data cache for the output buffer to ensure CPU reads the latest data from memory
    SCB_InvalidateDCache_by_Addr(output, (int32_t)mera_output_size());
    TfLiteTensor outputTensor;
    create_int8_tensor(&outputTensor, output, 0.00390625, -128);

    arm::app::Classifier classifier;
    std::vector<arm::app::ClassificationResult> results;
    arm::app::ImgClassPostProcess postProcess = arm::app::ImgClassPostProcess(&outputTensor,
            classifier,
            results);

    if (!postProcess.DoPostProcess()) {
        error("Post-processing failed.");
        return false;
    }

    /* Add results to context for access outside handler. */
    if (!PresentInferenceResult(results)) {
        return false;
    }


    return true;
}

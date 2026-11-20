/*
 * Copyright (c) 2021 Arm Limited. All rights reserved.
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
#include "UseCaseHandler.hpp"
#include "YoloFastestModel.hpp"
#include "UseCaseCommonUtils.hpp"
#include "DetectorPostProcessing.h"
#include "ImageUtils.hpp"
#include "log_macros.h"
#include "sample_files.h"
#include <inttypes.h>
#include "tensorflow/lite/micro/micro_time.h"

/* used for presentation, original images are read-only"*/
static uint8_t g_image_buffer[INPUT_IMAGE_WIDTH * INPUT_IMAGE_HEIGHT * FORMAT_MULTIPLY_FACTOR] = { };

namespace arm
{
namespace app
{

/* Object detection classification handler. */
bool ObjectDetectionHandler(ApplicationContext &ctx, uint32_t imgIndex, bool runAll)
{
    auto &model = ctx.Get<Model&> ("model");

    if (!model.IsInited ())
    {
        printf ("Model is not initialised! Terminating processing.\n");
        return false;
    }

    auto curImIdx = ctx.Get<uint32_t> ("imgIndex");

    TfLiteTensor *inputTensor = model.GetInputTensor (0);

    if (!inputTensor->dims)
    {
        printf ("Invalid input tensor dims\n");
        return false;
    }
    else if (inputTensor->dims->size < 3)
    {
        printf ("Input tensor dimension should be >= 3\n");
        return false;
    }

    TfLiteIntArray *inputShape = model.GetInputShape (0);

    std::vector<DetectionResult> results;

    do
    {
        /* Strings for presentation/logging. */
        std::string str_inf
        { "Running inference... " };

        const uint8_t *curr_image = get_sample_data_ptr (ctx.Get<uint32_t> ("imgIndex"));

        /* Copy over the data  and convert to gryscale */
#if DISPLAY_RGB_IMAGE            
        memcpy (g_image_buffer, curr_image, INPUT_IMAGE_WIDTH * INPUT_IMAGE_HEIGHT * FORMAT_MULTIPLY_FACTOR);
#else 
            rgb_to_grayscale(curr_image,g_image_buffer,INPUT_IMAGE_WIDTH,INPUT_IMAGE_HEIGHT);    
#endif /*DISPLAY_RGB_IMAGE*/           

        rgb_to_grayscale (curr_image, inputTensor->data.uint8, INPUT_IMAGE_WIDTH, INPUT_IMAGE_HEIGHT);

        /* If the data is signed. */
        if (model.IsDataSigned ())
        {
            image::ConvertImgToInt8 (inputTensor->data.data, inputTensor->bytes);
        }

        /* Run inference over this image. */
        info("Running inference on image %" PRIu32 " => %s\n", ctx.Get<uint32_t> ("imgIndex"),
             get_sample_data_filename (ctx.Get<uint32_t> ("imgIndex")));

        uint32_t cpuCyclesBegin = tflite::GetCurrentTimeTicks ();

        if (!RunInference (model))
        {
            return false;
        }

        uint32_t cpuCyclesStop = tflite::GetCurrentTimeTicks () - cpuCyclesBegin;

        info("Inference runtime: %" PRIu32 " microseconds\n\n", cpuCyclesStop);

        /* Dtector post-processng*/
        TfLiteTensor *output_arr[2] =
        { nullptr, nullptr };
        output_arr[0] = model.GetOutputTensor (0);
        output_arr[1] = model.GetOutputTensor (1);
        runPostProcessing (g_image_buffer, output_arr, results);

        /* Add results to context for access outside handler. */
        ctx.Set<std::vector<DetectionResult>> ("results", results);

#if VERIFY_TEST_OUTPUT
            arm::app::DumpTensor(outputTensor);
#endif /* VERIFY_TEST_OUTPUT */

        info("Final results:\n");
        info("Total number of inferences: 1\n");

        for (uint32_t i = 0; i < results.size (); ++i)
        {

            if (0)
            {
                info("%" PRIu32 ")  (%f) -> %s {x=%d,y=%d,w=%d,h=%d}\n", i, results[i].m_normalisedVal,
                     "Detection box:", results[i].m_x0, results[i].m_y0, results[i].m_w, results[i].m_h);
            }
            else
            {
                info("%" PRIu32 ")  (%f) -> %s {x=%d,y=%d,w=%d,h=%d}\n", i, results[i].m_normalisedVal,
                     "Detection box:", results[i].m_x0, results[i].m_y0, results[i].m_w, results[i].m_h);
            }
        }

    }
    while (runAll && ctx.Get<uint32_t> ("imgIndex") != curImIdx);

    return true;
}

} /* namespace app */
} /* namespace arm */

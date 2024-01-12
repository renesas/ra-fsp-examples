/*
 * SPDX-FileCopyrightText: Copyright 2021-2022 Arm Limited and/or its affiliates <open-source-office@arm.com>
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
#include <InputFiles_img.hpp>
#include <UseCaseHandler_img.hpp>
#include "Classifier.hpp"
#include "MobileNetModel.hpp"
#include "ImageUtils.hpp"
#include "UseCaseCommonUtils.hpp"
#include "log_macros.h"
#include "ImgClassProcessing.hpp"

#include "camera_layer.h"
extern "C" {
#include "timer.h"

}

#include <cinttypes>

using ImgClassClassifier = arm::app::Classifier;

extern uint8_t bsp_camera_out_buffer888[];
extern uint8_t bsp_cls_model_ip_buffer888[];
extern uint8_t bsp_det_crop_model_ip_buffer888[];

#define DST_HEIGHT 224
#define DST_WIDTH  224
#define SRC_HEIGHT 240
#define SRC_WIDTH  240
#define img_channel 3
#define crop_offset 40

extern "C" {
	extern uint64_t image_classification_inference_time;
}


void nearestNeighborSamplingCls(uint8_t* srcImage, uint8_t* dstImage, bool pad) {

    int f_dst_width;
    int f_dst_height;
    int incr_pointer = 0;
    if(pad)
    {
        int max_val = SRC_WIDTH>SRC_HEIGHT?SRC_WIDTH:SRC_HEIGHT;
        float scale = (float)DST_WIDTH/(float)max_val;
        f_dst_width = SRC_WIDTH*scale;
        f_dst_height = SRC_HEIGHT*scale;

        int pad_w_offset = f_dst_width<DST_WIDTH?(DST_WIDTH-f_dst_width)/2:0;
        int pad_h_offset =  f_dst_height<DST_HEIGHT?(DST_HEIGHT-f_dst_height)/2:0;

        incr_pointer = pad_h_offset*3*DST_WIDTH + 3*pad_w_offset;
    }

    else
    {
        f_dst_width = DST_WIDTH;
        f_dst_height = DST_HEIGHT;
    }

    for(int y = 0; y < f_dst_height; y++)
    {
        for (int x = 0; x < f_dst_width; x++)
        {

            int srcX = (int)(x * ((float)SRC_WIDTH / f_dst_width));
            int srcY = (int)(y * ((float)SRC_HEIGHT / f_dst_height));
            int srcIndex = (srcY * SRC_WIDTH + srcX) * 3;
            int dstIndex = (y * DST_WIDTH + x) * 3 + incr_pointer;

            dstImage[dstIndex]     = srcImage[srcIndex];
            dstImage[dstIndex + 1] = srcImage[srcIndex + 1];
            dstImage[dstIndex + 2] = srcImage[srcIndex + 2];
        }
    }

}



namespace arm {
namespace app {

    /* Image classification inference handler. */
    bool ClassifyImageHandler(ApplicationContext& ctx)
    {
        auto& model = ctx.Get<Model&>("model");

        if (!model.IsInited()) {
            error("Model is not initialized! Terminating processing.\n");
            return false;
        }

        TfLiteTensor* inputTensor = model.GetInputTensor(0);
        TfLiteTensor* outputTensor = model.GetOutputTensor(0);
        if (!inputTensor->dims) {
            error("Invalid input tensor dims\n");
            return false;
        } else if (inputTensor->dims->size < 4) {
            error("Input tensor dimension should be = 4\n");
            return false;
        }

        /* Get input shape for displaying the image. */
        TfLiteIntArray* inputShape = model.GetInputShape(0);

        /* Set up pre and post-processing. */
        ImgClassPreProcess preProcess = ImgClassPreProcess(inputTensor, model.IsDataSigned());

        std::vector<ClassificationResult> results;
        ImgClassPostProcess postProcess = ImgClassPostProcess(outputTensor,
                ctx.Get<ImgClassClassifier&>("classifier"),
                results);
        {

                int i=0;
                int start_index = (crop_offset*img_channel);

                while(i<BSP_CAM_HEIGHT)
                {
                    memcpy(bsp_det_crop_model_ip_buffer888 + (i*img_channel*BSP_CAM_HEIGHT),
                           bsp_camera_out_buffer888 + (start_index + (i*img_channel*BSP_CAM_WIDTH)),
                           BSP_CAM_HEIGHT*img_channel);
                    i++;
                }

                nearestNeighborSamplingCls(bsp_det_crop_model_ip_buffer888,
                                        bsp_cls_model_ip_buffer888,
                                        false);

                const uint8_t* imgSrc = bsp_cls_model_ip_buffer888;

            if (nullptr == imgSrc) {
                error("Failed to get captured Image");
                return false;
            }

            /* Select the image to run inference with. */
            info("Running inference on Captured Image");

            const size_t imgSz = inputTensor->bytes < IMAGE_DATA_SIZE ?
                                  inputTensor->bytes : IMAGE_DATA_SIZE;

            /* Run the pre-processing, inference and post-processing. */
            if (!preProcess.DoPreProcess(imgSrc, imgSz)) {
                error("Pre-processing failed.");
                return false;
            }
            SCB_CleanDCache();
            uint64_t t1 = get_timestamp();
            if (!RunInference(model)) {
                error("Inference failed.");
                return false;
            }
            SCB_CleanDCache();
            image_classification_inference_time = get_timestamp() - t1;
            if (!postProcess.DoPostProcess()) {
                error("Post-processing failed.");
                return false;
            }

            /* Add results to context for access outside handler. */
            ctx.Set<std::vector<ClassificationResult>>("results", results);
            e_printf("\r\nInference time: %.1f ms", image_classification_inference_time/1000.f);
            if (!PresentInferenceResult(results)) {
                return false;
            }
        }

        return true;
    }

} /* namespace app */
} /* namespace arm */

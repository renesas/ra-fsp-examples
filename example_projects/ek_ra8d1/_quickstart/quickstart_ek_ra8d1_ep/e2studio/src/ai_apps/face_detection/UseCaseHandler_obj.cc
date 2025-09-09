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
#include <InputFiles_obj.hpp>
#include <UseCaseHandler_obj.hpp>
#include "YoloFastestModel.hpp"
#include "UseCaseCommonUtils.hpp"
#include "DetectorPostProcessing.hpp"
#include "DetectorPreProcessing.hpp"
#include "log_macros.h"
#include "camera_layer.h"

extern "C" {
#include "timer.h"
void update_detection_result(signed short index, signed short  x, signed short  y, signed short  w, signed short  h);
}

#include <cinttypes>

extern uint8_t bsp_camera_out_buffer888[];
extern uint8_t bsp_det_model_ip_buffer888[];
extern uint8_t bsp_det_rot_model_ip_buffer888[];
extern uint8_t bsp_det_crop_model_ip_buffer888[];
extern uint8_t bsp_camera_out_rot_buffer565[];
extern uint8_t bsp_camera_out_buffer565[];


#define DST_HEIGHT 192
#define DST_WIDTH  192
#define SRC_HEIGHT 240
#define SRC_WIDTH  240
#define img_channel 3
#define crop_offset 40

extern "C" {
	extern uint64_t face_detection_inference_time;
}


extern void nearestNeighborSampling(uint8_t* srcImage, uint8_t* dstImage, bool pad)
{

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
            // printf("height: %d, width: %d\n", y, x);
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

    /**
     * @brief           Presents inference results along using the data presentation
     *                  object.
     * @param[in]       results            Vector of detection results to be displayed.
     * @return          true if successful, false otherwise.
     **/
    static bool PresentInferenceResult(const std::vector<object_detection::DetectionResult>& results);

    /**
     * @brief           Draw boxes directly on the LCD for all detected objects.
     * @param[in]       results            Vector of detection results to be displayed.
     * @param[in]       imageStartX        X coordinate where the image starts on the LCD.
     * @param[in]       imageStartY        Y coordinate where the image starts on the LCD.
     * @param[in]       imgDownscaleFactor How much image has been downscaled on LCD.
     **/

    /* Object detection inference handler. */
    bool ObjectDetectionHandler(ApplicationContext& ctx, uint32_t imgIndex, bool runAll)
    {
        auto& model = ctx.Get<Model&>("model");

        if (!model.IsInited()) {
            error("Model is not initialized! Terminating processing.\n");
            return false;
        }

        TfLiteTensor* inputTensor = model.GetInputTensor(0);
        TfLiteTensor* outputTensor0 = model.GetOutputTensor(0);
        TfLiteTensor* outputTensor1 = model.GetOutputTensor(1);

        if (!inputTensor->dims) {
            error("Invalid input tensor dims\n");
            return false;
        } else if (inputTensor->dims->size < 3) {
            error("Input tensor dimension should be >= 3\n");
            return false;
        }

        TfLiteIntArray* inputShape = model.GetInputShape(0);

        const int inputImgCols = inputShape->data[YoloFastestModel::ms_inputColsIdx];
        const int inputImgRows = inputShape->data[YoloFastestModel::ms_inputRowsIdx];

        /* Set up pre and post-processing. */
        DetectorPreProcess preProcess = DetectorPreProcess(inputTensor, true, model.IsDataSigned());

        std::vector<object_detection::DetectionResult> results;

        arm::app::object_detection::PostProcessParams postProcessParams {
            inputImgRows, inputImgCols, object_detection::originalImageSize,
            object_detection::anchor1, object_detection::anchor2
        };
        DetectorPostProcess postProcess = DetectorPostProcess(outputTensor0, outputTensor1,
                results, postProcessParams);
        {
            /* Ensure there are no results leftover from previous inference when running all. */
            results.clear();
            int i=0;
                int start_index = (crop_offset*img_channel);

                while(i<BSP_CAM_HEIGHT)
                {
                    memcpy(bsp_det_crop_model_ip_buffer888 + (i*img_channel*BSP_CAM_HEIGHT),
                           bsp_camera_out_buffer888 + (start_index + (i*img_channel*BSP_CAM_WIDTH)),
                           BSP_CAM_HEIGHT*img_channel);
                    i++;
                }


                nearestNeighborSampling(bsp_det_crop_model_ip_buffer888,
                                        bsp_det_model_ip_buffer888,
                                        false);

                const uint8_t* currImage = bsp_det_model_ip_buffer888;

            const size_t copySz = inputTensor->bytes < IMAGE_DATA_SIZE ?
                                inputTensor->bytes : IMAGE_DATA_SIZE;

            /* Run the pre-processing, inference and post-processing. */
            if (!preProcess.DoPreProcess(currImage, copySz)) {
                error("Pre-processing failed.");
                return false;
            }
            SCB_CleanDCache();
            /* Run inference over this image. */
            uint64_t t1 = get_timestamp();
            if (!RunInference(model)) {
                error("Inference failed.");
                return false;
            }
            SCB_CleanDCache();
            face_detection_inference_time = get_timestamp() - t1;

            if (!postProcess.DoPostProcess()) {
                error("Post-processing failed.");
                return false;
            }

            e_printf("\r\nInference time: %.1f ms", face_detection_inference_time/1000.f);
            if (!PresentInferenceResult(results)) {
                return false;
            }

        }
        return true;
    }

    static bool PresentInferenceResult(const std::vector<object_detection::DetectionResult>& results)
    {
    	for (uint32_t i = 0; i < 5; i++)
    	{
            update_detection_result(i, 0, 0, 0, 0);
    	}
    	for (uint32_t i = 0; i < results.size(); ++i) {

            e_printf("%" PRIu32 ") (%f) -> %s {x=%f,y=%f,w=%f,h=%f}", i,
                results[i].m_normalisedVal, "Detection box:",
				results[i].m_x0*1.0, results[i].m_y0*1.0, results[i].m_w*1.0, results[i].m_h*1.0 );
            update_detection_result(i,results[i].m_x0, results[i].m_y0, results[i].m_w, results[i].m_h );
        }

        return true;
    }
} /* namespace app */
} /* namespace arm */

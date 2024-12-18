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
#include "UseCaseHandler_obj.hpp"
#include "YoloFastestModel.hpp"
#include "UseCaseCommonUtils.hpp"
#include "DetectorPostProcessing.hpp"
#include "DetectorPreProcessing.hpp"
#include "log_macros.h"


extern "C" {
#include "dwt.h"
#include "common_util.h"
#include "ceu_ctl_and_data_processing.h"
extern uint64_t face_detection_inference_time;
void update_detection_result(uint16_t index, signed short  x, signed short  y, signed short  w, signed short  h);
}

#include <cinttypes>


#define IMAGE_DATA_SIZE  (DET_MODEL_IMG_SIZE_X*DET_MODEL_IMG_SIZE_X)


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
        * Entry point for activating an AI inference operation. The function activate the pre-inference, inference and post inference actions.
        * @param[in]       ApplicationContext& ctx: A reference to ApplicationContext, which allows access to the the information for the model.
        **/
    /* Object detection inference handler. */
 bool ObjectDetectionHandler(ApplicationContext& ctx)
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

        DetectorPreProcess preProcess = DetectorPreProcess(inputTensor, false, false);
        std::vector<object_detection::DetectionResult> results;
        
        arm::app::object_detection::PostProcessParams postProcessParams {
            inputImgRows, inputImgCols, object_detection::originalImageSize,
            object_detection::anchor1, object_detection::anchor2
        };
        DetectorPostProcess postProcess = DetectorPostProcess(outputTensor0, outputTensor1,
                results, postProcessParams);

		/* Ensure there are no results leftover from previous inference when running all. */
		results.clear();



	    /* Image is already cropped, resized and converted at this point. */
        const uint8_t* currImage = (uint8_t *) model_buffer_int8;
		const size_t copySz = inputTensor->bytes < IMAGE_DATA_SIZE ?
							inputTensor->bytes : IMAGE_DATA_SIZE;

		/* Run the pre-processing, inference and post-processing. */
		if (!preProcess.DoPreProcess(currImage, copySz)) {
			error("Pre-processing failed.");
			return false;
		}

		/* Run inference over this image. */
		volatile uint32_t old_counter =  GetCycleCounter();
		if (!RunInference(model)) {
			error("Inference failed.");
			return false;
		}

		face_detection_inference_time = (uint32_t)((GetCycleCounter() - old_counter)/SCALE_DWT_TO_MS);
		if (!postProcess.DoPostProcess()) {
			error("Post-processing failed.");
			return false;
		}

		if (!PresentInferenceResult(results)) {
			return false;
		}


        return true;
    }

    static bool PresentInferenceResult(const std::vector<object_detection::DetectionResult>& results)
    {
    	for (uint16_t i = 0; i < MAX_DETECTION_NUMS; i++)
    	{
            update_detection_result(i, (signed short)0, (signed short)0, (signed short)0, (signed short)0);
    	}
    	for (uint16_t i = 0; i < results.size(); ++i) {
    		update_detection_result(i, (signed short)results[i].m_x0, (signed short)results[i].m_y0, (signed short)results[i].m_w, (signed short)results[i].m_h );
        }

        return true;
    }
} /* namespace app */

} /* namespace arm */


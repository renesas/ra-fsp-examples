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
#include "DetectorPostProcessing.hpp"
#include "PlatformMath.hpp"

#include <cmath>

namespace arm {
namespace app {

// Constructor for the DetectorPostProcess class
DetectorPostProcess::DetectorPostProcess(
        TfLiteTensor* modelOutput0,	// Pointer to the first output tensor of the model
        TfLiteTensor* modelOutput1, // Pointer to the second output tensor of the model
        std::vector<object_detection::DetectionResult>& results,	// Reference to a vector to store detection results
        const object_detection::PostProcessParams& postProcessParams)	// Reference to post-processing parameters
        :   m_outputTensor0{modelOutput0},	// Initialize member variable with the first output tensor
            m_outputTensor1{modelOutput1},	// Initialize member variable with the second output tensor
            m_results{results},	// Initialize member variable with the results vector
            m_postProcessParams{postProcessParams}	// Initialize member variable with post-process parameters
{
    /* Init PostProcessing */
    this->m_net = object_detection::Network{
        .inputWidth  = postProcessParams.inputImgCols,	// Set input image width from post-process parameters
        .inputHeight = postProcessParams.inputImgRows,	// Set input image height from post-process parameters
        .numClasses  = postProcessParams.numClasses,	// Set number of classes from post-process parameters
        .branches =	// Initialize the branches
            {object_detection::Branch{ // First branch
									  .resolution  = postProcessParams.inputImgCols / 32,	// Calculate resolution
                                      .numBox      = 3,	// Number of boxes
                                      .anchor      = postProcessParams.anchor1,	// Anchor value for the first branch
                                      .modelOutput = this->m_outputTensor0->data.int8,	// Model output for the first tensor
                                      .scale       = (static_cast<TfLiteAffineQuantization*>(		// Scale value for the first tensor
                                                    this->m_outputTensor0->quantization.params))
                                                   ->scale->data[0],
                                      .zeroPoint = (static_cast<TfLiteAffineQuantization*>(	// Zero point value for the first tensor
                                                        this->m_outputTensor0->quantization.params))
                                                       ->zero_point->data[0],
                                      .size = this->m_outputTensor0->bytes},	// Size of the first tensor
             object_detection::Branch{	// Second branch
                                      .resolution  = postProcessParams.inputImgCols / 16,	// Calculate resolution
                                      .numBox      = 3,	// Number of boxes
                                      .anchor      = postProcessParams.anchor2,	// Anchor value for the second branch
                                      .modelOutput = this->m_outputTensor1->data.int8,	// Model output for the second tensor
                                      .scale       = (static_cast<TfLiteAffineQuantization*>(	// Scale value for the second tensor
                                                    this->m_outputTensor1->quantization.params))
                                                   ->scale->data[0],
                                      .zeroPoint = (static_cast<TfLiteAffineQuantization*>(	// Zero point value for the second tensor
                                                        this->m_outputTensor1->quantization.params))
                                                       ->zero_point->data[0],
                                      .size = this->m_outputTensor1->bytes}},	// Size of the second tensor
        .topN = postProcessParams.topN};	// Set the top N value from post-process parameters
    /* End init */
}
/*********************************************************************************************************************
 *  Performance NMS for multiple classes of detections, iteratively suppressing overlapping bounding boxes
 *  for each class based on their probabilities and the specified IoU threshold.
***********************************************************************************************************************/
bool DetectorPostProcess::DoPostProcess()
{
    /* Start postprocessing */
    int originalImageWidth  = m_postProcessParams.originalImageSize;
    int originalImageHeight = m_postProcessParams.originalImageSize;

    std::forward_list<image::Detection> detections;
    GetNetworkBoxes(this->m_net, originalImageWidth, originalImageHeight, m_postProcessParams.threshold, detections);

    /* Do nms */
    CalculateNMS(detections, this->m_net.numClasses, this->m_postProcessParams.nms);

    for (auto& it: detections) {
        float xMin = it.bbox.x - it.bbox.w / 2.0f;
        float xMax = it.bbox.x + it.bbox.w / 2.0f;
        float yMin = it.bbox.y - it.bbox.h / 2.0f;
        float yMax = it.bbox.y + it.bbox.h / 2.0f;

        if (xMin < 0) {
            xMin = 0;
        }
        if (yMin < 0) {
            yMin = 0;
        }
        if (xMax > originalImageWidth) {
            xMax = originalImageWidth;
        }
        if (yMax > originalImageHeight) {
            yMax = originalImageHeight;
        }

        float boxX = xMin;
        float boxY = yMin;
        float boxWidth = xMax - xMin;
        float boxHeight = yMax - yMin;

        for (int j = 0; j < this->m_net.numClasses; ++j) {
            if (it.prob[j] > 0) {

                object_detection::DetectionResult tmpResult = {};
                tmpResult.m_normalisedVal = it.prob[j];
                tmpResult.m_x0 = boxX;
                tmpResult.m_y0 = boxY;
                tmpResult.m_w = boxWidth;
                tmpResult.m_h = boxHeight;

                this->m_results.push_back(tmpResult);
            }
        }
    }
    return true;
}
/*********************************************************************************************************************
* Insert new detections while maintaining the top N detections based on confidence scores with the top of the list having the largest objectness score
* @param[IN]   std::forward_list<image::Detection>& detections: the neural network output
* @param[IN]   image::Detection& det: A reference to the new detection that needs to be inserted into the list
***********************************************************************************************************************/
void DetectorPostProcess::InsertTopNDetections(std::forward_list<image::Detection>& detections, image::Detection& det)
{
    std::forward_list<image::Detection>::iterator it;
    std::forward_list<image::Detection>::iterator last_it;
    for ( it = detections.begin(); it != detections.end(); ++it ) {
        if(it->objectness > det.objectness)
            break;
        last_it = it;
    }
    if(it != detections.begin()) {
        detections.emplace_after(last_it, det);
        detections.pop_front();
    }
}

/*********************************************************************************************************************
 *  This function processes the output of an object detection neural network, generates bounding box predictions,
 *  calculates the scores and class probabilities, and retains the top N detections based on confidence scores.
 *  @param[IN]   object_detection::Network& net: the neural network output
 *  @param[IN]   int imageWidth: width of the input image.
 *  @param[IN]   int imageHeight: height of the input image.
 *  @param[IN]   float threshold: the confidence threshold. Bounding boxes with confidence scores below this threshold will be discarded.
 *  @param[IN]   std::forward_list<image::Detection>& detections: A forward list of image::Detection objects.
 *  			 This list will be populated with the detected objects.
***********************************************************************************************************************/
void DetectorPostProcess::GetNetworkBoxes(
        object_detection::Network& net,
        int imageWidth,
        int imageHeight,
        float threshold,
        std::forward_list<image::Detection>& detections)
{
    int numClasses = net.numClasses;
    int num = 0;
    auto det_objectness_comparator = [](image::Detection& pa, image::Detection& pb) {
        return pa.objectness < pb.objectness;
    };
    for (size_t i = 0; i < net.branches.size(); ++i) {
        int height   = net.branches[i].resolution;
        int width    = net.branches[i].resolution;
        int channel  = net.branches[i].numBox*(5+numClasses);

        for (int h = 0; h < net.branches[i].resolution; h++) {
            for (int w = 0; w < net.branches[i].resolution; w++) {
                for (int anc = 0; anc < net.branches[i].numBox; anc++) {

                    /* Objectness score */
                    int bbox_obj_offset = h * width * channel + w * channel + anc * (numClasses + 5) + 4;
                    float objectness = math::MathUtils::SigmoidF32(
                            (static_cast<float>(net.branches[i].modelOutput[bbox_obj_offset])
                            - (float)net.branches[i].zeroPoint
                            ) * net.branches[i].scale);

                    if(objectness > threshold) {
                        image::Detection det;
                        det.objectness = objectness;
                        /* Get bbox prediction data for each anchor, each feature point */
                        int bbox_x_offset = bbox_obj_offset -4;
                        int bbox_y_offset = bbox_x_offset + 1;
                        int bbox_w_offset = bbox_x_offset + 2;
                        int bbox_h_offset = bbox_x_offset + 3;
                        int bbox_scores_offset = bbox_x_offset + 5;

                        det.bbox.x = (static_cast<float>(net.branches[i].modelOutput[bbox_x_offset])
                                - (float)net.branches[i].zeroPoint) * net.branches[i].scale;
                        det.bbox.y = (static_cast<float>(net.branches[i].modelOutput[bbox_y_offset])
                                - (float)net.branches[i].zeroPoint) * net.branches[i].scale;
                        det.bbox.w = (static_cast<float>(net.branches[i].modelOutput[bbox_w_offset])
                                - (float)net.branches[i].zeroPoint) * net.branches[i].scale;
                        det.bbox.h = (static_cast<float>(net.branches[i].modelOutput[bbox_h_offset])
                                - (float)net.branches[i].zeroPoint) * net.branches[i].scale;

                        float bbox_x, bbox_y;

                        /* Eliminate grid sensitivity trick involved in YOLOv4 */
                        bbox_x = math::MathUtils::SigmoidF32(det.bbox.x);
                        bbox_y = math::MathUtils::SigmoidF32(det.bbox.y);
                        det.bbox.x = (float)((bbox_x + (float)w) / (float)(width));
                        det.bbox.y = (float)((bbox_y + (float)h) / (float)(height));

                        det.bbox.w = (float)(std::exp(det.bbox.w) * net.branches[i].anchor[anc*2] / net.inputWidth);
                        det.bbox.h = (float)(std::exp(det.bbox.h) * net.branches[i].anchor[anc*2+1] / net.inputHeight);

                        for (int s = 0; s < numClasses; s++) {
                            float sig = math::MathUtils::SigmoidF32(
                                    (static_cast<float>(net.branches[i].modelOutput[bbox_scores_offset + s]) -
                                    (float)net.branches[i].zeroPoint) * net.branches[i].scale
                                    ) * objectness;
                            det.prob.emplace_back((sig > threshold) ? sig : 0);
                        }

                        /* Correct_YOLO_boxes */
                        det.bbox.x *= imageWidth;
                        det.bbox.w *= imageWidth;
                        det.bbox.y *= imageHeight;
                        det.bbox.h *= imageHeight;

                        if (num < net.topN || net.topN <=0) {
                            detections.emplace_front(det);
                            num += 1;
                        } else if (num == net.topN) {
                            detections.sort(det_objectness_comparator);
                            InsertTopNDetections(detections, det);
                            num += 1;
                        } else {
                            InsertTopNDetections(detections, det);
                        }
                    }
                }
            }
        }
    }
    if(num > net.topN)
        num -=1;
}

} /* namespace app */
} /* namespace arm */

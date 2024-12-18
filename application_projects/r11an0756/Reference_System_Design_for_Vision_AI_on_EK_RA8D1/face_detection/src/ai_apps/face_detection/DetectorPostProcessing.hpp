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
#ifndef DETECTOR_POST_PROCESSING_HPP
#define DETECTOR_POST_PROCESSING_HPP

#include <ai_apps/common/BaseProcessing.hpp>
#include <ai_apps/common/ImageUtils.hpp>
#include <ai_apps/face_detection/DetectionResult.hpp>
#include <ai_apps/face_detection/YoloFastestModel.hpp>
#include <forward_list>

namespace arm {
namespace app {
namespace object_detection {
	// post-processing parameters
    struct PostProcessParams {
        int inputImgRows{};			// Number of rows i the input image
        int inputImgCols{};			// Number of columns in the input image
        int originalImageSize{};	// Original size of the image
        const float* anchor1;		// Pointer to the first set of anchor boxes
        const float* anchor2;		// Pointer to the second set of anchor boxes
        float threshold = 0.5f;		// Threshold of object detection
        float nms = 0.45f;			// Non-maximum suppression threshold
        int numClasses = 1;			// Number of classes for object detection
        int topN = 0;				// Top N results to consider

    // Constructor to initialize the post-processing parameters
	PostProcessParams(int iptRows, int iptCols, int size, const float* a1, const float* a2):
	    inputImgRows(iptRows), inputImgCols(iptCols), originalImageSize(size), anchor1(a1), anchor2(a2){}
		
    };

    // Structure to represent a branch in the object detection network
    struct Branch {
        int resolution;				// Resolution of the branch
        int numBox;					// Number of boxes in the branch
        const float* anchor;		// Pointer to the anchor boxes
        int8_t* modelOutput;		// Pointer to the model output
        float scale;				// Scale factor for the output
        int zeroPoint;				// Zero point for quantization
        size_t size;				// Size of the branch
    };

    // Structure to represent the object detection network
    struct Network {
        int inputWidth;					// Width of the input image
        int inputHeight;				// Height of the input image
        int numClasses;					// Number of classes for object detection
        std::vector<Branch> branches;	// Vector of branches in the network
        int topN;						// Top N results to consider
    };

} /* namespace object_detection */

    /**
     * @brief   Post-processing class for Object Detection use case.
     *          Implements methods declared by BasePostProcess and anything else needed
     *          to populate result vector.
     */
    class DetectorPostProcess : public BasePostProcess {
    public:
        /**
         * @brief        Constructor.
         * @param[in]    outputTensor0       Pointer to the TFLite Micro output Tensor at index 0.
         * @param[in]    outputTensor1       Pointer to the TFLite Micro output Tensor at index 1.
         * @param[out]   results             Vector of detected results.
         * @param[in]    postProcessParams   Struct of various parameters used in post-processing.
         **/
        explicit DetectorPostProcess(TfLiteTensor* outputTensor0,
                                     TfLiteTensor* outputTensor1,
                                     std::vector<object_detection::DetectionResult>& results,
                                     const object_detection::PostProcessParams& postProcessParams);

        /**
         * @brief    Should perform YOLO post-processing of the result of inference then
         *           populate Detection result data for any later use.
         * @return   true if successful, false otherwise.
         **/
        bool DoPostProcess() override;

    private:
        TfLiteTensor* m_outputTensor0;                                   /* Output tensor index 0 */
        TfLiteTensor* m_outputTensor1;                                   /* Output tensor index 1 */
        std::vector<object_detection::DetectionResult>& m_results;       /* Single inference results. */
        const object_detection::PostProcessParams& m_postProcessParams;  /* Post processing param struct. */
        object_detection::Network m_net;                                 /* YOLO network object. */

        /**
         * @brief       Insert the given Detection in the list.
         * @param[in]   detections   List of detections.
         * @param[in]   det          Detection to be inserted.
         **/
        void InsertTopNDetections(std::forward_list<image::Detection>& detections, image::Detection& det);

        /**
         * @brief        Given a Network calculate the detection boxes.
         * @param[in]    net           Network.
         * @param[in]    imageWidth    Original image width.
         * @param[in]    imageHeight   Original image height.
         * @param[in]    threshold     Detections threshold.
         * @param[out]   detections    Detection boxes.
         **/
        void GetNetworkBoxes(object_detection::Network& net,
                             int imageWidth,
                             int imageHeight,
                             float threshold,
                             std::forward_list<image::Detection>& detections);
    };

} /* namespace app */
} /* namespace arm */

#endif /* DETECTOR_POST_PROCESSING_HPP */

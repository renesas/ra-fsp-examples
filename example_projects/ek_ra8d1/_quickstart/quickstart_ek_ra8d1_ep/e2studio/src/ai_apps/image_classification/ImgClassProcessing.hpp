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
#ifndef IMG_CLASS_PROCESSING_HPP
#define IMG_CLASS_PROCESSING_HPP

#include <ai_apps/common/BaseProcessing.hpp>
#include <ai_apps/common/Classifier.hpp>

namespace arm {
namespace app {

    /**
     * @brief   Pre-processing class for Image Classification use case.
     *          Implements methods declared by BasePreProcess and anything else needed
     *          to populate input tensors ready for inference.
     */
    class ImgClassPreProcess : public BasePreProcess {

    public:
        /**
         * @brief       Constructor
         * @param[in]   inputTensor     Pointer to the TFLite Micro input Tensor.
         * @param[in]   convertToInt8   Should the image be converted to Int8 range.
         **/
        explicit ImgClassPreProcess(TfLiteTensor* inputTensor, bool convertToInt8);

        /**
         * @brief       Should perform pre-processing of 'raw' input image data and load it into
         *              TFLite Micro input tensors ready for inference
         * @param[in]   input      Pointer to the data that pre-processing will work on.
         * @param[in]   inputSize  Size of the input data.
         * @return      true if successful, false otherwise.
         **/
        bool DoPreProcess(const void* input, size_t inputSize) override;

    private:
        TfLiteTensor* m_inputTensor;
        bool m_convertToInt8;
    };

    /**
     * @brief   Post-processing class for Image Classification use case.
     *          Implements methods declared by BasePostProcess and anything else needed
     *          to populate result vector.
     */
    class ImgClassPostProcess : public BasePostProcess {

    public:
        /**
         * @brief       Constructor
         * @param[in]   outputTensor  Pointer to the TFLite Micro output Tensor.
         * @param[in]   classifier    Classifier object used to get top N results from classification.
         * @param[in]   results       Vector of classification results to store decoded outputs.
         **/
        ImgClassPostProcess(TfLiteTensor* outputTensor, Classifier& classifier,
                            std::vector<ClassificationResult>& results);

        /**
         * @brief       Should perform post-processing of the result of inference then
         *              populate classification result data for any later use.
         * @return      true if successful, false otherwise.
         **/
        bool DoPostProcess() override;

    private:
        TfLiteTensor* m_outputTensor;
        Classifier& m_imgClassifier;
        std::vector<ClassificationResult>& m_results;
    };

} /* namespace app */
} /* namespace arm */

#endif /* IMG_CLASS_PROCESSING_HPP */

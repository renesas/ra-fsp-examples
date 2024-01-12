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
#include "ImgClassProcessing.hpp"

#include "ImageUtils.hpp"
#include "log_macros.h"

namespace arm {
namespace app {

    ImgClassPreProcess::ImgClassPreProcess(TfLiteTensor* inputTensor, bool convertToInt8)
    :m_inputTensor{inputTensor},
     m_convertToInt8{convertToInt8}
    {}

    bool ImgClassPreProcess::DoPreProcess(const void* data, size_t inputSize)
    {
        if (data == nullptr) {
            error("Data pointer is null");
            return false;
        }

        auto input = static_cast<const uint8_t*>(data);

        std::memcpy(this->m_inputTensor->data.data, input, inputSize);
        debug("Input tensor populated \n");

        if (this->m_convertToInt8) {
            image::ConvertImgToInt8(this->m_inputTensor->data.data, this->m_inputTensor->bytes);
        }

        return true;
    }

    ImgClassPostProcess::ImgClassPostProcess(TfLiteTensor* outputTensor, Classifier& classifier,
                                             std::vector<ClassificationResult>& results)
            :m_outputTensor{outputTensor},
             m_imgClassifier{classifier},
             m_results{results}
    {}

    bool ImgClassPostProcess::DoPostProcess()
    {
        return this->m_imgClassifier.GetClassificationResults(
                this->m_outputTensor, this->m_results,
                5, false);
    }

} /* namespace app */
} /* namespace arm */

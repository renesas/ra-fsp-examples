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
#include "DetectorPreProcessing.hpp"
#include "ImageUtils.hpp"
#include "log_macros.h"

namespace arm {
namespace app {

    DetectorPreProcess::DetectorPreProcess(TfLiteTensor* inputTensor, bool rgb2Gray, bool convertToInt8)
    :   m_inputTensor{inputTensor},
        m_rgb2Gray{rgb2Gray},
        m_convertToInt8{convertToInt8}
    {}

    bool DetectorPreProcess::DoPreProcess(const void* data, size_t inputSize) {
        if (data == nullptr) {
            error("Data pointer is null");
        }

        auto input = static_cast<const uint8_t*>(data);

        if (this->m_rgb2Gray) {
            image::RgbToGrayscale(input, this->m_inputTensor->data.uint8, this->m_inputTensor->bytes);
        } else {
            std::memcpy(this->m_inputTensor->data.data, input, inputSize);
        }
        debug("Input tensor populated \n");

        if (this->m_convertToInt8) {
            image::ConvertImgToInt8(this->m_inputTensor->data.data, this->m_inputTensor->bytes);
        }

        return true;
    }

} /* namespace app */
} /* namespace arm */

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
#include <ai_common/ImageUtils.hpp>
#include "ImgClassProcessing.hpp"
#include "ethosu_log.h"

#include "ai_application_config.h"

namespace arm {
namespace app {

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
                AI_MAX_DETECTION_NUM, false);
    }

} /* namespace app */
} /* namespace arm */

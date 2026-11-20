/*
 * SPDX-FileCopyrightText: Copyright 2021, 2023 Arm Limited and/or its affiliates <open-source-office@arm.com>
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
#ifndef CLASSIFICATION_RESULT_HPP
#define CLASSIFICATION_RESULT_HPP

#include <cstdint>
#include <string>

namespace arm {
namespace app {

    /**
     * @brief   Class representing a single classification result.
     */
    class ClassificationResult {
    public:
        double          m_normalisedVal = 0.0;
        std::string     m_label;
        uint32_t        m_labelIdx = 0;

        ClassificationResult() = default;
        ~ClassificationResult() = default;
    };

} /* namespace app */
} /* namespace arm */

#endif /* CLASSIFICATION_RESULT_HPP */

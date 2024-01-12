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
#ifndef DETECTION_RESULT_HPP
#define DETECTION_RESULT_HPP


namespace arm {
namespace app {
namespace object_detection {

    /**
     * @brief   Class representing a single detection result.
     */
    class DetectionResult {
    public:
        /**
         * @brief       Constructor
         * @param[in]   normalisedVal   Result normalized value
         * @param[in]   x0              Top corner x starting point
         * @param[in]   y0              Top corner y starting point
         * @param[in]   w               Detection result width
         * @param[in]   h               Detection result height
         **/
        DetectionResult(double normalisedVal,int32_t x0,int32_t y0, int32_t w,int32_t h) :
                m_normalisedVal(normalisedVal),
                m_x0(x0),
                m_y0(y0),
                m_w(w),
                m_h(h)
            {
            }

        DetectionResult() = default;
        ~DetectionResult() = default;

        double  m_normalisedVal{0.0};
        int32_t     m_x0{0};
        int32_t     m_y0{0};
        int32_t     m_w{0};
        int32_t     m_h{0};
    };

} /* namespace object_detection */
} /* namespace app */
} /* namespace arm */

#endif /* DETECTION_RESULT_HPP */

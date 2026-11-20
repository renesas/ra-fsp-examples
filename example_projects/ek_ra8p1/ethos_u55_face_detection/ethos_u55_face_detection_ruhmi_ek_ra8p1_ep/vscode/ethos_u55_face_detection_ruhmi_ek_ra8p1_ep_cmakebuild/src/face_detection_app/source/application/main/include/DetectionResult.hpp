/*
 * Copyright (c) 2021 Arm Limited. All rights reserved.
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

#include <string>

namespace arm {
namespace app {

    /**
     * @brief   Class representing a single detection result.
     */
    class DetectionResult {
    
    public:
        double  m_normalisedVal = 0.0;
        int     m_x0=0;
        int     m_y0=0;
        int     m_w=0;
        int     m_h=0;
       
        DetectionResult() = default;
        ~DetectionResult() = default;
        
        DetectionResult(double normalisedVal,int x0,int y0, int w,int h) :
                m_normalisedVal(normalisedVal),
                m_x0(x0),
                m_y0(y0),
                m_w(w),
                m_h(h) 
            {
            };
        
    };

} /* namespace app */
} /* namespace arm */

#endif /* DETECTION_RESULT_HPP */

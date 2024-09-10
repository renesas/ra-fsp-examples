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
#ifndef BASE_PROCESSING_HPP
#define BASE_PROCESSING_HPP

#include <cstddef>

namespace arm {
namespace app {

    /**
     * @brief   Base class exposing pre-processing API.
     *          Use cases should provide their own PreProcessing class that inherits from this one.
     *          All steps required to take raw input data and populate tensors ready for inference
     *          should be handled.
     */
    class BasePreProcess {

    public:
        virtual ~BasePreProcess() = default;

        /**
         * @brief       Should perform pre-processing of 'raw' input data and load it into
         *              TFLite Micro input tensors ready for inference
         * @param[in]   input      Pointer to the data that pre-processing will work on.
         * @param[in]   inputSize  Size of the input data.
         * @return      true if successful, false otherwise.
         **/
        virtual bool DoPreProcess(const void* input, size_t inputSize) = 0;
    };

    /**
     * @brief   Base class exposing post-processing API.
     *          Use cases should provide their own PostProcessing class that inherits from this one.
     *          All steps required to take inference output and populate results vectors should be handled.
     */
    class BasePostProcess {

    public:
        virtual ~BasePostProcess() = default;

        /**
         * @brief       Should perform post-processing of the result of inference then populate
         *              populate result data for any later use.
         * @return      true if successful, false otherwise.
         **/
        virtual bool DoPostProcess() = 0;
    };

} /* namespace app */
} /* namespace arm */

#endif /* BASE_PROCESSING_HPP */
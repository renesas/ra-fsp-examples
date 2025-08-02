/*
 * SPDX-FileCopyrightText: Copyright 2021-2022, 2024-2025 Arm Limited and/or its
 * affiliates <open-source-office@arm.com>
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
#ifndef USECASE_COMMON_UTILS_HPP
#define USECASE_COMMON_UTILS_HPP

#include "Model.hpp"
#include "Classifier.hpp"

namespace arm {
namespace app {
    /**
     * @brief           Presents inference results for image classification use cases
     *                  using the data presentation object.
     * @param[in]       results     Vector of classification results to be displayed.
     * @return          true if successful, false otherwise.
     **/
    bool PresentInferenceResult(const std::vector<arm::app::ClassificationResult>& results);

    /**
     * @brief           Run inference using given model
     *                  object. If profiling is enabled, it will log the
     *                  statistics too.
     * @param[in]       model      Reference to the initialised model.
     * @param[in]       profiler   Reference to the initialised profiler.
     * @return          true if inference succeeds, false otherwise.
     **/
    bool RunInference(Model& model);

#ifdef INTERACTIVE_MODE
    /**
     * @brief           Wait for the user to provide any input.
     **/
    void AwaitUserInput();
#endif /* INTERACTIVE_MODE */

#if VERIFY_TEST_OUTPUT
    /**
     * @brief       Helper function to dump a tensor to stdout
     * @param[in]   tensor  tensor to be dumped
     * @param[in]   lineBreakForNumElements     number of elements
     *              after which line break will be added.
     **/
    void DumpTensor(const TfLiteTensor* tensor,
                    size_t lineBreakForNumElements = 16);


    void DumpTensorData(const uint8_t* tensorData,
                        size_t size,
                        size_t lineBreakForNumElements = 16);
#endif /* VERIFY_TEST_OUTPUT */
} /* namespace app */
} /* namespace arm */


#endif /* USECASE_COMMON_UTILS_HPP */

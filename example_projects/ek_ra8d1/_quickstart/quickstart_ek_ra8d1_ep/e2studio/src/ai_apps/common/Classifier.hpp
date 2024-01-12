/*
 * SPDX-FileCopyrightText: Copyright 2021-2022 Arm Limited and/or its affiliates <open-source-office@arm.com>
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
#ifndef CLASSIFIER_HPP
#define CLASSIFIER_HPP

#include <ai_apps/common/ClassificationResult.hpp>
#include <ai_apps/common/TensorFlowLiteMicro.hpp>
#include <vector>

namespace arm {
namespace app {

    /**
     * @brief   Classifier - a helper class to get certain number of top
     *          results from the output vector from a classification NN.
     **/
    class Classifier{
    public:
        /** @brief Constructor. */
        Classifier() = default;

        virtual ~Classifier() = default;

        /**
         * @brief       Gets the top N classification results from the
         *              output vector.
         * @param[in]   outputTensor   Inference output tensor from an NN model.
         * @param[out]  vecResults     A vector of classification results.
         *                             populated by this function.
         * @param[in]   topNCount      Number of top classifications to pick.
         * @param[in]   useSoftmax     Whether Softmax normalisation should be applied to output.
         * @return      true if successful, false otherwise.
         **/

        virtual bool GetClassificationResults(
            TfLiteTensor* outputTensor,
            std::vector<ClassificationResult>& vecResults,
            uint32_t topNCount,
            bool use_softmax);

        /**
        * @brief       Populate the elements of the Classification Result object.
        * @param[in]   topNSet        Ordered set of top 5 output class scores.
        * @param[out]  vecResults     A vector of classification results.
        *                             populated by this function.
        **/

        void SetVectorResults(
            std::set<std::pair<float, uint32_t>>& topNSet,
            std::vector<ClassificationResult>& vecResults);

    protected:
        /**
         * @brief       Utility function that gets the top N classification results from the
         *              output vector.
         * @param[in]   tensor       Inference output tensor from an NN model.
         * @param[out]  vecResults   A vector of classification results
         *                           populated by this function.
         * @param[in]   topNCount    Number of top classifications to pick.
         * @return      true if successful, false otherwise.
         **/

        bool GetTopNResults(const std::vector<float>& tensor,
                            std::vector<ClassificationResult>& vecResults,
                            uint32_t topNCount);
    };

} /* namespace app */
} /* namespace arm */

#endif /* CLASSIFIER_HPP */

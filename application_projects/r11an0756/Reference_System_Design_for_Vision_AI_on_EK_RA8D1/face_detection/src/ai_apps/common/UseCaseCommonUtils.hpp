/*
 * SPDX-FileCopyrightText: Copyright 2021-2022  Arm Limited and/or its affiliates <open-source-office@arm.com>
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

#include <ai_apps/common/AppContext.hpp>
#include <ai_apps/common/TensorFlowLiteMicro.hpp>
#include <ai_apps/common/Model.hpp>




namespace common {

  enum OPCODES {
        MENU_OPT_RUN_INF_NEXT = 1, /* Run on next vector. */
        MENU_OPT_RUN_INF_CHOSEN, /* Run on a user provided vector index. */
        MENU_OPT_RUN_INF_ALL, /* Run inference on all. */
        MENU_OPT_SHOW_MODEL_INFO, /* Show model info. */
        MENU_OPT_LIST_IFM /* List the current IFM. */
  };

}

namespace arm {
namespace app {
    /**
     * @brief           Run inference using given model
     *                  object. If profiling is enabled, it will log the
     *                  statistics too.
     * @param[in]       model      Reference to the initialised model.
     * @param[in]       profiler   Reference to the initialised profiler.
     * @return          true if inference succeeds, false otherwise.
     **/
    bool RunInference(arm::app::Model& model);

    /**
     * @brief           Read input and return as an integer.
     * @return          Integer value corresponding to the user input.
     **/
    int ReadUserInputAsInt();

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

    /**
     * @brief       List the files baked in the application.
     * @param[in]   ctx   Reference to the application context.
     * @return      true or false based on event being handled.
     **/
    bool ListFilesHandler(ApplicationContext& ctx);

} /* namespace app */
} /* namespace arm */


#endif /* USECASE_COMMON_UTILS_HPP */

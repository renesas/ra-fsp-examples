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
#ifndef TENSORFLOW_LITE_MICRO_LOCAL_HPP
#define TENSORFLOW_LITE_MICRO_LOCAL_HPP

#include "tensorflow/lite/c/common.h"
#include "tensorflow/lite/kernels/internal/tensor_ctypes.h"

namespace arm {
namespace app {

    /** Struct for quantization parameters. */
    struct QuantParams {
        float   scale   = 1.0;
        int     offset  = 0;
    };

    /**
     * @brief       Gets the quantization parameters from a tensor
     * @param[in]   tensor  pointer to the tensor.
     * @return      QuantParams object.
     */
    QuantParams GetTensorQuantParams(TfLiteTensor* tensor);

} /* namespace app */
} /* namespace arm */

/**
 * @brief Prints the tensor flow version in use to stdout.
 */
void PrintTensorFlowVersion();

/**
 * @brief   String logging functionality expected to be defined
 *          by TensorFlow Lite Micro's error reporter.
 * @param[in]   s   Pointer to the string.
 */
extern "C" void DebugLog(const char* s) __attribute__((__weak__));

#endif /* TENSORFLOW_LITE_MICRO_LOCAL_HPP */

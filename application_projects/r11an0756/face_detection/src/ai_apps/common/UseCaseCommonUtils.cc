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

#include "UseCaseCommonUtils.hpp"
#include "ImageUtils.hpp"
#include "log_macros.h"
#include <cinttypes>

extern "C"{
	void update_classification_result(unsigned index, unsigned short category, float probability);
}


namespace arm {
namespace app {


bool RunInference(arm::app::Model& model)
{
    bool runInf = model.RunInference();

    return runInf;
}

int ReadUserInputAsInt()
{
 //   int8_t chInput[128];
 //   memset(chInput, 0, sizeof(chInput));
 //   get_line(chInput, 2);
 //   return atoi((const char*)chInput);
    return 1;
}

void DumpTensorData(const uint8_t* tensorData,
                    size_t size,
                    size_t lineBreakForNumElements)
{
    char strhex[8];
    std::string strdump;

    for (size_t i = 0; i < size; ++i) {
        if (0 == i % lineBreakForNumElements) {
            printf("%s\n\t", strdump.c_str());
            strdump.clear();
        }
        snprintf(strhex, sizeof(strhex) - 1,
                 "0x%02x, ", tensorData[i]);
        strdump += std::string(strhex);
    }

    if (!strdump.empty()) {
        printf("%s\n", strdump.c_str());
    }
}

void DumpTensor(const TfLiteTensor* tensor, const size_t lineBreakForNumElements)
{
    if (!tensor) {
        error("invalid tensor\n");
        return;
    }

    const uint32_t tensorSz = tensor->bytes;
    const auto* tensorData = tflite::GetTensorData<uint8_t>(tensor);

    DumpTensorData(tensorData, tensorSz, lineBreakForNumElements);
}


} /* namespace app */
} /* namespace arm */

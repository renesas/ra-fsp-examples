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
#include <ai_apps/image_classification/InputFiles_img.hpp>
#include <ai_apps/image_classification/Labels.h>
#include "UseCaseCommonUtils.hpp"
#include "ImageUtils.hpp"
#include "log_macros.h"
#include <cinttypes>

extern "C"{
	void update_classification_result(unsigned index, unsigned short category, float probability);
}
void DisplayCommonMenu()
{
    printf("\n\n");
    printf("User input required\n");
    printf("Enter option number from:\n\n");
    printf("  %u. Classify next ifm\n", common::MENU_OPT_RUN_INF_NEXT);
    printf("  %u. Classify ifm at chosen index\n", common::MENU_OPT_RUN_INF_CHOSEN);
    printf("  %u. Run classification on all ifm\n", common::MENU_OPT_RUN_INF_ALL);
    printf("  %u. Show NN model info\n", common::MENU_OPT_SHOW_MODEL_INFO);
    printf("  %u. List ifm\n\n", common::MENU_OPT_LIST_IFM);
    printf("  Choice: ");
}

bool PresentInferenceResult(const std::vector<arm::app::ClassificationResult> &results)
{
    const char** labels = getLabelPtr();
	for (uint32_t i = 0; i < results.size(); ++i) {
        e_printf("%" PRIu32 ") %" PRIu32 " (%f) -> %s", i,
            results[i].m_labelIdx, results[i].m_normalisedVal,
			labels[results[i].m_labelIdx]);
        update_classification_result((unsigned)i, (unsigned short)results[i].m_labelIdx, (float)results[i].m_normalisedVal);
    }

    return true;
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

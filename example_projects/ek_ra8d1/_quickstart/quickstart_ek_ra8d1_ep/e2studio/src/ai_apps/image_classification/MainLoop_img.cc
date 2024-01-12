/*
 * SPDX-FileCopyrightText: Copyright 2021 - 2022 Arm Limited and/or its affiliates <open-source-office@arm.com>
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
#include <InputFiles_img.hpp>           /* For input images. */
#include <Labels.h>               /* For label strings. */
#include <UseCaseHandler_img.hpp>       /* Handlers for different user options. */
#include "Classifier.hpp"           /* Classifier. */
#include "MobileNetModel.hpp"       /* Model class for running inference. */
#include "UseCaseCommonUtils.hpp"   /* Utils functions. */
#include "BufAttributes.hpp"        /* Buffer attributes to be applied */
#include "log_macros.h"

extern uint8_t tensorArena[0x0080000];

namespace arm {
namespace app {
    namespace img_class {
        extern uint8_t* GetModelPointer();
        extern size_t GetModelLen();
    } /* namespace img_class */
} /* namespace app */
} /* namespace arm */
void main_loop_img()
{

	arm::app::MobileNetModel model_cls;  /* Model wrapper object. */

	using ImgClassClassifier = arm::app::Classifier;

    /* Load the model. */
    if (!model_cls.Init(tensorArena,
                        sizeof(tensorArena),
                        arm::app::img_class::GetModelPointer(),
                        arm::app::img_class::GetModelLen())) {
        error("Failed to initialize model\n");
        return;
    }

    /* Instantiate application context. */
    arm::app::ApplicationContext caseContext;

    caseContext.Set<arm::app::Model&>("model", model_cls);

    ImgClassClassifier classifier;  /* Classifier wrapper object. */
    caseContext.Set<arm::app::Classifier&>("classifier", classifier);

    /* Loop. */
    bool executionSuccessful = true;
    executionSuccessful = ClassifyImageHandler(caseContext);
}

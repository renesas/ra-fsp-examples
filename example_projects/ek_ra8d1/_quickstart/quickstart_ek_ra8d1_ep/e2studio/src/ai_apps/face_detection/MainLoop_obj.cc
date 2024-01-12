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
#include <InputFiles_obj.hpp>             /* For input images. */
#include <UseCaseHandler_obj.hpp>         /* Handlers for different user options. */
#include "YoloFastestModel.hpp"       /* Model class for running inference. */
#include "UseCaseCommonUtils.hpp"     /* Utils functions. */
#include "log_macros.h"
#include "BufAttributes.hpp"        /* Buffer attributes to be applied */

extern uint8_t tensorArena[0x0080000];

namespace arm {
namespace app {
    namespace object_detection {
        extern uint8_t* GetModelPointer();
        extern size_t GetModelLen();
    } /* namespace object_detection */
} /* namespace app */
} /* namespace arm */

static void DisplayDetectionMenu()
{
    printf("\n\n");
    printf("User input required\n");
    printf("Enter option number from:\n\n");
    printf("  %u. Run detection on next ifm\n", common::MENU_OPT_RUN_INF_NEXT);
    printf("  %u. Run detection ifm at chosen index\n", common::MENU_OPT_RUN_INF_CHOSEN);
    printf("  %u. Run detection on all ifm\n", common::MENU_OPT_RUN_INF_ALL);
    printf("  %u. Show NN model info\n", common::MENU_OPT_SHOW_MODEL_INFO);
    printf("  %u. List ifm\n\n", common::MENU_OPT_LIST_IFM);
    printf("  Choice: ");
    fflush(stdout);
}
void main_loop_obj()
{
	arm::app::YoloFastestModel model_obj;  /* Model wrapper object. */

    /* Load the model. */
    if (!model_obj.Init(tensorArena,
                        sizeof(tensorArena),
                        arm::app::object_detection::GetModelPointer(),
                        arm::app::object_detection::GetModelLen())) {
        error("Failed to initialize model\n");
        return;
    }

    /* Instantiate application context. */
    arm::app::ApplicationContext caseContext;

    caseContext.Set<arm::app::Model&>("model", model_obj);

    /* Loop. */
    bool executionSuccessful = true;
    /* Loop. */
    executionSuccessful = ObjectDetectionHandler(caseContext, 0, false);
}

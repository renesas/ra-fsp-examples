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
#include <UseCaseHandler_obj.hpp>         /* Handlers for different user options. */
#include "YoloFastestModel.hpp"       /* Model class for running inference. */
#include "UseCaseCommonUtils.hpp"     /* Utils functions. */
#include "log_macros.h"
extern "C" {
#include "common_util.h"
}

extern uint8_t tensorArena[0x0080000];

namespace arm {
namespace app {
    namespace object_detection {
        extern uint8_t* GetModelPointer();
        extern size_t GetModelLen();
    } /* namespace object_detection */
} /* namespace app */
} /* namespace arm */


extern "C" face_det_err_t ai_init(void);

arm::app::YoloFastestModel model_obj;  /* Model wrapper object. */

/*********************************************************************************************************************
 *  ai_init function: Initialize the AI tensorArena, model pointer and length.
 *  @param[IN]   None
 *  @retval      FACE_DET_APP_SUCCESS	AI initialization is successful.
 *  			 FACE_DET_APP_AI_INIT	AI initialization failed.
***********************************************************************************************************************/
face_det_err_t ai_init()
{
	static bool model_init_obj = false;
	face_det_err_t face_det_status = FACE_DET_APP_SUCCESS;
	 if(!model_init_obj)
	    {
	        if (!model_obj.Init(tensorArena,
					sizeof(tensorArena),
					arm::app::object_detection::GetModelPointer(),
					arm::app::object_detection::GetModelLen()))
	        {
	            error("Failed to initialize model\n");
	            face_det_status = FACE_DET_APP_AI_INIT;
	        }
	        else
	        {
				model_init_obj = true;

	        }
	    }
	 return face_det_status;
}
bool main_loop_face_detection();
/*********************************************************************************************************************
 *  main_loop_face_detection function: sets up the application context with a model object and then calls the
 *  ObjectDetectionHandler" function to perform the face detection.
 *  @param[IN]   None
 *  @retval      true	successful execution.
 *  			 false	handler call failed.
***********************************************************************************************************************/
bool main_loop_face_detection()
{
    /* Instantiate application context. */
    arm::app::ApplicationContext caseContext;

    caseContext.Set<arm::app::Model&>("model", model_obj);

    /* Loop. */
    bool executionSuccessful = true;
    /* Loop. */
    executionSuccessful = ObjectDetectionHandler(caseContext);
    return executionSuccessful;
}


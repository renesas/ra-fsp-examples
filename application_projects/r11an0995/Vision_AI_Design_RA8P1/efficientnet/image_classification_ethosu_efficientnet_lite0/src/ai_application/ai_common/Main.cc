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

/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/****************************************************************************\
 *               Main application file            *
\****************************************************************************/

#include <ai_common/log_macros.h>
#include "hal_data.h"

//#include "application_config.h"

extern "C" {
#include "common_util.h"
vision_ai_app_err_t image_classification(void);
}

extern bool main_loop_image_classification();

#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
__ASM(" .global __ARM_use_no_argv\n");
#endif

/*********************************************************************************************************************
 *  Wrapper function to interface with the AI inference operations
 *  @param[IN]   	None
 *  @retval      None
***********************************************************************************************************************/

vision_ai_app_err_t image_classification(void)
{
    vision_ai_app_err_t vision_ai_status = VISION_AI_APP_SUCCESS;

	if(false == main_loop_image_classification())
	{
	    vision_ai_status = VISION_AI_APP_ERR_AI_INFERENCE;
	}

    return vision_ai_status;
}

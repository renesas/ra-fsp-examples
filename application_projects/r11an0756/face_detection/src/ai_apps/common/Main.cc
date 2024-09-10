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

/****************************************************************************\
 *               Main application file            *
\****************************************************************************/

#include "log_macros.h"
#include "TensorFlowLiteMicro.hpp"  /* our inference logic api */
#include "hal_data.h"

extern "C" {
#include "common_util.h"
face_det_err_t face_detection(void);
}


uint8_t tensorArena[0x0080000];


extern bool main_loop_face_detection();

#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
__ASM(" .global __ARM_use_no_argv\n");
#endif



/*********************************************************************************************************************
 *  Wrapper function to interface with the AI inference operations
 *  @param[IN]   	None
 *  @retval      None
***********************************************************************************************************************/

face_det_err_t face_detection(void)
{
	face_det_err_t face_det_status = FACE_DET_APP_SUCCESS;

	if(false == main_loop_face_detection())
	{
		face_det_status = FACE_DET_APP_INFERENCE;
	}
    return face_det_status;
}

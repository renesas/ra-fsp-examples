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
#ifndef OBJ_DET_HANDLER_HPP
#define OBJ_DET_HANDLER_HPP

#include <ai_apps/common/AppContext.hpp>
#include "hal_data.h"

namespace arm {
namespace app {

    /**
     * @brief       Handles the inference event.
     * @param[in]   ctx        Pointer to the application context.
     * @param[in]   imgIndex   Index to the image to run object detection.
     * @param[in]   runAll     Flag to request classification of all the available images.
     * @return      true or false based on execution success.
     **/
    bool ObjectDetectionHandler(ApplicationContext& ctx);

} /* namespace app */
} /* namespace arm */

#endif /* OBJ_DET_HANDLER_HPP */

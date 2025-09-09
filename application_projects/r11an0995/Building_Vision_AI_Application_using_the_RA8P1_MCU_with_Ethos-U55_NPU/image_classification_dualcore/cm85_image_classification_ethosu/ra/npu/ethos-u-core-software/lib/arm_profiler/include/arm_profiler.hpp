/*
 * SPDX-FileCopyrightText: Copyright 2021-2022, 2024 Arm Limited and/or its affiliates <open-source-office@arm.com>
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ARM_PROFILER_H
#define ARM_PROFILER_H

#include "tensorflow/lite/micro/compatibility.h"
#include "tensorflow/lite/micro/micro_profiler_interface.h"
#include <memory>

// TODO: Merge this profiler with EthosUprofiler.
namespace tflite {
class ArmProfiler : public MicroProfilerInterface {
public:
    ArmProfiler(size_t max_events = 200);
    uint32_t BeginEvent(const char *tag);
    void EndEvent(uint32_t event_handle);
    uint64_t GetTotalTicks() const;
    void ReportResults() const;

private:
    size_t max_events_;
    std::unique_ptr<const char *[]> tags_;
    std::unique_ptr<uint32_t[]> start_ticks_;
    std::unique_ptr<uint32_t[]> end_ticks_;

    size_t num_events_;

    TF_LITE_REMOVE_VIRTUAL_DELETE;
};

} // namespace tflite

#endif

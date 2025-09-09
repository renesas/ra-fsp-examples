/*
 * SPDX-FileCopyrightText: Copyright 2021-2022 Arm Limited and/or its affiliates <open-source-office@arm.com>
 *
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

#ifndef LAYER_BY_LAYER_PROFILER_H
#define LAYER_BY_LAYER_PROFILER_H

#include "EventRecorder.h"
#include "tensorflow/lite/micro/compatibility.h"
#include "tensorflow/lite/micro/micro_profiler_interface.h"
#include <memory>
#include <vector>

// NOTE: This profiler only works on systems with 1 NPU due to the use of
// ethosu_reserve_driver().
namespace tflite {
class LayerByLayerProfiler : public MicroProfilerInterface {
public:
    enum Backend { PRINTF, EVENT_RECORDER };

    LayerByLayerProfiler(size_t max_events = 200,
                         Backend backend   = PRINTF,
                         int32_t event_id  = EventID(EventLevelError, EvtStatistics_No, EventRecordNone));

    uint32_t BeginEvent(const char *tag);
    void EndEvent(uint32_t event_handle);
    int32_t GetTotalTicks() const;
    void Log() const;

private:
    std::unique_ptr<const char *[]> tags_;
    std::unique_ptr<uint64_t[]> start_ticks_;
    std::unique_ptr<uint64_t[]> end_ticks_;

    size_t max_events_;
    Backend backend;
    int32_t event_id;
    size_t num_events_;

    TF_LITE_REMOVE_VIRTUAL_DELETE;
};

} // namespace tflite

#endif

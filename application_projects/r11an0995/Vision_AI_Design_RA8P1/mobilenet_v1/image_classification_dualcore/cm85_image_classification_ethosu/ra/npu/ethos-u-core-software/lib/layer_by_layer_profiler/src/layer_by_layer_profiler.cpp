/*
 * Copyright (c) 2021-2022 Arm Limited. All rights reserved.
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

#include "tensorflow/lite/kernels/internal/compatibility.h"
#include "tensorflow/lite/micro/micro_log.h"
#include "tensorflow/lite/micro/micro_profiler.h"
#include "tensorflow/lite/micro/micro_time.h"

#include <string.h>

#include "ethosu_log.h"
#include "layer_by_layer_profiler.hpp"
#include <inttypes.h>
#include <stdio.h>

namespace tflite {

LayerByLayerProfiler::LayerByLayerProfiler(size_t max_events, Backend _backend, int32_t _event_id) :
    max_events_(max_events), backend(_backend), event_id(_event_id), num_events_(0) {

    tags_        = std::make_unique<const char *[]>(max_events);
    start_ticks_ = std::make_unique<uint64_t[]>(max_events);
    end_ticks_   = std::make_unique<uint64_t[]>(max_events);
}

// NOTE: THIS PROFILER ONLY WORKS ON SYSTEMS WITH 1 NPU
uint32_t LayerByLayerProfiler::BeginEvent(const char *tag) {
    if (num_events_ == max_events_) {
        MicroPrintf("Profiling event overflow, max: %u events", max_events_);
        num_events_ = 0;
    }

    tags_[num_events_]        = tag;
    start_ticks_[num_events_] = GetCurrentTimeTicks();
    end_ticks_[num_events_] =
        start_ticks_[num_events_]; // NOTE: In case an EndEvent() doesn't trigger, cycles reports as 0

    return num_events_++;
}

// NOTE: THIS PROFILER ONLY WORKS ON SYSTEMS WITH 1 NPU
void LayerByLayerProfiler::EndEvent(uint32_t event_handle) {
    TFLITE_DCHECK(event_handle < max_events_);

    end_ticks_[event_handle] = GetCurrentTimeTicks();

    if (backend == PRINTF) {
        LOG("%s : cycle_cnt : %" PRIu64 " cycles\n",
            tags_[event_handle],
            end_ticks_[event_handle] - start_ticks_[event_handle]);
    } else {
        EventRecord2(event_id, (int32_t)event_handle, end_ticks_[event_handle] - start_ticks_[event_handle]);
    }
}

int32_t LayerByLayerProfiler::GetTotalTicks() const {
    int32_t ticks = 0;

    for (size_t i = 0; i < num_events_; ++i) {
        ticks += static_cast<int32_t>(end_ticks_[i] - start_ticks_[i]);
    }

    return ticks;
}

void LayerByLayerProfiler::Log() const {

#if !defined(TF_LITE_STRIP_ERROR_STRINGS)
    if (backend == PRINTF) {
        for (size_t i = 0; i < num_events_; ++i) {
            uint64_t ticks = end_ticks_[i] - start_ticks_[i];
            LOG("%s took %" PRIu64 " cycles", tags_[i], ticks);
        }
    }
#endif
}

} // namespace tflite

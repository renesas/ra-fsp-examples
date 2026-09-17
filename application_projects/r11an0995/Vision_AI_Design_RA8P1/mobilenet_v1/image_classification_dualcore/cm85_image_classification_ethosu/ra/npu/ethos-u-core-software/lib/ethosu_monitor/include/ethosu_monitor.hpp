/*
 * SPDX-FileCopyrightText: Copyright 2021-2023 Arm Limited and/or its affiliates <open-source-office@arm.com>
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

#ifndef ETHOSU_MONITOR_H
#define ETHOSU_MONITOR_H

#include "EventRecorder.h"
#include "EventRecorderConf.h"
#include <algorithm>
#include <ethosu_driver.h>
#include <pmu_ethosu.h>
#include <stdint.h>
#include <vector>

class EthosUMonitor {

public:
    enum Backend { PRINTF, EVENT_RECORDER };

    /**
     * @param backend   Select which backend to output performance data to.
     * @param merge     Merge performance samples if QREAD or STATUS has not changed
     */
    EthosUMonitor(Backend backend = PRINTF, bool merge = true);

    template <typename T>
    void configure(ethosu_driver *drv, const T &eventIds) {
        // Reset previous record
        prevRecord.qread  = -1;
        prevRecord.status = -1;
        mergeCount        = 0;

        // Set event ids
        numEvents = std::min(static_cast<size_t>(ETHOSU_PMU_NCOUNTERS), eventIds.size());
        for (size_t i = 0; i < numEvents; i++) {
            ethosuEventIds[i] = eventIds[i];
        }

        // config pmu and set driver
        ETHOSU_PMU_Enable(drv);

        for (size_t i = 0; i < ETHOSU_PMU_NCOUNTERS; i++) {
            ETHOSU_PMU_Set_EVTYPER(drv, i, static_cast<ethosu_pmu_event_type>(ethosuEventIds[i]));
            ETHOSU_PMU_CNTR_Enable(drv, 1u << i);
        }

        ETHOSU_PMU_CNTR_Enable(drv, ETHOSU_PMU_CCNT_Msk);

        ETHOSU_PMU_EVCNTR_ALL_Reset(drv);
        ETHOSU_PMU_CYCCNT_Reset(drv);
    }

    void release(ethosu_driver *drv);

    void monitorSample(ethosu_driver *drv);

    size_t getMergeCount() const;

private:
    struct EthosuEventRecord {
        uint64_t cycleCount;
        uint32_t qread;
        uint32_t status;
        struct {
            uint32_t eventConfig;
            uint32_t eventCount;
        } event[ETHOSU_PMU_NCOUNTERS];
    };

    static constexpr int32_t EthosuEventComponentNo = 0x00;

    ethosu_pmu_event_type ethosuEventIds[ETHOSU_PMU_NCOUNTERS];
    size_t numEvents;
    const Backend backend;
    const bool merge;
    size_t mergeCount;
    EthosuEventRecord prevRecord;
};

#endif

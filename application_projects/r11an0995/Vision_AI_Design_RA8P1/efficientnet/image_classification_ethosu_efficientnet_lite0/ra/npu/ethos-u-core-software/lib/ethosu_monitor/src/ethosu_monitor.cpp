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

#include "ethosu_monitor.hpp"
#include "ethosu_log.h"
#include <inttypes.h>
#include <stdio.h>

EthosUMonitor::EthosUMonitor(Backend __backend, bool _merge) : backend(__backend), merge(_merge) {}

void EthosUMonitor::monitorSample(ethosu_driver *drv) {
    switch (backend) {
    case EVENT_RECORDER: {
        const EthosuEventRecord record = {ETHOSU_PMU_Get_CCNTR(drv),
                                          ETHOSU_PMU_Get_QREAD(drv),
                                          ETHOSU_PMU_Get_STATUS(drv),
                                          {{ethosuEventIds[0], ETHOSU_PMU_Get_EVCNTR(drv, 0)},
                                           {ethosuEventIds[1], ETHOSU_PMU_Get_EVCNTR(drv, 1)},
                                           {ethosuEventIds[2], ETHOSU_PMU_Get_EVCNTR(drv, 2)},
                                           {ethosuEventIds[3], ETHOSU_PMU_Get_EVCNTR(drv, 3)}}};

        // Merge records if qread or status has not changed
        if (merge && prevRecord.qread == record.qread && prevRecord.status == record.status) {
            mergeCount++;
            break;
        }

        prevRecord.qread  = record.qread;
        prevRecord.status = record.status;

        EventRecordData(EventID(EventLevelDetail, EthosuEventComponentNo, 0), &record, sizeof(record));
        break;
    }
    case PRINTF:
    default:
        for (size_t i = 0; i < numEvents; i++) {
            LOG("ethosu_pmu_cntr%zd : %" PRIu32 "\n", i, ETHOSU_PMU_Get_EVCNTR(drv, 0));
        }
    }
}

void EthosUMonitor::release(ethosu_driver *drv) {
    ETHOSU_PMU_Disable(drv);
}

size_t EthosUMonitor::getMergeCount() const {
    return mergeCount;
}

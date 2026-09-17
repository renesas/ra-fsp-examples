/*
 * Copyright (c) 2016-2021 Arm Limited. All rights reserved.
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

#ifndef __EVENT_RECORDER_H
#define __EVENT_RECORDER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


// Defines for parameter level for macro EventID
#define EventLevelError         0x00000U    ///< Run-time error in the component
#define EventLevelAPI           0x10000U    ///< API function call 
#define EventLevelOp            0x20000U    ///< Internal operation 
#define EventLevelDetail        0x30000U    ///< Additional detailed information of operations

// Defines for parameter comp_no for macro EventID
#define EvtStatistics_No        0xEFU       ///< Component number for \ref Event_Execution_Statistic in \ref EventID
#define EvtPrintf_No            0xFEU       ///< Component number for \ref printf_redirect in \ref EventID

/// Composed event \em id from level, component number and message number
#define EventID(level, comp_no, msg_no) ((level & 0x30000U) | ((comp_no & 0xFFU) << 8) | (msg_no & 0xFFU))

// Defines for parameter recording for Event Recorder Setup Functions
#define EventRecordNone         0x00U       ///< Record events is disabled
#define EventRecordError        0x01U       ///< Record events with level \ref EventLevelError
#define EventRecordAPI          0x02U       ///< Record events with level \ref EventLevelAPI
#define EventRecordOp           0x04U       ///< Record events with level \ref EventLevelOp
#define EventRecordDetail       0x08U       ///< Record events with level \ref EventLevelDetail
#define EventRecordAll          0x0FU       ///< Record events with any level


// Callback function for user provided timer -----------------------------------

/// Setup timer hardware.
/// \return       status (1=Success, 0=Failure)
extern uint32_t EventRecorderTimerSetup (void);

/// Get timer frequency.
/// \return       timer frequency in Hz
extern uint32_t EventRecorderTimerGetFreq (void);

/// Get timer count.
/// \return       timer count (32-bit)
extern uint32_t EventRecorderTimerGetCount (void);


// Event Recorder Setup Functions ----------------------------------------------

/// Initialize Event Recorder
/// \param[in]    recording   initial level mask for event record filter
/// \param[in]    start       initial recording setup (1=start, 0=stop)
/// \return       status (1=Success, 0=Failure)
extern uint32_t EventRecorderInitialize (uint32_t recording, uint32_t start);

/// Enable recording of events with specified level and component range
/// \param[in]    recording   level mask for event record filter
/// \param[in]    comp_start  first component number of range
/// \param[in]    comp_end    last Component number of range
/// \return       status (1=Success, 0=Failure)
extern uint32_t EventRecorderEnable (uint32_t recording, uint32_t comp_start, uint32_t comp_end);

/// Disable recording of events with specified level and component range
/// \param[in]    recording   level mask for event record filter
/// \param[in]    comp_start  first component number of range
/// \param[in]    comp_end    last Component number of range
/// \return       status (1=Success, 0=Failure)
extern uint32_t EventRecorderDisable (uint32_t recording, uint32_t comp_start, uint32_t comp_end);

/// Start event recording
/// \return       status (1=Success, 0=Failure)
extern uint32_t EventRecorderStart (void);

/// Stop event recording
/// \return       status (1=Success, 0=Failure)
extern uint32_t EventRecorderStop (void);

/// Update Event Recorder timestamp clock
/// \return       status (1=Success, 0=Failure)
extern uint32_t EventRecorderClockUpdate (void);


// Event Data Recording Functions ----------------------------------------------

/// Record an event with variable data size
/// \param[in]    id     event identifier (level, component number, message number)
/// \param[in]    data   event data buffer
/// \param[in]    len    event data length
/// \return       status (1=Success, 0=Failure)
extern uint32_t EventRecordData (uint32_t id, const void *data, uint32_t len);

/// Record an event with two 32-bit data values
/// \param[in]    id     event identifier (level, component number, message number)
/// \param[in]    val1   first data value
/// \param[in]    val2   second data value
/// \return       status (1=Success, 0=Failure)
extern uint32_t EventRecord2 (uint32_t id, uint32_t val1, uint32_t val2);

/// Record an event with four 32-bit data values
/// \param[in]    id     event identifier (level, component number, message number)
/// \param[in]    val1   first data value
/// \param[in]    val2   second data value
/// \param[in]    val3   third data value
/// \param[in]    val4   fourth data value
/// \return       status (1=Success, 0=Failure)
extern uint32_t EventRecord4 (uint32_t id, uint32_t val1, uint32_t val2, uint32_t val3, uint32_t val4);


// Event Start/Stop macros for execution statistics ----------------------------

/// \param[in]    slot   slot number (up to 16 slots, 0..15) 
#define EventStartA(slot)          EventRecord2 (0xEF00U+EventLevelError+(slot), ((uint32_t) __FILE__), __LINE__)

/// \param[in]    slot   slot number (up to 16 slots, 0..15) 
/// \param[in]    v1     first data value
/// \param[in]    v2     second data value
#define EventStartAv(slot, v1, v2) EventRecord2 (0xEF10U+EventLevelError+((slot) & 0xFU), (v1), (v2))

/// \param[in]    slot   slot number (up to 16 slots, 0..15) 
#define EventStopA(slot)           EventRecord2 (0xEF20U+EventLevelError+((slot) & 0xFU), ((uint32_t) __FILE__), __LINE__)

/// \param[in]    slot   slot number (up to 16 slots, 0..15) 
/// \param[in]    v1     first data value
/// \param[in]    v2     second data value
#define EventStopAv(slot, v1, v2)  EventRecord2 (0xEF30U+EventLevelError+((slot) & 0xFU), (v1), (v2))

/// \param[in]    slot   slot number (up to 16 slots, 0..15) 
#define EventStartB(slot)          EventRecord2 (0xEF40U+EventLevelAPI+((slot) & 0xFU), ((uint32_t) __FILE__), __LINE__)

/// \param[in]    slot   slot number (up to 16 slots, 0..15) 
/// \param[in]    v1     first data value
/// \param[in]    v2     second data value
#define EventStartBv(slot, v1, v2) EventRecord2 (0xEF50U+EventLevelAPI+((slot) & 0xFU), (v1), (v2))

/// \param[in]    slot   slot number (up to 16 slots, 0..15) 
#define EventStopB(slot)           EventRecord2 (0xEF60U+EventLevelAPI+((slot) & 0xFU), ((uint32_t) __FILE__), __LINE__)

/// \param[in]    slot   slot number (up to 16 slots, 0..15) 
/// \param[in]    v1     first data value
/// \param[in]    v2     second data value
#define EventStopBv(slot, v1, v2)  EventRecord2 (0xEF70U+EventLevelAPI+((slot) & 0xFU), (v1), (v2))

/// \param[in]    slot   slot number (up to 16 slots, 0..15) 
#define EventStartC(slot)          EventRecord2 (0xEF80U+EventLevelOp+((slot) & 0xFU), ((uint32_t) __FILE__), __LINE__)

/// \param[in]    slot   slot number (up to 16 slots, 0..15) 
/// \param[in]    v1     first data value
/// \param[in]    v2     second data value
#define EventStartCv(slot, v1, v2) EventRecord2 (0xEF90U+EventLevelOp+((slot) & 0xFU), (v1), (v2))

/// \param[in]    slot   slot number (up to 16 slots, 0..15) 
#define EventStopC(slot)           EventRecord2 (0xEFA0U+EventLevelOp+((slot) & 0xFU), ((uint32_t) __FILE__), __LINE__)

/// \param[in]    slot   slot number (up to 16 slots, 0..15) 
/// \param[in]    v1     first data value
/// \param[in]    v2     second data value
#define EventStopCv(slot, v1, v2)  EventRecord2 (0xEFB0U+EventLevelOp+((slot) & 0xFU), (v1), (v2))

/// \param[in]    slot   slot number (up to 16 slots, 0..15) 
#define EventStartD(slot)          EventRecord2 (0xEFC0U+EventLevelDetail+((slot) & 0xFU), ((uint32_t) __FILE__), __LINE__)

/// \param[in]    slot   slot number (up to 16 slots, 0..15)
/// \param[in]    v1     first data value
/// \param[in]    v2     second data value
#define EventStartDv(slot, v1, v2) EventRecord2 (0xEFD0U+EventLevelDetail+((slot) & 0xFU), (v1), (v2))

/// \param[in]    slot   slot number (up to 16 slots, 0..15) 
#define EventStopD(slot)           EventRecord2 (0xEFE0U+EventLevelDetail+((slot) & 0xFU), ((uint32_t) __FILE__), __LINE__)

/// \param[in]    slot   slot number (up to 16 slots, 0..15) 
/// \param[in]    v1     first data value
/// \param[in]    v2     second data value
#define EventStopDv(slot, v1, v2)  EventRecord2 (0xEFF0U+EventLevelDetail+((slot) & 0xFU), (v1), (v2))

#ifdef __cplusplus
}
#endif

#endif /* __EVENT_RECORDER_H */

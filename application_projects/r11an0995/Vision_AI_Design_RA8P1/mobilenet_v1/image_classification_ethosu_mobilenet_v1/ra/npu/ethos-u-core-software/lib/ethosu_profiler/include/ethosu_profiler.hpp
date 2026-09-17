/*
 * SPDX-FileCopyrightText: Copyright 2023 Arm Limited and/or its affiliates <open-source-office@arm.com>
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

#pragma once

#include <stdint.h>

#if defined __has_include
#if __has_include(<ethosu_profiler_impl.hpp>)
#define HAS_IMPL
#include <ethosu_profiler_impl.hpp>
#endif
#endif

#ifndef HAS_IMPL
struct ethosu_profiler_context {};
#endif

void ethosu_profiler_start(struct ethosu_profiler_context *ctx);

void ethosu_profiler_end(struct ethosu_profiler_context *ctx);

void ethosu_profiler_reset(struct ethosu_profiler_context *ctx);

uint64_t ethosu_profiler_get_pmu_cycles(struct ethosu_profiler_context *ctx);

void ethosu_profiler_add_to_pmu_cycles(struct ethosu_profiler_context *ctx, uint64_t cycles);

void ethosu_profiler_add_to_pmu_event(struct ethosu_profiler_context *ctx, uint32_t index, uint32_t value);

void ethosu_profiler_report(struct ethosu_profiler_context *ctx);

/*
 * SPDX-FileCopyrightText: Copyright 2021 Arm Limited and/or its affiliates
 * <open-source-office@arm.com> SPDX-License-Identifier: Apache-2.0
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
#ifndef ML_EMBEDDED_CORE_LOG_H
#define ML_EMBEDDED_CORE_LOG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>
#include <stdio.h>

#define LOG_LEVEL_TRACE 0
#define LOG_LEVEL_DEBUG 1
#define LOG_LEVEL_INFO  2
#define LOG_LEVEL_WARN  3
#define LOG_LEVEL_ERROR 4
#define LOG_LEVEL 2

#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_LEVEL_INFO
#endif /*LOG_LEVEL*/

#if !defined(UNUSED)
#define UNUSED(x) ((void)(x))
#endif /* #if !defined(UNUSED) */

#if (LOG_LEVEL == LOG_LEVEL_TRACE)
#define trace(...)      \
    printf("\rTRACE - "); \
    printf(__VA_ARGS__)
#else
#define trace(...)
#endif /* LOG_LEVEL == LOG_LEVEL_TRACE */

#if (LOG_LEVEL <= LOG_LEVEL_DEBUG)
#define debug(...)      \
    printf("\rDEBUG - "); \
    printf(__VA_ARGS__)
#else
#define debug(...)
#endif /* LOG_LEVEL > LOG_LEVEL_TRACE */

#if (LOG_LEVEL <= LOG_LEVEL_INFO)
#define info(...)      \
    printf("\rINFO - "); \
    printf(__VA_ARGS__)
#else
#define info(...)
#endif /* LOG_LEVEL > LOG_LEVEL_DEBUG */

#if (LOG_LEVEL <= LOG_LEVEL_WARN)
#define warn(...)      \
    printf("\rWARN - "); \
    printf(__VA_ARGS__)
#else
#define warn(...)
#endif /* LOG_LEVEL > LOG_LEVEL_INFO */

#if (LOG_LEVEL <= LOG_LEVEL_ERROR)
#define printf_err(...) \
    printf("\rERROR - "); \
    printf(__VA_ARGS__)
#else
#define printf_err(...)
#endif /* LOG_LEVEL > LOG_LEVEL_INFO */

#ifdef __cplusplus
}
#endif

#endif /* ML_EMBEDDED_CORE_LOG_H */

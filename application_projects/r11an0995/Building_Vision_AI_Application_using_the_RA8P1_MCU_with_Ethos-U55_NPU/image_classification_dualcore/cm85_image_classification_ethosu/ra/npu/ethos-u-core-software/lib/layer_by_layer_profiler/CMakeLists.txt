#
# Copyright (c) 2021 Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: Apache-2.0
#
# Licensed under the Apache License, Version 2.0 (the License); you may
# not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an AS IS BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

if (NOT TARGET ethosu_core_driver)
    return()
endif()

add_library(layer_by_layer_profiler INTERFACE)

target_link_libraries(layer_by_layer_profiler INTERFACE
    ethosu_core_driver
    tflu event_recorder
    ethosu_log)

target_include_directories(layer_by_layer_profiler INTERFACE
    include)

target_sources(layer_by_layer_profiler INTERFACE
    src/layer_by_layer_profiler.cpp)

target_compile_definitions(layer_by_layer_profiler INTERFACE
    LAYER_BY_LAYER_PROFILER)

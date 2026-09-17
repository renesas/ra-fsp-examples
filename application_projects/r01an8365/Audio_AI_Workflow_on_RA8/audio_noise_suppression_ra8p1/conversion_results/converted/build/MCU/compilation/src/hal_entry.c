/*
 * This file is developed by EdgeCortix Inc. to be used with certain Renesas Electronics Hardware only.
 *
 * Copyright © 2025 EdgeCortix Inc. Licensed to Renesas Electronics Corporation with the
 * right to sublicense under the Apache License, Version 2.0.
 *
 * This file also includes source code originally developed by the Renesas Electronics Corporation.
 * The Renesas disclaimer below applies to any Renesas-originated portions for usage of the code.
 *
 * The Renesas Electronics Corporation
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED 'AS IS' AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Changed from original python code to C source code.
 * Copyright (C) 2017 Renesas Electronics Corporation. All rights reserved.
 *
 * This file also includes source codes originally developed by the TensorFlow Authors which were distributed under the following conditions.
 *
 * The TensorFlow Authors
 * Copyright 2023 The Apache Software Foundation
 *
 * This product includes software developed at
 * The Apache Software Foundation (http://www.apache.org/).
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
 *
 */

#include "hal_data.h"
#include "SEGGER_RTT/SEGGER_RTT.h"
#include "pmu_ethosu.h"
#include "stdio.h"
#include "../../../ra/board/ra8p1_ek/board.h"
#include "model.h"
#include "model_io_data.h"
uint32_t GetCurrentTimeTicks() {
    static bool is_initialized = false;
    if (!is_initialized) {
        ARM_PMU_Enable();
        DCB->DEMCR |= DCB_DEMCR_TRCENA_Msk;
        ARM_PMU_CYCCNT_Reset();
        ARM_PMU_CNTR_Enable(PMU_CNTENSET_CCNTR_ENABLE_Msk);
        is_initialized = true;
    }
    return ARM_PMU_Get_CCNTR();
}

int CompareOutput(const uint8_t* reference, const uint8_t* actual_output, const uint64_t size_in_elements, const float epsilon) {
    int mismatches = 0;
    char actual_out_str[30];
    char ref_out_str[30];
    char diff_str[30];
    for (uint32_t i = 0; i < size_in_elements; i += 1) {
        float diff = fabsf(actual_output[i] - reference[i]);
        if (diff > epsilon) {
            mismatches += 1;
            sprintf(diff_str, "%d", (int)roundf(diff / epsilon));
            SEGGER_RTT_printf(0, "%s != %s (%sx)\r\n", actual_out_str, ref_out_str, diff_str);
        } else {
            //SEGGER_RTT_printf(0, "%s == %s\r\n", actual_out_str, ref_out_str);
        }
    }
    return mismatches;
}

void hal_entry(void)
{
    // NPU Initialization
    SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0, SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL);
    int status = FSP_SUCCESS;
    status = RM_ETHOSU_Open(&g_rm_ethosu0_ctrl, &g_rm_ethosu0_cfg);
    if (status != FSP_SUCCESS) {
      SEGGER_RTT_WriteString(0, "Failed to start NPU\r\n");
      return;
    }
    ETHOSU_PMU_Enable(&g_ethosu0);
    ETHOSU_PMU_CNTR_Enable(&g_ethosu0, ETHOSU_PMU_CCNT_Msk);
    int events[] = {
      ETHOSU_PMU_NPU_IDLE,
      ETHOSU_PMU_NPU_ACTIVE,
      ETHOSU_PMU_AXI0_ENABLED_CYCLES,
      ETHOSU_PMU_AXI1_ENABLED_CYCLES
    };
    for (uint32_t i = 0; i < sizeof(events) / sizeof(events[0]); i += 1) {
      ETHOSU_PMU_Set_EVTYPER(&g_ethosu0, i, events[i]);
      ETHOSU_PMU_CNTR_Enable(&g_ethosu0, 1u << i);
    }
    ETHOSU_PMU_CYCCNT_Reset(&g_ethosu0);
    ETHOSU_PMU_EVCNTR_ALL_Reset(&g_ethosu0);

    // RUN MODEL
    int32_t ntimes = 2;
    uint64_t total_cycles = 0;
    uint64_t total_npu_cycles = 0;

    for (int n = 0; n < ntimes; n += 1) {
        memcpy(GetModelInputPtr_main_input_int8(), model_main_input_int8, model_main_input_int8_SIZE);
        memcpy(GetModelInputPtr_denoise_gru_prev_state_int8(), model_denoise_gru_prev_state_int8, model_denoise_gru_prev_state_int8_SIZE);
        memcpy(GetModelInputPtr_noise_gru_prev_state_int8(), model_noise_gru_prev_state_int8, model_noise_gru_prev_state_int8_SIZE);
        memcpy(GetModelInputPtr_vad_gru_prev_state_int8(), model_vad_gru_prev_state_int8, model_vad_gru_prev_state_int8_SIZE);

        ETHOSU_PMU_CYCCNT_Reset(&g_ethosu0);
        ETHOSU_PMU_EVCNTR_ALL_Reset(&g_ethosu0);

        uint32_t cpuCyclesBegin = GetCurrentTimeTicks();
        RunModel(false);
        uint32_t cpuCycles = GetCurrentTimeTicks() - cpuCyclesBegin;

        uint32_t npu_idle = ETHOSU_PMU_Get_EVCNTR(&g_ethosu0, 0);
        uint32_t npu_active = ETHOSU_PMU_Get_EVCNTR(&g_ethosu0, 1);
        uint32_t npu_axi0_en = ETHOSU_PMU_Get_EVCNTR(&g_ethosu0, 2);
        uint32_t npu_axi1_en = ETHOSU_PMU_Get_EVCNTR(&g_ethosu0, 3);

        total_cycles += cpuCycles;
        total_npu_cycles += npu_active;
        SEGGER_RTT_printf(0, "Inference took %d cycles\r\n", cpuCycles);

        SEGGER_RTT_printf(0, "NPU: npu_idle: %d\r\n", npu_idle * 2);
        SEGGER_RTT_printf(0, "NPU: npu_active: %d\r\n", npu_active * 2);
        SEGGER_RTT_printf(0, "NPU: npu_axi0_en: %d\r\n", npu_axi0_en * 2);
        SEGGER_RTT_printf(0, "NPU: npu_axi1_en: %d\r\n", npu_axi1_en * 2);
        SEGGER_RTT_printf(0, "CPU part: %d\r\n", (cpuCycles - (npu_active * 2)));
    }
    ETHOSU_PMU_Disable(&g_ethosu0);

    SEGGER_RTT_printf(0, "times: %d\r\n", ntimes);
    SEGGER_RTT_printf(0, "Average inference took: %d cycles\r\n", total_cycles);
    SEGGER_RTT_printf(0, "Average NPU inference took: %d cycles\r\n", total_npu_cycles);

    // Comparing outputs
    {
        SEGGER_RTT_printf(0, "Comparing output 0\r\n");
        int misses = CompareOutput(model_Identity_3_int8_70089, GetModelOutputPtr_Identity_3_int8_70089(), model_Identity_3_int8_70089_COUNT, 0.1f);
        if (misses == 0) {
            SEGGER_RTT_WriteString(0, "MATCH\r\n");
        } else {
            SEGGER_RTT_printf(0, "FAILED: %d/%d misses\r\n", misses, model_Identity_3_int8_70089_COUNT);
        }
    }

    {
        SEGGER_RTT_printf(0, "Comparing output 1\r\n");
        int misses = CompareOutput(model_Identity_2_int8_70118, GetModelOutputPtr_Identity_2_int8_70118(), model_Identity_2_int8_70118_COUNT, 0.1f);
        if (misses == 0) {
            SEGGER_RTT_WriteString(0, "MATCH\r\n");
        } else {
            SEGGER_RTT_printf(0, "FAILED: %d/%d misses\r\n", misses, model_Identity_2_int8_70118_COUNT);
        }
    }

    {
        SEGGER_RTT_printf(0, "Comparing output 2\r\n");
        int misses = CompareOutput(model_Identity_int8_70148, GetModelOutputPtr_Identity_int8_70148(), model_Identity_int8_70148_COUNT, 0.1f);
        if (misses == 0) {
            SEGGER_RTT_WriteString(0, "MATCH\r\n");
        } else {
            SEGGER_RTT_printf(0, "FAILED: %d/%d misses\r\n", misses, model_Identity_int8_70148_COUNT);
        }
    }

    {
        SEGGER_RTT_printf(0, "Comparing output 3\r\n");
        int misses = CompareOutput(model_Identity_1_int8_70152, GetModelOutputPtr_Identity_1_int8_70152(), model_Identity_1_int8_70152_COUNT, 0.1f);
        if (misses == 0) {
            SEGGER_RTT_WriteString(0, "MATCH\r\n");
        } else {
            SEGGER_RTT_printf(0, "FAILED: %d/%d misses\r\n", misses, model_Identity_1_int8_70152_COUNT);
        }
    }

    {
        SEGGER_RTT_printf(0, "Comparing output 4\r\n");
        int misses = CompareOutput(model_Identity_4_int8_70156, GetModelOutputPtr_Identity_4_int8_70156(), model_Identity_4_int8_70156_COUNT, 0.1f);
        if (misses == 0) {
            SEGGER_RTT_WriteString(0, "MATCH\r\n");
        } else {
            SEGGER_RTT_printf(0, "FAILED: %d/%d misses\r\n", misses, model_Identity_4_int8_70156_COUNT);
        }
    }

    // Stop NPU
    status = RM_ETHOSU_Close(&g_rm_ethosu0_ctrl);
    if (status != FSP_SUCCESS) {
      SEGGER_RTT_WriteString(0, "Failed to stop NPU\r\n");
      return;
    }

}

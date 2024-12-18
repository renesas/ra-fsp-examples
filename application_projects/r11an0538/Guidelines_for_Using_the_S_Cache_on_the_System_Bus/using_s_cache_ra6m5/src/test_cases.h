/***********************************************************************************************************************
 * File Name    : test_cases.h
 * Description  : Contains data structures and functions used in test_cases.h
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef TEST_CASES_H_
#define TEST_CASES_H_
#include "common_utils.h"

#define FLUSH_IN_DMA_COMPLETE_CALLBACK  0
#define FLUSH_IN_APP                    1

void standared_dev_calc_s_cache_disabled(void);
void standard_dev_calc_s_cache_enabled_no_s_cache_flush(void);
void standard_dev_calc_s_cache_enabled_data_buffer_non_cacheable(void);
void track_time_used_s_cache_disabled(void);
void track_time_used_s_cache_enabled_flushed(void);
void track_time_used_s_cache_enabled_sram_used_by_dma_noncacheable(void);
void standard_dev_calc_s_cache_enabled_flushed(uint32_t method);

#endif /* TEST_CASES_H_ */

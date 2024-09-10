/***********************************************************************************************************************
 * File Name    : s_cache.h
 * Description  : Contains data structures and functions used in s_cache.h
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/ 

#ifndef S_CACHE_H_
#define S_CACHE_H_
#include "common_utils.h"

/* flush s cache */
void flush_s_cache(void);
/* enable s cache */
void enable_s_cache(void);
/*disable s cache */
void disable_s_cache(void);
/* set the s cache line_size */
void select_s_cache_line_size(bool line_size_32);
#endif /* S_CACHE_H_ */

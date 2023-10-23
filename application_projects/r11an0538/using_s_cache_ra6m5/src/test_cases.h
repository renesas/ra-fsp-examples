/***********************************************************************************************************************
 * File Name    : test_cases.h
 * Description  : Contains data structures and functions used in test_cases.h
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright [2020-2021] Renesas Electronics Corporation and/or its affiliates.  All Rights Reserved.
 *
 * This software and documentation are supplied by Renesas Electronics America Inc. and may only be used with products
 * of Renesas Electronics Corp. and its affiliates ("Renesas").  No other uses are authorized.  Renesas products are
 * sold pursuant to Renesas terms and conditions of sale.  Purchasers are solely responsible for the selection and use
 * of Renesas products and Renesas assumes no liability.  No license, express or implied, to any intellectual property
 * right is granted by Renesas. This software is protected under all applicable laws, including copyright laws. Renesas
 * reserves the right to change or discontinue this software and/or this documentation. THE SOFTWARE AND DOCUMENTATION
 * IS DELIVERED TO YOU "AS IS," AND RENESAS MAKES NO REPRESENTATIONS OR WARRANTIES, AND TO THE FULLEST EXTENT
 * PERMISSIBLE UNDER APPLICABLE LAW, DISCLAIMS ALL WARRANTIES, WHETHER EXPLICITLY OR IMPLICITLY, INCLUDING WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NONINFRINGEMENT, WITH RESPECT TO THE SOFTWARE OR
 * DOCUMENTATION.  RENESAS SHALL HAVE NO LIABILITY ARISING OUT OF ANY SECURITY VULNERABILITY OR BREACH.  TO THE MAXIMUM
 * EXTENT PERMITTED BY LAW, IN NO EVENT WILL RENESAS BE LIABLE TO YOU IN CONNECTION WITH THE SOFTWARE OR DOCUMENTATION
 * (OR ANY PERSON OR ENTITY CLAIMING RIGHTS DERIVED FROM YOU) FOR ANY LOSS, DAMAGES, OR CLAIMS WHATSOEVER, INCLUDING,
 * WITHOUT LIMITATION, ANY DIRECT, CONSEQUENTIAL, SPECIAL, INDIRECT, PUNITIVE, OR INCIDENTAL DAMAGES; ANY LOST PROFITS,
 * OTHER ECONOMIC DAMAGE, PROPERTY DAMAGE, OR PERSONAL INJURY; AND EVEN IF RENESAS HAS BEEN ADVISED OF THE POSSIBILITY
 * OF SUCH LOSS, DAMAGES, CLAIMS OR COSTS.
 **********************************************************************************************************************/

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

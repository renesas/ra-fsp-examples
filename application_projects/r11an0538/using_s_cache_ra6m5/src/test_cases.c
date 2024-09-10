/***********************************************************************************************************************
 * File Name    : test_cases.c
 * Description  : Contains data structures and functions used in test_cases.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/ 


#include "test_cases.h"
#include "common_utils.h"
#include "transfer_initialise.h"
#include "s_cache.h"
#include "dwt.h"
#include "timer_initialise.h"
#include "dmac_transfers.h"

#define NUM_CYCLES_TO_CALCULATE     1000
extern int32_t g_dest_sine_cosine_data[DATA_SIZE];

extern bool invalidate_app;

const bsp_delay_units_t bsp_delay_units = BSP_DELAY_UNITS_MILLISECONDS;
int32_t sine_cosine_sq[DATA_SIZE] = {0};
int32_t temp_sram_data[NUM_CYCLES_TO_CALCULATE] = {0};

static int32_t calculate_stdev(void);
static void set_none_cacheable_regions(void);

/*********************************************************************************************************************
 *  @brief       calculate the standard deviation
 *  @param[IN]   None
 *  @retval      standard deviation
***********************************************************************************************************************/
static int32_t calculate_stdev(void)
{
    int32_t result_std = 0;
    for(volatile int index = 0; index<180; index++)
    {
       R_BSP_SoftwareDelay(10, bsp_delay_units);
       sine_cosine_sq[index] =(g_dest_sine_cosine_data[index]>>16)*(g_dest_sine_cosine_data[index]>>16) + (g_dest_sine_cosine_data[index] & 0x0000FFFF)*(g_dest_sine_cosine_data[index]&0x0000FFFF);

    }
    arm_std_q31(sine_cosine_sq, DATA_SIZE, &result_std);

    return result_std;

 }
/*********************************************************************************************************************
 *  @brief       acquire and print the standard deviation with s cache disabled
 *  @param[IN]   None
 *  @retval      None
***********************************************************************************************************************/
void standared_dev_calc_s_cache_disabled(void)
{
    int32_t result_std = 0;

    memset(g_dest_sine_cosine_data, 0, 180*4);
    dma_transfer_sine_cosine_operation();

    result_std = calculate_stdev();

    dmac_transfer_deinit(&g_transfer_sine_wave_ctrl, SINE_WAVE);
     /* De-initialize AGT */
     agt_timer_deinit();

    APP_PRINT("\r\nStandard deviation when s cache is disabled is %d\n\n", result_std);
}

/*********************************************************************************************************************
 *  @brief       acquire and print the standard deviation with s cache enabled without s cache flush
 *  @param[IN]   None
 *  @retval      None
***********************************************************************************************************************/
void standard_dev_calc_s_cache_enabled_no_s_cache_flush(void)
{
    int32_t result_std = 0;
    enable_s_cache();
    memset(g_dest_sine_cosine_data, 0, 180*4);

    dma_transfer_sine_cosine_operation();

    result_std = calculate_stdev();
    dmac_transfer_deinit(&g_transfer_sine_wave_ctrl, SINE_WAVE);
    /* De-initialize AGT */
    agt_timer_deinit();
    APP_PRINT("\r\nStandard deviation when s cache is enabled but not flushed is %d\n\n", result_std);
    disable_s_cache();
}

/*********************************************************************************************************************
 *  @brief       acquire and print the standard deviation with s cache enabled and flushed
 *  @param[IN]   None
 *  @retval      None
***********************************************************************************************************************/

void standard_dev_calc_s_cache_enabled_flushed(uint32_t method)
{
    int32_t result_std = 0;
    enable_s_cache();
    memset(g_dest_sine_cosine_data, 0, 180*4);

    dma_transfer_sine_cosine_operation();

    if(FLUSH_IN_APP == method)
    {
        flush_s_cache();
    }
    result_std = calculate_stdev();
    dmac_transfer_deinit(&g_transfer_sine_wave_ctrl, SINE_WAVE);
    /* De-initialize AGT */
    agt_timer_deinit();
    if(FLUSH_IN_APP == method)
   {
        APP_PRINT("\r\nStandard deviation when s cache is enabled and flushed in app is %d\n\n", result_std);
   }
    else
    {
        APP_PRINT("\r\nStandard deviation when s cache is enabled and flushed in dma transfer complete callback is %d\n\n", result_std);
    }
    disable_s_cache();
}

/*********************************************************************************************************************
 *  @brief       get the cycle count used when s cache is disabled and print it on the RTT viewer
 *  @param[IN]   None
 *  @retval      None
***********************************************************************************************************************/
void track_time_used_s_cache_disabled(void)
{
    uint32_t cycle_count= 0;

    memset(g_dest_sine_cosine_data, 0, 180*4);

    dma_transfer_sine_cosine_operation();
    dmac_transfer_deinit(&g_transfer_sine_wave_ctrl, SINE_WAVE);
    /* De-initialize AGT */
    agt_timer_deinit();
    ResetCycleCounter();
    for(volatile int new_index = 0; new_index < NUM_CYCLES_TO_CALCULATE; new_index++)
    {
         for(volatile int index = 0; index<180; index++)
        {
             sine_cosine_sq[index] =(g_dest_sine_cosine_data[index]>>16)*(g_dest_sine_cosine_data[index]>>16) + (g_dest_sine_cosine_data[index] & 0x0000FFFF)*(g_dest_sine_cosine_data[index]&0x0000FFFF);
        }
    }
    cycle_count = GetCycleCounter();
    APP_PRINT("\r\nDWT cycle used when s cache is disabled is %d\n\n", cycle_count);

}

/*********************************************************************************************************************
 *  @brief       get the cycle count used when s cache is enabled and flushed and print it on the RTT viewer
 *  @param[IN]   None
 *  @retval      None
***********************************************************************************************************************/
void track_time_used_s_cache_enabled_flushed(void)
{
    uint32_t cycle_count= 0;
    enable_s_cache();

    memset(g_dest_sine_cosine_data, 0, 180*4);

    dma_transfer_sine_cosine_operation();
    dmac_transfer_deinit(&g_transfer_sine_wave_ctrl, SINE_WAVE);
    /* De-initialize AGT */
    agt_timer_deinit();
    if(true == invalidate_app)
    {
       flush_s_cache();
    }
    ResetCycleCounter();
    for(volatile int new_index = 0; new_index < NUM_CYCLES_TO_CALCULATE; new_index++)
    {
         for(volatile int index = 0; index<180; index++)
        {
             sine_cosine_sq[index] =(g_dest_sine_cosine_data[index]>>16)*(g_dest_sine_cosine_data[index]>>16) + (g_dest_sine_cosine_data[index] & 0x0000FFFF)*(g_dest_sine_cosine_data[index]&0x0000FFFF);
        }
    }
    cycle_count = GetCycleCounter();
    APP_PRINT("\r\nDWT cycle used is %d\n\n", cycle_count);
    disable_s_cache();

}

/*********************************************************************************************************************
 *  @brief       get the cycle count used when s cache is enabled and sram region used by the MPU is defined as non-cacheable
 *  @param[IN]   None
 *  @retval      None
***********************************************************************************************************************/

void track_time_used_s_cache_enabled_sram_used_by_dma_noncacheable(void)
{
    uint32_t cycle_count= 0;
    /*set the SRAM buffer used as non-cacheable */
    set_none_cacheable_regions();
    enable_s_cache();

    memset(g_dest_sine_cosine_data, 0, 180*4);

    dma_transfer_sine_cosine_operation();
    dmac_transfer_deinit(&g_transfer_sine_wave_ctrl, SINE_WAVE);
    /* De-initialize AGT */
    agt_timer_deinit();
    if(true == invalidate_app)
    {
       flush_s_cache();
    }
    ResetCycleCounter();
    for(volatile int new_index = 0; new_index < NUM_CYCLES_TO_CALCULATE; new_index++)
    {
         for(volatile int index = 0; index<180; index++)
        {
             sine_cosine_sq[index] =(g_dest_sine_cosine_data[index]>>16)*(g_dest_sine_cosine_data[index]>>16) + (g_dest_sine_cosine_data[index] & 0x0000FFFF)*(g_dest_sine_cosine_data[index]&0x0000FFFF);
        }
    }
    cycle_count = GetCycleCounter();
    APP_PRINT("\r\nDWT cycle used is %d\n\n", cycle_count);
    disable_s_cache();
    /* Disable MPU */
    ARM_MPU_Disable();
}

#define MPU_REGION_0        0U
#define REGION_0_ATTR_IDX   0U
#define READ_WRITE          0U
#define READ_ONLY           1U
#define PRIVILEGED_ONLY     0U
#define ANY_PRVILEGE        1U
#define EXECUTION_PERMITTED 0U
#define NO_EXECUTION        1U

static void set_none_cacheable_regions(void)
{
    uint32_t DMA_destination_address = (uint32_t)g_dest_sine_cosine_data;

    /* Disable MPU */
    ARM_MPU_Disable();

    /* MPU Region 0 */
    /* Outer shareable, Read/Write by any privilege level, Execution enable if read permitted, non-cacheable outer, non-cacheable inner */
    ARM_MPU_SetRegion(MPU_REGION_0, ARM_MPU_RBAR(DMA_destination_address, ARM_MPU_SH_OUTER, READ_WRITE, ANY_PRVILEGE, EXECUTION_PERMITTED),
                                    ARM_MPU_RLAR(DMA_destination_address+DATA_SIZE, REGION_0_ATTR_IDX));
    ARM_MPU_SetMemAttr(REGION_0_ATTR_IDX, ARM_MPU_ATTR(ARM_MPU_ATTR_NON_CACHEABLE, ARM_MPU_ATTR_NON_CACHEABLE));

    /* Enable MPU, Enable default memory map as background, MPU enabled during fault and NMI handlers */
    ARM_MPU_Enable(MPU_CTRL_PRIVDEFENA_Msk | MPU_CTRL_HFNMIENA_Msk);

}


/*********************************************************************************************************************
 *  @brief       get the cycle count used when s cache is enabled and flushed and print it on the RTT viewer
 *  @param[IN]   None
 *  @retval      None
***********************************************************************************************************************/
void standard_dev_calc_s_cache_enabled_data_buffer_non_cacheable(void)
{
    int32_t result_std = 0;
    /*set the SRAM buffer used as non-cacheable */
    set_none_cacheable_regions();
    enable_s_cache();
    memset(g_dest_sine_cosine_data, 0, 180*4);

    dma_transfer_sine_cosine_operation();

    result_std = calculate_stdev();
    dmac_transfer_deinit(&g_transfer_sine_wave_ctrl, SINE_WAVE);
    /* De-initialize AGT */
    agt_timer_deinit();

    APP_PRINT("\r\nStandard deviation when s cache is enabled and DMA buffer destination is in the non-cacheable region is %d\n\n", result_std);

    disable_s_cache();
    /* Disable MPU */
    ARM_MPU_Disable();
}

/***********************************************************************************************************************
 * File Name    : test_cases.c
 * Description  : Contains data structures and functions used in test_cases.c
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

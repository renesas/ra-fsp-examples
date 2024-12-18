/***********************************************************************************************************************
 * File Name    : hal_entry.c
 * Description  : Contains data structures and functions used in hal_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "dmac_transfers.h"
#include "timer_initialise.h"
#include "transfer_initialise.h"
#include "s_cache.h"
#include "arm_math.h"
#include "test_cases.h"
#include "dwt.h"

void R_BSP_WarmStart(bsp_warm_start_event_t event);


#define DEVIATION_WITH_S_CACHE_DISABLED                                 (1)
#define DEVIATION_WITH_S_CACHE_ENABLED_WITH_NO_INVALIDATION             (2)
#define DEVIATION_WITH_S_CACHE_ENABLED_INVALIDATED_DMA_COMPLETE_IRQ     (3)
#define DEVIATION_WITH_S_CACHE_ENABLED_INVALIDATED_IN_APP               (4)
#define DEVIATION_WITH_S_CACHE_ENABLED_DATA_BUFFER_NONCACHEABLE         (5)
#define TIME_USED_WITH_S_CACHE_DISABLED                                 (6)
#define TIME_USED_WITH_S_CACHE_ENABLED_INVALIDATED_DMA_COMPLETE         (7)
#define TIME_USED_WITH_S_CACHE_ENABLED_INVALIDATED_APP                  (8)
#define TIME_USED_WITH_S_CACHE_ENABLED_SRAM_NONCACHEABLE_REGION_USED    (9)
#define TIME_USED_WITH_S_CACHE_ENABLED_INVALIDATED_DMA_COMPLETE_LS_64         (10)
#define TIME_USED_WITH_S_CACHE_ENABLED_INVALIDATED_APP_LS_64                  (11)

#define MENU "\n    input 1 to calculate the standard deviation with s cache disabled \n\n\
    input 2 to calculate the standard deviation with s cache enabled with no cache invalidation\n\n\
    input 3 to calculate the standard deviation with s cache enabled and flushed in DMA_Complete interrupt \n\n\
    input 4 to calculate the standard deviation with s_cache enabled and flushed in application \n\n\
    input 5 to calculate the standard deviation with s_cache enabled and DMA buffer in non-cacheable region \n\n\
    input 6 to evaluate the DWT cycles used in 180000 sine^2 + cosine^2 calculations with s cache disabled \n\n\
    input 7 to evaluate the DWT cycles used in 180000 sine^2 + cosine^2 calculations with s cache flushed in DMA_Complete IRQ callback \n\t          with line size 32\n\n\
    input 8 to evaluate the DWT cycles used in 180000 sine^2 + cosine^2 calculations with s cache flushed in app with line size 32\n\n\
    input 9 to evaluate the DWT cycles used in 180000 sine^2 + cosine^2 calculations with sram region used by DMA as non-cacheable \n\t                 with line size 32\n\n\
    input 10 to evaluate the DWT cycles used in 180000 sine^2 + cosine^2 calculations with s cache flushed in DMA_Complete IRQ callback \n\t          with line size 64\n\n\
    input 11 to evaluate the DWT cycles used in 180000 sine^2 + cosine^2 calculations with s cache flushed in app with line size 64\n"

extern bool invalidate;
extern bool invalidate_app;
#define BUFF_SIZE         0x0F



/*******************************************************************************************************************//**
 * The RA Configuration tool generates main() and uses it to generate threads if an RTOS is used.  This function is
 * called by main() when no RTOS is used.
 **********************************************************************************************************************/
void hal_entry(void)
{
    uint8_t readBuff[BUFF_SIZE] =  { RESET_VALUE };
    APP_PRINT(MENU);
    InitCycleCounter();
    EnableCycleCounter();

    while(true)
    {
        if (APP_CHECK_DATA)
        {
            uint32_t rByte = APP_READ(readBuff);
            /* function returns the converted integral number as an int value.*/
            int32_t inputRead = atoi((char *)readBuff);

            if (rByte > RESET_VALUE)
            {
                switch (inputRead)
                {
                    case DEVIATION_WITH_S_CACHE_DISABLED:
                    {
                        invalidate = false;
                        invalidate_app = false;
                        standared_dev_calc_s_cache_disabled();
                    }
                    break;
                    case DEVIATION_WITH_S_CACHE_ENABLED_WITH_NO_INVALIDATION:
                    {
                        invalidate = false;
                        invalidate_app = false;
                        standard_dev_calc_s_cache_enabled_no_s_cache_flush();
                    }
                    break;
                    case DEVIATION_WITH_S_CACHE_ENABLED_INVALIDATED_DMA_COMPLETE_IRQ:
                    {
                        invalidate = true;
                        invalidate_app = false;
                        standard_dev_calc_s_cache_enabled_flushed(FLUSH_IN_DMA_COMPLETE_CALLBACK);
                    }
                    break;
                    case DEVIATION_WITH_S_CACHE_ENABLED_INVALIDATED_IN_APP:
                    {
                        invalidate_app = true;
                        invalidate = false;
                        standard_dev_calc_s_cache_enabled_flushed(FLUSH_IN_APP);
                    }
                    break;
                    /* XW */
                   case DEVIATION_WITH_S_CACHE_ENABLED_DATA_BUFFER_NONCACHEABLE:
                   {
                        invalidate_app = false;
                        invalidate = false;
                        standard_dev_calc_s_cache_enabled_data_buffer_non_cacheable();
                   }
                   break;
                    case TIME_USED_WITH_S_CACHE_DISABLED:
                   {
                       invalidate = false;
                       invalidate_app = false;
                       track_time_used_s_cache_disabled();
                   }
                   break;
                   case TIME_USED_WITH_S_CACHE_ENABLED_INVALIDATED_DMA_COMPLETE:
                   {
                       invalidate = true;
                       invalidate_app = false;
                       select_s_cache_line_size(true);
                       APP_PRINT("\r\nTest setup is: S cache is enabled with line size set to 32 and S cache is flushed in DMA complete interrupt.\n");
                       track_time_used_s_cache_enabled_flushed();
                   }
                   break;
                    case TIME_USED_WITH_S_CACHE_ENABLED_INVALIDATED_APP:
                    {
                        invalidate_app = true;
                        invalidate = false;
                        select_s_cache_line_size(true);
                        APP_PRINT("\r\nTest setup is: S cache is enabled with line size set to 32 and S cache is flushed in application.\n");
                        track_time_used_s_cache_enabled_flushed();
                    }
                    break;
                    case TIME_USED_WITH_S_CACHE_ENABLED_SRAM_NONCACHEABLE_REGION_USED:
                    {
                        invalidate_app = false;
                        invalidate = false;
                        select_s_cache_line_size(true);
                        APP_PRINT("\r\nTest setup is: S cache is enabled with line size set to 32 and SRAM region set as non-cacheable.\n");
                        track_time_used_s_cache_enabled_sram_used_by_dma_noncacheable();
                    }
                    break;
                    case TIME_USED_WITH_S_CACHE_ENABLED_INVALIDATED_DMA_COMPLETE_LS_64:
                     {
                         invalidate = true;
                         invalidate_app = false;
                         select_s_cache_line_size(false);
                         APP_PRINT("\r\nTest setup is: S cache is enabled with line size set to 64 and S cache is flushed in DMA complete interrupt.\n");
                         track_time_used_s_cache_enabled_flushed();
                     }
                     break;
                      case TIME_USED_WITH_S_CACHE_ENABLED_INVALIDATED_APP_LS_64:
                      {
                          invalidate_app = true;
                          invalidate = false;
                          select_s_cache_line_size(false);
                          APP_PRINT("\r\nTest setup is: S cache is enabled with line size set to 64 and S cache is flushed in application.\n");
                          track_time_used_s_cache_enabled_flushed();
                      }
                      break;
                    default:
                    {
                        APP_PRINT("\r\nunsupported command.\n");
                        APP_PRINT (MENU);
                    }
                    break;
                 }
             }
        }
    }
}

/*******************************************************************************************************************//**
 * This function is called at various points during the startup process.  This implementation uses the event that is
 * called right before main() to set up the pins.
 *
 * @param[in]  event    Where at in the start up process the code is currently at
 **********************************************************************************************************************/
void R_BSP_WarmStart(bsp_warm_start_event_t event)
{
    if (BSP_WARM_START_POST_C == event)
    {
        /* C runtime environment and system clocks are setup. */

        /* Configure pins. */
        R_IOPORT_Open (&g_ioport_ctrl, &g_bsp_pin_cfg);
    }
}

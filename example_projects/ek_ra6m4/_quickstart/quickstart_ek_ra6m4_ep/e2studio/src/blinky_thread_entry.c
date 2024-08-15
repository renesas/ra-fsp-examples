/**********************************************************************************************************************
 * File Name    : blinky_thread_entry.c
 * Version      : .
 * Description  : Re-purposed Blinky thread used for monitoring memory use.
 *********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "blinky_thread.h"
#include "common_utils.h"
#include "portable.h"

#define MEMORY_MONITOR_ENABLE (0)

#if MEMORY_MONITOR_ENABLE == 1
static screen_buff[256] = {};
#endif

/**********************************************************************************************************************
 * Function Name: blinky_thread_entry
 * Description  : Blinky Thread entry function.
 * Argument     : pvParameters
 * Return Value : .
 *********************************************************************************************************************/
void blinky_thread_entry (void * pvParameters)
{
    HeapStats_t system_status;

    FSP_PARAMETER_NOT_USED(pvParameters);

    while (1)
    {
        vPortGetHeapStats( &system_status );

#if MEMORY_MONITOR_ENABLE == 1
        if(last_balance != (system_status.xNumberOfSuccessfulAllocations - system_status.xNumberOfSuccessfulFrees))
        {
            sprintf(screen_buff, "\r\n "
                    "AvailableHeapSpaceInBytes %lu, "
                    "xMinimumEverFreeBytesRemaining %lu, "
                    "xNumberOfSuccessfulAllocations %lu "
                    "xNumberOfSuccessfulFrees %lu "
                    "potential leak (diff alloc n free) [%lu]) \r\n",
                    system_status.xAvailableHeapSpaceInBytes,
                    system_status.xMinimumEverFreeBytesRemaining,
                    system_status.xNumberOfSuccessfulAllocations,
                    system_status.xNumberOfSuccessfulFrees,
                    (system_status.xNumberOfSuccessfulAllocations - system_status.xNumberOfSuccessfulFrees)
                    );

            print_to_console(screen_buff);

            last_balance = (system_status.xNumberOfSuccessfulAllocations - system_status.xNumberOfSuccessfulFrees);
        }
#endif

        vTaskDelay(500);
    }
}
/**********************************************************************************************************************
 End of function blinky_thread_entry
 *********************************************************************************************************************/

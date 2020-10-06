/**********************************************************************************************************************
 * File Name    : blinky_thread_entry.c
 * Version      : .
 * Description  : Re-purposed Blinky thread used for monitoring memory use.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * Copyright [2020] Renesas Electronics Corporation and/or its affiliates.  All Rights Reserved.
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
 *********************************************************************************************************************/

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

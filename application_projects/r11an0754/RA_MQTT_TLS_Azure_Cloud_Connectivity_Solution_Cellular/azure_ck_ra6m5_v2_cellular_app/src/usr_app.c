/***********************************************************************************************************************
 * File Name    : usr_app.c
 * Description  : Contains data structures and functions used for the Application init and its utilities
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright [2023] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
 *
 * The contents of this file (the "contents") are proprietary and confidential to Renesas Electronics Corporation
 * and/or its licensors ("Renesas") and subject to statutory and contractual protections.
 *
 * This file is subject to a Renesas FSP license agreement. Unless otherwise agreed in an FSP license agreement with
 * Renesas: 1) you may not use, copy, modify, distribute, display, or perform the contents; 2) you may not use any name
 * or mark of Renesas for advertising or publicity purposes or in connection with your use of the contents; 3) RENESAS
 * MAKES NO WARRANTY OR REPRESENTATIONS ABOUT THE SUITABILITY OF THE CONTENTS FOR ANY PURPOSE; THE CONTENTS ARE PROVIDED
 * "AS IS" WITHOUT ANY EXPRESS OR IMPLIED WARRANTY, INCLUDING THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, AND NON-INFRINGEMENT; AND 4) RENESAS SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, OR
 * CONSEQUENTIAL DAMAGES, INCLUDING DAMAGES RESULTING FROM LOSS OF USE, DATA, OR PROJECTS, WHETHER IN AN ACTION OF
 * CONTRACT OR TORT, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE CONTENTS. Third-party contents
 * included in this file may be subject to different terms.
 **********************************************************************************************************************/
#include "common_utils.h"
#include "usr_app.h"
#include "nx_azure_iot_hub_client.h"
#include "sample_config.h"
#include "nx_api.h"
#include "nx_azure_iot_json_reader.h"
#include "nx_azure_iot_json_writer.h"
#include "nx_azure_iot_provisioning_client.h"
#include "user_choice.h"


#define DOUBLE_DECIMAL_PLACE_DIGITS                                    (2)
#define THREE_DECIMAL_PLACE_DIGITS                                     (3)

#define SAMPLE_PNP_ENVIRONMENTAL_COMPONENT_NAME                       "Environmental"

/*******************************************************************************************************************//**
 * @brief       Send data to queue
 * @param[in]   queue_ptr       Pointer to the address of the queue
 * @param[in]   source_ptr      Pointer to the address of the data
 * @retval      NX_SUCCESS      Initialize successfully
 * @retval      Other           Initialize unsuccessfully
 ***********************************************************************************************************************/
UINT send_data_to_queue(TX_QUEUE *queue_ptr, VOID *source_ptr)
{
    UINT status = TX_SUCCESS;
    status = tx_queue_front_send(queue_ptr, source_ptr, TX_NO_WAIT);
    if(TX_QUEUE_FULL == status)
    {
        status = tx_queue_flush(queue_ptr);
        if (TX_SUCCESS == status)
        {
            status = tx_queue_front_send(queue_ptr, source_ptr, TX_NO_WAIT);
        }
        else
        {
            /* Do nothing */
        }
    }
    else if (TX_SUCCESS == status)
    {
        IotLogDebug("Send data to the queue successfully \r\n");
    }
    else
    {
        IotLogDebug("Send data to the queue unsuccessfully. Error code: %d \r\n", status);
    }
    return status;
}


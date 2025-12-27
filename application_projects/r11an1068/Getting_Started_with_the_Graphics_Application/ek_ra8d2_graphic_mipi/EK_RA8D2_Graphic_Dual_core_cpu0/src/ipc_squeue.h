/***********************************************************************************************************************
 * File Name    : ipc_squeue.h
 * Description  : This file contains the implementation of Queue-based FreeRTOS and IPC mechanisms for initializing,
 *                transmitting, and receiving data between dual-core processors. It integrates IPC hardware features
 *                with FreeRTOS queues to enable efficient inter-core communication.
 * This header file is intended for use with the corresponding implementation in ipc_squeue.c.
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef IPC_SQUEUE_H_
#define IPC_SQUEUE_H_

#include "ipc_thread.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* common err of queue operation */
typedef enum e_queue_status
{
    QUEUE_SUCCESS,
    QUEUE_FULL,
    QUEUE_ERROR,
    QUEUE_INVALID_PARAMETER,
    EQUEUE_OK,
    DQUEUE_OK,
    QUEUE_EMPTY
}queue_status_t;

typedef struct st_pend_data
{
    uint32_t data;
    uint8_t  is_data_pending;
}BackupData_t;

/* Configure for FreeRtos queue receive */
#define ITEM_SIZE           (4UL)
#define ITEM_NUM            (128UL + 1UL)
#define MAX_MESSAGE_LENGTH  (ITEM_NUM * ITEM_SIZE)  // The total size of the receive queue will be: ITEM_SIZE * ITEM_NUM bytes.
#define TIMEOUT_SHORT       pdMS_TO_TICKS(50)
#define TIMEOUT_LONG        pdMS_TO_TICKS(1000)
#define TIMEOUT_NONE        (0x00)
#define TIMEOUT_FOREVER     (portMAX_DELAY)

/* Define common */
#define SET_FLAG            (0x01)
#define RESET_FLAG          (0x00)
#define SET_VALUE           (0x01)
#define RESET_VALUE         (0x00)
#define FULL_FLAG           (1 << 0)
#define NULL                ((void*)0)
#define APP_ERR_ASSERTION   (-1)

#ifndef APP_ASSERT
 #define APP_ASSERT(a)                                          \
	    {                                                       \
	        if ((a))                                            \
	        {                                                   \
	            (void) 0;                  /* Do nothing */     \
	        }                                                   \
	        else                                                \
	        {                                                   \
	            return APP_ERR_ASSERTION;                       \
	        }                                                   \
	    }
#endif

/* Function declare*/
fsp_err_t queue_ipc_init(const ipc_instance_t *ipc0, const ipc_instance_t *ipc1);
int32_t send_to_queue(void * pBuffer, uint32_t length, TickType_t  timeout);
int32_t receive_from_queue(void * pBuffer, uint32_t length, TickType_t  timeout);
#endif /* IPC_SQUEUE_H_ */

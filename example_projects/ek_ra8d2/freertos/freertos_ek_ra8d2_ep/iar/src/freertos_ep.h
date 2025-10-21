/***********************************************************************************************************************
 * File Name    : freertos_ep.h
 * Description  : Contains macros, data structures and functions used common to the EP
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef FREERTOS_EP_H_
#define FREERTOS_EP_H_

#define EP_INFO     "\r\nThe example project demonstrates the use of Message Queue and Semaphore between tasks and"\
                    "\r\ninterrupts. Message Queue is demonstrated both between tasks and between a task and an"\
                    "\r\ninterrupt. A GPT timer periodically generates an interrupt every 1000 milliseconds."\
                    "\r\nFor the first few seconds, messages are shared between Sender Task, Receiver Task, and the"\
                    "\r\nGPT msgq (Message Queue) ISR. The Receiver Task pends on Message Queue, receives messages,"\
                    "\r\nand displays them on RTT Viewer every 500 milliseconds. For the next few seconds, the"\
                    "\r\nSemaphore Task waits for a semaphore to be released by the GPT sem (Semaphore) ISR. GPT sem"\
                    "\r\nISR releases semaphore periodically every 1000 milliseconds. All tasks run at the same"\
                    "\r\npriority level.\r\n"

#define WAIT_TIME                   (500u)          /* Wait time for task on Semaphore and Message Queue */
#define TASK_SUSPEND_COUNT          (10u)           /* No of times the ISR executes total for a */
                                                    /* (TASK_SUSPEND_COUNT * timer period) second run time */

/* Message structure */
typedef struct
{
    uint8_t data;
    char *task_name;
}msg_t;

/* Task handles */
extern TaskHandle_t receiver_task;
extern TaskHandle_t semaphore_task;

#endif /* FREERTOS_EP_H_ */

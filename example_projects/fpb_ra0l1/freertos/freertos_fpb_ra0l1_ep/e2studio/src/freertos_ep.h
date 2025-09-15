/***********************************************************************************************************************
 * File Name    : freertos_ep.h
 * Description  : Contains macros, data structures and functions commonly used in the EP.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef FREERTOS_EP_H_
#define FREERTOS_EP_H_

#define EP_INFO     "\r\nMessages are shared between Sender Task, ISR and Receiver Task for the first few seconds."\
                    "\r\nLater, Sender and Receiver Tasks will be suspended, and timer will be stopped. Semaphore is"\
                    "\r\nacquired and released between Semaphore Task and ISR for the next few seconds, and Semaphore"\
                    "\r\nTask is suspended. To restart the application, power cycle the board.\r\n"

#define WAIT_TIME                   (500u)          /* Wait time for task on Semaphore and Message Queue */
#define TASK_SUSPEND_COUNT          (10u)           /* Number of times the ISR executes in total, resulting in a total
                                                     * run time of (TASK_SUSPEND_COUNT * timer period) seconds */

/* Message structure */
typedef struct {
    uint8_t data;
    char *task_name;
}msg_t;

/* Task handles */
extern TaskHandle_t receiver_task;
extern TaskHandle_t semaphore_task;

#endif /* FREERTOS_EP_H_ */

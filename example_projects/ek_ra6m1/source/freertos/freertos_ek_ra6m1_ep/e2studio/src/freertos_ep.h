/***********************************************************************************************************************
 * File Name    : freertos_ep.h
 * Description  : Contains macros, data structures and functions used  common to the EP
 ***********************************************************************************************************************/
/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#ifndef FREERTOS_EP_H_
#define FREERTOS_EP_H_

#define EP_INFO			"\r\nMessages are shared between Sender_Task, ISR and Receiver_Task for the first 60 seconds.\n"\
						"Later, Sender and Receiver Tasks will be suspended timer will be stopped. Semaphore is acquired\n"\
						"and released between Semaphore Task and ISR for the next 60 seconds and Semaphore Task is suspended.\n"\
						"To restart the application, power cycle the board.\n"

#define WAIT_TIME 					(500u)			// Wait time for task on Semaphore and Message Queue
#define TASK_SUSPEND_COUNT			(60u)			// No of times the ISR executes total for a 60sec run time

/* Message Structure */
typedef struct {
    uint8_t data;
    char *task_name;
}msg_t;

/* Task handles */
extern TaskHandle_t receiver_task;
extern TaskHandle_t semaphore_task;

#endif /* FREERTOS_EP_H_ */

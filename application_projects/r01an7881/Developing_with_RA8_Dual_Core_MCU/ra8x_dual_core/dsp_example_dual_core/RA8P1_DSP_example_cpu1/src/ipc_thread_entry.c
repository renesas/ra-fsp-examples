/***********************************************************************************************************************
 * File Name    : ipc_thread_entry.c
 * Description  : This file contains the necessary initialization routines and user-defined handling of the IPC module,
 *                including support for interrupts, FIFO, and other IPC-related mechanisms.
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#include "ipc_thread.h"
#include "ipc_squeue.h"

/*******************************************************************************************************************//**
 * @brief     IPC channel 1 callback function.
 *
 * This function is called automatically by the IPC driver when an event occurs on IPC channel 1.
 * It is typically used to handle inter-core communication events such as message arrival,
 * notification signaling between CPUs.
 *
 * - CPU1 can use 7 (MIs) IPC interrupts (channels 1 to 7).
 * - CPU0 can use 6 (MIs) IPC interrupts (channels 2 to 7).
 *
 * - The hardware IPC FIFO is managed internally for queue interface purposes,
 *   and should not be accessed directly by the application code.
 *
 * - To transfer data between cores:
 *     + The sending core should call send_to_queue().
 *     + The receiving core should call receive_from_queue().
 *
 * This approach ensures safe and structured message passing between CPU0 and CPU1.
 * @param[in]  p_args   Pointer to the structure containing IPC callback event information,
 *                      including the event type and channel information.
 *
 * @retval     None
 **********************************************************************************************************************/
void g_ipc1_callback(ipc_callback_args_t *p_args)
{
    switch(p_args->event)
    {
        case IPC_EVENT_IRQ1:
        case IPC_EVENT_IRQ2:
        case IPC_EVENT_IRQ3:
        case IPC_EVENT_IRQ4:
        case IPC_EVENT_IRQ5:
        case IPC_EVENT_IRQ6:
        case IPC_EVENT_IRQ7:
            break;
        default:
            break;
    }
}

/* Ipc Thread entry function */
/* pvParameters contains TaskHandle_t */
void ipc_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);

    /* If want to use ipc_queue to communication between 2 cores
     * queue_ipc_init() must be call first on both core */
    queue_ipc_init(&g_ipc0,&g_ipc1);

    while (1)
    {
        vTaskDelay (1);
    }
}

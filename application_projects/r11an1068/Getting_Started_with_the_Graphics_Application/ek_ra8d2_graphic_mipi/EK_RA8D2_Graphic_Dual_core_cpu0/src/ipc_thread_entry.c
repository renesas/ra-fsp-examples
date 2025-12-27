/***********************************************************************************************************************
 * File Name    : ipc_thread_entry.c
 * Description  : This file contains the necessary initialization routines and user-defined handling of the IPC module,
 *                including support for interrupts, FIFO, and other IPC-related mechanisms.
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#include "ipc_squeue.h"
#include "Application.h"

/*******************************************************************************************************************//**
 * @brief     IPC channel 0 callback function.
 *
 * This function is called automatically by the IPC driver when an event occurs on IPC channel 0.
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
void g_ipc0_callback(ipc_callback_args_t *p_args)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    switch(p_args->event)
    {
        case IPC_EVENT_IRQ2:
            xSemaphoreGiveFromISR(g_touch_reset_semaphore, &xHigherPriorityTaskWoken);
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
            break;
        case IPC_EVENT_IRQ3:
            /* Trigger GUI update date & time*/
            APPW_SetVarData(ID_VAR_TIME_UPDATE, 1);
            break;
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

    static GUI_PID_STATE State;
    State.Pressed = 0;
    State.x = 0;
    State.y = 0;

    /* If want to use ipc_queue to communication between 2 cores
     * queue_ipc_init() must be call first on both core */
    queue_ipc_init(&g_ipc0,&g_ipc1);

    while (1)
    {
        /* Get touch point from queue and send to emWin Thread*/
        receive_from_queue(&State, sizeof(State), TIMEOUT_FOREVER);

        /* Send the touch state to emWin thread */
        GUI_PID_StoreState(&State);
        vTaskDelay (1);
    }
}

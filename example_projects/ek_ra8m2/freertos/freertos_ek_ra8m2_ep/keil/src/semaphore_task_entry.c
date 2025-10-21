/***********************************************************************************************************************
 * File Name    : semaphore_task_entry.c
 * Description  : Contains function definitions.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "semaphore_task.h"
#include "common_utils.h"
#include "freertos_ep.h"
#include "gpt_timer_initialize.h"

/*******************************************************************************************************************//**
 * @addtogroup freertos_ep
 * @{
 **********************************************************************************************************************/

/* Flag to suspend Semaphore task */
static bool b_suspend_semphr_task = false;

/* Semaphore Task entry function */
/* pvParameters contains TaskHandle_t */
void semaphore_task_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED(pvParameters);

    /* The rate at which the task waits on the Semaphore availability */
    TickType_t Semphr_wait_ticks = pdMS_TO_TICKS(WAIT_TIME);

    APP_PRINT("\r\nSemaphore Task: Starting g_periodic_timer_sem timer");
    /* Start GPT timer to 'Give' Semaphore periodically at 1 second for semaphore_task */
    fsp_err_t fsp_err = gpt_timer_init(&g_periodic_timer_sem_ctrl, &g_periodic_timer_sem_cfg);
    /* Handle error */
    if(FSP_SUCCESS != fsp_err)
    {
        APP_ERR_TRAP(fsp_err);
    }

    while (true)
    {
        if (true != b_suspend_semphr_task)
        {
            APP_PRINT("\r\nSemaphore Task: Waiting for GPT ISR to release semaphore\r\n");

            if (pdPASS == xSemaphoreTake(g_binary_semaphore, Semphr_wait_ticks))
            {
                /* Print if the semaphore is acquired successfully */
                APP_PRINT("\r\nSemaphore Task: Acquired semaphore successfully\r\n");
            }
        }
        else
        {
            /* Stop the GPT timer */
            fsp_err = R_GPT_Stop(&g_periodic_timer_sem_ctrl);
            /* Handle error */
            if (FSP_SUCCESS != fsp_err)
            {
                if (FSP_SUCCESS != R_GPT_Close(&g_periodic_timer_sem_ctrl))
                {
                    /* Print out in case of error */
                    APP_ERR_PRINT("\r\nSemaphore Task: R_GPT_Close API failed\r\n");
                }
                /* Print out in case of error */
                APP_ERR_PRINT("\r\nSemaphore Task: R_GPT_Stop API failed\r\n");
                APP_ERR_TRAP(fsp_err);
            }
            APP_PRINT("\r\nSemaphore Task: GPT timer stopped\r\n");

            fsp_err = R_GPT_Close(&g_periodic_timer_sem_ctrl);
            if (FSP_SUCCESS != fsp_err)
            {
                /* Print out in case of error */
                APP_ERR_PRINT("\r\nSemaphore Task: R_GPT_Close API failed\r\n");
                APP_ERR_TRAP(fsp_err);
            }

            APP_PRINT("\r\nSemaphore Task: Suspending Semaphore Task\r\n");
            APP_PRINT("\r\nSemaphore Task: Application ends here. Restart to run the application again\r\n");
            /* Suspends calling task */
            vTaskSuspend(RESET_VALUE);
        }
    }
}

/*******************************************************************************************************************//**
 * @brief      User-defined GPT IRQ callback for semaphore handling.
 * @param[in]  p_args   Pointer to timer callback argument data.
 * @retval     None
 **********************************************************************************************************************/
void periodic_timer_semphr_cb(timer_callback_args_t *p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);

    /* Counter to track task suspend count */
    static uint8_t semphr_counter = RESET_VALUE;

    /* Variable is set to true if the priority of the unblocked task is higher
     * than that of the task that was running when the interrupt occurred */
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    /* If the ISR has executed for 10 seconds, set the flag to suspend the semaphore task */
    if(TASK_SUSPEND_COUNT == semphr_counter)
    {
        /* Set flag to suspend Semaphore tasks */
        b_suspend_semphr_task = true;
    }
    else
    {
        /* Release semaphore */
        xSemaphoreGiveFromISR(g_binary_semaphore, &xHigherPriorityTaskWoken);

        /* Increment counter to suspend task */
        semphr_counter++;
    }

    /* A context switch will be performed if xHigherPriorityTaskWoken is set to pdTRUE */
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/*******************************************************************************************************************//**
 * @} (end addtogroup freertos_ep)
 **********************************************************************************************************************/

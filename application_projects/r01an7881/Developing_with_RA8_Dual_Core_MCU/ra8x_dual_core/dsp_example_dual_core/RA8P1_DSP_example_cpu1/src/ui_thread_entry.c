/***********************************************************************************************************************
 * File Name    : serial_thread_entry.c
 * Description  : This file handles UART peripheral initialization and provides basic terminal output functionality.
 *                It sends formatted data to the terminal when available, typically for logging of arm fft example from
 *                CPU0.
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#include <ui_thread.h>
#include "ipc_squeue.h"
#include "uart_control.h"

/* External interrupt channel 13 */
#define   IRQ_BTS1          (13)

/* Function declaration */
void icu_callback(external_irq_callback_args_t *p_args);

/*****************************************************************************************************************
 * @brief      icu_callback function.
 * This function is invoked by the ICU driver in response to external interrupt events.
 * @param[in]  p_args   Pointer to the structure containing callback event information.
 * @retval     None
 **********************************************************************************************************************/
void icu_callback(external_irq_callback_args_t *p_args)
{
    if(IRQ_BTS1 == p_args->channel)
    {
        /* Trigger CPU0 to start fft function */
        R_IPC_EventGenerate(&g_ipc0_ctrl, IPC_GENERATE_EVENT_IRQ2);
    }
}
/* User Interface Thread entry function */
/* pvParameters contains TaskHandle_t */
void ui_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);

    fsp_err_t err = FSP_SUCCESS;
    unsigned char transfer_data[MAX_MESSAGE_LENGTH] = {0};

    /* Initialize UART*/
    uart_init(&g_uart8);

    /* Open ICU module */
    err = R_ICU_ExternalIrqOpen(&g_external_irq13_ctrl, &g_external_irq13_cfg);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        /* ICU Open failure message */
        APP_ERR_PRINT ("\r\n**R_ICU_ExternalIrqOpen API FAILED**\r\n");
    }

    /* Enable ICU module */
     err = R_ICU_ExternalIrqEnable(&g_external_irq13_ctrl);
     /* Handle error */
     if (FSP_SUCCESS != err)
     {
         /* ICU Enable failure message */
         APP_ERR_PRINT ("\r\n**R_ICU_ExternalIrqEnable API FAILED**\r\n");
     }

    APP_PRINT("\r\nPress s1 button to start application ...\r\n");
    while (1)
    {
        int32_t received_byted = receive_from_queue(transfer_data, MAX_MESSAGE_LENGTH, 0);
        if (received_byted > 0)
        {
            uart_write_message(transfer_data, (uint32_t)received_byted, NULL);
        }
        vTaskDelay(1);
    }
}

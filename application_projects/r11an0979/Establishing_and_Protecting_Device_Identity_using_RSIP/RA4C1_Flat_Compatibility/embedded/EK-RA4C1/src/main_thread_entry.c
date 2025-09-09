/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#include "main_thread.h"
#include "framedProtocolTarget.h"
#include "stdio.h"

extern uint16_t len;
extern uint8_t fpTxBuffer[];

#define APP_ERR_TRAP(a)             if(a) {__asm("BKPT #0\n");} /* trap the error location */

static bool data_ready = false;
static uint32_t data_size = 0;
uint16_t fpNumRxBytes = 0;
static uint8_t                      g_uart_buf[READ_BUFFER_SIZE];
static uint16_t uart_rx_index = 0;
static uint16_t expected_len = 0;
volatile bool g_uart_tx_finished = false;

/* Main Thread entry function */
/* pvParameters contains TaskHandle_t */
void main_thread_entry(void *pvParameters)
{
    fsp_err_t status = FSP_SUCCESS;

    status = R_SCI_UART_Open (&g_uart_ctrl, &g_uart_cfg);
    if (FSP_SUCCESS != status)
    {
        /* Fatal error */
        APP_ERR_TRAP(status);
    }

    while(true)
    {
        /* Delay task for 5 ms */
        vTaskDelay(5);

        if(data_ready == true)
        {
            for(uint32_t index = 0; index < data_size; index++)
            {
               fpReceiveByte(g_uart_buf[index]);
            }
            data_ready = false;

            /* send the response to PC */
            status = R_SCI_UART_Write(&g_uart_ctrl, fpTxBuffer, len);

            if (FSP_SUCCESS != status)
            {
              /* Fatal error */
                APP_ERR_TRAP(status);
            }
        }
    }

    FSP_PARAMETER_NOT_USED(pvParameters);
}

/*****************************************************************************************************************
 *  @brief      UART user callback
 *  @param[in]  p_args
 *  @retval     None
 ****************************************************************************************************************/
void user_uart_callback(uart_callback_args_t *p_args)
{
    if (UART_EVENT_RX_CHAR == p_args->event)
    {
        if (uart_rx_index < READ_BUFFER_SIZE)
        {
            g_uart_buf[uart_rx_index++] = (uint8_t)p_args->data;

            if (uart_rx_index == 3)
            {
                uint16_t payload_len = (g_uart_buf[1] << 8) | g_uart_buf[2];
                expected_len = payload_len + 5; // 1 (start) + 2 (len) + 1 (opcode) + payload + 1 (checksum) + 1 (end)
                data_size = expected_len;
            }

            if ((expected_len > 0) && (uart_rx_index == expected_len))
            {
                data_ready = true;
                uart_rx_index = 0;
                expected_len = 0;
            }
        }
        else
        {
            // Overflow, reset state
            uart_rx_index = 0;
            expected_len = 0;
        }
    }
    else if (UART_EVENT_TX_COMPLETE == p_args->event)
    {
        g_uart_tx_finished = true;
    }
}

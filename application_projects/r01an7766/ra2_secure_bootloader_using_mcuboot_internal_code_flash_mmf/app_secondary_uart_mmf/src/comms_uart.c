/***********************************************************************************************************************
 * File Name    : comms_uart.c
 * Description  : Contains UART communication functions
 ***********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
************************************************************************************************************************/

#include "comms.h"
#include "r_uart_api.h"
#include "downloader_thread.h"

static volatile uart_event_t g_uart0_event = 0;
static volatile bool g_timer4_event = false;
static volatile uint16_t g_timer4_count = 0;

/* GPT timer callback */
void xm_1sec_timeout_cb(timer_callback_args_t *p_args)
{
    if (TIMER_EVENT_CYCLE_END == p_args->event)
    {
    	/* increment tick count every second */
    	g_timer4_count++;
    }
}

void user_uart_callback(uart_callback_args_t *p_args)
{
    g_uart0_event = p_args->event;
}

/* open the COM port */
fsp_err_t comms_open(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_SCI_UART_Open(&g_uart0_ctrl, &g_uart0_cfg);
    if (FSP_SUCCESS == err)
    {
    	/* Initialize timer module to monitor comms timeout */
        err = R_GPT_Open( &g_xm_1sec_timer_ctrl,  &g_xm_1sec_timer_cfg);
        if (FSP_SUCCESS != err)
        {
            return err;
        }
    }

    return err;
}

void comms_send(uint8_t *p_src, uint32_t len)
{
    fsp_err_t err = FSP_SUCCESS;

    g_uart0_event = 0;
    err = R_SCI_UART_Write(&g_uart0_ctrl, p_src, len);
    if (FSP_SUCCESS == err)
    {
        while ((UART_EVENT_TX_COMPLETE != g_uart0_event))
        {
            ;
        }
    }
}
/*******************************************************************************************************************//**
 * @brief This function read the UART data and uses the GPT timer overflow to control the time out
 * @param[IN]   p_dest                  destination data buffer
 * @param[IN]   len                     length of the data
 * @param[IN]   timeout_seconds         timeout in seconds
 * @retval      FSP_SUCCESS             successful UART operation.
 * @retval      Any Other Error code    unsuccessful UART operation.
 **********************************************************************************************************************/
fsp_err_t comms_read(uint8_t * p_dest, uint32_t *len, uint32_t timeout_seconds)
{
    fsp_err_t err = FSP_SUCCESS;

    if (NO_TIMEOUT != timeout_seconds)
    {
        err = R_GPT_Stop( &g_xm_1sec_timer_ctrl);
        if (FSP_SUCCESS != err)
        {
            return err;
        }

        g_timer4_event = false;
        g_timer4_count = 0;
        err = R_GPT_Start( &g_xm_1sec_timer_ctrl);
        if (FSP_SUCCESS != err)
        {
            return err;
        }
    }

    g_uart0_event = 0;
    err = R_SCI_UART_Read(&g_uart0_ctrl, p_dest, *len);
    if (FSP_SUCCESS == err)
    {
        while ((UART_EVENT_RX_COMPLETE != g_uart0_event) && (true != g_timer4_event))
        {
            if (g_timer4_count >= timeout_seconds)
            {
            	/* timer tick count is 1 second : see setup in FSP Configuration for g_xm_1sec_timer General PWM (r_gpt) */
            	g_timer4_event = true;
            }
            if ((portMAX_DELAY == timeout_seconds) || (12 == timeout_seconds))
            {
            	/* Yield to other threads while waiting a long time */
            	vTaskDelay(1);
            }
        }
    }

    err = R_GPT_Stop( &g_xm_1sec_timer_ctrl);
    err |= R_GPT_Reset( &g_xm_1sec_timer_ctrl);

    if (FSP_SUCCESS != err)
    {
        return err;
    }

    if (false == g_timer4_event)
    {
        err = FSP_SUCCESS;
    }
    else
    {
        err = FSP_ERR_TIMEOUT;
    }

    return err;
}

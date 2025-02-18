/***********************************************************************************************************************
 * File Name    : comms_uart.c
 * Description  : Contains UART communication functions
 ***********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
************************************************************************************************************************/

#include "comms.h"
#include "r_uart_api.h"
#include "downloader_thread.h"

static volatile uart_event_t g_uart0_event = 0;
static volatile bool g_timer0_event = false;

/* GPT timer callback */
void xm_10sec_timeout_cb(timer_callback_args_t *p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);

    g_timer0_event = true;
}

void user_uart_callback(uart_callback_args_t *p_args)
{
    g_uart0_event = p_args->event;
}

/* open the COM port */
fsp_err_t comms_open(void)
{
    fsp_err_t err;
#if BSP_FEATURE_SCI_VERSION == 1U
    err = R_SCI_UART_Open(&g_uart0_ctrl, &g_uart0_cfg);
#else
    err = R_SCI_B_UART_Open(&g_uart0_ctrl, &g_uart0_cfg);
#endif
    if (FSP_SUCCESS == err)
    {
        err = R_GPT_Open( &g_xm_10sec_timer_ctrl,  &g_xm_10sec_timer_cfg);
    }

    return err;
}

void comms_send(uint8_t * p_src, uint32_t len)
{
    fsp_err_t err;

    g_uart0_event = 0;
#if BSP_FEATURE_SCI_VERSION == 1U
    err = R_SCI_UART_Write(&g_uart0_ctrl, p_src, len);
#else
    err = R_SCI_B_UART_Write(&g_uart0_ctrl, p_src, len);
#endif
    if (FSP_SUCCESS == err)
    {
        while ((UART_EVENT_TX_COMPLETE != g_uart0_event))
        {
            ;
        }
    }
}
/*******************************************************************************************************************//**
 * @brief This function read the UART data and uses the GPT time overflow to control the time out
 * @param[IN]   p_dest                  destination data buffer
 * @param[IN]   len                     length of the data
 * @param[IN]   timeout_milliseconds    timeout in milliseconds
 * @retval      FSP_SUCCESS             Upon successful FLash_HP data flash operations.
 * @retval      Any Other Error code    Upon unsuccessful Flash_HP data flash operations.
 **********************************************************************************************************************/
fsp_err_t comms_read(uint8_t *p_dest, uint32_t *len, uint32_t timeout_milliseconds)
{
    fsp_err_t err;

    if (NO_TIMEOUT != timeout_milliseconds)
    {
        err = R_GPT_Stop(&g_xm_10sec_timer_ctrl);
        if (FSP_SUCCESS != err)
        {
            return err;
        }

        err = R_GPT_PeriodSet(&g_xm_10sec_timer_ctrl, (RAW_COUNT_MS * timeout_milliseconds));
        if (FSP_SUCCESS != err)
        {
            return err;
        }

        g_timer0_event = false;
        err = R_GPT_Start(&g_xm_10sec_timer_ctrl);
        if (FSP_SUCCESS != err)
        {
            return err;
        }
    }
    else
    {
        g_timer0_event = false;
    }

    g_uart0_event = 0;
#if BSP_FEATURE_SCI_VERSION == 1U
    err = R_SCI_UART_Read(&g_uart0_ctrl, p_dest, *len);
#else
    err = R_SCI_B_UART_Read(&g_uart0_ctrl, p_dest, *len);
#endif
    if (FSP_SUCCESS == err)
    {
        while ((UART_EVENT_RX_COMPLETE != g_uart0_event) && (true != g_timer0_event))
        {
            ;
        }
    }

    err = R_GPT_Stop(&g_xm_10sec_timer_ctrl);
    err = R_GPT_Reset(&g_xm_10sec_timer_ctrl);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    if (false == g_timer0_event)
    {
        return FSP_SUCCESS;
    }
    else
    {
        return FSP_ERR_TIMEOUT;
    }
}

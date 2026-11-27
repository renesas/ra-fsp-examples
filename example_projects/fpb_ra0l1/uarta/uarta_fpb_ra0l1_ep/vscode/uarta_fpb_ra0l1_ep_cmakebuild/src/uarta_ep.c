/***********************************************************************************************************************
 * File Name    : uarta_ep.c
 * Description  : Contains data structures and functions used in uarta_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2024 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "uarta_ep.h"

/* Global variables */
uint8_t g_rx_buffer[BUFFER_LEN] = {RESET_VALUE};
uint8_t g_tx_buffer[BUFFER_LEN] = {RESET_VALUE};
volatile uint16_t g_rx_len = RESET_VALUE;
volatile bool g_rx_flag = false;
volatile fsp_err_t g_uarta_rx_error = FSP_SUCCESS;
uint32_t g_baud_rate = RESET_VALUE;

/* Private variables */
static volatile uart_event_t s_uarta_event = (uart_event_t)RESET_VALUE;
static uarta_baud_setting_t s_uarta_baud_rate =
{
    .utanck_clock_b.utasel = RESET_VALUE,
    .utanck_clock_b.utanck = RESET_VALUE,
    .brgca = RESET_VALUE,
    .delay_time = RESET_VALUE
};
static uarta_extended_cfg_t s_uarta_ep_cfg_extend;
static uart_cfg_t s_uarta_ep_cfg;

/* Look-up table for converting UARTA clock source, used to get the clock value from R_BSP_SourceClockHzGet */
static const fsp_priv_source_clock_t uarta_f_uta0_sel_lut[] =
{
    [UARTA_CLOCK_SOURCE_SOSC_LOCO] = FSP_PRIV_CLOCK_LOCO,
    [UARTA_CLOCK_SOURCE_MOSC] = FSP_PRIV_CLOCK_MAIN_OSC,
    [UARTA_CLOCK_SOURCE_HOCO] = FSP_PRIV_CLOCK_HOCO,
    [UARTA_CLOCK_SOURCE_MOCO] = FSP_PRIV_CLOCK_MOCO
};

/* Private function */
static uint16_t uarta_calculate_delay_time(uarta_baud_setting_t const * const p_baud_setting);

/***********************************************************************************************************************
 * Function Name: uarta_init.
 * Description  : This function initializes the UARTA module.
 * Arguments    : None.
 * Return Value : FSP_SUCCESS               Initialized successfully.
 *                Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t uarta_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Open UARTA module */
    err = R_UARTA_Open(&g_uarta_ctrl, &g_uarta_cfg);

    return err;
}
/***********************************************************************************************************************
* End of function uarta_init
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: uarta_send.
 * Description  : This function writes data to the terminal application on the host PC.
 * Arguments    : p_data                    Pointer to data buffer.
 * Return Value : FSP_SUCCESS               Written successfully.
 *                Any other error codes     Failed to write.
 **********************************************************************************************************************/
fsp_err_t uarta_send(uint8_t * p_data)
{
    fsp_err_t err = FSP_SUCCESS;
    uint32_t timeout = RESET_VALUE;

    /* Set a timeout value according to the length of the input data */
    timeout = (uint32_t)strlen((char *)p_data) * TIME_OUT_FOR_ONE_BYTE;

    /* Reset UARTA callback event */
    s_uarta_event = (uart_event_t)RESET_VALUE;

    /* Perform UARTA write operation */
    err = R_UARTA_Write(&g_uarta_ctrl, p_data, strlen((char *)p_data));
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Wait for UART_EVENT_TX_COMPLETE event */
    while (UART_EVENT_TX_COMPLETE != s_uarta_event)
    {
        /* Check time elapsed to avoid infinite loop */
        if (RESET_VALUE == timeout)
        {
            return FSP_ERR_TIMEOUT;
        }

        /* Decrease timeout value then software delay */
        timeout--;
        R_BSP_SoftwareDelay(TIME_OUT_VALUE, TIME_OUT_UNIT);
    }

    return err;
}
/***********************************************************************************************************************
* End of function uarta_send
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: uarta_calculate_baud_rate.
 * Description  : This function recalculates the settings for the requested baud rate, then reopens UARTA with
 *                those settings.
 * Arguments    : None.
 * Return Value : FSP_SUCCESS               Reconfigured successfully.
 *                Any other error codes     Failed to reconfigure.
 **********************************************************************************************************************/
fsp_err_t uarta_calculate_baud_rate(void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint8_t clock_source = RESET_VALUE;
    uint8_t percent_error = RESET_VALUE;
    uint16_t wait_time = RESET_VALUE;

    uarta_deinit();

    /* Get clock source from the UARTA configuration */
    clock_source = g_uarta_cfg_extend.p_baud_setting->utanck_clock_b.utasel;

    /* Create memory copy of UARTA extended configuration and then copy new configuration values */
    memcpy((void *)&s_uarta_ep_cfg_extend, (void *)&g_uarta_cfg_extend, sizeof(uarta_extended_cfg_t));

    /* Create memory copy of UARTA configuration and update with new extended configuration structure */
    memcpy((void *)&s_uarta_ep_cfg, (void *)&g_uarta_cfg, sizeof(uart_cfg_t));

    /* Get baud rate percent error */
    if ((14400 == g_baud_rate) || (57600 == g_baud_rate) || (115200 == g_baud_rate))
    {
        percent_error = UARTA_BAUD_ERROR_SCALING_1;
    }
    else
    {
        percent_error = UARTA_BAUD_ERROR_SCALING_2;
    }

    /* Calculate baud rate settings for the requested baud rate */
    err = R_UARTA_BaudCalculate(g_baud_rate, percent_error, clock_source, &s_uarta_baud_rate);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Update delay time, baud rate for the working UARTA configuration */
    wait_time = uarta_calculate_delay_time(&s_uarta_baud_rate);
    s_uarta_baud_rate.delay_time = wait_time;
    s_uarta_ep_cfg_extend.p_baud_setting = &s_uarta_baud_rate;
    s_uarta_ep_cfg.p_extend = (void *)&s_uarta_ep_cfg_extend;

    /* Reopen UARTA with the new baud rate settings */
    err = R_UARTA_Open(&g_uarta_ctrl, &s_uarta_ep_cfg);

    return err;
}
/***********************************************************************************************************************
* End of function uarta_calculate_baud_rate
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: uarta_get_baud_rate.
 * Description  : This function is used to get baud rate for each UARTA baud rate option.
 * Arguments    : None.
 * Return Value : FSP_SUCCESS               Selected successfully.
 *                Any other error codes     Failed to select.
 **********************************************************************************************************************/
fsp_err_t uarta_get_baud_rate(void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint8_t user_input = RESET_VALUE;

    g_baud_rate = RESET_VALUE;

    while (RESET_VALUE == g_baud_rate)
    {
        err = uarta_send((uint8_t *)EP_MENU);
        if (FSP_SUCCESS != err)
        {
            return err;
        }

        /* Wait for the user to enter a menu selection */
        while (true != g_rx_flag)
        {
            /* Do nothing */
        }
        g_rx_flag = false;

        /* Get user input */
        user_input = (uint8_t)atoi((char *)g_rx_buffer);
        memset(g_rx_buffer, NULL_CHAR, sizeof(g_rx_buffer));

        switch (user_input)
        {
            case UARTA_9600:
            {
                g_baud_rate = 9600;
                break;
            }

            case UARTA_14400:
            {
                g_baud_rate = 14400;
                break;
            }

            case UARTA_19200:
            {
                g_baud_rate = 19200;
                break;
            }

            case UARTA_38400:
            {
                g_baud_rate = 38400;
                break;
            }

            case UARTA_57600:
            {
                g_baud_rate = 57600;
                break;
            }

            case UARTA_115200:
            {
                g_baud_rate = 115200;
                break;
            }

            default:
            {
                err = uarta_send((uint8_t *)EP_INVALID_INPUT);
                if (FSP_SUCCESS != err)
                {
                    return err;
                }
                break;
            }
        }
    }

    return err;
}
/***********************************************************************************************************************
* End of function uarta_get_baud_rate
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: uarta_calculate_delay_time.
 * Description  : This function calculates the wait time needed to enable TX/RX.
 * Arguments    : p_baud_setting            Pointer to the baud rate settings.
 * Return Value : delay_time                Delay time value.
 **********************************************************************************************************************/
static uint16_t uarta_calculate_delay_time(uarta_baud_setting_t const * const p_baud_setting)
{
    uint16_t delay_time = RESET_VALUE;

    uint8_t uta0ck = p_baud_setting->utanck_clock_b.utanck;
    uint8_t utasel = p_baud_setting->utanck_clock_b.utasel;

    /* Get UARTA clock divider shift */
    uint32_t divider_shift = (uint32_t)(uta0ck & UARTA_CLOCK_DIV_MASK);

    /* Calculate frequency of the UARTA operation clock */
    uint32_t f_uta0 = (R_BSP_SourceClockHzGet(uarta_f_uta0_sel_lut[utasel]) >> divider_shift);

    /* Round up especially for the use case f_uta0 > UARTA_CONVERT_TO_MICRO_SECOND */
    delay_time = (uint16_t)((UARTA_CONVERT_TO_MICRO_SECOND + f_uta0) / f_uta0);

    return delay_time;
}
/***********************************************************************************************************************
* End of function uarta_calculate_delay_time
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: uarta_deinit.
 * Description  : This function de-initializes the opened UARTA module.
 * Arguments    : None.
 * Return Value : None.
 **********************************************************************************************************************/
void uarta_deinit(void)
{
    /* Close the opened UARTA module */
    if (0U != g_uarta_ctrl.open)
    {
        R_UARTA_Close(&g_uarta_ctrl);
    }
}
/***********************************************************************************************************************
* End of function uarta_deinit
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: uarta_callback.
 * Description  : This function is a callback for the UARTA module.
 * Arguments    : p_args                    Pointer to the callback argument.
 * Return Value : None.
 **********************************************************************************************************************/
void uarta_callback(uart_callback_args_t * p_args)
{
    static volatile uint16_t rx_count = RESET_VALUE;

    if (NULL != p_args)
    {
        /* Get UARTA callback event */
        s_uarta_event = p_args->event;

        switch (p_args->event)
        {
            /* Handle the character reception event */
            case UART_EVENT_RX_CHAR:
            {
                /* If Enter is pressed by the user, set flag to process the data */
                if ((CR_ASCII == p_args->data) || (LF_ASCII == p_args->data))
                {
                    /* Set flag if at least one character has been received */
                    if (RESET_VALUE != rx_count)
                    {
                        g_rx_len = rx_count;
                        g_rx_flag = true;
                        rx_count = RESET_VALUE;
                    }
                }

                /* Read all data provided by the user until Enter button is pressed */
                else
                {
                    /* Check the number of characters received to ensure there is no buffer overflow */
                    if (BUFFER_LEN > rx_count)
                    {
                        g_rx_buffer[rx_count++] = (uint8_t)p_args->data;
                    }
                }
                break;
            }

            /* Handle reception error events */
            case UART_EVENT_ERR_PARITY:
            {
                g_uarta_rx_error = FSP_ERR_PARITY;
                break;
            }

            case UART_EVENT_ERR_FRAMING:
            {
                g_uarta_rx_error = FSP_ERR_FRAMING;
                break;
            }

            case UART_EVENT_ERR_OVERFLOW:
            {
                g_uarta_rx_error = FSP_ERR_RXBUF_OVERFLOW;
                break;
            }

            default:
            {
                /* Do nothing */
            }
        }
    }
}
/***********************************************************************************************************************
* End of function uarta_callback
***********************************************************************************************************************/

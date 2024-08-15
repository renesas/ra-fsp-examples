/***********************************************************************************************************************
 * File Name    : uart_terminal.c
 * Description  : Contains macros, data structures and functions used  common to the EP
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "uart_terminal.h"

/**********************************************************************************************************************
* Private global variables declarations
**********************************************************************************************************************/
static volatile uart_event_t g_uart_event = RESET_VALUE;

/*******************************************************************************************************************//**
 *  @brief      SCI UART callback function
 *  @param[in]  p_args
 *  @retval     None
 **********************************************************************************************************************/
void uart_callback (uart_callback_args_t *p_args)
{
    /* Check null pointer */
    if (NULL == p_args)
    {
        return;
    }

    /* Check correct SCI UART channel */
    if (g_uart_cfg.channel != p_args->channel)
    {
        return;
    }

    /* Get SCI UART event */
    g_uart_event = p_args->event;
}

/*******************************************************************************************************************//**
 *  @brief      This function will initializes the SCI UART module which transfer the EP log to the host PC
 *  @param      None
 *  @retval     FSP_SUCCESS    Upon successful operation
 *  @retval     Any Other Error code apart from FSP_SUCCES
 **********************************************************************************************************************/
fsp_err_t uart_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Initialize SCI UART channel with baud rate 115200 */
#if (BSP_FEATURE_SCI_VERSION == 1U)
    err = R_SCI_UART_Open(&g_uart_ctrl, &g_uart_cfg);
#elif defined(BOARD_RA0E1_FPB)
    err = R_UARTA_Open(&g_uart_ctrl, &g_uart_cfg);
#else
    err = R_SCI_B_UART_Open(&g_uart_ctrl, &g_uart_cfg);
#endif

    return err;
}

/*******************************************************************************************************************//**
 *  @brief      This function will de-initialize SCI UART module
 *  @param      None
 *  @retval     None
 **********************************************************************************************************************/
void uart_deinit(void)
{
    /* De-initialize SCI UART if it opened */
    if (g_uart_ctrl.open)
    {
#if (BSP_FEATURE_SCI_VERSION == 1U)
        R_SCI_UART_Close(&g_uart_ctrl);
#elif defined(BOARD_RA0E1_FPB)
        R_UARTA_Close(&g_uart_ctrl);
#else
        R_SCI_B_UART_Close(&g_uart_ctrl);
#endif
    }
}

/*******************************************************************************************************************//**
 *  @brief      This function will transfer a message to the host PC
 *  @param[in]  p_buf   pointer to message buffer
 *  @retval     FSP_SUCCESS    Upon successful operation
 *  @retval     Any Other Error code apart from FSP_SUCCES
 **********************************************************************************************************************/
fsp_err_t uart_print(uint8_t * p_buf)
{
    fsp_err_t   err     = FSP_SUCCESS;
    uint32_t    len     = RESET_VALUE;
    uint32_t    timeout = RESET_VALUE;

    /* Calculate length of message received */
    len = (uint32_t)(strlen((char *)p_buf));

    /* Set timeout value according to message length */
    timeout = len;

    /* Reset SCI UART callback variable */
    g_uart_event = RESET_VALUE;

    /* Perform SCI UART write operation */
#if (BSP_FEATURE_SCI_VERSION == 1U)
    err = R_SCI_UART_Write (&g_uart_ctrl, p_buf, len);
#elif defined(BOARD_RA0E1_FPB)
    err = R_UARTA_Write (&g_uart_ctrl, p_buf, len);
#else
    err = R_SCI_B_UART_Write (&g_uart_ctrl, p_buf, len);
#endif

    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Check for event transfer complete */
    while (UART_EVENT_TX_COMPLETE != g_uart_event)
    {
        /* Check time elapsed to avoid infinite loop */
        if (RESET_VALUE == timeout)
        {
            return FSP_ERR_TIMEOUT;
        }

        /* Decrease timeout value, then delay the transfer time by one character */
        timeout --;
        R_BSP_SoftwareDelay(UART_CHAR_TIMEOUT_VALUE, UART_CHAR_TIMEOUT_UNIT);
    }

    return err;
}

/*******************************************************************************************************************//**
 *  @brief      This function will transfer the example project information to the host PC
 *  @param      None
 *  @retval     FSP_SUCCESS    Upon successful operation
 *  @retval     Any Other Error code apart from FSP_SUCCES
 **********************************************************************************************************************/
fsp_err_t uart_ep_info_print (void)
{
    fsp_err_t           err                                 = FSP_SUCCESS;
    uint8_t             uart_buffer [UART_EP_INFO_SIZE]     = {RESET_VALUE};
    fsp_pack_version_t  version                             = {RESET_VALUE};

    /* version get API for FLEX pack information */
    err = R_FSP_VersionGet(&version);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Convert EP version, FSP version to buffer */
    snprintf((char *)uart_buffer, (size_t)UART_EP_INFO_SIZE, BANNER_VER,
            EP_VERSION, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);

    /* Print EP version and FSP version to terminal on Host PC via SCI UART */
    err = uart_print(uart_buffer);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Print EP version and FSP version to terminal on Host PC via SCI UART */
    err = uart_print((uint8_t *)BANNER_INFO);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    return err;
}

/*******************************************************************************************************************//**
 *  @brief      This function will transfer an error information to the host PC
 *  @param[in]  p_func  pointer to function name string
 *  @param[in]  p_info  pointer to error information string
 *  @retval     FSP_SUCCESS    Upon successful operation
 *  @retval     Any Other Error code apart from FSP_SUCCES
 **********************************************************************************************************************/
fsp_err_t uart_error_info_print (uint8_t * p_func, uint8_t * p_info)
{
    fsp_err_t   err                                 = FSP_SUCCESS;
    uint8_t     uart_buffer [UART_ERROR_INFO_SIZE]  = {RESET_VALUE};

    /* Convert to string buffer */
    snprintf((char *)uart_buffer, (size_t)UART_ERROR_INFO_SIZE,
            "[ERR] In Function: %s(), %s", (char *)p_func, (char *)p_info);

    /* Print to terminal on Host PC via SCI UART */
    err = uart_print(uart_buffer);
    return err;
}

/*******************************************************************************************************************//**
 *  @brief      This function will transfer an error code in hexadecimal to the host PC
 *  @param[in]  err_code    error code in hexadecimal
 *  @retval     FSP_SUCCESS    Upon successful operation
 *  @retval     Any Other Error code apart from FSP_SUCCES
 **********************************************************************************************************************/
fsp_err_t uart_error_code_print (fsp_err_t err_code)
{
    fsp_err_t   err                                 = FSP_SUCCESS;
    uint8_t     uart_buffer [UART_ERROR_CODE_SIZE]  = {RESET_VALUE};

    /* Convert to string buffer */
    snprintf((char *)uart_buffer, (size_t)UART_ERROR_CODE_SIZE, "\r\nReturned Error Code: 0x%x \r\n", err_code);

    /* Print to terminal on Host PC via SCI UART */
    err = uart_print(uart_buffer);
    return err;
}


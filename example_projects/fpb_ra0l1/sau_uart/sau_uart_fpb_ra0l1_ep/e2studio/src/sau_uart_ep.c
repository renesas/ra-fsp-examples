/***********************************************************************************************************************
 * File Name    : sau_uart_ep.c
 * Description  : Contains data structures and functions.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "sau_uart_ep.h"

/* Private variables */
static uint8_t g_rx_buffer [BUFFER_LEN] = {RESET_VALUE};
static uint8_t g_tx_buffer [BUFFER_LEN] = {RESET_VALUE};
static volatile uint16_t g_rx_len = RESET_VALUE;
static volatile uart_event_t g_sau_uart_event = (uart_event_t)RESET_VALUE;
static volatile bool g_rx_flag = false;

/* Private function declarations */
static fsp_err_t sau_uart_init(void);
static fsp_err_t sau_uart_send(uint8_t * p_data);
static fsp_err_t sau_uart_operation(void);
static void sau_uart_deinit(void);
static void handle_error(fsp_err_t err,  const char * err_str);

/*******************************************************************************************************************//**
 *  @brief       UART user callback.
 *  @param[in]   p_args
 *  @retval      None
 **********************************************************************************************************************/
void sau_uart_callback(uart_callback_args_t *p_args)
{
    static volatile uint16_t rx_count = RESET_VALUE;

    if (NULL != p_args)
    {
        /* Get SAU UART callback event */
        g_sau_uart_event = p_args->event;

        /* Handle the character reception event */
        if (UART_EVENT_RX_CHAR == p_args->event)
        {
            switch (p_args->data)
            {
                /* If Enter is pressed by the user, set flag to process the data */
                case CR_ASCII:
                case LF_ASCII:
                    /* Set flag if at least one character has been received */
                    if (RESET_VALUE != rx_count)
                    {
                        g_rx_len = rx_count;
                        g_rx_flag = true;
                        rx_count = RESET_VALUE;
                    }
                    break;

                /* Read all data provided by the user until enter button is pressed */
                default:
                    /* Check the number of characters received to ensure there is no buffer overflow */
                    if (BUFFER_LEN > rx_count)
                    {
                        g_rx_buffer[rx_count ++] = (uint8_t)p_args->data;
                    }
                    break;
            }
        }
    }
}

/*******************************************************************************************************************//**
 *  @brief       This function is used to initialize the related module and start the SAU UART example operation.
 *  @param[in]   None
 *  @retval      None
 **********************************************************************************************************************/
void sau_uart_ep_entry(void)
{
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version = { RESET_VALUE };

    /* Version get API for FLEX pack information */
    R_FSP_VersionGet(&version);

    /* Print the EP banner on the RTT Viewer */
    APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor,
              version.version_id_b.patch);

    /* Delay to ensure banner display on RTT Viewer */
    R_BSP_SoftwareDelay(RTT_DELAY_VALUE, BSP_DELAY_UNITS_MILLISECONDS);

    /* Print the EP information on the RTT Viewer */
    APP_PRINT(EP_INFO);

    /* Delay to ensure EP information display on RTT Viewer */
    R_BSP_SoftwareDelay(RTT_DELAY_VALUE, BSP_DELAY_UNITS_MILLISECONDS);

    /* Print the EP message on the RTT Viewer */
    APP_PRINT(EP_MESSAGE);

    /* Delay to ensure EP information display on RTT Viewer */
    R_BSP_SoftwareDelay(RTT_DELAY_VALUE, BSP_DELAY_UNITS_MILLISECONDS);

    /* Initializes the SAU UART module */
    err = sau_uart_init();
    handle_error(err, "sau_uart_init failed\r\n");

    while (true)
    {
        /* Perform the SAU UART operation */
        err = sau_uart_operation();
        handle_error(err, "sau_uart_operation failed\r\n");
    }
}

/*******************************************************************************************************************//**
 *  @brief       This function initializes the SAU UART module.
 *  @param[in]   None
 *  @retval      FSP_SUCCESS    Upon successful operation.
 *  @retval      Any other error code apart from FSP_SUCCESS.
 **********************************************************************************************************************/
static fsp_err_t sau_uart_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_SAU_UART_Open(&g_sau_uart_ctrl, &g_sau_uart_cfg);
    APP_ERR_RETURN(err, "R_SAU_UART_Open API failed\r\n");

    return err;
}

/*******************************************************************************************************************//**
 *  @brief       This function writes data to the terminal application on the host PC.
 *  @param[in]   p_data         Pointer to data buffer.
 *  @retval      FSP_SUCCESS    Upon successful operation.
 *  @retval      Any other error code apart from FSP_SUCCESS.
 **********************************************************************************************************************/
static fsp_err_t sau_uart_send(uint8_t * p_data)
{
    fsp_err_t err = FSP_SUCCESS;
    uint32_t timeout = RESET_VALUE;

    /* Set a timeout value according to the length of the input data */
    timeout = (uint32_t)strlen((char *)p_data) * TIME_OUT_FOR_ONE_BYTE ;

    /* Reset SAU UART callback event */
    g_sau_uart_event = (uart_event_t)RESET_VALUE;

    /* Perform SAU UART write operation */
    err = R_SAU_UART_Write(&g_sau_uart_ctrl, p_data, strlen((char *)p_data));
    APP_ERR_RETURN(err, "R_SAU_UART_Write API failed\r\n");

    /* Wait for UART_EVENT_TX_COMPLETE event */
    while (UART_EVENT_TX_COMPLETE != g_sau_uart_event)
    {
        /* Check time elapsed to avoid infinite loop */
        if(RESET_VALUE == timeout)
        {
            APP_ERR_RETURN(FSP_ERR_TIMEOUT, "No SAU UART callback events received\r\n");
        }

        /* Decrease timeout value then software delay */
        timeout --;
        R_BSP_SoftwareDelay(TIME_OUT_VALUE, TIME_OUT_UNIT);
    }

    return err;
}

/*******************************************************************************************************************//**
 *  @brief       This is the SAU UART operation function.
 *  @param[in]   None
 *  @retval      FSP_SUCCESS    Upon successful operation.
 *  @retval      Any other error code apart from FSP_SUCCESS.
 **********************************************************************************************************************/
static fsp_err_t sau_uart_operation(void)
{
    fsp_err_t err = FSP_SUCCESS;

    if (true == g_rx_flag)
    {
        /* Clear the SAU UART receive flag to detect new frames */
        g_rx_flag = false;

        /* Copy the received data frame to transmit buffer */
        memcpy(g_tx_buffer, g_rx_buffer, g_rx_len);

        /* Clean the receive buffer */
        memset(g_rx_buffer, NULL_CHAR, sizeof(g_rx_buffer));

        /* Print the received data to RTT Viewer */
        APP_PRINT("Received a string: %s\r\n", (char *)g_tx_buffer);

        size_t tx_buffer_len = strlen((char*)g_tx_buffer);

        /* Move the transmit buffer to add echo message */
        memmove((char*)g_tx_buffer + ECHO_MESSAGE_LEN, (char*)g_tx_buffer, tx_buffer_len + 1);

        /* Add echo message for the transmit buffer */
        memcpy((char*)g_tx_buffer, ECHO_MESSAGE, ECHO_MESSAGE_LEN);

        /* Add a newline character for the transmit buffer */
        strcat((char*)g_tx_buffer, "\r\n");

        /* Send received data back to the host PC */
        err = sau_uart_send(g_tx_buffer);
        APP_ERR_RETURN(err, "sau_uart_send failed\r\n");

        /* Clean the SAU UART transmit buffer */
        memset(g_tx_buffer, NULL_CHAR, sizeof(g_tx_buffer));
    }

    return err;
}

/*******************************************************************************************************************//**
 *  @brief       This function de-initializes the SAU UART module.
 *  @param[in]   None
 *  @retval      None
 **********************************************************************************************************************/
static void sau_uart_deinit(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Close the SAU UART module if already open */
    if (MODULE_CLOSE != g_sau_uart_ctrl.open)
    {
        err = R_SAU_UART_Close(&g_sau_uart_ctrl);
        if(FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("R_SAU_UART_Close API failed\r\n");
        }
    }
}

/*******************************************************************************************************************//**
 *  @brief       If an error occurs, this function handles the error, closes all opened modules, prints and traps error
 *  @param[in]   err        Error value.
 *  @param[in]   err_str    Error string.
 *  @retval      None
 **********************************************************************************************************************/
static void handle_error(fsp_err_t err,  const char * err_str)
{
    if(FSP_SUCCESS != err)
    {
        /* Print the error */
        APP_PRINT(err_str);

        /* De-initializes the SAU UART module */
        sau_uart_deinit();

        /* Trap the error */
        APP_ERR_TRAP(err);
    }
}

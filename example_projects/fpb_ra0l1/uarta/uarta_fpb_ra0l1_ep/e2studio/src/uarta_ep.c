/***********************************************************************************************************************
 * File Name    : uarta_ep.c
 * Description  : Contains data structures and functions.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "uarta_ep.h"

/* Private Variables */
static uarta_baud_setting_t g_uarta_baud_rate =
{
 .utanck_clock_b.utasel = RESET_VALUE,
 .utanck_clock_b.utanck = RESET_VALUE,
 .brgca                 = RESET_VALUE,
 .delay_time            = RESET_VALUE
};

static uarta_extended_cfg_t g_uarta_ep_cfg_extend;
static uart_cfg_t g_uarta_ep_cfg;
static uint8_t g_rx_buffer[BUFFER_LEN] = {RESET_VALUE};
static uint8_t g_tx_buffer[BUFFER_LEN] = {RESET_VALUE};
static volatile uint16_t g_rx_len = RESET_VALUE;
static volatile uart_event_t g_uarta_event = (uart_event_t)RESET_VALUE;
static volatile bool g_rx_flag = false;
static uint32_t g_baud_rate = RESET_VALUE;

/* The buffer contains user input */
static char g_rtt_buffer[BUFFER_SIZE_DOWN] = {RESET_VALUE};

/* Look-up table for converting UARTA clock source, used to get the clock value from R_BSP_SourceClockHzGet */
static fsp_priv_source_clock_t uarta_f_uta0_sel_lut[] =
{
    [UARTA_CLOCK_SOURCE_SOSC_LOCO] = FSP_PRIV_CLOCK_LOCO,
    [UARTA_CLOCK_SOURCE_MOSC]      = FSP_PRIV_CLOCK_MAIN_OSC,
    [UARTA_CLOCK_SOURCE_HOCO]      = FSP_PRIV_CLOCK_HOCO,
    [UARTA_CLOCK_SOURCE_MOCO]      = FSP_PRIV_CLOCK_MOCO
};

/* Private function declarations */
static fsp_err_t uarta_init(void);
static fsp_err_t uarta_send(uint8_t * p_data);
static fsp_err_t uarta_calculate_baud_rate(void);
static fsp_err_t uarta_operation(void);
static void uarta_deinit(void);
static void handle_error(fsp_err_t err, const char * err_str);
static uint8_t get_user_input(void);
static uint16_t uarta_calculate_delay_time(uarta_baud_setting_t const * const p_baud_setting);
static uint32_t uarta_get_baud_rate(void);

/***********************************************************************************************************************
 *  Function Name: uarta_callback
 *  Description  : UART user callback
 *  Arguments    : p_args
 *  Return Value : None
 **********************************************************************************************************************/
void uarta_callback(uart_callback_args_t *p_args)
{
    static volatile uint16_t rx_count = RESET_VALUE;

    if (NULL != p_args)
    {
        /* Get UARTA callback event */
        g_uarta_event = p_args->event;

        /* Handle the character reception event */
        switch (p_args->event)
        {
            case UART_EVENT_RX_CHAR:
            {
                /* If Enter is pressed by user, set flag to process the data */
                if (CR_ASCII == p_args->data || LF_ASCII == p_args->data)
                {
                    /* Set flag if at least one character has been received */
                    if (RESET_VALUE != rx_count)
                    {
                        g_rx_len  = rx_count;
                        g_rx_flag = true;
                        rx_count  = RESET_VALUE;
                    }
                }
                /* Read all data provided by user until enter button is pressed */
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

            /* Handle reception errors event */
            case UART_EVENT_ERR_PARITY:
            {
                handle_error(FSP_ERR_PARITY, "\r\nParity error occurs!!!\r\n");
                break;
            }
            case UART_EVENT_ERR_FRAMING:
            {
                handle_error(FSP_ERR_FRAMING, "\r\nFraming error occurs!!!\r\n");
                break;
            }
            case UART_EVENT_ERR_OVERFLOW:
            {
                handle_error(FSP_ERR_RXBUF_OVERFLOW, "\r\nReceive queue overflow!!!\r\n");
                break;
            }
            default:
                /* Do nothing */
        }
    }
}
/***********************************************************************************************************************
* End of function uarta_callback
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: uarta_ep_entry
 *  Description  : This function initializes the UARTA module and performs UARTA operation.
 *  Arguments    : None
 *  Return Value : None
 **********************************************************************************************************************/
void uarta_ep_entry(void)
{
    fsp_err_t err = FSP_SUCCESS;

    fsp_pack_version_t version = { RESET_VALUE };

    /* Version get API for FLEX pack information */
    R_FSP_VersionGet(&version);

    /* Print the EP banner on the RTT Viewer */
    APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor,\
              version.version_id_b.patch);

    /* Delay to ensure EP banner display on the RTT Viewer */
    R_BSP_SoftwareDelay(RTT_DELAY_VALUE, BSP_DELAY_UNITS_MILLISECONDS);

    /* Print the EP information on the RTT Viewer */
    APP_PRINT(EP_INFO);

    /* Delay to ensure EP information display on the RTT Viewer */
    R_BSP_SoftwareDelay(RTT_DELAY_VALUE, BSP_DELAY_UNITS_MILLISECONDS);

    /* Calculate baud rate for the UARTA module */
    err = uarta_calculate_baud_rate();
    handle_error(err, "\r\nuarta_calculate_baud_rate failed!!!\r\n");
    APP_PRINT("\r\nUARTA calculated baud rate successfully!!!\r\n");

    /* Initialize the UARTA module */
    err = uarta_init();
    handle_error(err, "\r\nuarta_init failed!!!\r\n");
    APP_PRINT("\r\nUARTA initialized successfully!!!\r\n");

    /* Print the EP message on the RTT Viewer */
    APP_PRINT(EP_MESSAGE, g_baud_rate);

    /* Delay to ensure EP message display on the RTT Viewer */
    R_BSP_SoftwareDelay(RTT_DELAY_VALUE, BSP_DELAY_UNITS_MILLISECONDS);

    while (true)
    {
        /* Perform the UARTA operation */
        err = uarta_operation();
        handle_error(err, "\r\nuarta_operation failed!!!\r\n");
    }
}
/***********************************************************************************************************************
* End of function uarta_ep_entry
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: uarta_init
 *  Description  : This function initializes the UARTA module.
 *  Arguments    : None
 *  Return Value : FSP_SUCCESS    Upon successful operation.
 *                 Any other error code apart from FSP_SUCCESS.
 **********************************************************************************************************************/
static fsp_err_t uarta_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_UARTA_Open(&g_uarta_ctrl, &g_uarta_ep_cfg);
    APP_ERR_RETURN(err, "\r\nR_UARTA_Open API failed!!!\r\n");

    return err;
}
/***********************************************************************************************************************
* End of function uarta_init
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: uarta_send
 *  Description  : This function writes data to the terminal application on the host PC.
 *  Arguments    : p_data         Pointer to data buffer.
 *  Return Value : FSP_SUCCESS    Upon successful operation.
 *                 Any other error code apart from FSP_SUCCESS.
 **********************************************************************************************************************/
static fsp_err_t uarta_send(uint8_t * p_data)
{
    fsp_err_t err = FSP_SUCCESS;

    uint32_t timeout = RESET_VALUE;

    /* Set a timeout value according to the length of the input data */
    timeout = (uint32_t)strlen((char *)p_data) * TIME_OUT_FOR_ONE_BYTE ;

    /* Reset UARTA callback event */
    g_uarta_event = (uart_event_t)RESET_VALUE;

    /* Perform UARTA write operation */
    err = R_UARTA_Write(&g_uarta_ctrl, p_data, strlen((char *)p_data));
    APP_ERR_RETURN(err, "\r\nR_UARTA_Write API failed!!!\r\n");

    /* Wait for UART_EVENT_TX_COMPLETE event */
    while (UART_EVENT_TX_COMPLETE != g_uarta_event)
    {

        /* Check time elapsed to avoid infinite loop */
        if (RESET_VALUE == timeout)
        {
            APP_ERR_RETURN(FSP_ERR_TIMEOUT, "\r\nNo UARTA callback events received!!!\r\n");
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
 *  Function Name: uarta_operation
 *  Description  : This is the UARTA operation function.
 *  Arguments    : None
 *  Return Value : FSP_SUCCESS    Upon successful operation.
 *                 Any other error code apart from FSP_SUCCESS.
 **********************************************************************************************************************/
static fsp_err_t uarta_operation(void)
{
    fsp_err_t err = FSP_SUCCESS;

    if (true == g_rx_flag)
    {
        /* Clear the UARTA receive flag to detect new frames */
        g_rx_flag = false;

        /* Copy the received data frame to transmit buffer */
        memcpy(g_tx_buffer, g_rx_buffer, g_rx_len);

        /* Clean the receive buffer */
        memset(g_rx_buffer, NULL_CHAR, sizeof(g_rx_buffer));

        /* Print the received data to RTT Viewer */
        APP_PRINT("Received a string: %s\r\n", (char *)g_tx_buffer);

        size_t echo_message_len = strlen(ECHO_MESSAGE);
        size_t tx_buffer_len    = strlen((char*)g_tx_buffer);

        /* Move the transmit buffer to add echo message */
        memmove((char*)g_tx_buffer + echo_message_len, (char*)g_tx_buffer, tx_buffer_len + 1);

        /* Add echo message for the transmit buffer */
        memcpy((char*)g_tx_buffer, ECHO_MESSAGE, echo_message_len);

        /* Add a newline character for the transmit buffer */
        strcat((char*)g_tx_buffer, "\r\n");

        /* Send received data back to the host PC */
        err = uarta_send(g_tx_buffer);
        APP_ERR_RETURN(err, "uarta_send failed!!!\r\n");

        /* Clean the UARTA transmit buffer */
        memset(g_tx_buffer, NULL_CHAR, sizeof(g_tx_buffer));
    }

    return err;
}
/***********************************************************************************************************************
* End of function uarta_operation
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: get_user_input
 * Description  : This function is used to get the user input.
 * Arguments    : None
 * Return Value : input_value    Return input buffer after receive enter key.
 **********************************************************************************************************************/
static uint8_t get_user_input(void)
{
    uint8_t input_value = RESET_VALUE;

    /* Clean RTT buffer */
    memset(g_rtt_buffer, NULL_CHAR, sizeof(g_rtt_buffer));

    /* Wait until there is any user input */
    while (!APP_CHECK_DATA)
    {
        ;
    }

    /* Get user input and store in g_rtt_buffer */
    APP_READ(g_rtt_buffer);

    /* Convert data in g_rtt_buffer to integer and store in input_value */
    input_value = (uint8_t)atoi(g_rtt_buffer);

    return input_value;
}
/***********************************************************************************************************************
* End of function get_user_input
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: uarta_get_baud_rate
 * Description  : This function is used to get baud rate for each UARTA baud rate option.
 * Arguments    : None
 * Return Value : baud_rate     Return baud rate value in bps.
 **********************************************************************************************************************/
static uint32_t uarta_get_baud_rate(void)
{
    uint32_t baud_rate  = RESET_VALUE;
    uint8_t  user_input = RESET_VALUE;

    /* Print the EP menu on the RTT Viewer */
    APP_PRINT(EP_MENU);

    /* Get user input option for baud rate */
    user_input = get_user_input();

    switch (user_input)
    {
        case UARTA_9600:
        {
            baud_rate = 9600;
            break;
        }
        case UARTA_14400:
        {
            baud_rate = 14400;
            break;
        }
        case UARTA_19200:
        {
            baud_rate = 19200;
            break;
        }
        case UARTA_38400:
        {
            baud_rate = 38400;
            break;
        }
        case UARTA_57600:
        {
            baud_rate = 57600;
            break;
        }
        case UARTA_115200:
        {
            baud_rate = 115200;
            break;
        }
        default:
            APP_PRINT("\r\nInvalid input. Please enter valid input!!!\r\n");
    }

    return baud_rate;
}
/***********************************************************************************************************************
* End of function uarta_get_baud_rate
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: uarta_calculate_baud_rate
 * Description  : This function is used to calculate baud rate for each UARTA baud rate option.
 * Arguments    : None
 * Return Value : FSP_SUCCESS    Upon successful operation.
 *                Any other error code apart from FSP_SUCCESS.
 **********************************************************************************************************************/
static fsp_err_t uarta_calculate_baud_rate(void)
{
    fsp_err_t err = FSP_SUCCESS;

    g_baud_rate = RESET_VALUE;

    uint8_t  clock_source  = RESET_VALUE;
    uint8_t  percent_error = RESET_VALUE;
    uint16_t wait_time     = RESET_VALUE;

    /* Get clock source from UARTA configuration */
    clock_source = g_uarta_cfg_extend.p_baud_setting->utanck_clock_b.utasel;

    /* Create memory copy of UARTA extended configuration and then copy new configuration values */
    memcpy((void *) &g_uarta_ep_cfg_extend, (void *)&g_uarta_cfg_extend, sizeof(uarta_extended_cfg_t));

    /* Create memory copy of UARTA configuration and update with new extended configuration structure */
    memcpy((void *) &g_uarta_ep_cfg, (void *)&g_uarta_cfg, sizeof(uart_cfg_t));

    while (RESET_VALUE == g_baud_rate)
    {

        /* Get baud rate from the user input */
        g_baud_rate = uarta_get_baud_rate();
    }

    /* Print the baud rate was selected on the RTT Viewer */
    APP_PRINT("\r\nBaud rate was selected = %d bps\r\n", g_baud_rate);

    /* Get baud rate percent error and store in percent_error */
    if ((14400 == g_baud_rate) || (57600 == g_baud_rate) || (115200 == g_baud_rate))
    {
        percent_error = UARTA_BAUD_ERROR_SCALING_1;
    }
    else
    {
        percent_error = UARTA_BAUD_ERROR_SCALING_2;
    }

    /* Calculate baud rate */
    err = R_UARTA_BaudCalculate(g_baud_rate, percent_error, clock_source, &g_uarta_baud_rate);

    if (FSP_SUCCESS == err)
    {

        /* Get delay time */
        wait_time = uarta_calculate_delay_time(&g_uarta_baud_rate);

        /* Update delay time, baud rate for UARTA extended configuration, UARTA configuration */
        g_uarta_baud_rate.delay_time         = wait_time;
        g_uarta_ep_cfg_extend.p_baud_setting = &g_uarta_baud_rate;
        g_uarta_ep_cfg.p_extend              = (void *)&g_uarta_ep_cfg_extend;
    }

    return err;
}
/***********************************************************************************************************************
* End of function uarta_calculate_baud_rate
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: uarta_calculate_delay_time
 * Description  : This function calculates the wait time to enable TX.
 * Arguments    : p_baud_setting    Pointer to baud rate settings.
 * Return Value : delay_time        Return delay time value.
 **********************************************************************************************************************/
static uint16_t uarta_calculate_delay_time(uarta_baud_setting_t const *const p_baud_setting)
{
    uint16_t delay_time = RESET_VALUE;

    uint8_t uta0ck = p_baud_setting->utanck_clock_b.utanck;
    uint8_t utasel = p_baud_setting->utanck_clock_b.utasel;

    /* Get UARTA clock divider shift */
    uint32_t divider_shift = (uint32_t)(uta0ck & UARTA_CLOCK_DIV_MASK);

    /* Calculate frequency UARTA operation clock */
    uint32_t f_uta0 = (R_BSP_SourceClockHzGet(uarta_f_uta0_sel_lut[utasel]) >> divider_shift);

    /* Round up especially for the use case f_uta0 > UARTA_CONVERT_TO_MICRO_SECOND */
    delay_time = (uint16_t)((UARTA_CONVERT_TO_MICRO_SECOND + f_uta0) / f_uta0);

    return delay_time;
}
/***********************************************************************************************************************
* End of function uarta_calculate_delay_time
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: uarta_deinit
 *  Description  : This function de-initializes the UARTA module.
 *  Arguments    : None
 *  Return Value : None
 **********************************************************************************************************************/
static void uarta_deinit(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Close the UARTA module if it already opened */
    if (MODULE_CLOSE != g_uarta_ctrl.open)
    {
        err = R_UARTA_Close(&g_uarta_ctrl);
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\nR_UARTA_Close API failed!!!\r\n");
        }
    }
}
/***********************************************************************************************************************
* End of function uarta_deinit
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: handle_error
 *  Description  : If an error occurred, this function handles the error, closes all opened modules,
 *                 prints and traps error.
 *  Arguments    : err        Error value.
 *                 err_str    Error string.
 *  Return Value : None
 **********************************************************************************************************************/
static void handle_error(fsp_err_t err, const char * err_str)
{
    if (FSP_SUCCESS != err)
    {
        /* Print the error */
        APP_PRINT(err_str);

        /* De-initialize the UARTA module */
        uarta_deinit();

        /* Trap the error */
        APP_ERR_TRAP(err);
    }
}
/***********************************************************************************************************************
* End of function handle_error
***********************************************************************************************************************/

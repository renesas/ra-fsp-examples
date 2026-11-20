/***********************************************************************************************************************
 * File Name    : lin_master_example_app.c
 * Description  : Example application layer for the SCI_B LIN MASTER EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - sdhi_thread_entry.c calls a single function: lin_master_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include "lin_master_example_app.h"
#include "console/uart_console.h"

/* Private variables */
static const uint8_t s_expected_data_id_20h[] = {0x01, 0x02};
static const uint8_t s_expected_data_id_21h[] = {0x03, 0x04};
static const uint8_t s_expected_data_id_22h[] = {0x05, 0x06};
static const uint8_t s_expected_data_id_23h[] = {0x07, 0x08};

static const lin_frame_expectation_t s_expected_frames[] =
{
    {READ_FRAME_ID_10H, s_expected_data_id_20h, sizeof(s_expected_data_id_20h)},
    {READ_FRAME_ID_11H, s_expected_data_id_21h, sizeof(s_expected_data_id_21h)},
    {READ_FRAME_ID_12H, s_expected_data_id_22h, sizeof(s_expected_data_id_22h)},
    {READ_FRAME_ID_13H, s_expected_data_id_23h, sizeof(s_expected_data_id_23h)},
};

/* Private functions */
static void lin_master_configure_baudrate(void);
static void lin_master_operation(bool is_write);
static void lin_validate_frame(uint8_t id, const uint8_t *p_data, size_t len);
static uint8_t get_user_input(uint8_t min_value, uint8_t max_value);
static void handle_error(fsp_err_t err, const char * err_str);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the LIN MASTER example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void lin_master_example_app_run(void)
{
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version = {RESET_VALUE};

    uint32_t status = uart_console_init();
    if (FSP_SUCCESS != status)
    {
        return;     /* UART failed to open — cannot print anything */
    }

    /* Get FSP version information */
    R_FSP_VersionGet(&version);

    /* Example project information printed on the terminal */
    printf(BANNER_INFO, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    printf(EP_INFO);

    /* Open LIN master interface */
    err = lin_master_open();
    handle_error(err, "lin_master_open failed.\r\n");

    /* Configure timeout management if enabled */
#if ENABLE_MASTER_TIMEOUT_MANAGEMENT
    err = timer_open();
    handle_error(err, "timer_open failed.\r\n");
#endif /* ENABLE_MASTER_TIMEOUT_MANAGEMENT */

    printf(MAIN_MENU);

    while (true)
    {
        if (uart_console_has_char())
        {
            int32_t selection = getchar();

            switch (selection)
            {
                case MASTER_SET_BAUDRATE:
                {
                    /* Configure the LIN baud rate */
                    lin_master_configure_baudrate();
                    break;
                }
                case MASTER_WRITE:
                {
                    lin_master_operation(IS_WRITE);
                    break;
                }
                case MASTER_READ:
                {
                    lin_master_operation(IS_READ);
                    break;
                }

                #if BSP_PERIPHERAL_SAU_PRESENT
                case WAKE_UP_SLAVE:
                {
                    err = lin_master_send_wakeup();
                    handle_error(err, "lin_master_send_wakeup failed\r\n");
                    break;
                }
                #endif /* BSP_PERIPHERAL_SAU_PRESENT */

                default:
                {
                    printf("\r\nInvalid input. Provide a valid input\r\n");
                    break;
                }
            }
            printf(MAIN_MENU);
        }
    }
}

/*******************************************************************************************************************//**
 * @brief     This function configures the LIN communication baud rate.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void lin_master_configure_baudrate(void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint32_t s_lin_baudrate_options[LIN_BAUDRATE_COUNT]  = {2400, 4800, 9600, 10400, 14400, 19200};

    printf(BAUDRATE_OPTION);

    uint8_t option = get_user_input(MIN_BAUDRATE_INDEX, MAX_BAUDRATE_INDEX);

    g_baudrate = s_lin_baudrate_options[option - 1];
    printf("\r\n\r\nSelected baud rate: %d bps", g_baudrate);

    err = lin_master_baudset(g_baudrate);
    handle_error(err, "lin_master_baudset failed\r\n");
    printf("\r\nLIN baud rate successfully updated!!!");
}

/*******************************************************************************************************************//**
 * @brief     This function performs LIN transmit or receive operation.
 * @param[in] is_write                  Operation mode.
 * @retval    None.
 **********************************************************************************************************************/
static void lin_master_operation(bool is_write)
{
    char recv_str[32] = {'\0'};
    char *ptr = NULL;
    fsp_err_t err = FSP_SUCCESS;

    printf(is_write ? WRITE_MENU : READ_MENU);

    while (true)
    {
        uint8_t option = get_user_input(MIN_NUMBER_OF_RX_FRAME_ID, MAX_NUMBER_OF_RX_FRAME_ID);
        switch (option)
        {
            case EXIT_INDEX:
            {
                return;
            }

            default:
            {
                if (is_write)
                {
                    err = lin_master_transmit(option - 1);
                    handle_error(err, "lin_master_transmit failed\r\n");

                    printf("\r\n\r\nMaster writes message with frame ID=0x%x successfully\r\n",
                            g_lin_master_tx_transfer_info[option - 1].id);
                }
                else
                {
                    err = lin_master_receive(option - 1);
                    handle_error(err, "lin_master_receive failed\r\n");

                    ptr = recv_str;

                    for (size_t i = 0; i < g_lin_master_rx_transfer_info[option - 1].num_bytes; i++)
                    {
                        ptr += sprintf(ptr, "0x%02x ", g_master_rx_buf[i]);
                    }

                    printf("\r\n\r\nMaster received data of ID=0x%x: %s",g_received_id, recv_str);
                    lin_validate_frame(g_lin_master_tx_header_transfer[option - 1].id, g_master_rx_buf,
                            g_lin_master_rx_transfer_info[option - 1].num_bytes);
                }
                break;
            }
        }
    }
}

/*******************************************************************************************************************//**
 * @brief     This function validates the received LIN frame.
 * @param[in] id                        Frame ID.
 * @param[in] p_data                    Pointer to received data.
 * @param[in] len                       Length of received data.
 * @retval    None.
 **********************************************************************************************************************/
static void lin_validate_frame(uint8_t id, const uint8_t * p_data, size_t len)
{
    for (size_t i = 0U; i < (sizeof(s_expected_frames) / sizeof(s_expected_frames[0])); i++)
    {
        const lin_frame_expectation_t * p_frame = &s_expected_frames[i];

        if (p_frame->id == id)
        {
            if (len != p_frame->length)
            {
                printf("\r\nFrame 0x%02X: Length mismatch. Expected %u bytes, received %u bytes\r\n",
                       id, (unsigned int)p_frame->length, (unsigned int)len);
                return;
            }

            if (0 == memcmp(p_data, p_frame->p_expected, len))
            {
                printf("Frame 0x%02X: Data validation successful\r\n", id);
            }
            else
            {
                printf("Frame 0x%02X: Data mismatch detected\r\n", id);
            }

            return;
        }
    }
}

/*******************************************************************************************************************//**
 * @brief     This function gets user input.
 * @param[in] min_value                 Minimum valid value.
 * @param[in] max_value                 Maximum valid value.
 * @retval    value                     User input value.
 **********************************************************************************************************************/
static uint8_t get_user_input(uint8_t min_value, uint8_t max_value)
{
    uint8_t value = RESET_VALUE;

    do
    {
        while (!uart_console_has_char())
        {
            /* Do nothing */
        }

        int32_t input_value = getchar();
        value = (uint8_t)(input_value - '0');

        if ((value < min_value) || (value > max_value))
        {
            printf("\r\nInvalid input. Please enter a value from %u to %u: ", min_value, max_value);
        }
    } while ((value < min_value) || (value > max_value));

    return value;
}

/*******************************************************************************************************************//**
 * @brief     This function handles an error by printing error message and code, de-initializing the related modules,
 *            and entering a trap state.
 * @param[in] err                       Error code.
 * @param[in] err_str                   Error message.
 * @retval    None.
 **********************************************************************************************************************/
static void handle_error(fsp_err_t err, const char * err_str)
{
    if (FSP_SUCCESS != err)
    {
        printf("\r\n[ERR] %s", err_str);
        printf("Returned Error Code: 0x%x\r\n", (unsigned int) err);
        uart_console_flush();
        lin_master_close();
#if ENABLE_MASTER_TIMEOUT_MANAGEMENT
        timer_close();
#endif
        __BKPT(0);
    }
}

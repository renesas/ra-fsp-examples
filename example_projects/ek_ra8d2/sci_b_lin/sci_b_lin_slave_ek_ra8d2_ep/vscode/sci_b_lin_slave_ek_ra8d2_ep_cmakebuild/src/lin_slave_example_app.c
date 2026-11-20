/***********************************************************************************************************************
 * File Name    : lin_slave_example_app.c
 * Description  : Example application layer for the LIN Slave EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - hal_entry.c calls a single function: lin_slave_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include <string.h>
#include "lin_slave_example_app.h"

/* Private variables */
static const uint8_t s_expected_data_id_20h[] = {0x01, 0x02, 0x03};
static const uint8_t s_expected_data_id_21h[] = {0x11, 0x12, 0x13};
static const uint8_t s_expected_data_id_22h[] = {0x21, 0x22, 0x23};
static const uint8_t s_expected_data_id_23h[] = {0x31, 0x32, 0x33};

static const lin_frame_expectation_t s_expected_frames[] =
{
    {READ_FRAME_ID_20H, s_expected_data_id_20h, sizeof(s_expected_data_id_20h)},
    {READ_FRAME_ID_21H, s_expected_data_id_21h, sizeof(s_expected_data_id_21h)},
    {READ_FRAME_ID_22H, s_expected_data_id_22h, sizeof(s_expected_data_id_22h)},
    {READ_FRAME_ID_23H, s_expected_data_id_23h, sizeof(s_expected_data_id_23h)},
};

/* Private functions */
static void lin_slave_configure_baudrate(void);
static void lin_slave_handle_frame(void);
static void lin_validate_frame(uint8_t id, const uint8_t * p_data, size_t len);
static uint8_t get_user_input(uint8_t min_value, uint8_t max_value);
static void handle_error(fsp_err_t err, const char * err_str);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the LIN Slave example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void lin_slave_example_app_run(void)
{
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version = {RESET_VALUE};

    uint32_t status = uart_console_init();
    if (FSP_SUCCESS != status)
    {
        return;
    }

    /* Get FSP version information */
    R_FSP_VersionGet(&version);

    /* Print example project information */
    printf(BANNER_INFO, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    printf(EP_INFO);

    /* Open LIN Slave interface */
    err = lin_slave_open();
    handle_error(err, "lin_slave_open failed.\r\n");

#if BSP_PERIPHERAL_SAU_PRESENT
    /* Open low-power mode module */
    err = lin_slave_lpm_open();
    handle_error(err, "lin_slave_lpm_open failed.\r\n");
#endif /* BSP_PERIPHERAL_SAU_PRESENT */

    printf(MAIN_MENU);

    while (true)
    {
        /* Handle user input */
        if (uart_console_has_char())
        {
            int32_t selection = getchar();

            switch (selection)
            {
                case SLAVE_SET_BAUDRATE:
                {
                    lin_slave_configure_baudrate();
                    break;
                }

#if BSP_PERIPHERAL_SAU_PRESENT
                case SLAVE_ENTER_LPM:
                {
                    printf("\r\n\r\nEntering software standby mode...\r\n");

                    err = lin_slave_sleep_enter();
                    handle_error(err, "lin_slave_sleep_enter failed.\r\n");

                    err = lin_slave_sleep_exit();
                    handle_error(err, "lin_slave_sleep_exit failed.\r\n");

                    printf("\r\nWake-up signal detected. Exiting sleep mode!!!\r\n");
                    break;
                }
#endif /* BSP_PERIPHERAL_SAU_PRESENT */

                default:
                {
                    printf("\r\nInvalid input. Provide a valid input.\r\n");
                    break;
                }
            }

            printf(MAIN_MENU);
        }

        /* Handle LIN frame after receiving a header */
        if (g_lin_event_flags & LIN_EVENT_RX_HEADER_COMPLETE)
        {
            g_lin_event_flags &= (uint32_t) (~LIN_EVENT_RX_HEADER_COMPLETE);

            lin_slave_handle_frame();
        }
    }
}

/*******************************************************************************************************************//**
 * @brief     This function configures the LIN communication baud rate.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void lin_slave_configure_baudrate(void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint32_t s_lin_baudrate_options[LIN_BAUDRATE_COUNT] = {2400, 4800, 9600, 10400, 14400, 19200};

    printf(BAUDRATE_OPTION);

    uint8_t option = get_user_input(MIN_BAUDRATE_INDEX, MAX_BAUDRATE_INDEX);
    g_slave_baudrate = s_lin_baudrate_options[option - 1U];

    printf("\r\n\r\nSelected baud rate: %lu bps", (unsigned long)g_slave_baudrate);
    err = lin_slave_baudset(g_slave_baudrate);
    handle_error(err, "lin_slave_baudset failed.\r\n");

    printf("\r\nLIN baud rate successfully updated!!!");
}

/*******************************************************************************************************************//**
 * @brief     This function handles the received LIN frame header.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void lin_slave_handle_frame(void)
{
    fsp_err_t err = FSP_SUCCESS;
    int32_t frame_id_index = RESET_VALUE;
    char data_str[32] = {NULL_CHAR};
    char * p_str = data_str;

    frame_id_index = lin_slave_find_transfer_index(g_received_id);

    if ((frame_id_index >= (int32_t)MIN_INDEX_OF_FRAME_ID_WRITE) &&
            (frame_id_index <= (int32_t)MAX_INDEX_OF_FRAME_ID_WRITE))
    {
        /* Receive frame data from the LIN Master */
        err = lin_slave_receive((uint8_t)frame_id_index);
        handle_error(err, "lin_slave_receive failed.\r\n");

        p_str = data_str;

        for (size_t i = 0U; i < g_lin_slave_transfer_info[frame_id_index].num_bytes; i++)
        {
            p_str += sprintf(p_str, "0x%02x ", g_slave_rx_buf[i]);
        }
        printf("\r\n\r\nSlave received data with ID=0x%x: %s\r\n", g_received_id, data_str);
        lin_validate_frame(g_received_id, g_slave_rx_buf, g_lin_slave_transfer_info[frame_id_index].num_bytes);
    }
    else if ((frame_id_index >= (int32_t) MIN_INDEX_OF_FRAME_ID_READ) &&
             (frame_id_index <= (int32_t) MAX_INDEX_OF_FRAME_ID_READ))
    {
        /* Transmit frame data to the LIN Master */
        err = lin_slave_transmit((uint8_t)frame_id_index);
        handle_error(err, "lin_slave_transmit failed.\r\n");

        p_str = data_str;
        for (size_t i = 0U; i < g_lin_slave_transfer_info[frame_id_index].num_bytes; i++)
        {
            p_str += sprintf(p_str, "0x%02x ", g_lin_slave_transfer_info[frame_id_index].p_information[i]);
        }
        printf("\r\n\r\nSlave transmitted data with ID=0x%x: %s\r\n", g_received_id, data_str);
    }
    else
    {
        /* Do nothing */
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
                printf("Frame 0x%02X: Length mismatch. Expected %u bytes, received %u bytes\r\n",
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
 * @brief     This function handles an error by printing error message and code, de-initializing
 *            the related modules, and entering a trap state.
 * @param[in] err                       Error code.
 * @param[in] err_str                   Error message.
 * @retval    None.
 **********************************************************************************************************************/
static void handle_error(fsp_err_t err, const char * err_str)
{
    if (FSP_SUCCESS != err)
    {
        printf("\r\n[ERR] %s", err_str);
        printf("Returned Error Code: 0x%x\r\n", (unsigned int)err);
        uart_console_flush();
        lin_slave_close();
#if BSP_PERIPHERAL_SAU_PRESENT
        lin_slave_lpm_close();
#endif /* BSP_PERIPHERAL_SAU_PRESENT */
        __BKPT(0);
    }
}

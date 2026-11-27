/***********************************************************************************************************************
 * File Name    : ssi_slave_example_app.c
 * Description  : Example application layer for the SSI Slave EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - hal_entry.c calls a single function: ssi_slave_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "ssi_slave_ep.h"
#include "ssi_slave_example_app.h"
#include "console/uart_console.h"

/* Command input buffer reused for mode selection and termination input */
static unsigned char g_rbyte[BUFFER_SIZE] = {RESET_VALUE};

/* Private functions */
static void master_write_slave_read_operation(void);
static void master_read_slave_write_operation(void);
static void user_input(char *buf, int len);
static void handle_error(fsp_err_t err, const char * err_str);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the SSI Slave example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void ssi_slave_example_app_run(void)
{
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version = {RESET_VALUE};

    uint32_t status = uart_console_init();
    if (FSP_SUCCESS != status)
    {
        return;     /* UART failed to open — cannot print anything */
    }

    /* Get FLEX pack version information */
    R_FSP_VersionGet(&version);

    /* Example project information printed on the terminal */
    printf(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    printf(EP_INFO);

    /* Open SSI channel */
    err = ssi_init();
    handle_error(err, "ssi_init failed\r\n");

    printf("\r\nOperating mode: SLAVE");

    while (true)
    {
        printf(EP_MENU);

        while (!uart_console_has_char())
        {
            /* Do nothing */
        }

        /* Clean buffer */
        memset(&g_rbyte[0U], NULL_CHAR, BUFFER_SIZE);
        user_input((char *) g_rbyte, (int) sizeof(g_rbyte));
        g_selection_value = (uint8_t) atoi((char *) g_rbyte);

        switch (g_selection_value)
        {
            case MASTER_WRITE_SLAVE_READ:
            {
                printf("\r\n\r\nMASTER WRITE - SLAVE READ mode selected\r\n");

                /* Perform Master write – Slave read operation */
                master_write_slave_read_operation();
                break;
            }

            case MASTER_READ_SLAVE_WRITE:
            {
                printf("\r\n\r\nMASTER READ - SLAVE WRITE mode selected\r\n");

                /* Perform Master read – Slave write operation */
                master_read_slave_write_operation();
                break;
            }

            default:
            {
                printf("\r\nInvalid input. Please enter valid input");
                break;
            }
        }
    }
}

/*******************************************************************************************************************//**
 * @brief     This function performs the Master write - Slave read operation by reading data from the Master.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void master_write_slave_read_operation(void)
{
    fsp_err_t err = FSP_SUCCESS;
    g_buf_row_index = 0U;
    g_read_counter = 0U;
    g_read_in_cb = false;

    /* Wait for incoming data */
    printf("Reading the data from the Master...\r\n");

    /* Clean the buffer before reading the data */
    memset(g_dest_buff, RESET_VALUE, sizeof(g_dest_buff));

    /* Read data from the Master */
    err = ssi_read(g_buf_row_index);
    handle_error(err, "ssi_read failed\r\n");

    while (READ_COUNTS > g_read_counter)
    {
        if (true == g_read_in_cb)
        {
            /* Reset the read flag */
            g_read_in_cb = false;

            /* Print read data */
            printf("\r\n");
            for (uint16_t index = 0U; index < BUF_FIFO_STAGE; index++)
            {
                printf("0x%02x, ", g_dest_buff[g_buf_row_index + 1U][index]);
            }

            /* Read data from the Master */
            if ((I2S_EVENT_IDLE == g_i2s_event) || (I2S_EVENT_RX_FULL == g_i2s_event))
            {
                err = ssi_read(g_buf_row_index);
                handle_error(err, "ssi_read failed\r\n");

                /* Increase read counter */
                g_read_counter++;

                /* Print read data */
                printf("\r\n");
                for (uint16_t index = 0U; index < BUF_FIFO_STAGE; index++)
                {
                    printf("0x%02x, ", g_dest_buff[g_buf_row_index][index]);
                }
            }
        }
    }

    printf("\r\nRead operation from the Master completed\r\n");
    printf("Press any key to terminate the process\r\n");

    while (!uart_console_has_char())
    {
        /* Do nothing */
    }

    /* Clean the user input to avoid an infinite loop */
    getchar();
}

/*******************************************************************************************************************//**
 * @brief     This function performs the Master read - Slave write operation by sending data to the Master.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void master_read_slave_write_operation(void)
{
    fsp_err_t err = FSP_SUCCESS;
    g_buf_row_index = 0U;
    g_write_counter = 0U;
    g_send_data_in_main_loop = false;

    printf("Writing the data to the Master...\r\n");

    /* Write data to the Master */
    err = ssi_write(g_buf_row_index);
    handle_error(err, "ssi_write failed\r\n");

    while (WRITE_COUNTS > g_write_counter)
    {
        if ((true == g_send_data_in_main_loop))
        {
            /* Write data to the Master */
            if ((I2S_EVENT_IDLE == g_i2s_event) || (I2S_EVENT_TX_EMPTY == g_i2s_event))
            {
                g_send_data_in_main_loop = false;
                err = ssi_write(g_buf_row_index);
                handle_error(err, "ssi_write failed\r\n");

                g_write_counter++;
            }
        }
        R_BSP_SoftwareDelay(WAIT_TIME, BSP_DELAY_UNITS_MICROSECONDS);
    }

    printf("Write operation to the Master completed\r\n");
    printf("Press any key to terminate the process\r\n");

    while (!uart_console_has_char())
    {
        /* Do nothing */
    }

    /* Clean the user input to avoid an infinite loop */
    getchar();
}

/*******************************************************************************************************************//**
 * @brief         This function reads a line of user input from stdin and removes trailing newline characters.
 * @param[in,out] buf                   Pointer to buffer where the input string is stored.
 * @param[in]     len                   Maximum number of characters to read (buffer size).
 * @retval        None.
 **********************************************************************************************************************/
static void user_input(char *buf, int len)
{
    fgets(buf, len, stdin);
    buf[strcspn(buf, "\r\n")] = '\0';
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
        printf("Returned Error Code: 0x%x\r\n", (unsigned int)err);
        uart_console_flush();
        ssi_deinit();
        __BKPT(0);
    }
}

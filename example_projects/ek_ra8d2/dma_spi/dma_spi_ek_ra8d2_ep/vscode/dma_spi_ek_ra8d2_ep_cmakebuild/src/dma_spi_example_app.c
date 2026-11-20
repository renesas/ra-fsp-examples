/***********************************************************************************************************************
 * File Name    : dma_spi_example_app.c
 * Description  : Example application layer for the DMA SPI EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - hal_entry.c calls a single function: dma_spi_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "console/uart_console.h"
#include "dma_spi_example_app.h"

/* Master transmit buffer */
static uint32_t g_master_tx_buff[BUFFER_SIZE] = {RESET_VALUE};

/* Master receive buffer */
static uint32_t g_master_rx_buff[BUFFER_SIZE] = {RESET_VALUE};

/* Slave transmit buffer */
static uint32_t g_slave_tx_buff[BUFFER_SIZE] = {RESET_VALUE};

/* Slave receive buffer */
static uint32_t g_slave_rx_buff[BUFFER_SIZE] = {RESET_VALUE};

/* Wait counter for wait operation monitoring */
static volatile uint32_t g_wait_count = MAX_COUNT;

/* Private functions */
static void spi_write_and_read_operation(void);
static void spi_write_read_operation(void);
static fsp_err_t spi_wait_event(void);
static void user_input(char *buf, int len);
static void handle_error(fsp_err_t err, const char * err_str);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the DMA SPI example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void dma_spi_example_app_run(void)
{
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version = {RESET_VALUE};
    char input_buff[BUFFER_SIZE] = {RESET_VALUE};
    uint8_t user_option = RESET_VALUE;

    /* Get FLEX pack version information */
    R_FSP_VersionGet(&version);

    uint32_t status = uart_console_init();
    if (FSP_SUCCESS != status)
    {
        return;     /* UART failed to open — cannot print anything */
    }

    /* Example project information printed on the terminal */
    printf(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    printf(EP_INFO);

    /* Initialize SPI driver */
    err = spi_init();
    handle_error(err, "spi_init failed\r\n");
    printf("\r\nSPI driver initialized successfully");

    /* Menu for user selection */
    printf(EP_MENU);

    while (EXIT != user_option)
    {
        if (uart_console_has_char())
        {
            /* Get the user option */
            memset(&input_buff[0], NULL_CHAR, BUFFER_SIZE);
            user_input(input_buff, (int)sizeof(input_buff));
            user_option = (uint8_t)atoi(input_buff);

            switch (user_option)
            {
                case WRITE_AND_READ:
                {
                    spi_write_and_read_operation();
                    break;
                }

                case WRITE_READ:
                {
                    spi_write_read_operation();
                    break;
                }

                case EXIT:
                {
                    spi_deinit();
                    break;
                }

                default:
                {
                    printf(EP_MENU);
                    break;
                }
            }

            if ((WRITE_AND_READ <= user_option) && (EXIT > user_option))
            {
                printf("\r\nEnter any key (other than 1,2,3) to go back to the main menu\r\n");
            }
        }
    }
    printf("\r\n\r\nExiting SPI demo...");
    printf("\r\nRestart the MCU to run the application again\r\n");
}

/*******************************************************************************************************************//**
 * @brief     This function performs both SPI Write and Read operations individually.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void spi_write_and_read_operation(void)
{
    fsp_err_t err = FSP_SUCCESS;            /* Error status */
    uint32_t num_bytes = RESET_VALUE;       /* Number of bytes read from the terminal */
    uint32_t spi_length = RESET_VALUE;      /* Master SPI length */
    uint8_t padding_needed = RESET_VALUE;   /* Padding needed for 4-byte alignment */

    /* Clean buffers */
    memset(&g_master_tx_buff[0], NULL_CHAR, (BUFFER_SIZE * BITS_TO_BYTES));
    memset(&g_master_rx_buff[0], NULL_CHAR, (BUFFER_SIZE * BITS_TO_BYTES));
    memset(&g_slave_rx_buff[0], NULL_CHAR, (BUFFER_SIZE * BITS_TO_BYTES));

    /* Input to Master buffer */
    printf("\r\n\r\nEnter text input for Master buffer. Data size should be less than 64 bytes."
           "\r\nUser input: ");
    while (0 == num_bytes)
    {
        if (uart_console_has_char())
        {
            user_input((char *)g_master_tx_buff, sizeof(g_master_tx_buff));
            num_bytes = strlen((char *)g_master_tx_buff);

            if (0 == num_bytes)
            {
                printf("\r\nNo input\r\n");
            }
        }
    }

    /* Align the user input length to a 4-byte boundary and calculate
     * the number of 4-byte words required for the SPI transfer. */
    if (num_bytes % BITS_TO_BYTES != RESET_VALUE)
    {
        /* Calculate padding needed for 4-byte alignment */
        padding_needed = BITS_TO_BYTES - (num_bytes % BITS_TO_BYTES);
        num_bytes += padding_needed;
    }

    spi_length = num_bytes / BITS_TO_BYTES;

    /* Slave receives data from Master */
    err = spi_read(&g_spi_slave_ctrl, g_slave_rx_buff, spi_length, SPI_BIT_WIDTH_32_BITS);
    handle_error(err, "spi_read from Master failed\r\n");

    /* Master sends data to Slave */
    err = spi_write(&g_spi_master_ctrl, g_master_tx_buff, spi_length, SPI_BIT_WIDTH_32_BITS);
    handle_error(err, "spi_write to Slave failed\r\n");

    /* Wait until Master sends data and Slave receives it completely */
    err = spi_wait_event();
    handle_error(err, ERR_INFO);

    /* Display Master to Slave transmission */
    printf("\r\n\r\nMaster transmitted user input data to Slave\r\n");

    /* Slave sends data to Master */
    err = spi_write(&g_spi_slave_ctrl, g_slave_rx_buff, spi_length, SPI_BIT_WIDTH_32_BITS);
    handle_error(err, "spi_write to Master failed\r\n");

    /* Master receives data from Slave */
    err = spi_read(&g_spi_master_ctrl, g_master_rx_buff, spi_length, SPI_BIT_WIDTH_32_BITS);
    handle_error(err, "spi_read from Slave failed\r\n");

    /* Wait until Slave sends data and Master receives it completely */
    err = spi_wait_event();
    handle_error(err, ERR_INFO);

    /* Display Slave to Master transmission */
    printf("\r\nSlave transmitted the data back to Master\r\n");

    /* Check whether transmitted data is equal to received data */
    if (BUFF_EQUAL == memcmp(g_master_tx_buff, g_master_rx_buff, num_bytes))
    {
        /* Display Master's received data on the terminal */
        printf("\r\nMaster received data: %s\r\n", (char *) g_master_rx_buff);
    }
    else
    {
        /* Return a parity error in case of a data mismatch */
        printf("\r\nReceived data does not match with transmitted data\r\n");
        handle_error(FSP_ERR_SPI_PARITY, ERR_INFO);
    }

    printf("\r\nWrite and Read operations are successful\r\n");
}

/*******************************************************************************************************************//**
 * @brief     This function performs WriteRead operation for both Master and Slave simultaneously.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void spi_write_read_operation(void)
{
    fsp_err_t err = FSP_SUCCESS;                /* Error status */
    uint32_t num_bytes_master = RESET_VALUE;    /* Number of bytes read from the terminal for Master buffer */
    uint32_t num_bytes_slave = RESET_VALUE;     /* Number of bytes read from the terminal for Slave buffer */
    uint32_t spi_length_master = RESET_VALUE;   /* Master SPI length */
    uint8_t padding_needed = RESET_VALUE;       /* Padding needed for 4-byte alignment */

    /* Clean buffers */
    memset(&g_master_tx_buff[0], NULL_CHAR, (BUFFER_SIZE * BITS_TO_BYTES));
    memset(&g_master_rx_buff[0], NULL_CHAR, (BUFFER_SIZE * BITS_TO_BYTES));
    memset(&g_slave_tx_buff[0], NULL_CHAR, (BUFFER_SIZE * BITS_TO_BYTES));
    memset(&g_slave_rx_buff[0], NULL_CHAR, (BUFFER_SIZE * BITS_TO_BYTES));

    /* Input to Master buffer */
    printf("\r\n\r\nEnter text input for Master buffer. Data size should be less than 64 bytes."
           "\r\nUser input: ");
    while (0 == num_bytes_master)
    {
        if (uart_console_has_char())
        {
            user_input((char *)g_master_tx_buff, sizeof(g_master_tx_buff));
            num_bytes_master = strlen((char *)g_master_tx_buff);

            if (0 == num_bytes_master)
            {
                printf("\r\nNo input\r\n");
            }
        }
    }

    /* Align the user input length to a 4-byte boundary and calculate
     * the number of 4-byte words required for the SPI transfer. */
    if (num_bytes_master % BITS_TO_BYTES != RESET_VALUE)
    {
        /* Calculate padding needed for 4-byte alignment */
        padding_needed = BITS_TO_BYTES - (num_bytes_master % BITS_TO_BYTES);
        num_bytes_master += padding_needed;
    }

    spi_length_master = num_bytes_master / BITS_TO_BYTES;

    /* Input to Slave buffer */
    printf("\r\n\r\nEnter text input for Slave buffer. Data size should be less than 64 bytes."
           "\r\nUser input: ");
    while (0 == num_bytes_slave)
    {
        if (uart_console_has_char())
        {
            user_input((char *)g_slave_tx_buff, sizeof(g_slave_tx_buff));
            num_bytes_slave = strlen((char *)g_slave_tx_buff);

            if (0 == num_bytes_slave)
            {
                printf("\r\nNo input\r\n");
            }
        }
    }

    /* Slave sends data to Master and receives data from Master */
    err = spi_write_read(&g_spi_slave_ctrl, g_slave_tx_buff, g_slave_rx_buff, spi_length_master,
                         SPI_BIT_WIDTH_32_BITS);
    handle_error(err, "spi_write_read for Slave failed\r\n");

    /* Master sends data to Slave and receives data from Slave */
    err = spi_write_read(&g_spi_master_ctrl, g_master_tx_buff, g_master_rx_buff, spi_length_master,
                         SPI_BIT_WIDTH_32_BITS);
    handle_error(err, "spi_write_read for Master failed\r\n");

    /* Wait until the Master and Slave WriteRead operations are complete */
    err = spi_wait_event();
    handle_error(err, ERR_INFO);

    /* Display Master to Slave and Slave to Master transmissions */
    printf("\r\n\r\nMaster buffer data transmitted to Slave\r\n");
    printf("\r\nSlave buffer data transmitted to Master\r\n");

    /* Check whether Slave transmitted data is equal to Master received data */
    if (BUFF_EQUAL == memcmp(g_slave_tx_buff, g_master_rx_buff, num_bytes_master))
    {
        /* Display Master's received data on the terminal */
        printf("\r\nMaster received data: %s\r\n", (char *) g_master_rx_buff);
    }
    else
    {
        /* Return a parity error in case of a data mismatch */
        printf("\r\nMaster received data does not match with Slave transmitted Data\r\n");
        handle_error(FSP_ERR_SPI_PARITY, ERR_INFO);
    }

    /* Check whether Master transmitted data is equal to Slave received data */
    if (BUFF_EQUAL == memcmp(g_master_tx_buff, g_slave_rx_buff, num_bytes_master))
    {
        /* Display Slave's received data on the terminal */
        printf("\r\nSlave received data: %s\r\n", (char *) g_slave_rx_buff);
    }
    else
    {
        /* Return a parity error in case of a data mismatch */
        printf("\r\nSlave received data does not match with Master transmitted data\r\n");
        handle_error(FSP_ERR_SPI_PARITY, ERR_INFO);
    }

    printf("\r\nWriteRead operation is successful\r\n");
}

/*******************************************************************************************************************//**
 * @brief     This function waits for SPI transfer completion events from both Master and Slave.
 * @param[in] None.
 * @retval    FSP_SUCCESS               SPI transfer completed successfully.
 * @retval    FSP_ERR_TIMEOUT           Timeout occurred while waiting for SPI transfer completion.
 * @retval    FSP_ERR_ABORTED           SPI transfer was aborted.
 **********************************************************************************************************************/
static fsp_err_t spi_wait_event(void)
{
    fsp_err_t err = FSP_SUCCESS;

    while ((SPI_EVENT_TRANSFER_COMPLETE != g_master_event_flag) ||
           (SPI_EVENT_TRANSFER_COMPLETE != g_slave_event_flag))
    {
        g_wait_count--;
        if (MIN_COUNT >= g_wait_count)
        {
            /* Return a timeout error if SPI operation fails to complete */
            printf("\r\nSPI transfer timeout in Write/Read operation\r\n");
            err = FSP_ERR_TIMEOUT;
            break;
        }
        else if ((SPI_EVENT_TRANSFER_ABORTED == g_master_event_flag) ||
                 (SPI_EVENT_TRANSFER_ABORTED == g_slave_event_flag))
        {
            printf("\r\nSPI transfer aborted in Write/Read operation\r\n");
            err = FSP_ERR_ABORTED;
            break;
        }
        else
        {
            /* Do nothing */
        }
    }

    g_wait_count = MAX_COUNT;
    g_master_event_flag = (spi_event_t) RESET_VALUE;    /* Reset Master event flag */
    g_slave_event_flag = (spi_event_t) RESET_VALUE;     /* Reset Slave event flag */

    return err;
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
        spi_deinit();
        __BKPT(0);
    }
}

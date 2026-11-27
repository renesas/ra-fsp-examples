/***********************************************************************************************************************
 * File Name    : crc_example_app.c
 * Description  : Example application layer for the CRC EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - hal_entry.c calls a single function: crc_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include <string.h>
#include "crc_ep.h"
#include "crc_example_app.h"
#include "console/uart_console.h"

/* Private functions */
static void crc_operation(void);
static void uart_lookback(uint8_t const * const p_src, uint8_t * const p_dest, uint32_t const bytes);
static void toggle_led(void);
static void set_led(bsp_io_level_t b_value);
static void handle_error(fsp_err_t err, const char * err_str);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the CRC example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void crc_example_app_run(void)
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
    printf(BANNER_INFO, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    printf(EP_INFO);

    /* Open UART module */
    err = uart_init();
    handle_error(err, "uart_init failed\r\n");

    /* Open CRC module */
    err = crc_init();
    handle_error(err, "crc_init failed\r\n");

    printf(EP_GUIDE);

    while (true)
    {
        if (uart_console_has_char())
        {
            getchar();

            /* Before beginning the operation turn OFF LED */
            set_led(LED_OFF);

            /* Perform CRC operation */
            printf("\r\nStart CRC operation");
            crc_operation();
            printf(EP_GUIDE);
        }
    }
}

/*******************************************************************************************************************//**
 * @brief     This function processes CRC operations.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void crc_operation(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* CRC inputs structure */
    crc_input_t input_data;
    uint32_t normal_crc_value = RESET_VALUE;
    uint8_t input_buffer[BUF_LEN] = {0x05, 0x02, 0x03, 0x04};
    uint8_t dest_buffer[BUF_LEN] = {RESET_VALUE};
    uint8_t uart_data_len = RESET_VALUE;

    /* Update seed value and transfer uart_data_len as per the polynomial used */
    if (CRC_POLYNOMIAL_CRC_8 == g_crc_cfg.polynomial)
    {
        uart_data_len = EIGHT_BIT_DATA_LEN;   /* Data length for 8-bit polynomial operation */
    }
    else if ((CRC_POLYNOMIAL_CRC_16 == g_crc_cfg.polynomial) || (CRC_POLYNOMIAL_CRC_CCITT == g_crc_cfg.polynomial))
    {
        uart_data_len = SIXTEEN_BIT_DATA_LEN; /* Data length for 16-bit polynomial operation */
    }
    else
    {
        /* Display unsupported CRC polynomial message */
        handle_error(FSP_ERR_UNSUPPORTED, "The CRC module unsupported the selected CRC Polynomial");
    }

    /* Update CRC input structure for normal mode */
    input_data.num_bytes = NUM_BYTES;
    input_data.crc_seed = SEED_VALUE;
    input_data.p_input_buffer = &input_buffer;

    /* Calculate CRC value for input data in normal mode */
    err = crc_calculate(&input_data, &normal_crc_value);
    handle_error(err, "crc_calculate failed\r\n");

    /* Append calculated CRC value from normal mode to input buffer */
    if (CRC_POLYNOMIAL_CRC_8 == g_crc_cfg.polynomial)
    {
        /* Append 8-bit CRC value to input data */
        input_buffer[4] = (uint8_t) normal_crc_value;
    }
    else if ((CRC_POLYNOMIAL_CRC_16 == g_crc_cfg.polynomial) || (CRC_POLYNOMIAL_CRC_CCITT == g_crc_cfg.polynomial))
    {
        /* Extract the bytes from 16-bit CRC value and append to input buffer as per the selected byte order
         * in CRC configuration */
        if (CRC_BIT_ORDER_LMS_LSB == g_crc_cfg.bit_order)
        {
            input_buffer[4] = (uint8_t) (normal_crc_value & 0xFF);        /* Extract first byte */
            input_buffer[5] = (uint8_t) ((normal_crc_value >> 8) & 0xFF); /* Extract second byte */
        }
        else
        {
            input_buffer[5] = (uint8_t) (normal_crc_value & 0xFF);        /* Extract first byte */
            input_buffer[4] = (uint8_t) ((normal_crc_value >> 8) & 0xFF); /* Extract second byte */
        }
    }
    else
    {
        /* Do nothing */
    }

#if (BSP_FEATURE_CRC_HAS_SNOOP)
    uint32_t snoop_crc_value = RESET_VALUE;

    /* Enable snoop mode */
    err = crc_snoop_enable();
    handle_error(err, "crc_snoop_enable failed\r\n");

    /* Perform UART loopback */
    uart_lookback(input_buffer, dest_buffer, uart_data_len);

    /* Get CRC value in snoop mode for receive data */
    err = crc_calculated_value(&snoop_crc_value);
    handle_error(err, "crc_calculated_value failed\r\n");

    /* Disable snoop mode */
    err = crc_snoop_disable();
    handle_error(err, "crc_snoop_disable failed\r\n");

    /* Validate the CRC results */
    if (RESET_VALUE == snoop_crc_value)
    {
        printf("\r\nCRC operation is successful\r\n");

        /* Compare UART write and read data buffers */
        if (RESET_VALUE == memcmp(dest_buffer, input_buffer, uart_data_len))
        {
            printf("UART transmitted and received data successfully\r\n");

            /* Toggle LED as sign of successful operation */
            toggle_led();
        }
     }
     else
     {
         handle_error(FSP_ERR_ABORTED, "UART loopback transmission error\r\n");
     }
#else
    crc_input_t rx_input_data;
    uint32_t rx_normal_crc_value = RESET_VALUE;

    /* Perform UART loopback */
    uart_lookback(input_buffer, dest_buffer, uart_data_len);

    /* Update CRC input structure for normal mode */
    rx_input_data.num_bytes = NUM_BYTES;
    rx_input_data.crc_seed = SEED_VALUE;
    rx_input_data.p_input_buffer = &dest_buffer;

    /* Calculate CRC value for input data in normal mode */
    err = crc_calculate(&rx_input_data, &rx_normal_crc_value);
    handle_error(err, "crc_calculate failed\r\n");

    /* Compare CRC for TX buffer and RX buffer */
    if (normal_crc_value == rx_normal_crc_value)
    {
        printf("\r\nCRC operation is successful\r\n");

        /* Compare UART write and read data buffer */
        if (RESET_VALUE == memcmp(dest_buffer, input_buffer, uart_data_len))
        {
            printf("UART transmitted and received data successfully\r\n");

            /* Toggle LED as sign of successful operation */
            toggle_led();
         }
     }
     else
     {
         handle_error(FSP_ERR_ABORTED, "UART loopback transmission error\r\n");
     }
#endif /* BSP_FEATURE_CRC_HAS_SNOOP */
}

/*******************************************************************************************************************//**
 * @brief      This function processes UART loopback operations.
 * @param[in]  p_src                    Pointer to the source buffer.
 * @param[in]  bytes                    The number of bytes to transfer.
 * @param[out] p_dest                   Pointer to the destination buffer.
 * @retval     None.
 **********************************************************************************************************************/
static void uart_lookback(uint8_t const * const p_src, uint8_t * const p_dest, uint32_t const bytes)
{
    fsp_err_t err = FSP_SUCCESS;
    uint32_t uart_time_out = TIME_OUT;

    /* Clear callback event flag */
    g_b_uart_rx_flag = false;
    g_b_uart_tx_flag = false;

    /* Perform UART write operation */
    err = uart_write(p_src, bytes);
    handle_error(err, "uart_write failed\r\n");

    /* Perform UART read operation */
    err = uart_read(p_dest, bytes);
    handle_error(err, "uart_read failed\r\n");

    /* Wait for TX and RX complete events */
    while ((true != g_b_uart_tx_flag) || (true != g_b_uart_rx_flag))
    {
        /* Start checking for timeout to avoid infinite loop */
        --uart_time_out;

        /* Check for time elapse */
        if (RESET_VALUE == uart_time_out)
        {
            handle_error(FSP_ERR_TIMEOUT, "UART TX and RX events not received\r\n");
        }
    }
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
        set_led(LED_ON);
        crc_deinit();
        uart_deinit();
        __BKPT(0);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function toggles on-board LED, which is connected and supported by the BSP.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void toggle_led(void)
{
    set_led(LED_ON);
    R_BSP_SoftwareDelay(TOGGLE_DELAY, BSP_DELAY_UNITS_MILLISECONDS);
    set_led(LED_OFF);
}

/*******************************************************************************************************************//**
 * @brief     This function turns the on-board LED ON or OFF.
 * @param[in] b_value                   LED state.
 * @retval    None.
 **********************************************************************************************************************/
static void set_led(bsp_io_level_t b_value)
{
    R_IOPORT_PinWrite(g_ioport.p_ctrl, (bsp_io_port_pin_t)g_bsp_leds.p_leds[RESET_VALUE], b_value);
}

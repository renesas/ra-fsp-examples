/***********************************************************************************************************************
 * File Name    : dac_example_app.c
 * Description  : Example application layer for the DAC EP.
 *                - All printing via printf (routed to UART via uart_console + syscalls.c).
 *                - hal_entry.c calls a single function: dac_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "dac_ep.h"
#include "dac_example_app.h"
#include "console/uart_console.h"

/* Variable to store the user input for DAC conversion */
static int32_t g_input_read = RESET_VALUE;

/* Private function declarations */
static void user_input(char *buf, int len);
static fsp_err_t read_input_from_terminal(void);
static void handle_error(fsp_err_t err, const char *err_str);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the DAC example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void dac_example_app_run(void)
{
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version = {RESET_VALUE};

    /* Initialize UART console before any printing */
    uint32_t status = uart_console_init();
    if (FSP_SUCCESS != status)
    {
        return;     /* UART failed to open — cannot print anything */
    }

    /* Get FLEX pack version information */
    R_FSP_VersionGet(&version);

    /* Example project information printed on the terminal */
    printf(BANNER, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    printf(EP_INFO);

    /* Open DAC module */
    err = dac_open();
    handle_error(err, "dac_open failed\r\n");

    /* Open ADC module */
    err = adc_open();
    handle_error(err, "adc_open failed\r\n");

    /* Configure ADC scan parameters */
    err = adc_scan_cfg();
    handle_error(err, "adc_scan_cfg failed\r\n");

#if defined(BOARD_RA2A1_EK) || (BSP_PERIPHERAL_ADC_B_PRESENT)
    /* Calibrate the ADC */
    printf("\r\nADC calibration started\r\n");
    err = adc_calibrate();
    handle_error(err, "adc_calibrate failed\r\n");
    printf("\r\nADC calibration successful\r\n");
#endif /* BOARD_RA2A1_EK || BSP_PERIPHERAL_ADC_B_PRESENT */

    printf("\r\nEnter values for DAC conversion. Valid range: 1 to 4095");
    printf("\r\n\r\nUser input: ");

    while (true)
    {
        /* Process input when the user has submitted a line */
        if (uart_console_has_char())
        {
            err = read_input_from_terminal();
            if (FSP_SUCCESS != err)
            {
                printf("\r\nInvalid input. Valid range: 1 to 4095. Please enter valid input.");
                printf("\r\nUser input: ");
            }
            else
            {
                err = dac_adc_operations(&g_input_read);
                handle_error(err, "dac_adc_operations failed\r\n");
            }
        }

        /* Print ADC result when scan is complete */
        if (true == g_is_read_complete)
        {
            g_is_read_complete = false;
            printf("\r\nData read from ADC channel: %d\r\n", g_adc_data);
            printf("\r\nUser input: ");

            /* On RA4W1/RA6T1_RSSK, ADC was re-opened inside dac_adc_operations — close it after read */
#if defined(BOARD_RA4W1_EK) || defined(BOARD_RA6T1_RSSK)
            adc_deinit();
#endif /* BOARD_RA4W1_EK || BOARD_RA6T1_RSSK */
        }
        else if (true == g_error_flag)
        {
            g_error_flag = false;
            handle_error(err, "adc_read failed\r\n");
        }
        else
        {
            /* Do nothing */
        }
    }
}

/*******************************************************************************************************************//**
 * @brief     This function reads the user input from the terminal and processes it.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Read successfully.
 * @retval    Any other error codes     Failed to read.
 **********************************************************************************************************************/
static fsp_err_t read_input_from_terminal(void)
{
    fsp_err_t err = FSP_SUCCESS;
    char readBuff[BUFFER_SIZE] = {RESET_VALUE};

    /* Read input line from the terminal */
    user_input(readBuff, sizeof(readBuff));

    /* Convert the input string to an integer value */
    g_input_read = (int32_t)atoi(readBuff);

    if ((DAC_MIN_VAL <= g_input_read) && (DAC_MAX_VAL >= g_input_read))
    {
        printf("\r\nValue for DAC conversion: %d", g_input_read);
    }
    else
    {
        err = FSP_ERR_INVALID_ARGUMENT;
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief         This function reads a line of user input from stdin and remove trailing newline characters.
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
static void handle_error(fsp_err_t err, const char *err_str)
{
    if (FSP_SUCCESS != err)
    {
        printf("\r\n[ERR]: %s", err_str);
        printf("Returned Error Code: 0x%x\r\n", (unsigned int)err);
        uart_console_flush();

        /* De-initialize all opened modules */
        adc_deinit();
        dac_deinit();

        __BKPT(0);
    }
}

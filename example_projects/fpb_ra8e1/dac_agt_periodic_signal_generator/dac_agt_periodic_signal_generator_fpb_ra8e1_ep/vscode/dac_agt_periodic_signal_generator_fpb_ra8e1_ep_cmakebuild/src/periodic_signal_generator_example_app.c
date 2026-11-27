/***********************************************************************************************************************
 * File Name    : periodic_signal_generator_example_app.c
 * Description  : Example application layer for the DAC AGT Periodic Signal Generator EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - hal_entry.c calls a single function: dac_agt_periodic_signal_generator_ep_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "periodic_signal_generator_example_app.h"
#include "console/uart_console.h"

/* Private functions */
static void handle_error(fsp_err_t err, const char *err_str);
static void user_input(char *buf, int len);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the DAC AGT Periodic Signal Generator example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void dac_agt_periodic_signal_generator_ep_run(void)
{
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version = {RESET_VALUE};

    /* Initialize UART console before any printing */
    uint32_t status = uart_console_init();
    if (FSP_SUCCESS != status)
    {
        return;
    }

    /* Get FLEX pack version information */
    R_FSP_VersionGet(&version);

    /* Example project information printed on the terminal */
    printf(BANNER_INFO, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    printf(EP_INFO);

    /* Initialize DAC, DTC, and AGT modules */
    err = dac_init();
    handle_error(err, "dac_init failed\r\n");

    err = dtc_init();
    handle_error(err, "dtc_init failed\r\n");

    err = agt_init();
    handle_error(err, "agt_init failed\r\n");

    while (true)
    {
        /* Prompt the user to input the desired number of samples per period and validate the input range */
        printf(EP_GUIDE1, SPP_MIN_VAL, SPP_MAX_VAL);

        char input_buf[BUFFER_SIZE] = {RESET_VALUE};
        uint16_t input_spp = RESET_VALUE;
        bool b_valid = false;
        uint32_t freq_min_hz = RESET_VALUE;
        uint32_t freq_max_hz = RESET_VALUE;
        uint32_t input_freq = RESET_VALUE;

        while (!b_valid)
        {
            if (uart_console_has_char())
            {
                user_input(input_buf, (int)sizeof(input_buf));
                input_spp = (uint16_t)atoi(input_buf);
                if ((input_spp >= SPP_MIN_VAL) && (input_spp <= SPP_MAX_VAL))
                {
                    b_valid = true;
                }
                else
                {
                    printf("\r\nValue out of bounds.");
                    printf(EP_GUIDE1, SPP_MIN_VAL, SPP_MAX_VAL);
                }
            }
        }
        printf("\r\n%d samples per period selected", input_spp);

        /* Get frequency limits for the selected samples per period */
        freq_min_hz = RESET_VALUE;
        freq_max_hz = RESET_VALUE;
        dac_freq_limits_get(input_spp, &freq_min_hz, &freq_max_hz);

        /* Prompt the user to enter the desired output frequency and validate it against the allowed range */
        printf(EP_GUIDE2, freq_min_hz, freq_max_hz);

        input_freq = RESET_VALUE;
        b_valid = false;

        while (!b_valid)
        {
            if (uart_console_has_char())
            {
                user_input(input_buf, (int)sizeof(input_buf));
                input_freq = (uint32_t)atoi(input_buf);
                if ((input_freq >= freq_min_hz) && (input_freq <= freq_max_hz))
                {
                    b_valid = true;
                }
                else
                {
                    printf("\r\nValue out of bounds.");
                    printf(EP_GUIDE2, freq_min_hz, freq_max_hz);
                }
            }
        }
        printf("\r\n%u Hz selected", input_freq);

        /* Generate LUT, reconfigure DTC, and set AGT period */
        err = dac_output_set(input_spp, input_freq);
        handle_error(err, "dac_output_set failed\r\n");

        /* Print feedback to indicate the wave is being generated */
        printf("\r\nOutputting %u Hz wave with %d samples per period\r\n", input_freq, input_spp);
    }
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
        printf("\r\n[ERR] %s", err_str);
        printf("Returned Error Code: 0x%x\r\n", (unsigned int)err);
        uart_console_flush();
        /* De-initialize the opened modules */
        dac_deinit();
        agt_deinit();
        dtc_deinit();

        /* Trap here */
        __BKPT(0);
    }
}

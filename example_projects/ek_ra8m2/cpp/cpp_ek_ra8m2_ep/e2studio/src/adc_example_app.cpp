/***********************************************************************************************************************
 * File Name    : adc_example_app.cpp
 * Description  : Example application layer for the ADC C++ EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - hal_entry.cpp calls a single function: adc_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "adc_example_app.h"
#include "console/uart_console.h"

/* Private functions */
static fsp_err_t process_command(adc_ep &adc, int32_t cmd);
static void handle_error(fsp_err_t err, const char * err_str);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the ADC C++ example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void adc_example_app_run(void)
{
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version = {RESET_VALUE};
    uint16_t adc_data = RESET_VALUE;

    /* ADC object */
    adc_ep adc;

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
    printf(EP_MENU);

    while (true)
    {
        if (uart_console_has_char())
        {
            int32_t selection = getchar();
            err = process_command(adc, selection);
            handle_error(err, "process_input failed\r\n");
        }

        /* Read the ADC output data and status */
        if (g_ready_to_read)
        {
            adc_data = RESET_VALUE;

            err = adc.adc_read_data(&adc_data);
            handle_error(err, "adc_read_data failed\r\n");

            printf("\r\nMCU die temperature reading from ADC: %d\r\n", adc_data);

            /* The scan auto-stops after one reading in Single Scan mode (One-shot mode for ADC_D) */
            if (!g_ready_to_read)
            {
                printf("\r\nPress any other key (except 1 and 2) to go back to the main menu\r\n");
            }

#ifdef BOARD_RA2A1_EK
            if (adc.recalibration_was_triggered())
            {
                printf("\r\nInitiating calibration of ADC due to deviation in output continuously"\
                       "\r\nbeyond set limits\r\n");
            }
#endif /* BOARD_RA2A1_EK */

            /* Wait 1 second between successive readings */
            R_BSP_SoftwareDelay(ADC_READ_DELAY, BSP_DELAY_UNITS_SECONDS);
        }
    }
}

/*******************************************************************************************************************//**
 * @brief     This function processes the command input read from the terminal.
 * @param[in] adc                       Reference to the ADC object.
 * @param[in] cmd                       Command character read from the terminal.
 * @retval    FSP_SUCCESS               Performed operation successfully.
 * @retval    Any other error codes     Failed to perform operation.
 **********************************************************************************************************************/
static fsp_err_t process_command(adc_ep &adc, int32_t cmd)
{
    fsp_err_t err = FSP_SUCCESS;

    switch (cmd)
    {
        case SCAN_START:
        {
            if (g_ready_to_read)
            {
                printf("\r\nADC scan already in progress\r\n");
                break;
            }

            err = adc.adc_scan_start();
            if (FSP_SUCCESS != err)
            {
                return err;
            }
            printf("\r\nADC scan started\r\n");
            break;
        }

        case SCAN_STOP:
        {
            if ((!ADC_IS_SINGLE_SCAN()) && g_ready_to_read)
            {
                err = adc.adc_scan_stop();
                if (FSP_SUCCESS != err)
                {
                    return err;
                }
                printf("\r\nADC scan stopped\r\n");
            }
            else
            {
                printf("\r\nStop command is not supported in " MODE_1 ", or the user has not pressed"
                       "\r\nStart Scan in " MODE_2 "\r\n");
            }
            printf("\r\nPress any other key (except 1 and 2) to go back to the main menu\r\n");
            break;
        }

        default:
        {
            printf(EP_MENU);
            break;
        }
    }

    return err;
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
        printf("\r\n[ERR]: %s", err_str);
        printf("Returned Error Code: 0x%x\r\n", (unsigned int)err);
        uart_console_flush();
        adc_ep::deinit_adc_module();
        __BKPT(0);
    }
}

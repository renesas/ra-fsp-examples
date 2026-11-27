/***********************************************************************************************************************
 * File Name    : adc_example_app.c
 * Description  : Example application layer for the ADC EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - hal_entry.c calls a single function: adc_example_app_run().
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
#include "adc_example_app.h"
#include "console/uart_console.h"

/* Private functions */
static void print_adc_result(void);
static fsp_err_t adc_operation(int32_t cmd);
static void handle_error(fsp_err_t err, const char * err_str);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the ADC example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void adc_example_app_run(void)
{
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version = {RESET_VALUE};

    uint32_t status = uart_console_init();
    if (FSP_SUCCESS != status)
    {
        return;     /* UART failed to open — cannot print anything */
    }

    /* Get FLEX version information */
    R_FSP_VersionGet(&version);

    /* Print EP information */
    printf(BANNER_INFO, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    printf(EP_INFO);
    printf(EP_MENU);
    
    while (true)
    {
        if (uart_console_has_char())
        {
            int32_t selection = getchar();
            err = adc_operation(selection);
            handle_error(err, "process_input failed\r\n");
        }

        /* Read and display ADC data while scan is active */
        if (g_ready_to_read)
        {
            err = adc_read_data();
            if (FSP_SUCCESS == err)
            {
                print_adc_result();
            }
            else
            {
                handle_error(err, "adc_read_data failed\r\n");
            }
        }
    }
}

/*******************************************************************************************************************//**
 * @brief     This function prints raw ADC result.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void print_adc_result(void)
{
#ifdef BOARD_RA6T2_MCK
    printf("\r\nMCU Temperature Reading from ADC: %d\r\n", g_adc_data);
#else
    snprintf((char *)g_volt_str, sizeof(g_volt_str), "%0.2f", g_adc_volt);
    printf("\r\nThe Voltage Reading from ADC: %d\r\n", g_adc_data);
    printf("\r\nThe ADC input voltage: %s V\r\n", (char *)g_volt_str);

    /* Window compare */
#if !(BSP_PERIPHERAL_ADC_D_PRESENT) && !(BSP_PERIPHERAL_ADC_B_PRESENT)
    if ((g_window_comp_event && ADC_IS_SINGLE_SCAN()) || (ADC_MODE_CONTINUOUS_SCAN == g_adc_cfg.mode))
    {
        if (ADC_L_LMT > g_adc_data)
        {
            printf("\r\nADC Voltage is below the Lower Limit.\r\n");
            printf(EP_MENU);
        }
        else if (ADC_H_LMT < g_adc_data)
        {
            printf("\r\nADC Voltage is above the Upper Limit.\r\n");
            printf(EP_MENU);
        }
        else
        {
            ;/* Do nothing */
        }

    }
    if ((!g_window_comp_event) && (ADC_IS_SINGLE_SCAN()))
    {
        printf(EP_MENU);
    }
#else
    if (ADC_IS_SINGLE_SCAN())
    {
        printf(EP_MENU);
    }
#endif /* !BSP_PERIPHERAL_ADC_D_PRESENT || !BSP_PERIPHERAL_ADC_B_PRESENT */
#endif /* BOARD_RA6T2_MCK */
}

/*******************************************************************************************************************//**
 * @brief     This function processes ADC operations based on user input from the terminal.
 * @param[in] cmd                       Command value received from console input.
 * @retval    FSP_SUCCESS               Performed operation successfully.
 * @retval    Any other error codes     Failed to perform operation.
 **********************************************************************************************************************/
static fsp_err_t adc_operation(int32_t cmd)
{
    fsp_err_t err = FSP_SUCCESS;

    switch (cmd)
    {
        case SCAN_START:
        {
            /* Scan already running — caller prints message */
            if (g_ready_to_read)
            {
                printf("\r\nADC Scan already in progress\r\n");
                break;
            }

            err = adc_scan_start();
            if (FSP_SUCCESS != err)
            {
                return err;
            }
            printf("\r\nADC Started Scan\r\n");
            break;
        }
        case SCAN_STOP:
        {
            if ((!ADC_IS_SINGLE_SCAN()) && g_ready_to_read)
            {
                err = adc_scan_stop();
                if (FSP_SUCCESS == err)
                {
                    printf("\r\nADC Scan stopped\r\n");
                }
                else
                {
                    return err;
                }
            }
            else
            {
                printf("\r\nStop command is not supported in "MODE_1", or "
                       "the user has not pressed Start Scan in "MODE_2"\r\n");
            }
            printf(EP_MENU);
            break;
        }
        default:
        {
            printf("\r\nInvalid input\r\n");
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
        printf("\r\n[ERR] %s", err_str);
        printf("Returned Error Code: 0x%x\r\n", (unsigned int)err);
        uart_console_flush();
        deinit_adc_module();
        __BKPT(0);
    }
}

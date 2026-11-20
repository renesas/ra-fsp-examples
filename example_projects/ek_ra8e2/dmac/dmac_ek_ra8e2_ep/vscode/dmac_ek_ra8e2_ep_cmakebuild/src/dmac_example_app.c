/***********************************************************************************************************************
 * File Name    : dmac_example_app.c
 * Description  : Example application layer for the DMAC EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - hal_entry.c calls a single function: dmac_example_app_run().
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
#include "dmac_example_app.h"
#include "dmac_transfers.h"
#include "timer_initialise.h"
#include "transfer_initialise.h"
#include "console/uart_console.h"

/* Private functions */
static void handle_error(fsp_err_t err, const char * err_str);
static void user_input(char *buf, int len);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the DMAC example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void dmac_example_app_run(void)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Variable to store major/minor versions */
    fsp_pack_version_t version = {RESET_VALUE};

    /* Variables to get user option */
    char input_buff[BUFFER_SIZE] = {RESET_VALUE};
    uint8_t user_option = RESET_VALUE;

    uint32_t status = uart_console_init();
    if (FSP_SUCCESS != status)
    {
        return;     /* UART failed to open — cannot print anything */
    }

    /* Get FSP version */
    R_FSP_VersionGet(&version);

    /* Example project information printed on the terminal */
    printf(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor,
           version.version_id_b.patch);
    printf(EP_INFO);

    /* DMAC g_transfer_gpt_timer is configured to transfer 32-bit data
     * from a GPT counter register to the destination array. DMAC
     * transfer happens when 2 is pressed. The GPT timer continues running. */
    err = transfer_gpt_timer_operation();
    handle_error(err, "transfer_gpt_timer_operation failed\r\n");

    /* DMAC g_transfer_write_ioport is configured to transfer 32-bit
     * data (in repeat mode) from the source array to port control
     * register (LED). As a result, when 3 is pressed, LED state changes */
    err = transfer_write_ioport_operation();
    handle_error(err, "transfer_write_ioport_operation failed\r\n");

    printf("\r\n*** The GPT timer value will be updated in Round-Robin fashion ***\r\n");
    printf(EP_MENU);

    while (true)
    {
        /* Check for input data from the terminal */
        if (uart_console_has_char())
        {
            /* Get the user option */
            memset(&input_buff[0], NULL_CHAR, BUFFER_SIZE);
            user_input(input_buff, (int)sizeof(input_buff));
            user_option = (uint8_t)atoi(input_buff);

            if (START_TRANSFER_ON_LED_PORT == user_option)
            {
                /* Display information about g_transfer_led_blink transfer on the terminal and view on-board LED */
                printf("\r\n\r\nDMAC g_transfer_led_blink transfer in progress. Wait for 6 seconds\r\n");

                /* DMAC g_transfer_led_blink (runs in Normal mode) transfers data
                 * from source array to port control register (LED) for specified
                 * number of counts (60). DMAC g_transfer_led_blink alters the state
                 * of the LED and stops. */
                err = transfer_led_blink_operation();
                handle_error(err, "transfer_led_blink_operation failed\r\n");

                printf("\r\nDMAC g_transfer_led_blink transfer completed\r\n");

                /* De-initialize g_transfer_led_blink */
                dmac_transfer_deinit(&g_transfer_led_blink_ctrl, TRANSFER_LED_BLINK);

                /* De-initialize AGT */
                agt_timer_deinit();
            }
            else if (START_TRANSFER_GPT_VALUE == user_option)
            {
                /* Start DMAC transfer from GPT counter register to destination array */
                err = dmac_transfer_software_start(&g_transfer_gpt_value_ctrl);
                handle_error(err, "dmac_transfer_software_start failed\r\n");

                dmac_transfer_print_data();
            }
            else if (START_TRANSFER_TOGGLE_LED == user_option)
            {
                /* Start the DMAC transfer_write_ioport */
                err = dmac_transfer_software_start(&g_transfer_write_ioport_ctrl);
                handle_error(err, "dmac_transfer_software_start failed\r\n");

                printf("\r\n\r\nData successfully transferred on LED PORT\r\n");
            }
            else
            {
                printf("\r\n\r\nPlease provide a valid input!\r\n");
            }

            printf(EP_MENU);
        }
    }
}

/*******************************************************************************************************************//**
 * @brief         This function reads a line of user input from stdin and remove trailing newline characters.
 * @param[in,out] buf       Pointer to buffer where the input string is stored.
 * @param[in]     len       Maximum number of characters to read (buffer size).
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
 * @param[in] err           Error code.
 * @param[in] err_str       Error message.
 * @retval    None.
 **********************************************************************************************************************/
static void handle_error(fsp_err_t err, const char * err_str)
{
    if (FSP_SUCCESS != err)
    {
        printf("\r\n[ERR]: %s", err_str);
        printf("Returned Error Code: 0x%x\r\n", (unsigned int)err);
        uart_console_flush();

        /* De-initialize the opened modules */
        agt_timer_deinit();
        gpt_timer_deinit();
        dmac_transfer_deinit(&g_transfer_led_blink_ctrl, TRANSFER_LED_BLINK);
        dmac_transfer_deinit(&g_transfer_write_ioport_ctrl, TRANSFER_WRITE_IOPORT);
        dmac_transfer_deinit(&g_transfer_gpt_value_ctrl, TRANSFER_GPT_TIMER_VALUE);

        __BKPT(0);
    }
}

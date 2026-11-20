/***********************************************************************************************************************
 * File Name    : clock_output_example_app.c
 * Description  : Example application layer for the Clock Output EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - hal_entry.c calls a single function: clock_output_example_app_run().
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
#include "clock_output_ep.h"
#include "clock_output_example_app.h"
#include "console/uart_console.h"
#if (GPT_MEASURE_CLKOUT)
#include "gpt_input_capture.h"
#endif /* GPT_MEASURE_CLKOUT */

/* Private functions */
static void clock_output_operation(void);
#if (GPT_MEASURE_CLKOUT)
static void handle_error(fsp_err_t err, const char * err_str);
#endif /* GPT_MEASURE_CLKOUT */
static void user_input(char *buf, int len);

/***********************************************************************************************************************
 * Function Name: clock_output_example_app_run.
 * Description  : Single entry-point for the Clock Output example application.
 * Arguments    : None.
 * Return Value : None.
 **********************************************************************************************************************/
void clock_output_example_app_run(void)
{
    fsp_pack_version_t version = {RESET_VALUE};

    /* Get FLEX pack version information */
    R_FSP_VersionGet(&version);

    /* Turn OFF the error LED */
    R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[ERR_LED], (bsp_io_level_t)BSP_IO_LEVEL_LOW);

    uint32_t status = uart_console_init();
    if (FSP_SUCCESS != status)
    {
        /* Turn ON the error LED to indicate uart_console_init failed */
        R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[ERR_LED], (bsp_io_level_t)BSP_IO_LEVEL_HIGH);

        return;     /* UART failed to open — cannot print anything */
    }

    /* Print the EP banner on the terminal */
    printf(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);

    /* Print the EP information on the terminal */
    printf(EP_INFO);

    /* Print EP menu for user selection */
    printf(MENU);

    while (true)
    {
        /* Perform clock output operation */
        clock_output_operation();
    }
}
/***********************************************************************************************************************
* End of function clock_output_example_app_run
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: clock_output_operation.
 * Description  : This function performs clock output operation based on user input.
 * Arguments    : None.
 * Return Value : None.
 **********************************************************************************************************************/
static void clock_output_operation(void)
{
    uint8_t user_option = RESET_VALUE;
    char input_buff[BUFFER_SIZE] = {RESET_VALUE};

    if (uart_console_has_char())
    {
        /* Get the user option */
        memset(&input_buff[0], NULL_CHAR, BUFFER_SIZE);
        user_input(input_buff, (int)sizeof(input_buff));
        user_option = (uint8_t)atoi(input_buff);

        /* Configure clock output pin as peripheral clock out pin */
        R_IOPORT_PinCfg(&g_ioport_ctrl, CLKOUT_PIN, (uint32_t) IOPORT_CFG_PERIPHERAL_PIN | \
                        (uint32_t) IOPORT_PERIPHERAL_CLKOUT_COMP_RTC);

        /* Perform operation based on the user option */
        switch (user_option)
        {
#if !(defined (BOARD_RA6E2_FPB) || defined (BOARD_RA4E2_FPB))
            case START_MAIN_CLOCK:
            {
                printf("\r\n\r\n** Start Main clock output **\r\n");
                start_main_clock();

#if (GPT_MEASURE_CLKOUT)
                fsp_err_t err = FSP_SUCCESS;

                /* Perform GPT capture operation */
                err = gpt_capture_operation();
                handle_error(err, "gpt_capture_operation for Main clock failed!!!\r\n");

                /* Print clock output value */
                printf("\r\nCLKOUT frequency measurement value is %d Hz\r\n", g_clkout_value);
#endif /* GPT_MEASURE_CLKOUT */

                break;
            }
#endif /* BOARD_RA6E2_FPB, BOARD_RA4E2_FPB */

            case START_SUB_CLOCK:
            {
                printf("\r\n\r\n** Start Sub-clock output **\r\n");
                start_sub_clock();

#if (GPT_MEASURE_CLKOUT)
                fsp_err_t err = FSP_SUCCESS;

                /* Perform GPT capture operation */
                err = gpt_capture_operation();
                handle_error(err, "gpt_capture_operation for Sub-clock failed!!!\r\n");

                /* Print clock output value */
                printf("\r\nCLKOUT frequency measurement value is %d Hz\r\n", g_clkout_value);
#endif /* GPT_MEASURE_CLKOUT */

                break;
            }

            case STOP_ALL_CLOCKS:
            {
                printf("\r\n\r\n** Stop all clocks output **\r\n");
                stop_all_clocks();
                break;
            }

            default:
            {
                printf("\r\n\r\nInvalid input!!!");
                printf("\r\nPlease provide input from below option\r\n");
                break;
            }
        }

        /* Print EP menu for user selection */
        printf(MENU);
    }
}
/***********************************************************************************************************************
* End of function clock_output_operation
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: user_input.
 * Description  : This function reads a line of user input from stdin and removes trailing newline characters.
 * Arguments    : buf       Pointer to buffer where the input string is stored.
 *                len       Maximum number of characters to read (buffer size).
 * Return Value : None.
 **********************************************************************************************************************/
static void user_input(char *buf, int len)
{
    fgets(buf, len, stdin);
    buf[strcspn(buf, "\r\n")] = '\0';
}
/***********************************************************************************************************************
* End of function user_input
***********************************************************************************************************************/

#if (GPT_MEASURE_CLKOUT)
/***********************************************************************************************************************
 * Function Name: handle_error.
 * Description  : This function handles an error by printing error message and code, de-initializing the related
 *                modules, and entering a trap state.
 * Arguments    : err           Error code.
 *                err_str       Error message.
 * Return Value : None.
 **********************************************************************************************************************/
static void handle_error(fsp_err_t err, const char * err_str)
{
    if (FSP_SUCCESS != err)
    {
        printf("\r\n[ERR]: %s", err_str);
        printf("Returned Error Code: 0x%x\r\n", (unsigned int)err);
        uart_console_flush();
        gpt_deinit();
        __BKPT(0);
    }
}
/***********************************************************************************************************************
* End of function handle_error
***********************************************************************************************************************/
#endif /* GPT_MEASURE_CLKOUT */

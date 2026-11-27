/***********************************************************************************************************************
 * File Name    : poeg_example_app.c
 * Description  : Example application layer for the POEG EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - hal_entry.c calls a single function: poeg_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "poeg_example_app.h"
#include "console/uart_console.h"

/* Global variables */
#if (BSP_PERIPHERAL_ACMPHS_PRESENT) || (BSP_PERIPHERAL_ACMPHS_B_PRESENT)
comparator_info_t g_stabilize_time = {RESET_VALUE};
#if (BSP_PERIPHERAL_DAC_PRESENT) || (BSP_PERIPHERAL_DAC_B_PRESENT)
uint8_t g_num_bytes = RESET_VALUE;
uint16_t g_dac_val = RESET_VALUE;
#endif /* BSP_PERIPHERAL_DAC_PRESENT, BSP_PERIPHERAL_DAC_B_PRESENT */
#endif /* BSP_PERIPHERAL_ACMPHS_PRESENT, BSP_PERIPHERAL_ACMPHS_B_PRESENT */

/* Private functions */
static void handle_error(fsp_err_t err, const char *err_str);
static void user_input(char *buf, int len);
static fsp_err_t poeg_output_disable_and_reset_options(void);
static fsp_err_t reset_poeg_module(void);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the POEG example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void poeg_example_app_run(void)
{
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version = {RESET_VALUE};

    /* Get FLEX pack version information */
    R_FSP_VersionGet(&version);

    uint32_t status = uart_console_init();
    if (FSP_SUCCESS != status)
    {
        return;     /* UART failed to open — cannot print anything */
    }

    /* Example Project information printed on the terminal */
    printf(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    printf(EP_INFO);

    /* Turn OFF LED1 */
    R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[STATE_LED], LED_OFF);

    /* Initialize the POEG0 */
    err = init_poeg(&g_poeg0_ctrl, &g_poeg0_cfg, VECTOR_NUMBER_POEG0_EVENT);
    handle_error(err, "init_poeg for POEG0 failed\r\n");
    printf("\r\nPOEG0 initialized successfully\r\n");

    /* Initialize the POEG1 */
    err = init_poeg(&g_poeg1_ctrl, &g_poeg1_cfg, VECTOR_NUMBER_POEG1_EVENT);
    handle_error(err, "init_poeg for POEG1 failed\r\n");
    printf("POEG1 initialized successfully\r\n");

    /* Initialize the Timer1 in PWM mode */
    err = init_gpt_timer(&g_timer_pwm1_ctrl, &g_timer_pwm1_cfg);
    handle_error(err, "init_gpt_timer for g_timer_pwm1 failed\r\n");
    printf("Timer1 initialized successfully\r\n");

    /* Initialize the Timer2 in PWM mode */
    err = init_gpt_timer(&g_timer_pwm2_ctrl, &g_timer_pwm2_cfg);
    handle_error(err, "init_gpt_timer for g_timer_pwm2 failed\r\n");
    printf("Timer2 initialized successfully\r\n");

#if (BSP_PERIPHERAL_ACMPHS_PRESENT) || (BSP_PERIPHERAL_ACMPHS_B_PRESENT)
    /* Initialize the ACMPHS */
    err = enable_acmphs_mode_modules();
    handle_error(err, "enable_acmphs_mode_modules failed\r\n");
    printf("ACMPHS initialized successfully\r\n");
#endif /* BSP_PERIPHERAL_ACMPHS_PRESENT, BSP_PERIPHERAL_ACMPHS_B_PRESENT */

    while (true)
    {
        printf(MENU_TITLE);
        printf(EP_MENU);
        printf(OPTION_1_INFO);
#if (BSP_PERIPHERAL_ACMPHS_PRESENT) || (BSP_PERIPHERAL_ACMPHS_B_PRESENT)
        printf(OPTION_4_INFO);
#endif /* BSP_PERIPHERAL_ACMPHS_PRESENT, BSP_PERIPHERAL_ACMPHS_B_PRESENT */

        /* Process input only when the user has provided one */
        while (!uart_console_has_char());

        if (uart_console_has_char())
        {
            err = poeg_output_disable_and_reset_options();
            handle_error(err, "poeg_output_disable_and_reset_options failed\r\n");
        }
    }
}

/*******************************************************************************************************************//**
 * @brief     This function reads the command (input) from the terminal and processes the command for POEG
 *            output disable.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Operated successfully.
 * @retval    Any other error codes     Failed to operate.
 **********************************************************************************************************************/
static fsp_err_t poeg_output_disable_and_reset_options(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Variables to store the user input */
    char input_buff[BUFFER_SIZE] = {NULL_CHAR};
    uint8_t user_option = RESET_VALUE;
#if defined (BOARD_RA8E1_FPB) || defined (BOARD_RA8E2_EK)
    float dac_volt = {RESET_VALUE};
#endif /* BOARD_RA8E1_FPB, BOARD_RA8E2_EK */

    /* Get the user option */
    memset(&input_buff[0], NULL_CHAR, BUFFER_SIZE);
    user_input(input_buff, (int)sizeof(input_buff));
    user_option = (uint8_t)atoi((char *)input_buff);

    switch (user_option)
    {
        /* POEG_TRIGGER_ON_GTETRG_INPUT_PIN_LEVEL - Disables GPT output pins based
         * on GTETRG input pin level */
        case POEG_TRIGGER_ON_GTETRG_INPUT_PIN_LEVEL:
        {
            /* Start PWM Timer */
            err = start_gpt(&g_timer_pwm2_ctrl);
            handle_error(err, "start_gpt for g_timer_pwm2 failed\r\n");
            printf("\r\nStarted Timer2 in PWM mode");
            printf("\r\nEnter any key to disable GPT output using GTETRG trigger pin mode"
                   "\r\nUser input: ");

            while (true)
            {
                if (uart_console_has_char())
                {
                    memset(&input_buff[0], NULL_CHAR, BUFFER_SIZE);
                    user_input(input_buff, (int)sizeof(input_buff));
                    break;
                }
            }

            g_mode_flag = POEG_TRIGGER_ON_GTETRG_INPUT_PIN_LEVEL;

            /* Enable IRQ */
            NVIC_EnableIRQ(VECTOR_NUMBER_POEG1_EVENT);
            printf("\r\nGive active LOW signal at "GTETRG_PIN"\r\n");
            while (!g_interrupt_pin_trigger_flag)
            {
                /* Wait for interrupt flag */
            }
            printf("\r\nInterrupt occurs in Pin Trigger mode\r\n");
            printf("\r\nPOEG triggered successfully by the GTETRG input pin level\r\n");
            break;
        }

        /* POEG_OUTPUT_DISABLE_USING_SOFTWARE_API - Disables GPT output pins by software API */
        case POEG_OUTPUT_DISABLE_USING_SOFTWARE_API:
        {
            /* Start PWM Timer */
            err = start_gpt(&g_timer_pwm2_ctrl);
            handle_error(err, "start_gpt for g_timer_pwm2 failed\r\n");
            printf("\r\nStarted Timer2 in PWM mode");
            printf("\r\nEnter any key to disable GPT output using API"
                   "\r\nUser input: ");

            while (true)
            {
                if (uart_console_has_char())
                {
                    memset(&input_buff[0], NULL_CHAR, BUFFER_SIZE);
                    user_input(input_buff, (int)sizeof(input_buff));

                    /* Disable POEG1 GPT output pins using software API */
                    err = R_POEG_OutputDisable(&g_poeg1_ctrl);
                    break;
                }
            }
            handle_error(err, "R_POEG_OutputDisable API failed\r\n");
            printf("\r\nPOEG output disabled successfully by the software API\r\n");

            /* Turn ON LED1 */
            R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[STATE_LED], LED_ON);
            g_mode_flag = POEG_OUTPUT_DISABLE_USING_SOFTWARE_API;
            break;
        }

        /* POEG_OUTPUT_DISABLE_ON_GTIOC_OUTPUT_LEVEL - Disables GPT output pins on GTIOC pins level */
        case POEG_OUTPUT_DISABLE_ON_GTIOC_OUTPUT_LEVEL:
        {
            err = start_gpt(&g_timer_pwm1_ctrl);
            handle_error(err, "start_gpt for g_timer_pwm1 failed\r\n");
            printf("\r\nStarted Timer1 in PWM mode");
            printf("\r\nEnter any key to disable GPT PWM output level"
                   "\r\nUser input: ");
            while (true)
            {
                if (uart_console_has_char())
                {
                    memset(&input_buff[0], NULL_CHAR, BUFFER_SIZE);
                    user_input(input_buff, (int)sizeof(input_buff));
                    break;
                }
            }
            g_mode_flag = POEG_OUTPUT_DISABLE_ON_GTIOC_OUTPUT_LEVEL;

            /* Enable GPT output on GTIOCA pin.
             * When the GTIOCxA pin and the GTIOCxB pin are driven to an active level simultaneously,
             * the GPT generates an output-disable request to the POEG. */
            err = R_GPT_OutputEnable(&g_timer_pwm1_ctrl, GPT_IO_PIN_GTIOCA);
            handle_error(err, "R_GPT_OutputEnable API for g_timer_pwm1 failed\r\n");

            /* Enable IRQ */
            NVIC_EnableIRQ(VECTOR_NUMBER_POEG0_EVENT);

            while (!g_interrupt_output_level_flag)
            {
                /* Wait for interrupt flag */
            }
            printf("\r\nInterrupt occurs in GPT Output Level mode\r\n");
            printf("\r\nPOEG output disabled successfully by the GPT output level\r\n");
            break;
        }
#if (BSP_PERIPHERAL_ACMPHS_PRESENT) || (BSP_PERIPHERAL_ACMPHS_B_PRESENT)
        /* POEG_OUTPUT_DISABLE_ON_ACMPHS_EVENTS - Disables GPT output pins based on comparator crossing events */
        case POEG_OUTPUT_DISABLE_ON_ACMPHS_EVENTS:
        {
            printf("\r\nPlease refer to the Hardware Connections section to connect hardware correctly\r\n");

            /* Start PWM Timer */
            err = start_gpt(&g_timer_pwm2_ctrl);
            handle_error(err, "start_gpt for g_timer_pwm2 failed\r\n");
            printf("\r\nStarted Timer2 in PWM mode\r\n");

#if defined (BOARD_RA8E1_FPB) || defined (BOARD_RA8E2_EK)
            printf("\r\nInternal reference voltage (Vref) is used as reference voltage source."
                   "\r\nIt is set to 1.18V (Equivalent to a digital value 1465).\r\n");
#elif defined (BOARD_RA2T1_FPB)
            printf("\r\nInternal reference voltage (Vref) is used as reference voltage source."
                   "\r\nIt is set to 1.44V \r\n");
            printf("\r\nWhen the input voltage source is greater than the reference voltage (1.44V), POEG is"
                   "\r\ntriggered to request stopping the output in response to detection by the comparator\r\n");
#endif /* Specific board */

            g_mode_flag = POEG_OUTPUT_DISABLE_ON_ACMPHS_EVENTS;

            /* Enable IRQ */
            NVIC_EnableIRQ(VECTOR_NUMBER_POEG1_EVENT);
            while (!g_interrupt_acmphs_flag)
            {
#if (BSP_PERIPHERAL_DAC_PRESENT) || (BSP_PERIPHERAL_DAC_B_PRESENT)
                g_num_bytes = RESET_VALUE;
                g_dac_val = RESET_VALUE;

#if defined (BOARD_RA8E1_FPB) || defined (BOARD_RA8E2_EK)
                printf("\r\nEnter the DAC0 Value between (0 - 1400) or (1600 - 4095) to compare:");
#else
                printf("\r\nEnter the DAC1 Value between (0 - 2000) or (2100 - 4095) to compare:");
#endif /* BOARD_RA8E1_FPB, BOARD_RA8E2_EK */
                printf("\r\nIf the ADC input voltage exceeds "ADC_REF_VAL", the ACMPHS interrupt will occur;"
                       "\r\nOtherwise, no action is taken"
                       "\r\nUser input: ");
                while (RESET_VALUE == g_num_bytes)
                {
                    if (uart_console_has_char())
                    {
                        /* Get the user input */
                        memset(&input_buff[0], NULL_CHAR, BUFFER_SIZE);
                        user_input(input_buff, (int)sizeof(input_buff));
                        g_num_bytes = (uint8_t)strlen(input_buff);

                        if (RESET_VALUE == g_num_bytes)
                        {
                            printf("\r\nInvalid input\r\n");
                        }
                    }
                }

                /* Conversion from input string to integer value */
                g_dac_val = (uint16_t) (atoi((char *)input_buff));
                printf("\r\nInput DAC value: %d\r\n", g_dac_val);

                /* Check for out of range value */
                if (DAC_MAX_VAL < g_dac_val)
                {
                    printf("\r\nInvalid input, input value is out of range (0 - 4095)\r\n");
                }
                else if ((LOW_VAL < g_dac_val) && (HIGH_VAL > g_dac_val))
                {
                    printf("\r\nFluctuating voltage range\r\n");
                }
                else
                {
#if defined (BOARD_RA8E1_FPB) || defined (BOARD_RA8E2_EK)
                    /* Write to DAC0 channel */
                    err = DAC_WRITE(&g_dac0_ctrl, g_dac_val);
                    handle_error(err, "DAC_WRITE for DAC0 channel failed\r\n");

                    /* Conversion from digital value to analog value */
                    dac_volt = (float)((g_dac_val * V_REF) / (DAC_MAX_VAL + 1));
                    printf("\r\nInput voltage source for ACMPHS module: %.2f V\r\n", dac_volt);
#else
                    /* Write to DAC1 channel */
                    err = DAC_WRITE(&g_dac1_ctrl, g_dac_val);
                    handle_error(err, "DAC_WRITE for DAC1 channel failed\r\n");
#endif /* BOARD_RA8E1_FPB, BOARD_RA8E2_EK */

                    /* Wait for the minimum stabilization wait time */
                    R_BSP_SoftwareDelay(g_stabilize_time.min_stabilization_wait_us, BSP_DELAY_UNITS_MICROSECONDS);
                    R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_SECONDS);
                }
#endif /* BSP_PERIPHERAL_DAC_PRESENT, BSP_PERIPHERAL_DAC_B_PRESENT */
#if defined (BOARD_RA2T1_FPB)
                /* Wait for the minimum stabilization wait time */
                R_BSP_SoftwareDelay(g_stabilize_time.min_stabilization_wait_us, BSP_DELAY_UNITS_MICROSECONDS);
                R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_SECONDS);
#endif  /* BOARD_RA2T1_FPB */
            }
            printf("\r\nInterrupt occurs in ACMPHS mode\r\n");
            printf("\r\nPOEG output disabled successfully upon ACMPHS crossing events\r\n");
            break;
        }
#endif /* BSP_PERIPHERAL_ACMPHS_PRESENT, BSP_PERIPHERAL_ACMPHS_B_PRESENT */

        /* POEG_RESET - Resets the POEG status */
        case POEG_RESET:
        {
            err = reset_poeg_module();
            handle_error(err, "reset_poeg_module failed\r\n");
            break;
        }

        default:
        {
#if (BSP_PERIPHERAL_ACMPHS_PRESENT) || (BSP_PERIPHERAL_ACMPHS_B_PRESENT)
            printf("\r\nInvalid input, please enter the value between 1-5\r\n");
#else
            printf("\r\nInvalid input, please enter the value between 1-4\r\n");
#endif /* BSP_PERIPHERAL_ACMPHS_PRESENT, BSP_PERIPHERAL_ACMPHS_B_PRESENT */
            break;
        }
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function is used to reset the POEG module.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Reset successfully.
 * @retval    Any other error codes     Failed to reset.
 **********************************************************************************************************************/
static fsp_err_t reset_poeg_module(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Variable to store user input */
    char input_buff[BUFFER_SIZE] = {NULL_CHAR};
    uint8_t user_option = RESET_VALUE;

    printf(RESET_MENU);

    /* Process input only when the user has provided one */
    while (!uart_console_has_char());

    if (uart_console_has_char())
    {
        memset(&input_buff[0], NULL_CHAR, BUFFER_SIZE);
        user_input(input_buff, (int)sizeof(input_buff));
        user_option = (uint8_t)atoi((char *)input_buff);
    }
    switch (user_option)
    {
        case POEG_CHANNEL_0:
        {
            /* Disable GPT output on GTIOCA pin to prevent continuous triggering of POEG
             * due to sustained output signal */
            err = R_GPT_OutputDisable(&g_timer_pwm1_ctrl, GPT_IO_PIN_GTIOCA);
            if (FSP_SUCCESS != err)
            {
                return err;
            }

            err = R_POEG_Reset(&g_poeg0_ctrl);
            if (FSP_SUCCESS != err)
            {
                return err;
            }
            printf("\r\nReset POEG0 successfully\r\n");

            /* Turn OFF LED1 */
            R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[STATE_LED], LED_OFF);
            g_interrupt_output_level_flag = false;
            break;
        }

        case POEG_CHANNEL_1:
        {
            err = R_POEG_Reset(&g_poeg1_ctrl);
            if (FSP_SUCCESS != err)
            {
                return err;
            }
            printf("\r\nReset POEG1 successfully\r\n");

            /* Turn OFF LED1 */
            R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[STATE_LED], LED_OFF);
            g_interrupt_acmphs_flag = false;
            g_interrupt_pin_trigger_flag = false;
            break;
        }

        default:
        {
            printf("\r\nInvalid input, please enter the value 1 or 2\r\n");
            break;
        }
    }
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
static void handle_error(fsp_err_t err, const char *err_str)
{
    if (FSP_SUCCESS != err)
    {
        printf("\r\n[ERR] %s", err_str);
        printf("Returned Error Code: 0x%x\r\n", (unsigned int)err);
        uart_console_flush();
        deinit_gpt_timer(&g_timer_pwm1_ctrl);
        deinit_gpt_timer(&g_timer_pwm2_ctrl);
        deinit_poeg(&g_poeg0_ctrl);
        deinit_poeg(&g_poeg1_ctrl);
#if (BSP_PERIPHERAL_ACMPHS_PRESENT) || (BSP_PERIPHERAL_ACMPHS_B_PRESENT)
        deinit_acmphs();
        deinit_dac(&g_dac0_ctrl);
#if !(defined (BOARD_RA8E1_FPB) || defined (BOARD_RA8E2_EK))
        deinit_dac(&g_dac1_ctrl);
#endif /* BOARD_RA8E1_FPB, BOARD_RA8E2_EK */
#if defined (BOARD_RA6T2_MCK)
        deinit_adc(&g_adc0_ctrl);
#endif /* BOARD_RA6T2_MCK */
#endif /* BSP_PERIPHERAL_ACMPHS_PRESENT, BSP_PERIPHERAL_ACMPHS_B_PRESENT */
        __BKPT(0);
    }
}

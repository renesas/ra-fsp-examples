/***********************************************************************************************************************
 * File Name    : acmphs_example_app.c
 * Description  : Example application layer for the ACMPHS EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - hal_entry.c calls a single function: acmphs_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "acmphs_example_app.h"
#include "console/uart_console.h"

/* Private functions */
static void init_peripherals(comparator_info_t * p_stabilize_time);
static void run_compare_loop(const comparator_info_t * p_stabilize_time);
static void user_input(char * buf, int len);
static void handle_error(fsp_err_t err, const char * err_str);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the ACMPHS example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void acmphs_example_app_run(void)
{
    comparator_info_t stabilize_time = {RESET_VALUE};
    fsp_pack_version_t version = {RESET_VALUE};

    uint32_t status = uart_console_init();
    if (FSP_SUCCESS != status)
    {
        return;     /* UART failed to open — cannot print anything */
    }

    /* Get FLEX pack version information */
    R_FSP_VersionGet(&version);

    /* Print EP information */
    printf(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    printf(EP_INFO);
    init_peripherals(&stabilize_time);
    run_compare_loop(&stabilize_time);
}

/*******************************************************************************************************************//**
 * @brief      This function initializes the DAC (if available), opens the comparator, and retrieves its
 *             stabilization time.
 * @param[out] p_stabilize_time         Pointer to comparator_info_t used to store the comparator stabilization time.
 * @retval     None.
 **********************************************************************************************************************/
static void init_peripherals(comparator_info_t * p_stabilize_time)
{
    fsp_err_t err = FSP_SUCCESS;

#if (BSP_PERIPHERAL_DAC_PRESENT || BSP_PERIPHERAL_DAC_B_PRESENT)
#if (defined (BOARD_RA8E1_FPB) || defined (BOARD_RA8E2_EK))
    /* Open DAC0, write 0 for providing analog input voltage to comparator (avoids garbage
     * on the ACMPHS input), then start conversion */
    err = acmphs_ep_dac0_init(RESET_VALUE);
#else
    /* Open DAC0, write 2048 (≈ 1.65V) as reference voltage to comparator, then start conversion */
    err = acmphs_ep_dac0_init(DAC_REF_VAL);
#endif /* BOARD_RA8E1_FPB, BOARD_RA8E2_EK */
    handle_error(err, "acmphs_ep_dac0_init failed\r\n");

#if (!(defined (BOARD_RA8E1_FPB) || defined (BOARD_RA8E2_EK)))
    /* Open DAC1, write 0 for providing analog input voltage to comparator (avoids garbage
     * on the ACMPHS input), then start conversion */
    err = acmphs_ep_dac1_init(RESET_VALUE);
    handle_error(err, "acmphs_ep_dac1_init failed\r\n");
#endif /* BOARD_RA8E1_FPB, BOARD_RA8E2_EK */
#endif /* BSP_PERIPHERAL_DAC_PRESENT, BSP_PERIPHERAL_DAC_B_PRESENT */

    /* Open the comparator, fetch its minimum stabilization wait time, then enable its output */
    err = acmphs_ep_comparator_init(p_stabilize_time);
    handle_error(err, "acmphs_ep_comparator_init failed\r\n");
}

/*******************************************************************************************************************//**
 * @brief     This function performs comparator operations based on user input and displays the comparison result.
 * @param[in] p_stabilize_time          Pointer to comparator_info_t containing the stabilization wait time.
 * @retval    None.
 **********************************************************************************************************************/
static void run_compare_loop(const comparator_info_t * p_stabilize_time)
{
    fsp_err_t err = FSP_SUCCESS;
    char input_buff[BUFFER_SIZE] = {RESET_VALUE};
    comparator_status_t acmphs_status = {RESET_VALUE};
#if (BSP_PERIPHERAL_DAC_PRESENT || BSP_PERIPHERAL_DAC_B_PRESENT)
    uint16_t dac_val = RESET_VALUE;
#if (defined (BOARD_RA8E1_FPB) || defined (BOARD_RA8E2_EK))
    uint8_t volt_str[5] = {RESET_VALUE};
    float dac_volt = 0.0f;
#endif /* BOARD_RA8E1_FPB, BOARD_RA8E2_EK */
#endif /* BSP_PERIPHERAL_DAC_PRESENT */

    while (true)
    {
#if (BSP_PERIPHERAL_DAC_PRESENT || BSP_PERIPHERAL_DAC_B_PRESENT)
        dac_val = RESET_VALUE;
#endif /* BSP_PERIPHERAL_DAC_PRESENT, BSP_PERIPHERAL_DAC_B_PRESENT */

        /* Clear the input buffer */
        memset(&input_buff[0], NULL_CHAR, BUFFER_SIZE);

#if (BSP_PERIPHERAL_DAC_PRESENT || BSP_PERIPHERAL_DAC_B_PRESENT)
#if (defined (BOARD_RA8E1_FPB) || defined (BOARD_RA8E2_EK))
        printf("\r\nEnter the DAC0 value between (0 - 1400) or (1600 - 4095) to compare:"
               "\r\nUser input: ");
#else
        printf("\r\nEnter the DAC1 value between (0 - 2000) or (2100 - 4095) to compare:"
               "\r\nUser input: ");
#endif /* BOARD_RA8E1_FPB, BOARD_RA8E2_EK */
#else
        printf("\r\nEnter any key to perform compare operation:");
#endif /* BSP_PERIPHERAL_DAC_PRESENT, BSP_PERIPHERAL_DAC_B_PRESENT */

        /* Get user input */
        user_input(input_buff, (int)sizeof(input_buff));

#if (BSP_PERIPHERAL_DAC_PRESENT || BSP_PERIPHERAL_DAC_B_PRESENT)
        /* Conversion from input string to integer value */
        dac_val = (uint16_t)atoi(input_buff);
        printf("\r\n\r\nInput DAC value: %d", dac_val);

        /* Check for out of range value */
        if (DAC_MAX_VAL < dac_val)
        {
            printf("\r\n** Invalid input, DAC value is out of range (0 - 4095) **\r\n");
        }

        /* Check for DAC input range to avoid fluctuating voltage caused due to
         * electrical noise which may alter the comparator output */
        else if (LOW_VAL < dac_val && dac_val < HIGH_VAL)
        {
            printf("\r\n** Fluctuating voltage range **\r\n");
        }
        else
        {
#if (defined (BOARD_RA8E1_FPB) || defined (BOARD_RA8E2_EK))
            /* Write to DAC0 channel */
            err = acmphs_ep_dac0_write(dac_val);
            handle_error(err, "acmphs_ep_dac0_write failed\r\n");

            /* Conversion from digital value to analog value */
            dac_volt = (float) ((dac_val * V_REF) / (DAC_MAX_VAL + 1));
            snprintf((char *) volt_str, sizeof(volt_str), "%0.2f", dac_volt);
            printf("\r\nInput voltage source for ACMPHS module: %sV", volt_str);
#else
            /* Write to DAC1 channel */
            err = acmphs_ep_dac1_write(dac_val);
            handle_error(err, "acmphs_ep_dac1_write failed\r\n");
#endif /* BOARD_RA8E1_FPB, BOARD_RA8E2_EK */
#endif /* BSP_PERIPHERAL_DAC_PRESENT, BSP_PERIPHERAL_DAC_B_PRESENT */

            /* Wait for the minimum stabilization wait time */
            R_BSP_SoftwareDelay(p_stabilize_time->min_stabilization_wait_us, BSP_DELAY_UNITS_MICROSECONDS);

            err = acmphs_ep_status_refresh(&acmphs_status);
            handle_error(err, "acmphs_ep_status_refresh failed\r\n");

            /* Check if status of comparator is high */
            if (COMPARATOR_STATE_OUTPUT_HIGH == acmphs_status.state)
            {
                printf("\r\nComparator output high\r\n");
            }
            else
            {
                printf("\r\nComparator output low\r\n");
            }

            err = acmphs_ep_led_update(acmphs_status.state);
            handle_error(err, "acmphs_ep_led_update failed\r\n");
#if (BSP_PERIPHERAL_DAC_PRESENT || BSP_PERIPHERAL_DAC_B_PRESENT)
        }
#endif /* BSP_PERIPHERAL_DAC_PRESENT, BSP_PERIPHERAL_DAC_B_PRESENT */
    }
}

/*******************************************************************************************************************//**
 * @brief         This function reads a line of user input from stdin and removes trailing newline characters.
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
        acmphs_ep_close();
        __BKPT(0);
    }
}

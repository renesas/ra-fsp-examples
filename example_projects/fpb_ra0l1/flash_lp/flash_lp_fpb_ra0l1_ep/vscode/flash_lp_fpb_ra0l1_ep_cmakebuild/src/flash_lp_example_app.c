/***********************************************************************************************************************
 * File Name    : flash_lp_example_app.c
 * Description  : Example application layer for the FLASH LP EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - hal_entry.c calls a single function: flash_lp_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "flash_lp_ep.h"
#include "flash_lp_example_app.h"
#include "console/uart_console.h"

/* Global variables */
uint8_t g_menu_input = RESET_VALUE;
uint8_t g_block_num = RESET_VALUE;

/* Private functions */
static void flash_lp_operations(uint32_t flash_address);
static void flash_lp_access_window_operations(uint8_t block_addr);
static uint8_t block_num_input_and_validation(uint32_t flash_address_min, uint32_t flash_address_max);
static void user_input(char *buf, int len);
static void print_data(const uint8_t * p_buff);
static void handle_error(fsp_err_t err, const char *err_str);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the Flash LP example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void flash_lp_example_app_run(void)
{
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version = {RESET_VALUE};
    uint8_t block_window_access = {RESET_VALUE};
    char input_buff[BUFFER_SIZE] = {INITIAL_VALUE};

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

    /* Open Flash LP driver */
    err = flash_lp_init();
    handle_error(err, "flash_lp_init failed\r\n");

    printf(MAIN_MENU);

    while (true)
    {
        /* Process input only when the user has provided one */
        if (uart_console_has_char())
        {
            user_input(input_buff, BUFFER_SIZE);
            g_menu_input = (uint8_t)atoi(input_buff);

            switch (g_menu_input)
            {
                case DATA_FLASH:
                {
                    printf(SUB_MENU1);
                    g_block_num = block_num_input_and_validation(FLASH_DF_BLOCK0, FLASH_DF_BLOCK_END);

                    flash_lp_operations(FLASH_DF_BLOCK(g_block_num));

                    break;
                }
                case CODE_FLASH:
                {
                    printf(SUB_MENU2);
                    g_block_num = block_num_input_and_validation(FLASH_CF_BLOCK10, FLASH_CF_BLOCK_END);

                    flash_lp_operations(FLASH_CF_BLOCK(g_block_num));

                    /* Code flash access window example */
                    printf(SUB_MENU3);
                    block_window_access = block_num_input_and_validation(FLASH_CF_BLOCK10, FLASH_CF_BLOCK_PRE_END);

                    flash_lp_access_window_operations(block_window_access);

                    break;
                }
                default:
                {
                    printf("\r\nInvalid menu option selected\r\n");
                    break;
                }
            }
            printf(MAIN_MENU);
        }
    }
}

/*******************************************************************************************************************//**
 * @brief     This function performs blank check, erase, write, and read operations.
 * @param[in] flash_address             Flash block address used for flash operations.
 * @retval    None.
 **********************************************************************************************************************/
static void flash_lp_operations(uint32_t flash_address)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;
    uint8_t read_buff[BLOCK_SIZE] = {RESET_VALUE};
    uint8_t write_buff[BLOCK_SIZE] = {RESET_VALUE};

    /* Set write buffer and clear read buffer */
    for (uint8_t index = RESET_VALUE; BLOCK_SIZE > index; index++)
    {
        write_buff[index] = index;
    }

    if (CODE_FLASH == g_menu_input)
    {
        /* Disable interrupts to prevent vector table access while code flash is in P/E mode */
        __disable_irq();
    }

    /* Perform blank check before erase */
    err = flash_lp_blank_check(flash_address);
    handle_error(err, "flash_lp_blank_check failed\r\n");

    if (FLASH_RESULT_NOT_BLANK == g_blank_check_result)
    {
        /* Erase a block */
        err = flash_lp_erase(flash_address);
        handle_error(err, "flash_lp_erase failed\r\n");

        /* Perform blank check after erased */
        err = flash_lp_blank_check(flash_address);
        handle_error(err, "flash_lp_blank_check after erased failed\r\n");

        if (FLASH_RESULT_BLANK != g_blank_check_result)
        {
            handle_error(FSP_ERR_ERASE_FAILED, "Failed to erase\r\n");
        }
    }

    /* Read from data flash */
    memcpy(read_buff, (uint8_t*)flash_address, BLOCK_SIZE);

    err = flash_lp_write(write_buff, flash_address);
    handle_error(err, "flash_lp_write failed\r\n");

    printf("\r\nFlash erased successfully");
    printf("\r\nBlock %d is blank\r\n", g_block_num);

    /* Print the read data to the terminal */
    printf("\r\nData in the block:\r\n");
    print_data(read_buff);

    printf("\r\nWrote data into the flash block successfully\r\n");

    /* Read from data flash */
    memcpy(read_buff, (uint8_t*)flash_address, BLOCK_SIZE);

    /* Compare read_buff and write_buff */
    if (RESET_VALUE == memcmp(read_buff, write_buff, BLOCK_SIZE))
    {
        /* Print the read data to the terminal */
        printf("\r\nRead data matches written data. Data after wrote:\r\n");
        print_data(read_buff);
    }
    else
    {
        printf("\r\nPlease check configuration and addresses...\r\n");
        handle_error(FSP_ERR_PARITY, "Read data does not match Write data\r\n");
    }
}

/*******************************************************************************************************************//**
 * @brief     This function receives and validates a flash block number..
 * @param[in] flash_address_min         Minimum valid flash block address.
 * @param[in] flash_address_max         Maximum valid flash block address.
 * @retval    block_num                 Validated flash block number.
 **********************************************************************************************************************/
static uint8_t block_num_input_and_validation(uint32_t flash_address_min, uint32_t flash_address_max)
{
    char rByte[BUFFER_SIZE] = {INITIAL_VALUE};
    uint8_t block_num = RESET_VALUE;

    /* Wait until user data is available */
    while (uart_console_has_char())
    {
        /* Do nothing */
    }

    user_input(rByte, BUFFER_SIZE);
    block_num = (uint8_t)atoi(rByte);

    if (DATA_FLASH == g_menu_input)
    {
        while ((flash_address_min > FLASH_DF_BLOCK(block_num)) || (flash_address_max < FLASH_DF_BLOCK(block_num)))
        {
            printf("\r\n\r\nInvalid input. Please enter a valid block number: ");
            memset(rByte, RESET_VALUE, BUFFER_SIZE);

            /* Wait until user data is available */
            while (uart_console_has_char())
            {
                /* Do nothing */
            }

            user_input(rByte, BUFFER_SIZE);
            block_num = (uint8_t)atoi(rByte);
        }
    }
    else if (CODE_FLASH == g_menu_input)
    {
        while ((flash_address_min > FLASH_CF_BLOCK(block_num)) || (flash_address_max < FLASH_CF_BLOCK(block_num)))
        {
            printf("\r\n\r\nInvalid input. Please enter a valid block number: ");
            memset(rByte, RESET_VALUE, BUFFER_SIZE);

            /* Wait until user data is available */
            while (uart_console_has_char())
            {
                /* Do nothing */
            }

            user_input(rByte, BUFFER_SIZE);
            block_num = (uint8_t)atoi(rByte);
        }
    }
    else
    {
        /* Do nothing */
    }

    return block_num;
}

/*******************************************************************************************************************//**
 * @brief     This function demonstrates code flash access window operations.
 * @param[in] block_num                 Code flash block number.
 * @retval    None.
 **********************************************************************************************************************/
static void flash_lp_access_window_operations(uint8_t block_addr)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    uint8_t write_buff[BLOCK_SIZE] = {RESET_VALUE};

    /* Disable interrupts to prevent vector table access while code flash is in P/E mode */
    __disable_irq();

    /* Set Access window.
     * CAUTION: Highly recommended not to use this function if not aware of consequences OR
     * use it with the accessWindowClear API at the end of application.
     * This API locks the Code flash and the lock retains even after power cycle.
     * Which means, even after power cycle, the user will not be able to program the code to code flash if the
     * access window is wrongly set.
     *
     * WORKAROUND: If the user uses accessWindowSet and locks the window. Flash can be unlocked by running
     * different application performing just "open" call and "accessWindowClear()" running from the RAM. */

    err = flash_lp_access_window_set(block_addr);
    handle_error(err, "flash_lp_accesswindow_set failed\r\n");

    err = flash_lp_erase(FLASH_CF_BLOCK(block_addr));
    handle_error(err, "flash_lp_erase failed\r\n");

    err = flash_lp_write(write_buff, FLASH_CF_BLOCK(block_addr));
    handle_error(err, "flash_lp_write failed\r\n");

    err = flash_lp_access_window_clear();
    handle_error(err, "flash_lp_accesswindow_clear failed\r\n");

    __enable_irq();

    printf("\r\n\r\nCode flash access window set successfully\r\n");
    printf("Erased successfully\r\n");
    printf("Wrote successfully\r\n");
    printf("Code flash access window cleared successfully\r\n");
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
 * @brief     This function prints the contents of a data buffer in hexadecimal format.
 * @param[in] p_buff                    Pointer to the data buffer to be displayed.
 * @retval    None.
 **********************************************************************************************************************/
static void print_data(const uint8_t * p_buff)
{
    for (uint8_t index = 0U; index < BLOCK_SIZE; index++)
    {
        printf("%2.2x ", p_buff[index]);

        if (((index + 1U) % 8U) == 0U)
        {
            printf("\r\n");
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
static void handle_error(fsp_err_t err, const char *err_str)
{
    if (FSP_SUCCESS != err)
    {
        if (CODE_FLASH == g_menu_input)
        {
            __enable_irq();
        }

        printf("\r\n[ERR] %s", err_str);
        printf("Returned Error Code: 0x%x\r\n", (unsigned int)err);
        uart_console_flush();
        flash_lp_deinit();
        __BKPT(0);
    }
}

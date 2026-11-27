/***********************************************************************************************************************
 * File Name    : flash_hp_example_app.c
 * Description  : Example application layer for the FLASH HP EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - hal_entry.c calls a single function: flash_hp_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "flash_hp_ep.h"
#include "flash_hp_example_app.h"
#include "console/uart_console.h"

/* Private functions */
static void handle_error(fsp_err_t err, const char * err_str);
static fsp_err_t flash_hp_code_flash_operations(void);
static fsp_err_t flash_hp_data_flash_operations(void);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the Flash HP example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void flash_hp_example_app_run(void)
{
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version = {RESET_VALUE};
    int32_t read_data = RESET_VALUE;

    uint32_t status = uart_console_init();
    if (FSP_SUCCESS != status)
    {
        return;     /* UART failed to open — cannot print anything */
    }

    /* Get FLEX pack version information */
    R_FSP_VersionGet(&version);

    /* Example project information printed on the terminal */
    printf(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    printf(EP_INFO);

    /* Open Flash HP */
    err = flash_hp_init();
    handle_error(err, "flash_hp_init failed\r\n");

    /* Setup default block 0 as startup setup block */
    err = flash_hp_startup_block();
    handle_error(err, "flash_hp_startup_block failed\r\n");

    printf(EP_MENU);

    while (EXIT != read_data)
    {
        /* Check for input data from the terminal */
        if (uart_console_has_char())
        {
            /* Get the user option */
            read_data = getchar();
            switch (read_data)
            {
                case CODE_FLASH:
                {
                    err = flash_hp_code_flash_operations();
                    handle_error(err, "flash_hp_code_flash_operations failed\r\n");
                    break;
                }

                case DATA_FLASH:
                {
                    err = flash_hp_data_flash_operations();
                    handle_error(err, "flash_hp_data_flash_operations failed\r\n");
                    break;
                }

                case EXIT:
                {
                    printf("\r\n\r\nExiting the flash_hp, the user has to restart the application");
                    flash_hp_deinit();
                    break;
                }

                default:
                {
                    printf(EP_MENU);
                    break;
                }
            }
            if ((CODE_FLASH <= read_data) && (EXIT > read_data))
            {
                printf("\r\nEnter any key (other than 1,2,3) to go back to the main menu\r\n");
            }
            read_data = RESET_VALUE;
        }
    }
}

/*******************************************************************************************************************//**
 * @brief     This function performs code flash erase, write, read-back verification, and prints the results.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Operated successfully.
 * @retval    Any other error codes     Failed to operate.
 **********************************************************************************************************************/
static fsp_err_t flash_hp_code_flash_operations(void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint8_t write_buffer[BLOCK_SIZE] = {RESET_VALUE};
    uint8_t read_buffer[BLOCK_SIZE] = {RESET_VALUE};

    printf("\r\n\r\nEntering to code flash operations");

    /* Fill write buffer with sequential data */
    for (uint8_t index = RESET_VALUE; index < BLOCK_SIZE; index++)
    {
        write_buffer[index] = index;
    }

    /* Disable interrupts to prevent vector table access while code flash is in P/E mode */
    __disable_irq();

    /* Erase target code flash block before programming */
    err = flash_hp_erase(FLASH_HP_CF_BLOCK_8);
    if (FSP_SUCCESS != err)
    {
        __enable_irq();
        return err;
    }

    /* Verify that flash area is completely erased */
    err = flash_hp_blank_check(FLASH_HP_CF_BLOCK_8, FLASH_HP_CF_BLOCK_SIZE);
    if (FSP_SUCCESS != err)
    {
        __enable_irq();
        return err;
    }

    /* Write code flash data */
    err = flash_hp_write(write_buffer, FLASH_HP_CF_BLOCK_8, BLOCK_SIZE);
    if (FSP_SUCCESS != err)
    {
        __enable_irq();
        return err;
    }

    __enable_irq();

    printf("\r\nData in the code flash block erased successfully");
    printf("\r\nThe code flash block is blank");
    printf("\r\nData written into the code flash block successfully");

    /* Read back data from code flash */
    memcpy(read_buffer, (uint8_t *) FLASH_HP_CF_BLOCK_8, BLOCK_SIZE);

    /* Verify write and read data match */
    if (RESET_VALUE != memcmp(read_buffer, write_buffer, BLOCK_SIZE))
    {
        return FSP_ERR_INVALID_DATA;
    }
    printf("\r\nRead and write data match");

    /* Print the read data on the terminal */
    printf("\r\nRead data the code flash block: \r\n\r\n");
    READ_DATA_PRINT(read_buffer);

#if (BSP_FEATURE_FLASH_SUPPORTS_ACCESS_WINDOW)
    err = flash_hp_access_window_operations(write_buffer);
#endif /* BSP_FEATURE_FLASH_SUPPORTS_ACCESS_WINDOW */

    return err;
}

/*******************************************************************************************************************//**
 * @brief    This function performs data flash erase, write, read-back verification, and prints the results.
* @param[in] None.
 * @retval   FSP_SUCCESS                Operated successfully.
 * @retval   Any other error codes      Failed to operate.
 **********************************************************************************************************************/
static fsp_err_t flash_hp_data_flash_operations(void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint8_t write_buffer[BLOCK_SIZE] = {RESET_VALUE};
    uint8_t read_buffer[BLOCK_SIZE] = {RESET_VALUE};

    printf("\r\n\r\nEntering to data flash operations");

    /* Erase target data flash block before programming */
    err = flash_hp_erase(FLASH_HP_DF_BLOCK_1);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    printf("\r\nData in the data flash block erased successfully");

    /* Verify that flash area is completely erased */
    err = flash_hp_blank_check(FLASH_HP_DF_BLOCK_1, FLASH_HP_DF_BLOCK_SIZE);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    printf("\r\nThe data flash block is blank");

    /* Fill write buffer with sequential data */
    for (uint8_t index = RESET_VALUE; index < BLOCK_SIZE; index++)
    {
        write_buffer[index] = index;
    }

    /* Write data to data flash */
    err = flash_hp_write(write_buffer, FLASH_HP_DF_BLOCK_1, BLOCK_SIZE);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    printf("\r\nData written into the data flash block successfully");

    /* Read back data from data flash */
    memcpy(read_buffer, (uint8_t *) FLASH_HP_DF_BLOCK_1, BLOCK_SIZE);

    /* Verify write and read data match */
    if (RESET_VALUE != memcmp(read_buffer, write_buffer, BLOCK_SIZE))
    {
        return FSP_ERR_INVALID_DATA;
    }
    printf("\r\nRead and write data match");

    /* Print the read data on the terminal */
    printf("\r\nRead data in the data flash block: \r\n\r\n");
    READ_DATA_PRINT(read_buffer);

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
        flash_hp_deinit();
        __BKPT(0);
    }
}

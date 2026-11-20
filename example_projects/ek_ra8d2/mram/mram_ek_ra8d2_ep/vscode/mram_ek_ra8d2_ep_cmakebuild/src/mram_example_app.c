/***********************************************************************************************************************
 * File Name    : mram_example_app.c
 * Description  : Example application layer for the MRAM EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - hal_entry.c calls a single function: mram_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include <string.h>
#include "mram_ep.h"
#include "mram_example_app.h"
#include "console/uart_console.h"

/* Private variables */
static uint8_t s_write_buffer[NUM_BYTES] = {RESET_VALUE};
static uint8_t s_read_buffer[NUM_BYTES] = {RESET_VALUE};

/* Private functions */
static void handle_write_command(void);
static void handle_read_command(void);
static void handle_erase_command(void);
static void handle_error(fsp_err_t err, const char * err_str);

/*******************************************************************************************************************//**
 * @brief       Single entry-point for the MRAM example application.
 * @param[in]   None.
 * @retval      None.
 **********************************************************************************************************************/
void mram_example_app_run(void)
{
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version = {RESET_VALUE};
    uint32_t command = RESET_VALUE;

    uint32_t status = uart_console_init();
    if (FSP_SUCCESS != status)
    {
        return;     /* UART failed to open — cannot print anything */
    }

    /* Get FSP version information */
    R_FSP_VersionGet(&version);

    /* Example project information printed on the terminal */
    printf(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    printf(EP_INFO);

    /* Initialize the MRAM module */
    err = mram_init();
    handle_error(err, "mram_init failed\r\n");

    printf(MENU);
    printf(EP_GUIDE);

    while (EXIT != command)
    {
        if (uart_console_has_char())
        {
            command = (uint32_t) (getchar() - '0');

            switch (command)
            {
                case MRAM_WRITE:
                {
                    handle_write_command();
                    break;
                }

                case MRAM_READ:
                {
                    handle_read_command();
                    break;
                }

                case MRAM_ERASE:
                {
                    handle_erase_command();
                    break;
                }

                case EXIT:
                {
                    printf("\r\nExiting the MRAM application, restart it to run again.");
                    mram_deinit();
                    break;
                }

                default:
                {
                    printf(MENU);
                    printf(EP_GUIDE);
                    break;
                }
            }

            if ((MRAM_WRITE <= command) && (EXIT > command))
            {
                printf("\r\nEnter any key (other than 1, 2, 3, 4) to go back to the main menu");
                printf(EP_GUIDE);
                command = RESET_VALUE;
            }
        }
    }
}

/*******************************************************************************************************************//**
 * @brief       This function writes sequential data to MRAM and verifies the written data.
 *              The write region is defined by MRAM_START_ADDR and NUM_BYTES macros.
 * @param[in]   None.
 * @retval      None.
 **********************************************************************************************************************/
static void handle_write_command(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Check blank before writing */
    if (!mram_is_blank(MRAM_START_ADDR, NUM_BYTES))
    {
        printf("\r\nExisting data detected in the MRAM region. The region will be overwritten.\r\n");
    }

    /* Set the write buffer */
    for (uint8_t index = 0; index < NUM_BYTES; index++)
    {
        s_write_buffer[index] = index;
    }

    printf("\r\n\r\nWriting to MRAM...");

    /* Write data to MRAM. There is no need to perform an erase first. */
    err = mram_write(MRAM_START_ADDR, s_write_buffer, NUM_BYTES);
    handle_error(err, "mram_write failed\r\n");

    printf("\r\nMRAM write successful. Data written: 0x00 to 0x%02X\r\n", NUM_BYTES - 1);

    /* Compare the data written to MRAM with the original buffer */
    if (RESET_VALUE == memcmp((const void *) MRAM_START_ADDR, s_write_buffer, NUM_BYTES))
    {
        printf("\r\nData verification successful\r\n");
    }
    else
    {
        printf("\r\nData verification failed: mismatch detected\r\n");
    }
}

/*******************************************************************************************************************//**
 * @brief       This function reads data from MRAM and prints it to the terminal in hexadecimal format.
 *              The read region is defined by MRAM_START_ADDR and NUM_BYTES macros.
 * @param[in]   None.
 * @retval      None.
 **********************************************************************************************************************/
static void handle_read_command(void)
{
    printf("\r\n\r\nReading from MRAM...");

    /* Read data from MRAM */
    memcpy(s_read_buffer, (uint8_t const *) MRAM_START_ADDR, NUM_BYTES);

    printf("\r\nMRAM read successful\r\n");

    /* Print the data read from MRAM to the terminal */
    printf("\r\nRead data:\r\n");
    for (uint8_t index = RESET_VALUE; NUM_BYTES > index; index++)
    {
        printf("%02X ", s_read_buffer[index]);
        if (0 == (index + 1) % 8)
        {
            printf("\r\n");
        }
    }
}

/*******************************************************************************************************************//**
 * @brief       This function erases MRAM blocks starting from a predefined address.
 *              The erase region is defined by MRAM_START_ADDR and BLOCK_NUM macros.
 * @param[in]   None.
 * @retval      None.
 **********************************************************************************************************************/
static void handle_erase_command(void)
{
    fsp_err_t err = FSP_SUCCESS;

    printf("\r\n\r\nErasing MRAM...");

    err = mram_erase(MRAM_START_ADDR, BLOCK_NUM);
    handle_error(err, "mram_erase failed\r\n");

    printf("\r\nMRAM erase successful\r\n");
}

/*******************************************************************************************************************//**
 * @brief       This function handles an error by printing error message and code, de-initializing the related
 *              modules, and entering a trap state.
 * @param[in]   err                         Error code.
 * @param[in]   err_str                     Error message.
 * @retval      None.
 **********************************************************************************************************************/
static void handle_error(fsp_err_t err, const char * err_str)
{
    if (FSP_SUCCESS != err)
    {
        printf("\r\n[ERR] %s", err_str);
        printf("Returned Error Code: 0x%x\r\n", (unsigned int) err);
        uart_console_flush();
        mram_deinit();
        __BKPT(0);
    }
}

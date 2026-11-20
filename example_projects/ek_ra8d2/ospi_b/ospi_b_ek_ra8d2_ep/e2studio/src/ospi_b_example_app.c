/***********************************************************************************************************************
 * File Name    : ospi_b_example_app.c
 * Description  : Example application layer for the OSPI_B EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - hal_entry.c calls a single function: ospi_b_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "ospi_b_ep.h"
#include "ospi_b_example_app.h"
#include "console/uart_console.h"

/* Global variable */
const char * gp_mode_name = APP_SPI_MODE;

/* Function declarations */
static void ospi_b_set_protocol(spi_flash_protocol_t target_protocol);
static void ospi_b_operation (uint8_t * p_address);
static void handle_error(fsp_err_t err, const char * err_str);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the OSPI_B example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void ospi_b_example_app_run(void)
{
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version = {RESET_VALUE};
    uint32_t flash_id = RESET_VALUE;
    uint8_t * p_ospi_b_address = NULL;

    uint32_t status = uart_console_init();
    if (FSP_SUCCESS != status)
    {
        return;     /* UART failed to open — cannot print anything */
    }

    /* Get FLEX version information */
    R_FSP_VersionGet(&version);

    /* Example project information printed on the terminal */
    printf(BANNER_INFO, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    printf(EP_INFO);

    /* Initialize timer */
    err = timer_init();
    handle_error(err, "timer_init failed\r\n");

    /* Initialize OSPI driver module and Flash device in SPI mode */
    err = ospi_b_init();
    handle_error(err, "ospi_b_init failed\r\n");

    /* Read Flash device ID */
    err = ospi_b_read_device_id(&flash_id);
    handle_error(err, "ospi_b_read_device_id failed\r\n");

    /* Print flash device ID */
    printf("\r\nDevice ID read successfully");
    printf("\r\nManufacturer ID = 0x%x, Memory Interface Type = 0x%x, Density = 0x%x\r\n",
              (uint8_t)flash_id, (uint8_t)(flash_id >> 8), (uint8_t)(flash_id >> 16));

    printf(EP_MENU);

    while (true)
    {
        if (uart_console_has_char())
        {
            int32_t selection = getchar();
            switch (selection)
            {
                case SPI_MODE:
                {
                    /* Set OSPI to SPI mode */
                    ospi_b_set_protocol(SPI_FLASH_PROTOCOL_EXTENDED_SPI);

                    /* Set pointer to flash device memory for SPI operation */
                    p_ospi_b_address = OSPI_B_APP_ADDRESS(OSPI_B_SECTOR_FIRST);

                    /* Perform OSPI operations */
                    gp_mode_name = APP_SPI_MODE;
                    ospi_b_operation(p_ospi_b_address);
                    break;
                }

                case OPI_MODE:
                {
                    /* Set OSPI to OPI mode */
                    ospi_b_set_protocol(SPI_FLASH_PROTOCOL_8D_8D_8D);

                    /* Set pointer to flash device memory for OPI operation */
                    p_ospi_b_address = OSPI_B_APP_ADDRESS(OSPI_B_SECTOR_SECOND);

                    /* Perform OSPI operations */
                    gp_mode_name = APP_OPI_MODE;
                    ospi_b_operation(p_ospi_b_address);
                    break;
                }

                default:
                {
                    printf ("\r\n\r\nInvalid input.\r\n");
                    break;
                }
            }
            printf(EP_MENU);
        }
    }
}

/*******************************************************************************************************************//**
 * @brief     This function switches the OSPI_B interface between SPI and OPI protocol modes.
 * @param[in] target_protocol           Target protocol mode.
 * @retval    None.
 **********************************************************************************************************************/
static void ospi_b_set_protocol(spi_flash_protocol_t target_protocol)
{
    fsp_err_t err = FSP_SUCCESS;
    bsp_octaclk_settings_t octaclk =
    {
        .source_clock = RESET_VALUE,
        .divider = RESET_VALUE
    };
    uint8_t register_value = RESET_VALUE;
    uint32_t write_cmd = RESET_VALUE;
    uint32_t read_cmd = RESET_VALUE;

    memset(&g_transfer, 0, sizeof(g_transfer));

    /* Already in target mode */
    if (target_protocol == g_ospi_b_ctrl.spi_protocol)
    {
        return;
    }

    /* Check supported transition */
    if ((SPI_FLASH_PROTOCOL_EXTENDED_SPI != g_ospi_b_ctrl.spi_protocol) &&
        (SPI_FLASH_PROTOCOL_8D_8D_8D != g_ospi_b_ctrl.spi_protocol))
    {
        handle_error(FSP_ERR_INVALID_MODE, "Flash device does not support this mode\r\n");
    }

    /* Transfer write enable command */
    err = ospi_b_write_enable();
    handle_error(err, "ospi_b_write_enable failed\r\n");

    if (SPI_FLASH_PROTOCOL_8D_8D_8D == target_protocol)
    {
        /* Change to OPI mode */
        write_cmd = SPI_WRITE;
        read_cmd = OPI_READ;
        register_value = OPI_REGISTER;

        /* Change the OCTACLK clock in DDR mode */
        octaclk.source_clock = BSP_CLOCKS_SOURCE_CLOCK_PLL2P;
        octaclk.divider = OPI_CLK_DIV;
    }
    else
    {
        /* Change to SPI mode */
        write_cmd = OPI_WRITE;
        read_cmd = SPI_READ;
        register_value = SPI_REGISTER;

        /* Change the OCTACLK clock in SDR mode */
        octaclk.source_clock = SPI_CLK_SRC;
        octaclk.divider = SPI_CLK_DIV;
    }

    g_transfer = g_ospi_b_direct_transfer[write_cmd];
    g_transfer.data = register_value;
    err = ospi_b_transfer(&g_transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
    handle_error(err, "ospi_b_transfer failed\r\n");

    R_BSP_OctaclkUpdate(&octaclk);

    /* Switch OSPI module mode to target mode */
    err = ospi_b_spi_protocol(target_protocol);
    handle_error(err, "ospi_b_spi_protocol failed\r\n");

    /* Read back and verify register data */
    g_transfer = g_ospi_b_direct_transfer[read_cmd];
    err = ospi_b_transfer(&g_transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
    handle_error(err, "ospi_b_transfer failed\r\n");

    if ((uint8_t)g_transfer.data != register_value)
    {
        handle_error(FSP_ERR_ABORTED, "Data mismatched\r\n");
    }
}

/*******************************************************************************************************************//**
 * @brief     This function performs OSPI operations.
 * @param[in] p_address                 Pointer to flash device memory.
 * @retval    None.
 **********************************************************************************************************************/
static void ospi_b_operation (uint8_t * p_address)
{
    fsp_err_t err = FSP_SUCCESS;
    uint32_t execute_time = RESET_VALUE;

    printf(SUB_MENU, gp_mode_name);

    while (true)
    {
        if (uart_console_has_char())
        {
            int32_t read_opt = getchar();
            switch (read_opt)
            {
                case WRITE_OPERATION:
                {
                    /* Performs OSPI write operation */
                    err = ospi_b_write_operation(p_address, &execute_time);
                    handle_error(err, "ospi_b_write_operation failed\r\n");
                    printf("\r\n\r\n%s: Write %d bytes successfully in %d nanoseconds",
                           gp_mode_name, (uint32_t)(OSPI_B_APP_DATA_SIZE), execute_time);
                    break;
                }

                case READ_OPERATION:
                {
                    /* Performs OSPI read operation */
                    err = ospi_b_read_operation(p_address, &execute_time);
                    handle_error(err, "ospi_b_read_operation failed\r\n");
                    printf("\r\n\r\n%s: Read %d bytes successfully in %d nanoseconds",
                           gp_mode_name, (uint32_t)(OSPI_B_APP_DATA_SIZE), execute_time);

                    /* Compare data read and data written */
                    if (RESET_VALUE == memcmp(&g_read_data, &g_write_data, (size_t)OSPI_B_APP_DATA_SIZE))
                    {
                        printf("\r\n%s: Data matched", gp_mode_name);
                    }
                    else
                    {
                        printf("\r\n%s: Data mismatched", gp_mode_name);
                    }
                    break;
                }

                case ERASE_OPERATION:
                {
                    /* Performs OSPI erase operation */
                    err = ospi_b_erase_operation(p_address, &execute_time);
                    handle_error(err, "ospi_b_erase_operation failed\r\n");
                    printf("\r\n\r\n%s: Erase successfully in %d nanoseconds", gp_mode_name, execute_time);

                    break;
                }

                case EXIT:
                {
                    printf("\r\n\r\nBack to menu\r\n");
                    return;
                }

                default:
                {
                    printf("\r\n\r\nInvalid input.");
                    break;
                }
            }
            printf(SUB_MENU, gp_mode_name);
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
static void handle_error(fsp_err_t err, const char * err_str)
{
    if (FSP_SUCCESS != err)
    {
        printf("\r\n[ERR] %s", err_str);
        printf("Returned Error Code: 0x%x\r\n", (unsigned int)err);
        uart_console_flush();

        /* De-initialize the timer instance if it opened */
        timer_deinit();
        ospi_b_deinit();
        __BKPT(0);
    }
}

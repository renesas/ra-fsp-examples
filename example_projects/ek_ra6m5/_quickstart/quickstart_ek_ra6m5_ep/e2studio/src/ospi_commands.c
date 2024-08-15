/**********************************************************************************************************************
 * File Name    : ospi_commands.c
 * Version      : .
 * Description  : .
 *********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2021 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "ospi_commands.h"

spi_flash_direct_transfer_t g_ospi_test_direct_transfer[OSPI_TEST_COMMAND_MAX] =
{
    [OSPI_TEST_COMMAND_WRITE_ENABLE_SPI_MODE] =
    {
        .command        = OSPI_TEST_WRITE_ENABLE_COMMAND_SPI_MODE,
        .address        = 0U,
        .data           = 0U,
        .command_length = 1U,
        .address_length = 0U,
        .data_length    = 0U,
        .dummy_cycles   = 0U
    },
    [OSPI_TEST_COMMAND_WRITE_DISABLE_SPI_MODE] =
    {
        .command        = OSPI_TEST_WRITE_DISABLE_COMMAND_SPI_MODE,
        .address        = 0U,
        .data           = 0U,
        .command_length = 1U,
        .address_length = 0U,
        .data_length    = 0U,
        .dummy_cycles   = 0U
    },
    [OSPI_TEST_COMMAND_READ_STATUS_SPI_MODE] =
    {
        .command        = OSPI_TEST_READ_STATUS_COMMAND_SPI_MODE,
        .address        = 0U,
        .data           = 0U,
        .command_length = 1U,
        .address_length = 0U,
        .data_length    = 1U,
        .dummy_cycles   = 0U
    },
    [OSPI_TEST_COMMAND_SECTOR_ERASE_SPI_MODE] =
    {
        .command        = OSPI_COMMAND_SECTOR_ERASE_SPI_MODE,
        .address        = 0U,
        .data           = 0U,
        .command_length = 1U,
        .address_length = 4U,
        .data_length    = 0U,
        .dummy_cycles   = 0U
    },
    [OSPI_TEST_COMMAND_SECTOR_ERASE_3SPI_MODE] =
    {
        .command        = OSPI_COMMAND_SECTOR_ERASE_3SPI_MODE,
        .address        = 0U,
        .data           = 0U,
        .command_length = 1U,
        .address_length = 3U,
        .data_length    = 0U,
        .dummy_cycles   = 0U
    },
    [OSPI_TEST_COMMAND_BLOCK_ERASE_SPI_MODE] =
    {
        .command        = OSPI_COMMAND_BLOCK_ERASE_SPI_MODE,
        .address        = 0U,
        .data           = 0U,
        .command_length = 1U,
        .address_length = 4U,
        .data_length    = 0U,
        .dummy_cycles   = 0U
    },
    [OSPI_TEST_COMMAND_WRITE_CR2_SPI_MODE] =
    {
        .command        = OSPI_COMMAND_WCR2_SPI_MODE,
        .address        = 0U,
        .data           = 0U,
        .command_length = 1U,
        .address_length = 4U,
        .data_length    = 1U,
        .dummy_cycles   = 0U
    },
    [OSPI_TEST_COMMAND_WRITE_CR2_3SPI_MODE] =
    {
        .command        = OSPI_COMMAND_WCR2_SPI_MODE,
        .address        = 0U,
        .data           = 0U,
        .command_length = 1U,
        .address_length = 4U,
        .data_length    = 1U,
        .dummy_cycles   = 0U
    },
    [OSPI_TEST_COMMAND_WRITE_CR2_OPI_MODE] =
    {
        .command        = OSPI_COMMAND_WCR2_OPI_MODE,
        .address        = 0U,
        .data           = 0U,
        .command_length = 2U,
        .address_length = 4U,
        .data_length    = 1U,
        .dummy_cycles   = 0U
    },

    [OSPI_TEST_COMMAND_READ_CR2_SPI_MODE] =
    {
        .command        = OSPI_COMMAND_RCR2_SPI_MODE,
        .address        = 0U,
        .data           = 0U,
        .command_length = 1U,
        .address_length = 4U,
        .data_length    = 1U,
        .dummy_cycles   = 0U
    },
    [OSPI_TEST_COMMAND_READ_CR2_3SPI_MODE] =
    {
        .command        = OSPI_COMMAND_RCR2_SPI_MODE,
        .address        = 0U,
        .data           = 0U,
        .command_length = 1U,
        .address_length = 4U,
        .data_length    = 1U,
        .dummy_cycles   = 0U
    },
    [OSPI_TEST_COMMAND_READ_CR2_OPI_MODE] =
    {
        .command        = OSPI_COMMAND_RCR2_OPI_MODE,
        .address        = 0U,
        .data           = 0U,
        .command_length = 2U,
        .address_length = 4U,
        .data_length    = 1U,
        .dummy_cycles   = 4U
    },
    [OSPI_TEST_COMMAND_WRITE_ENABLE_OPI_MODE] =
    {
        .command        = OSPI_TEST_WRITE_ENABLE_COMMAND_OPI_MODE,
        .address        = 0U,
        .data           = 0U,
        .command_length = 2U,
        .address_length = 0U,
        .data_length    = 0U,
        .dummy_cycles   = 0U
    },
    [OSPI_TEST_COMMAND_WRITE_DISABLE_OPI_MODE] =
    {
        .command        = OSPI_TEST_WRITE_DISABLE_COMMAND_OPI_MODE,
        .address        = 0U,
        .data           = 0U,
        .command_length = 2U,
        .address_length = 0U,
        .data_length    = 0U,
        .dummy_cycles   = 0U
    },
    [OSPI_TEST_COMMAND_SECTOR_ERASE_OPI_MODE] =
    {
        .command        = OSPI_COMMAND_SECTOR_ERASE_OPI_MODE,
        .address        = 0U,
        .data           = 0U,
        .command_length = 2U,
        .address_length = 4U,
        .data_length    = 0U,
        .dummy_cycles   = 0U
    },
    [OSPI_TEST_COMMAND_BLOCK_ERASE_OPI_MODE] =
    {
        .command        = OSPI_COMMAND_BLOCK_ERASE_OPI_MODE,
        .address        = 0U,
        .data           = 0U,
        .command_length = 2U,
        .address_length = 4U,
        .data_length    = 0U,
        .dummy_cycles   = 0U
    },
    [OSPI_TEST_COMMAND_READ_STATUS_OPI_MODE] =
    {
        .command        = OSPI_TEST_READ_STATUS_COMMAND_OPI_MODE,
        .address        = 0U,
        .data           = 0U,
        .command_length = 2U,
        .address_length = 4U,
        .data_length    = 1U,
        .dummy_cycles   = 4U
    },

    [OSPI_TEST_COMMAND_BURST_MODE] =
    {
        .command        = OSPI_COMMAND_BURST_MODE,
        .address        = 0U,
        .data           = 02U,
        .command_length = 2U,
        .address_length = 0U,
        .data_length    = 1U,
        .dummy_cycles   = 0U
    }
};

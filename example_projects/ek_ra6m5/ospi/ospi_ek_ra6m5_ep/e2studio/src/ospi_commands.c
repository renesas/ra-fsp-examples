/***********************************************************************************************************************
 * File Name    : ospi_commands.c
 * Description  : Contains function definition.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "ospi_commands.h"
#include "ospi_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup ospi_commands.c
 * @{
 **********************************************************************************************************************/

spi_flash_direct_transfer_t g_ospi_direct_transfer [OSPI_COMMAND_MAX] =
{
    [OSPI_COMMAND_WRITE_ENABLE_SPI_MODE] =
    {
    .command        = OSPI_WRITE_ENABLE_COMMAND_SPI_MODE,
    .address        = OSPI_CR2_ADDRESS_HEX_0,
    .data           = OSPI_CR2_DATA_HEX_00,
    .command_length = LENGTH_ONE,
    .address_length = LENGTH_ZERO,
    .data_length    = LENGTH_ZERO,
    .dummy_cycles   = LENGTH_ZERO
    },
    [OSPI_COMMAND_WRITE_DISABLE_SPI_MODE] =
    {
    .command        = OSPI_WRITE_DISABLE_COMMAND_SPI_MODE,
    .address        = OSPI_CR2_ADDRESS_HEX_0,
    .data           = OSPI_CR2_DATA_HEX_00,
    .command_length = LENGTH_ONE,
    .address_length = LENGTH_ZERO,
    .data_length    = LENGTH_ZERO,
    .dummy_cycles   = LENGTH_ZERO
    },
    [OSPI_COMMAND_READ_STATUS_SPI_MODE] =
    {
    .command        = OSPI_READ_STATUS_COMMAND_SPI_MODE,
    .address        = OSPI_CR2_ADDRESS_HEX_0,
    .data           = OSPI_CR2_DATA_HEX_00,
    .command_length = LENGTH_ONE,
    .address_length = LENGTH_ZERO,
    .data_length    = LENGTH_ONE,
    .dummy_cycles   = LENGTH_ZERO
    },
    [OSPI_COMMAND_SECTOR_ERASE_SPI_MODE] =
    {
    .command        = OSPI_ERASE_COMMAND_SECTOR_SPI_MODE,
    .address        = OSPI_CR2_ADDRESS_HEX_0,
    .data           = OSPI_CR2_DATA_HEX_00,
    .command_length = LENGTH_ONE,
    .address_length = LENGTH_FOUR,
    .data_length    = LENGTH_ZERO,
    .dummy_cycles   = LENGTH_ZERO
    },
    [OSPI_COMMAND_SECTOR_ERASE_3SPI_MODE] =
    {
    .command        = OSPI_ERASE_COMMAND_SECTOR_3SPI_MODE,
    .address        = OSPI_CR2_ADDRESS_HEX_0,
    .data           = OSPI_CR2_DATA_HEX_00,
    .command_length = LENGTH_ONE,
    .address_length = LENGTH_THREE,
    .data_length    = LENGTH_ZERO,
    .dummy_cycles   = LENGTH_ZERO
    },
    [OSPI_COMMAND_BLOCK_ERASE_SPI_MODE] =
    {
    .command        = OSPI_ERASE_COMMAND_BLOCK_SPI_MODE,
    .address        = OSPI_CR2_ADDRESS_HEX_0,
    .data           = OSPI_CR2_DATA_HEX_00,
    .command_length = LENGTH_ONE,
    .address_length = LENGTH_FOUR,
    .data_length    = LENGTH_ZERO,
    .dummy_cycles   = LENGTH_ZERO
    },
    [OSPI_COMMAND_BLOCK_ERASE_3SPI_MODE] =
    {
    .command        = OSPI_ERASE_COMMAND_BLOCK_3SPI_MODE,
    .address        = OSPI_CR2_ADDRESS_HEX_0,
    .data           = OSPI_CR2_DATA_HEX_00,
    .command_length = LENGTH_ONE,
    .address_length = LENGTH_THREE,
    .data_length    = LENGTH_ZERO,
    .dummy_cycles   = LENGTH_ZERO
    },
    [OSPI_COMMAND_WRITE_CR2_SPI_MODE] =
    {
    .command        = OSPI_COMMAND_WCR2_SPI_MODE,
    .address        = OSPI_CR2_ADDRESS_HEX_0,
    .data           = OSPI_CR2_DATA_HEX_00,
    .command_length = LENGTH_ONE,
    .address_length = LENGTH_FOUR,
    .data_length    = LENGTH_ONE,
    .dummy_cycles   = LENGTH_ZERO
    },
    [OSPI_COMMAND_WRITE_CR2_3SPI_MODE] =
    {
    .command        = OSPI_COMMAND_WCR2_SPI_MODE,
    .address        = OSPI_CR2_ADDRESS_HEX_0,
    .data           = OSPI_CR2_DATA_HEX_00,
    .command_length = LENGTH_ONE,
    .address_length = LENGTH_FOUR,
    .data_length    = LENGTH_ONE,
    .dummy_cycles   = LENGTH_ZERO
    },
    [OSPI_COMMAND_WRITE_CR2_OPI_MODE] =
    {
    .command        = OSPI_COMMAND_WCR2_OPI_MODE,
    .address        = OSPI_CR2_ADDRESS_HEX_0,
    .data           = OSPI_CR2_DATA_HEX_00,
    .command_length = LENGTH_TWO,
    .address_length = LENGTH_FOUR,
    .data_length    = LENGTH_ONE,
    .dummy_cycles   = LENGTH_ZERO
    },

    [OSPI_COMMAND_READ_CR2_SPI_MODE] =
    {
    .command        = OSPI_COMMAND_RCR2_SPI_MODE,
    .address        = OSPI_CR2_ADDRESS_HEX_0,
    .data           = OSPI_CR2_DATA_HEX_00,
    .command_length = LENGTH_ONE,
    .address_length = LENGTH_FOUR,
    .data_length    = LENGTH_ONE,
    .dummy_cycles   = LENGTH_ZERO
    },
    [OSPI_COMMAND_READ_CR2_3SPI_MODE] =
    {
    .command        = OSPI_COMMAND_RCR2_SPI_MODE,
    .address        = OSPI_CR2_ADDRESS_HEX_0,
    .data           = OSPI_CR2_DATA_HEX_00,
    .command_length = LENGTH_ONE,
    .address_length = LENGTH_FOUR,
    .data_length    = LENGTH_ONE,
    .dummy_cycles   = LENGTH_ZERO
    },
    [OSPI_COMMAND_READ_CR2_OPI_MODE] =
    {
    .command        = OSPI_COMMAND_RCR2_OPI_MODE,
    .address        = OSPI_CR2_ADDRESS_HEX_0,
    .data           = OSPI_CR2_DATA_HEX_00,
    .command_length = LENGTH_TWO,
    .address_length = LENGTH_FOUR,
    .data_length    = LENGTH_ONE,
    .dummy_cycles   = LENGTH_FOUR
    },
    [OSPI_COMMAND_WRITE_ENABLE_OPI_MODE] =
    {
    .command        = OSPI_WRITE_ENABLE_COMMAND_OPI_MODE,
    .address        = OSPI_CR2_ADDRESS_HEX_0,
    .data           = OSPI_CR2_DATA_HEX_00,
    .command_length = LENGTH_TWO,
    .address_length = LENGTH_ZERO,
    .data_length    = LENGTH_ZERO,
    .dummy_cycles   = LENGTH_ZERO
    },
    [OSPI_COMMAND_WRITE_DISABLE_OPI_MODE] =
    {
    .command        = OSPI_WRITE_DISABLE_COMMAND_OPI_MODE,
    .address        = OSPI_CR2_ADDRESS_HEX_0,
    .data           = OSPI_CR2_DATA_HEX_00,
    .command_length = LENGTH_TWO,
    .address_length = LENGTH_ZERO,
    .data_length    = LENGTH_ZERO,
    .dummy_cycles   = LENGTH_ZERO
    },
    [OSPI_COMMAND_SECTOR_ERASE_OPI_MODE] =
    {
    .command        = OSPI_ERASE_COMMAND_SECTOR_OPI_MODE,
    .address        = OSPI_CR2_ADDRESS_HEX_0,
    .data           = OSPI_CR2_DATA_HEX_00,
    .command_length = LENGTH_TWO,
    .address_length = LENGTH_FOUR,
    .data_length    = LENGTH_ZERO,
    .dummy_cycles   = LENGTH_ZERO
    },
    [OSPI_COMMAND_BLOCK_ERASE_OPI_MODE] =
    {
    .command        = OSPI_ERASE_COMMAND_BLOCK_OPI_MODE,
    .address        = OSPI_CR2_ADDRESS_HEX_0,
    .data           = OSPI_CR2_DATA_HEX_00,
    .command_length = LENGTH_TWO,
    .address_length = LENGTH_FOUR,
    .data_length    = LENGTH_ZERO,
    .dummy_cycles   = LENGTH_ZERO
    },
    [OSPI_COMMAND_READ_STATUS_OPI_MODE] =
    {
    .command        = OSPI_READ_STATUS_COMMAND_OPI_MODE,
    .address        = OSPI_CR2_ADDRESS_HEX_0,
    .data           = OSPI_CR2_DATA_HEX_00,
    .command_length = LENGTH_TWO,
    .address_length = LENGTH_FOUR,
    .data_length    = LENGTH_ONE,
    .dummy_cycles   = LENGTH_FOUR
    }
};
/*******************************************************************************************************************//**
 * @} (end addtogroup ospi_commands)
 **********************************************************************************************************************/

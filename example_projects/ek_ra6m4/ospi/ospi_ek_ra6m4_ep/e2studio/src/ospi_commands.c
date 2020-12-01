/***********************************************************************************************************************
 * File Name    : ospi_commands.c
 * Description  : Contains function definition.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#include "ospi_commands.h"
#include "ospi_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup ospi_commands.c
 * @{
 **********************************************************************************************************************/

spi_flash_direct_transfer_t ospi_direct_transfer [OSPI_COMMAND_MAX] =
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

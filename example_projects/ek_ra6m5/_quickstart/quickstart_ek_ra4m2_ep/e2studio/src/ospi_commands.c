/**********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO
 * THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2021 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * File Name    : ospi_commands.c
 * Version      : .
 * Description  : .
 *********************************************************************************************************************/

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

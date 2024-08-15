/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************//**********************************************************************************************************************
 * File Name    : ospi_commands.c
 * Version      : .
 * Description  : .
 *********************************************************************************************************************/

/***********************************************************************************************************************
 * Copyright [2023] Renesas Electronics Corporation and/or its affiliates.  All Rights Reserved.
 *
 * This software and documentation are supplied by Renesas Electronics America Inc. and may only be used with products
 * of Renesas Electronics Corp. and its affiliates ("Renesas").  No other uses are authorized.  Renesas products are
 * sold pursuant to Renesas terms and conditions of sale.  Purchasers are solely responsible for the selection and use
 * of Renesas products and Renesas assumes no liability.  No license, express or implied, to any intellectual property
 * right is granted by Renesas. This software is protected under all applicable laws, including copyright laws. Renesas
 * reserves the right to change or discontinue this software and/or this documentation. THE SOFTWARE AND DOCUMENTATION
 * IS DELIVERED TO YOU "AS IS," AND RENESAS MAKES NO REPRESENTATIONS OR WARRANTIES, AND TO THE FULLEST EXTENT
 * PERMISSIBLE UNDER APPLICABLE LAW, DISCLAIMS ALL WARRANTIES, WHETHER EXPLICITLY OR IMPLICITLY, INCLUDING WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NONINFRINGEMENT, WITH RESPECT TO THE SOFTWARE OR
 * DOCUMENTATION.  RENESAS SHALL HAVE NO LIABILITY ARISING OUT OF ANY SECURITY VULNERABILITY OR BREACH.  TO THE MAXIMUM
 * EXTENT PERMITTED BY LAW, IN NO EVENT WILL RENESAS BE LIABLE TO YOU IN CONNECTION WITH THE SOFTWARE OR DOCUMENTATION
 * (OR ANY PERSON OR ENTITY CLAIMING RIGHTS DERIVED FROM YOU) FOR ANY LOSS, DAMAGES, OR CLAIMS WHATSOEVER, INCLUDING,
 * WITHOUT LIMITATION, ANY DIRECT, CONSEQUENTIAL, SPECIAL, INDIRECT, PUNITIVE, OR INCIDENTAL DAMAGES; ANY LOST PROFITS,
 * OTHER ECONOMIC DAMAGE, PROPERTY DAMAGE, OR PERSONAL INJURY; AND EVEN IF RENESAS HAS BEEN ADVISED OF THE POSSIBILITY
 * OF SUCH LOSS, DAMAGES, CLAIMS OR COSTS.
 **********************************************************************************************************************/

#include "ospi_commands.h"

spi_flash_direct_transfer_t ospi_test_direct_transfer[OSPI_TEST_COMMAND_MAX] =
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
        .address_length = 3U,
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
        .command        = OSPI_COMMAND_READ_ANY_REGISTER_SPI,
        .address        = 3U,
        .data           = 0U,
        .command_length = 1U,
        .address_length = 3U,
        .data_length    = 1U,
        .dummy_cycles   = 5U
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
        .command        = OSPI_TEST_WRITE_ENABLE_COMMAND_DOPI_MODE,
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
        .command        = OSPI_TEST_READ_STATUS_COMMAND_DOPI_MODE,
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
    },
    [OSPI_TEST_COMMAND_READ_MANUFACTURER_ID] =
    {
        .command        = OSPI_COMMAND_READ_MANUFACTURER_ID,
        .address        = 0U,
        .data           = 0U,
        .command_length = 1U,
        .address_length = 0U,
        .data_length    = 6U,
        .dummy_cycles   = 0U
    }
};


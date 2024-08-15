/***********************************************************************************************************************
 * File Name    : ospi_b_commands.c
 * Description  : Contains function definition.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "ospi_b_commands.h"
#include "ospi_b_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup ospi_b_commands.c
 * @{
 **********************************************************************************************************************/

spi_flash_direct_transfer_t g_ospi_b_direct_transfer [OSPI_B_TRANSFER_MAX] =
{
 /* Transfer structure for SPI mode */
    [OSPI_B_TRANSFER_WRITE_ENABLE_SPI] =
    {
    .command        = OSPI_B_COMMAND_WRITE_ENABLE_SPI,
    .address        = OSPI_B_ADDRESS_DUMMY,
    .data           = OSPI_B_DATA_DUMMY,
    .command_length = OSPI_B_COMMAND_LENGTH_SPI,
    .address_length = OSPI_B_ADDRESS_LENGTH_ZERO,
    .data_length    = OSPI_B_DATA_LENGTH_ZERO,
    .dummy_cycles   = OSPI_B_DUMMY_CYCLE_WRITE_SPI
    },
    [OSPI_B_TRANSFER_WRITE_CFR2V_SPI] =
    {
    .command        = OSPI_B_COMMAND_WRITE_REGISTER_SPI,
    .address        = OSPI_B_ADDRESS_CFR2V_REGISTER,
    .data           = OSPI_B_DATA_CFR2V_REGISTER,
    .command_length = OSPI_B_COMMAND_LENGTH_SPI,
    .address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
    .data_length    = OSPI_B_DATA_LENGTH_ONE,
    .dummy_cycles   = OSPI_B_DUMMY_CYCLE_WRITE_SPI
    },
    [OSPI_B_TRANSFER_WRITE_CFR3V_SPI] =
    {
    .command        = OSPI_B_COMMAND_WRITE_REGISTER_SPI,
    .address        = OSPI_B_ADDRESS_CFR3V_REGISTER,
    .data           = OSPI_B_DATA_CFR3V_REGISTER,
    .command_length = OSPI_B_COMMAND_LENGTH_SPI,
    .address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
    .data_length    = OSPI_B_DATA_LENGTH_ONE,
    .dummy_cycles   = OSPI_B_DUMMY_CYCLE_WRITE_SPI
    },
    [OSPI_B_TRANSFER_WRITE_CFR5V_SPI] =
    {
    .command        = OSPI_B_COMMAND_WRITE_REGISTER_SPI,
    .address        = OSPI_B_ADDRESS_CFR5V_REGISTER,
    .data           = OSPI_B_DATA_DUMMY,
    .command_length = OSPI_B_COMMAND_LENGTH_SPI,
    .address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
    .data_length    = OSPI_B_DATA_LENGTH_ONE,
    .dummy_cycles   = OSPI_B_DUMMY_CYCLE_WRITE_SPI
    },
    [OSPI_B_TRANSFER_READ_STATUS_SPI] =
    {
    .command        = OSPI_B_COMMAND_READ_STATUS_SPI,
    .address        = OSPI_B_ADDRESS_DUMMY,
    .data           = OSPI_B_DATA_DUMMY,
    .command_length = OSPI_B_COMMAND_LENGTH_SPI,
    .address_length = OSPI_B_ADDRESS_LENGTH_ZERO,
    .data_length    = OSPI_B_DATA_LENGTH_ONE,
    .dummy_cycles   = OSPI_B_DUMMY_CYCLE_READ_STATUS_SPI
    },
    [OSPI_B_TRANSFER_READ_CFR2V_SPI] =
    {
    .command        = OSPI_B_COMMAND_READ_REGISTER_SPI,
    .address        = OSPI_B_ADDRESS_CFR2V_REGISTER,
    .data           = OSPI_B_DATA_DUMMY,
    .command_length = OSPI_B_COMMAND_LENGTH_SPI,
    .address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
    .data_length    = OSPI_B_DATA_LENGTH_ONE,
    .dummy_cycles   = OSPI_B_DUMMY_CYCLE_READ_REGISTER_SPI
    },
    [OSPI_B_TRANSFER_READ_CFR3V_SPI] =
    {
    .command        = OSPI_B_COMMAND_READ_REGISTER_SPI,
    .address        = OSPI_B_ADDRESS_CFR3V_REGISTER,
    .data           = OSPI_B_DATA_DUMMY,
    .command_length = OSPI_B_COMMAND_LENGTH_SPI,
    .address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
    .data_length    = OSPI_B_DATA_LENGTH_ONE,
    .dummy_cycles   = OSPI_B_DUMMY_CYCLE_READ_REGISTER_SPI
    },
    [OSPI_B_TRANSFER_READ_CFR5V_SPI] =
    {
    .command        = OSPI_B_COMMAND_READ_REGISTER_SPI,
    .address        = OSPI_B_ADDRESS_CFR5V_REGISTER,
    .data           = OSPI_B_DATA_DUMMY,
    .command_length = OSPI_B_COMMAND_LENGTH_SPI,
    .address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
    .data_length    = OSPI_B_DATA_LENGTH_ONE,
    .dummy_cycles   = OSPI_B_DUMMY_CYCLE_READ_REGISTER_SPI
    },
    [OSPI_B_TRANSFER_READ_DEVICE_ID_SPI] =
    {
    .command        = OSPI_B_COMMAND_READ_DEVICE_ID_SPI,
    .address        = OSPI_B_ADDRESS_DUMMY,
    .data           = OSPI_B_DATA_DUMMY,
    .command_length = OSPI_B_COMMAND_LENGTH_SPI,
    .address_length = OSPI_B_ADDRESS_LENGTH_ZERO,
    .data_length    = OSPI_B_DATA_LENGTH_FOUR,
    .dummy_cycles   = OSPI_B_DUMMY_CYCLE_READ_STATUS_SPI
    },

    /* Transfer structure for OPI mode */
    [OSPI_B_TRANSFER_WRITE_ENABLE_OPI] =
    {
    .command        = OSPI_B_COMMAND_WRITE_ENABLE_OPI,
    .address        = OSPI_B_ADDRESS_DUMMY,
    .data           = OSPI_B_DATA_DUMMY,
    .command_length = OSPI_B_COMMAND_LENGTH_OPI,
    .address_length = OSPI_B_ADDRESS_LENGTH_ZERO,
    .data_length    = OSPI_B_DATA_LENGTH_ZERO,
    .dummy_cycles   = OSPI_B_DUMMY_CYCLE_WRITE_OPI
    },
    [OSPI_B_TRANSFER_WRITE_CFR2V_OPI] =
    {
    .command        = OSPI_B_COMMAND_WRITE_REGISTER_OPI,
    .address        = OSPI_B_ADDRESS_CFR2V_REGISTER,
    .data           = OSPI_B_DATA_CFR2V_REGISTER,
    .command_length = OSPI_B_COMMAND_LENGTH_OPI,
    .address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
    .data_length    = OSPI_B_DATA_LENGTH_TWO,
    .dummy_cycles   = OSPI_B_DUMMY_CYCLE_WRITE_OPI
    },
    [OSPI_B_TRANSFER_WRITE_CFR3V_OPI] =
    {
    .command        = OSPI_B_COMMAND_WRITE_REGISTER_OPI,
    .address        = OSPI_B_ADDRESS_CFR3V_REGISTER,
    .data           = OSPI_B_DATA_CFR3V_REGISTER,
    .command_length = OSPI_B_COMMAND_LENGTH_OPI,
    .address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
    .data_length    = OSPI_B_DATA_LENGTH_TWO,
    .dummy_cycles   = OSPI_B_DUMMY_CYCLE_WRITE_OPI
    },
    [OSPI_B_TRANSFER_WRITE_CFR5V_OPI] =
    {
    .command        = OSPI_B_COMMAND_WRITE_REGISTER_OPI,
    .address        = OSPI_B_ADDRESS_CFR5V_REGISTER,
    .data           = OSPI_B_DATA_DUMMY,
    .command_length = OSPI_B_COMMAND_LENGTH_OPI,
    .address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
    .data_length    = OSPI_B_DATA_LENGTH_TWO,
    .dummy_cycles   = OSPI_B_DUMMY_CYCLE_WRITE_OPI
    },
    [OSPI_B_TRANSFER_READ_STATUS_OPI] =
    {
    .command        = OSPI_B_COMMAND_READ_STATUS_OPI,
    .address        = OSPI_B_ADDRESS_DUMMY,
    .data           = OSPI_B_DATA_DUMMY,
    .command_length = OSPI_B_COMMAND_LENGTH_OPI,
    .address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
    .data_length    = OSPI_B_DATA_LENGTH_TWO,
    .dummy_cycles   = OSPI_B_DUMMY_CYCLE_READ_STATUS_OPI
    },
    [OSPI_B_TRANSFER_READ_CFR2V_OPI] =
    {
    .command        = OSPI_B_COMMAND_READ_REGISTER_OPI,
    .address        = OSPI_B_ADDRESS_CFR2V_REGISTER,
    .data           = OSPI_B_DATA_DUMMY,
    .command_length = OSPI_B_COMMAND_LENGTH_OPI,
    .address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
    .data_length    = OSPI_B_DATA_LENGTH_TWO,
    .dummy_cycles   = OSPI_B_DUMMY_CYCLE_READ_REGISTER_OPI
    },
    [OSPI_B_TRANSFER_READ_CFR3V_OPI] =
    {
    .command        = OSPI_B_COMMAND_READ_REGISTER_OPI,
    .address        = OSPI_B_ADDRESS_CFR3V_REGISTER,
    .data           = OSPI_B_DATA_DUMMY,
    .command_length = OSPI_B_COMMAND_LENGTH_OPI,
    .address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
    .data_length    = OSPI_B_DATA_LENGTH_TWO,
    .dummy_cycles   = OSPI_B_DUMMY_CYCLE_READ_REGISTER_OPI
    },
    [OSPI_B_TRANSFER_READ_CFR5V_OPI] =
    {
    .command        = OSPI_B_COMMAND_READ_REGISTER_OPI,
    .address        = OSPI_B_ADDRESS_CFR5V_REGISTER,
    .data           = OSPI_B_DATA_DUMMY,
    .command_length = OSPI_B_COMMAND_LENGTH_OPI,
    .address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
    .data_length    = OSPI_B_DATA_LENGTH_TWO,
    .dummy_cycles   = OSPI_B_DUMMY_CYCLE_READ_REGISTER_OPI
    },
    [OSPI_B_TRANSFER_READ_DEVICE_ID_OPI] =
    {
    .command        = OSPI_B_COMMAND_READ_DEVICE_ID_OPI,
    .address        = OSPI_B_ADDRESS_DUMMY,
    .data           = OSPI_B_DATA_DUMMY,
    .command_length = OSPI_B_COMMAND_LENGTH_OPI,
    .address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
    .data_length    = OSPI_B_DATA_LENGTH_FOUR,
    .dummy_cycles   = OSPI_B_DUMMY_CYCLE_READ_STATUS_OPI
    },
};

/*******************************************************************************************************************//**
 * @} (end addtogroup ospi_b_commands.c)
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * File Name    : ospi_commands.h
 * Description  : Contains data structures and functions used in ospi_commands.h
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef OSPI_COMMANDS_H_
#define OSPI_COMMANDS_H_

#include "hal_data.h"

#define DEVICE_SECTOR_SIZE                              (4096U)
#define DEVICE_BLOCK_SIZE                               (65536U)
#define OSPI_SPI_CMD_ERASE_SECTOR                       (0x21U)
#define OSPI_SPI_CMD_ERASE_BLOCK                        (0xDCU)
#define OSPI_SPI_CMD_ERASE_CHIP                         (0xC7U)
#define OSPI_OPI_CMD_ERASE_SECTOR                       (0x21DEU)
#define OSPI_OPI_CMD_ERASE_BLOCK                        (0xDC23U)
#define OSPI_OPI_CMD_ERASE_CHIP                         (0xC738U)
#define OSPI_COMMAND_RCR2_SPI_MODE                      (0x71U)
#define OSPI_COMMAND_WCR2_SPI_MODE                      (0x72U)
#define OSPI_COMMAND_RCR2_OPI_MODE                      (0x718EU)
#define OSPI_WRITE_ENABLE_COMMAND_OPI_MODE              (0x06F9U)
#define OSPI_WRITE_DISABLE_COMMAND_OPI_MODE             (0x04FBU)
#define OSPI_READ_STATUS_COMMAND_OPI_MODE               (0x05FAU)
#define OSPI_COMMAND_PAGE_PROGRAM_OPI_MODE              (0x12EDU)
#define OSPI_COMMAND_NORMAL_READ_OPI_MODE               (0xEC13U)
#define OSPI_COMMAND_READ_DUAL_OPI_MODE                 (0xEE11U)
#define OSPI_ERASE_COMMAND_SECTOR_OPI_MODE              (0x21DEU)
#define OSPI_ERASE_COMMAND_BLOCK_OPI_MODE               (0xDC23U)
#define OSPI_ERASE_COMMAND_CHIP_OPI_MODE                (0x609FU)
#define OSPI_COMMAND_WCR2_OPI_MODE                      (0x728DU)
#define OSPI_SPI_CMD_READ_DEVICE_ID                     (0x9FU)
#define OSPI_WRITE_ENABLE_COMMAND_SPI_MODE              (0x06U)
#define OSPI_WRITE_DISABLE_COMMAND_SPI_MODE             (0x04U)
#define OSPI_READ_STATUS_COMMAND_SPI_MODE               (0x05U)
#define OSPI_COMMAND_PAGE_PROGRAM_SPI_MODE              (0x12U)
#define OSPI_COMMAND_NORMAL_READ_SPI_MODE               (0x13U)
#define OSPI_COMMAND_NORMAL_FASTREAD_SPI_MODE           (0x0CU)
#define OSPI_ERASE_COMMAND_SECTOR_SPI_MODE              (0x21U)
#define OSPI_ERASE_COMMAND_BLOCK_SPI_MODE               (0xDCU)
#define OSPI_ERASE_COMMAND_CHIP_SPI_MODE                (0x60U)
#define OSPI_COMMAND_PAGE_PROGRAM_3SPI_MODE             (0x02U)
#define OSPI_COMMAND_NORMAL_READ_3SPI_MODE              (0x03U)
#define OSPI_COMMAND_NORMAL_FASTREAD_3SPI_MODE          (0x0BU)
#define OSPI_ERASE_COMMAND_SECTOR_3SPI_MODE             (0x20U)
#define OSPI_ERASE_COMMAND_BLOCK_3SPI_MODE              (0xD8U)
#define OSPI_ERASE_COMMAND_CHIP_3SPI_MODE               (0x60U)

#define OSPI_CR2_SOPI_MODE_BIT_MASK                     (0x01U)
#define OSPI_CR2_DOPI_MODE_BIT_MASK                     (0x02U)
#define OSPI_CR2_DQS_ON_SOPI_MODE_BIT_MASK              (0x2U)
#define OSPI_WEN_BIT_MASK                               (2U)
#define OSPI_DEVICE_ID                                  (0x3A85C2)
#define OSPI_MODE_OPI                                   (1)
#define OSPI_MODE_SPI                                   (0)


typedef enum e_ospi_command
{
    OSPI_COMMAND_WRITE_ENABLE_SPI_MODE = 0,
    OSPI_COMMAND_WRITE_DISABLE_SPI_MODE,
    OSPI_COMMAND_READ_STATUS_SPI_MODE,
    OSPI_COMMAND_SECTOR_ERASE_SPI_MODE,
    OSPI_COMMAND_BLOCK_ERASE_SPI_MODE,
    OSPI_COMMAND_WRITE_CR2_SPI_MODE,
    OSPI_COMMAND_READ_CR2_SPI_MODE,

    OSPI_COMMAND_WRITE_CR2_3SPI_MODE,
    OSPI_COMMAND_SECTOR_ERASE_3SPI_MODE,
    OSPI_COMMAND_READ_CR2_3SPI_MODE,
    OSPI_COMMAND_BLOCK_ERASE_3SPI_MODE,

    OSPI_COMMAND_WRITE_ENABLE_OPI_MODE,
    OSPI_COMMAND_WRITE_DISABLE_OPI_MODE,
    OSPI_COMMAND_SECTOR_ERASE_OPI_MODE,
    OSPI_COMMAND_BLOCK_ERASE_OPI_MODE,
    OSPI_COMMAND_WRITE_CR2_OPI_MODE,
    OSPI_COMMAND_READ_CR2_OPI_MODE,
    OSPI_COMMAND_READ_STATUS_OPI_MODE,
    OSPI_COMMAND_MAX
} ospi_command_t;

static const spi_flash_erase_command_t spi_erase_command_list[] =
{
 { .command = OSPI_SPI_CMD_ERASE_SECTOR, .size = DEVICE_SECTOR_SIZE },

 { .command = OSPI_SPI_CMD_ERASE_BLOCK, .size = DEVICE_BLOCK_SIZE },

 { .command = OSPI_SPI_CMD_ERASE_CHIP, .size = SPI_FLASH_ERASE_SIZE_CHIP_ERASE },
};

static const spi_flash_erase_command_t opi_erase_command_list[] =
{
 { .command = OSPI_OPI_CMD_ERASE_SECTOR, .size = DEVICE_SECTOR_SIZE },

 { .command = OSPI_OPI_CMD_ERASE_BLOCK, .size = DEVICE_BLOCK_SIZE },

 { .command = OSPI_OPI_CMD_ERASE_CHIP, .size = SPI_FLASH_ERASE_SIZE_CHIP_ERASE },
};

#endif /* OSPI_COMMANDS_H_ */

/***********************************************************************************************************************
 * File Name    : ospi_commands.h
 * Description  : .
 **********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2021 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "hal_data.h"

#ifndef OSPI_COMMANDS_H_
#define OSPI_COMMANDS_H_

#define OSPI_TEST_WRITE_ENABLE_COMMAND_SPI_MODE     (0x06U)
#define OSPI_TEST_WRITE_DISABLE_COMMAND_SPI_MODE    (0x04U)
#define OSPI_TEST_READ_STATUS_COMMAND_SPI_MODE      (0x05U)
#define OSPI_COMMAND_PAGE_PROGRAM_SPI_MODE          (0x12U)
#define OSPI_COMMAND_NORMAL_READ_SPI_MODE           (0x13U)
#define OSPI_COMMAND_NORMAL_FASTREAD_SPI_MODE       (0x0CU)
#define OSPI_COMMAND_SECTOR_ERASE_SPI_MODE          (0x21U)
#define OSPI_COMMAND_BLOCK_ERASE_SPI_MODE           (0xDCU)
#define OSPI_COMMAND_PAGE_PROGRAM_3SPI_MODE         (0x02U)
#define OSPI_COMMAND_NORMAL_READ_3SPI_MODE          (0x03U)
#define OSPI_COMMAND_NORMAL_FASTREAD_3SPI_MODE      (0x0BU)
#define OSPI_COMMAND_SECTOR_ERASE_3SPI_MODE         (0x20U)
#define OSPI_COMMAND_BLOCK_ERASE_3SPI_MODE          (0xD8U)
#define OSPI_COMMAND_CHIP_ERASE_3SPI_MODE           (0x60U)
#define OSPI_COMMAND_BURST_MODE                     (0xC0U)
#define OSPI_COMMAND_RCR2_SPI_MODE                  (0x71U)
#define OSPI_COMMAND_WCR2_SPI_MODE                  (0x72U)
#define OSPI_COMMAND_RCR2_OPI_MODE                  (0x718EU)
#define OSPI_TEST_WRITE_ENABLE_COMMAND_OPI_MODE     (0x06F9U)
#define OSPI_TEST_WRITE_DISABLE_COMMAND_OPI_MODE    (0x04FBU)
#define OSPI_TEST_READ_STATUS_COMMAND_OPI_MODE      (0x05FAU)
#define OSPI_COMMAND_PAGE_PROGRAM_OPI_MODE          (0x12EDU)
#define OSPI_COMMAND_NORMAL_READ_OPI_MODE           (0xEC13U)
#define OSPI_COMMAND_READ_DUAL_OPI_MODE             (0xEE11U)
#define OSPI_COMMAND_SECTOR_ERASE_OPI_MODE          (0x21DEU)
#define OSPI_COMMAND_BLOCK_ERASE_OPI_MODE           (0xDC23U)
#define OSPI_COMMAND_CHIP_ERASE_OPI_MODE            (0x609FU)
#define OSPI_COMMAND_WCR2_OPI_MODE                  (0x728DU)

typedef enum e_ospi_test_command
{
    OSPI_TEST_COMMAND_WRITE_ENABLE_SPI_MODE = 0,
    OSPI_TEST_COMMAND_WRITE_DISABLE_SPI_MODE,
    OSPI_TEST_COMMAND_READ_STATUS_SPI_MODE,
    OSPI_TEST_COMMAND_SECTOR_ERASE_SPI_MODE,
    OSPI_TEST_COMMAND_BLOCK_ERASE_SPI_MODE,
    OSPI_TEST_COMMAND_WRITE_CR2_SPI_MODE,
    OSPI_TEST_COMMAND_READ_CR2_SPI_MODE,

    OSPI_TEST_COMMAND_WRITE_CR2_3SPI_MODE,
    OSPI_TEST_COMMAND_SECTOR_ERASE_3SPI_MODE,
    OSPI_TEST_COMMAND_READ_CR2_3SPI_MODE,

    OSPI_TEST_COMMAND_WRITE_ENABLE_OPI_MODE,
    OSPI_TEST_COMMAND_WRITE_DISABLE_OPI_MODE,
    OSPI_TEST_COMMAND_SECTOR_ERASE_OPI_MODE,
    OSPI_TEST_COMMAND_BLOCK_ERASE_OPI_MODE,
    OSPI_TEST_COMMAND_WRITE_CR2_OPI_MODE,
    OSPI_TEST_COMMAND_READ_CR2_OPI_MODE,
    OSPI_TEST_COMMAND_READ_STATUS_OPI_MODE,
    OSPI_TEST_COMMAND_BURST_MODE,
    OSPI_TEST_COMMAND_MAX
} ospi_test_command_t;

extern spi_flash_direct_transfer_t        g_ospi_test_direct_transfer[OSPI_TEST_COMMAND_MAX];

#endif /* OSPI_COMMANDS_H_ */

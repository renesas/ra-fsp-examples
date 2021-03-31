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
/***********************************************************************************************************************
 * File Name    : ospi_commands.h
 * Description  : .
 **********************************************************************************************************************/

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

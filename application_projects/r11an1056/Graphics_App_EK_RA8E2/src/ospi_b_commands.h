/***********************************************************************************************************************
 * File Name    : ospi_b_commands.h
 * Description  : Contains data structures and functions used in ospi_b.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2023 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef OSPI_B_COMMANDS_H_
#define OSPI_B_COMMANDS_H_

#include "hal_data.h"

#define RESET_VALUE (0x00)

#define OSPI_B_PRV_COMSTT_MEMACCCH_MASK_AP             (0x03 << R_XSPI0_COMSTT_MEMACCCH_Pos)
#define OSPI_B_PRV_BMCTL1_CLEAR_PREFETCH_MASK_AP        (0x03 << R_XSPI0_BMCTL1_PBUFCLRCH_Pos)
/* Macro for OSPI command code */
#define OSPI_B_COMMAND_WRITE_ENABLE_SPI             (0x06)
#define OSPI_B_COMMAND_WRITE_ENABLE_OPI             (0x06F9)
#define OSPI_B_COMMAND_WCR2_SPI                     (0x72)
#define OSPI_B_COMMAND_WCR2_OPI                     (0x728D)
#define OSPI_B_COMMAND_WRITE_REGISTER_SPI           (OSPI_B_COMMAND_WCR2_SPI)
#define OSPI_B_COMMAND_WRITE_REGISTER_OPI           (OSPI_B_COMMAND_WCR2_OPI)
#define OSPI_B_COMMAND_READ_STATUS_SPI              (0x05)
#define OSPI_B_COMMAND_READ_STATUS_OPI              (0x05FA)
#define OSPI_B_COMMAND_RCR2_SPI                     (0x71)
#define OSPI_B_COMMAND_RCR2_OPI                     (0x718E)
#define OSPI_B_COMMAND_READ_REGISTER_SPI            (OSPI_B_COMMAND_RCR2_SPI)
#define OSPI_B_COMMAND_READ_REGISTER_OPI            (OSPI_B_COMMAND_RCR2_OPI)
#define OSPI_B_COMMAND_READ_DEVICE_ID_SPI           (0x9F)
#define OSPI_B_COMMAND_READ_DEVICE_ID_OPI           (0x9F60)

/* Macro for OSPI command length */
#define OSPI_B_COMMAND_LENGTH_SPI                   (1U)
#define OSPI_B_COMMAND_LENGTH_OPI                   (2U)

/* Macro for OSPI transfer address */
#define OSPI_B_ADDRESS_DUMMY                        (0U)
#define OSPI_B_ADDRESS_LENGTH_ZERO                  (0U)
#define OSPI_B_ADDRESS_LENGTH_THREE                 (3U)
#define OSPI_B_ADDRESS_LENGTH_FOUR                  (4U)

/* Macro for OSPI transfer data */
#define OSPI_B_DATA_DUMMY                           (0U)
#define OSPI_B_DATA_LENGTH_ZERO                     (0U)
#define OSPI_B_DATA_LENGTH_ONE                      (1U)
#define OSPI_B_DATA_LENGTH_TWO                      (2U)
#define OSPI_B_DATA_LENGTH_THREE                    (3U)
#define OSPI_B_DATA_LENGTH_FOUR                     (4U)

/* Flash device timing */
#define OSPI_B_TIME_UNIT                    (BSP_DELAY_UNITS_MICROSECONDS)
#define OSPI_B_TIME_RESET_SETUP             (2U)             /*  Type 50ns */
#define OSPI_B_TIME_RESET_PULSE             (1000U)          /*  Type 500us */
#define OSPI_B_TIME_ERASE_256K              (1500000U)       /*  Type 256KB sector is 331 KBps -> Type 0.773s */
#define OSPI_B_TIME_ERASE_4K                (100000U)        /*  Type 4KB sector is 95 KBps -> Type 0.042s */
#define OSPI_B_TIME_WRITE                   (10000U)         /*  Type 256B page (4KB/256KB) is 595/533 KBps -> Type */

/* Flash device status bit */
#define OSPI_B_WEN_BIT_MASK                 (0x00000002)
#define OSPI_B_BUSY_BIT_MASK                (0x00000001)

/* Macro for OSPI transfer dummy cycles */
#define OSPI_B_DUMMY_CYCLE_WRITE_SPI                (0U)
#define OSPI_B_DUMMY_CYCLE_WRITE_OPI                (0U)
#define OSPI_B_DUMMY_CYCLE_READ_STATUS_SPI          (0U)
#define OSPI_B_DUMMY_CYCLE_READ_STATUS_OPI          (4U)
#define OSPI_B_DUMMY_CYCLE_READ_REGISTER_SPI        (0U)
#define OSPI_B_DUMMY_CYCLE_READ_REGISTER_OPI        (4U)
#define OSPI_B_DUMMY_CYCLE_READ_MEMORY_SPI          (0U)
#define OSPI_B_DUMMY_CYCLE_READ_MEMORY_OPI          (10U)

/* Flash device sector size */
#define OSPI_B_SECTOR_SIZE_4K               (0x1000)
/* Flash device address space mapping */
#define OSPI_B_CS0_START_ADDRESS            (0x80000000)
#define OSPI_B_CS1_START_ADDRESS            (0x90000000)
#define OSPI_B_APP_ADDRESS(sector_no)    ((uint8_t *)(OSPI_B_CS1_START_ADDRESS + ((sector_no) * OSPI_B_SECTOR_SIZE_4K)))
#define OSPI_B_SECTOR_FIRST                 (0U)
#define OSPI_B_SECTOR_SECOND                (1U)
#define OSPI_B_SECTOR_THREE                 (2U)
#define OSPI_B_SECTOR_FOUR                  (3U)
#define OSPI_B_APP_DATA_SIZE                (64U)

/* Macro for flash device register address */
#define OSPI_B_ADDRESS_CR2_000H_REGISTER            (0x00000000)
#define OSPI_B_ADDRESS_CR2_300H_REGISTER            (0x00000300)

/* Macros for configure flash device */
#define OSPI_B_DATA_SET_SPI_CR2_000H                (0x00)
#define OSPI_B_DATA_SET_OPI_CR2_000H                (0x02)
#define OSPI_B_DATA_SET_CR2_300H                    (0x05)

typedef enum e_ospi_b_transfer
{

    OSPI_B_TRANSFER_WRITE_ENABLE_SPI = 0,
    OSPI_B_TRANSFER_WRITE_CR2_000H_SPI,
    OSPI_B_TRANSFER_WRITE_CR2_300H_SPI,
    OSPI_B_TRANSFER_READ_STATUS_SPI,
    OSPI_B_TRANSFER_READ_CR2_000H_SPI,
    OSPI_B_TRANSFER_READ_CR2_300H_SPI,
    OSPI_B_TRANSFER_READ_DEVICE_ID_SPI,
    OSPI_B_TRANSFER_WRITE_ENABLE_OPI,
    OSPI_B_TRANSFER_WRITE_CR2_000H_OPI,
    OSPI_B_TRANSFER_READ_STATUS_OPI,
    OSPI_B_TRANSFER_READ_CR2_000H_OPI,
    OSPI_B_TRANSFER_READ_CR2_300H_OPI,
    OSPI_B_TRANSFER_READ_DEVICE_ID_OPI,
    OSPI_B_TRANSFER_MAX
} ospi_b_transfer_t;

#endif /* OSPI_B_COMMANDS_H_ */

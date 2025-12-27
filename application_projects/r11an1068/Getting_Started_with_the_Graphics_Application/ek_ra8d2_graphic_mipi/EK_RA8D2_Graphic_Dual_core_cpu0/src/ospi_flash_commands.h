/***********************************************************************************************************************
 * File Name    : ospi_flash_commands.h
 * Description  : Contains data structures and functions used in ospi_b_control.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef OSPI_FLASH_COMMANDS_H_
#define OSPI_FLASH_COMMANDS_H_

/* Macros for OSPI command code */
#define OSPI_B_COMMAND_WRITE_ENABLE_SPI             (0x06)
#define OSPI_B_COMMAND_WRITE_ENABLE_OPI             (0x0606)
#define OSPI_B_COMMAND_READ_STATUS_SPI              (0x05)
#define OSPI_B_COMMAND_READ_STATUS_OPI              (0x0505)
#define OSPI_B_COMMAND_WCR_SPI                      (0x81)
#define OSPI_B_COMMAND_WCR_OPI                      (0x8181)
#define OSPI_B_COMMAND_WRITE_REGISTER_SPI           (OSPI_B_COMMAND_WCR_SPI)
#define OSPI_B_COMMAND_WRITE_REGISTER_OPI           (OSPI_B_COMMAND_WCR_OPI)
#define OSPI_B_COMMAND_RCR_SPI                      (0x85)
#define OSPI_B_COMMAND_RCR_OPI                      (0x8585)
#define OSPI_B_COMMAND_READ_REGISTER_SPI            (OSPI_B_COMMAND_RCR_SPI)
#define OSPI_B_COMMAND_READ_REGISTER_OPI            (OSPI_B_COMMAND_RCR_OPI)
#define OSPI_B_COMMAND_READ_DEVICE_ID_SPI           (0x9F)
#define OSPI_B_COMMAND_READ_DEVICE_ID_OPI           (0x9F9F)
#define OSPI_B_COMMAND_ENTER_4BYTES_ADDR            (0xB7)
#define OSPI_B_COMMAND_READ_FLAG_STATUS             (0x70)
#define OSPI_B_COMMAND_RESET_ENABLE                 (0x66)
#define OSPI_B_COMMAND_RESET_MEMORY                 (0x99)

/* Macros for OSPI command length */
#define OSPI_B_COMMAND_LENGTH_SPI                   (1U)
#define OSPI_B_COMMAND_LENGTH_OPI                   (2U)

/* Macros for OSPI transfer address */
#define OSPI_B_ADDRESS_DUMMY                        (0U)
#define OSPI_B_ADDRESS_LENGTH_ZERO                  (0U)
#define OSPI_B_ADDRESS_LENGTH_THREE                 (3U)
#define OSPI_B_ADDRESS_LENGTH_FOUR                  (4U)

/* Macros for OSPI transfer data */
#define OSPI_B_DATA_DUMMY                           (0U)
#define OSPI_B_DATA_LENGTH_ZERO                     (0U)
#define OSPI_B_DATA_LENGTH_ONE                      (1U)
#define OSPI_B_DATA_LENGTH_TWO                      (2U)
#define OSPI_B_DATA_LENGTH_THREE                    (3U)
#define OSPI_B_DATA_LENGTH_FOUR                     (4U)

/* Macros for OSPI transfer dummy cycles */
#define OSPI_B_DUMMY_CYCLE_WRITE_SPI                (0U)
#define OSPI_B_DUMMY_CYCLE_WRITE_OPI                (0U)
#define OSPI_B_DUMMY_CYCLE_READ_STATUS_SPI          (0U)
#define OSPI_B_DUMMY_CYCLE_READ_STATUS_OPI          (8U)

#define OSPI_B_DUMMY_CYCLE_READ_REGISTER_SPI        (8U)

#define OSPI_B_DUMMY_CYCLE_READ_REGISTER_OPI        (8U)

/* Macros for flash device register address */
#define OSPI_B_ADDRESS_CR_00H_REGISTER              (0x00000000)
#define OSPI_B_ADDRESS_CR_01H_REGISTER              (0x00000001)
#define OSPI_B_ADDRESS_CR_05H_REGISTER              (0x00000005)
#define OSPI_B_ADDRESS_CR_06H_REGISTER              (0x00000006)

/* Macros for configure flash device */
#define OSPI_B_DATA_SET_SPI_CR_00H                  (0xDF)
#define OSPI_B_DATA_SET_OPI_CR_00H                  (0xE7)
#define OSPI_B_DATA_SET_CR_01H                      (0x0D)
#define OSPI_B_EN_XIP_CODE                          (0xFE)
#define OSPI_B_DIS_XIP_CODE                         (0xFF)

typedef enum e_ospi_b_transfer
{
    OSPI_B_TRANSFER_WRITE_ENABLE_SPI = 0,
    OSPI_B_TRANSFER_WRITE_CR_00H_SPI,
    OSPI_B_TRANSFER_WRITE_CR_01H_SPI,
    OSPI_B_TRANSFER_ENTER_4BYTES_ADDRESS,

    OSPI_B_TRANSFER_READ_STATUS_SPI,
    OSPI_B_TRANSFER_READ_CR_00H_SPI,
    OSPI_B_TRANSFER_READ_CR_01H_SPI,

    OSPI_B_TRANSFER_READ_DEVICE_ID_SPI,

    OSPI_B_TRANSFER_WRITE_ENABLE_OPI,
    OSPI_B_TRANSFER_WRITE_CR_00H_OPI,
    OSPI_B_TRANSFER_READ_STATUS_OPI,
    OSPI_B_TRANSFER_READ_CR_00H_OPI,
    OSPI_B_TRANSFER_READ_CR_01H_OPI,
    OSPI_B_TRANSFER_READ_DEVICE_ID_OPI,

    OSPI_B_TRANSFER_ENTER_XIP_MODE_OPI,
    OSPI_B_TRANSFER_RESET_ENABLE,
    OSPI_B_TRANSFER_RESET_MEMORY,
    OSPI_B_TRANSFER_MAX
} ospi_b_transfer_t;

#endif /* OSPI_FLASH_COMMANDS_H_ */

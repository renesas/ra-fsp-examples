/***********************************************************************************************************************
 * File Name    : board_ospi.h
 * Description  : Contains macros data structures and functions used in board_ospi.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef BOARD_OSPI_H_
#define BOARD_OSPI_H_

#include "common_utils.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
/* Macro for OSPI command code */
#define OSPI_SECTOR_SIZE                                (4096U)
#define OSPI_32K_SIZE                                   (32768U)
#define OSPI_BLOCK_SIZE                                 (65536U)

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

#define OSPI_WEN_BIT_MASK                               (2U)

#define SPI_MODE                                        (0U)
#define SOPI_MODE                                       (1U)
#define DOPI_MODE                                       (2U)
#define INITIAL_INDEX                                   (0U)
#define LENGTH_ZERO                                     (0U)
#define LENGTH_ONE                                      (1U)
#define LENGTH_TWO                                      (2U)
#define LENGTH_THREE                                    (3U)
#define LENGTH_FOUR                                     (4U)

#define OSPI_CR2_DATA_HEX_02                            (0x2U)
#define OSPI_CR2_DATA_HEX_00                            (0x0U)
#define OSPI_CR2_DATA_HEX_05                            (0x5U)
#define OSPI_CR2_DATA_HEX_07                            (0x7U)
#define OSPI_CR2_ADDRESS_HEX_300                        (0x300U)
#define OSPI_CR2_ADDRESS_HEX_200                        (0x200U)
#define OSPI_CR2_ADDRESS_HEX_0                          (0x0U)

#define OSPI_CS0_START_ADDRESS                          (0x68000000)
#define OSPI_CS1_START_ADDRESS                          (0x70000000)

/* Macros for periodic delay and periodic line feed */
#define PERIODIC_PRINT_OUT                              (0x10000U)
#define PERIODIC_LINE_FEED                              (0x400000U)

/* Flash device timing */
#define OSPI_TIME_UNIT                                  (BSP_DELAY_UNITS_MICROSECONDS)
#define OSPI_TIME_RESET_SETUP                           (2U)        //  Type 50ns
#define OSPI_TIME_RESET_PULSE                           (1000U)     //  Type 500us
#define OSPI_TIME_ERASE_4K                              (400000U)   //  Typical sector erase cycle time is 25 ms (Max: 400 ms)
#define OSPI_TIME_WRITE                                 (750U)      //  Typical page program time is 0.15 ms (Max: 0.75 ms)

#define OM_RESET                                        (BSP_IO_PORT_06_PIN_15)
#define SPI_REFERENCE_ADDRESS                           (0x70001000)
#define SOPI_REFERNCE_ADDRESS                           (0x70002000)
#define DOPI_REFERNECE_ADDRESS                          (0x70003000)

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
 { .command = OSPI_SPI_CMD_ERASE_SECTOR, .size = OSPI_SECTOR_SIZE },

 { .command = OSPI_SPI_CMD_ERASE_BLOCK, .size = OSPI_BLOCK_SIZE },

 { .command = OSPI_SPI_CMD_ERASE_CHIP, .size = SPI_FLASH_ERASE_SIZE_CHIP_ERASE },
};

static const spi_flash_erase_command_t opi_erase_command_list[] =
{
 { .command = OSPI_OPI_CMD_ERASE_SECTOR, .size = OSPI_SECTOR_SIZE },

 { .command = OSPI_OPI_CMD_ERASE_BLOCK, .size = OSPI_BLOCK_SIZE },

 { .command = OSPI_OPI_CMD_ERASE_CHIP, .size = SPI_FLASH_ERASE_SIZE_CHIP_ERASE },
};

/***********************************************************************************************************************
 * Exported global functions (to be accessed by other files)
 **********************************************************************************************************************/
/***********************************************************************************************************************
 *  Function Name: ospi_init
 *  Description  : This function initializes OSPI module and Flash device.
 *  Arguments    : None
 *  Return Value : FSP_SUCCESS     Upon successful initialization of OSPI module and Flash device
 *                 Any Other Error code apart from FSP_SUCCESS Unsuccessful operation
 **********************************************************************************************************************/
fsp_err_t ospi_init (void);

/***********************************************************************************************************************
 *  Function Name: erase_ospi_flash
 *  Description  : This function erases the flash memory using lower level flash APIs.
 *  Arguments    : erase_size      The size of flash to be erased
 *  Return Value : FSP_SUCCESS     Upon successful operation
 *                 Any Other Error code apart from FSP_SUCCESS Unsuccessful operation
 **********************************************************************************************************************/
fsp_err_t erase_ospi_flash (uint32_t erase_size);

#endif /* BOARD_OSPI_H_ */

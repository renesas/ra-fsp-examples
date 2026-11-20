/***********************************************************************************************************************
 * File Name    : board_ospi.h
 * Description  : Contains macros data structures and functions used in board_ospi.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2024 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef BOARD_OSPI_H_
#define BOARD_OSPI_H_

#include "hal_data.h"

/* Macro for the reset value */
#define RESET_VALUE                                 (0x00)

/* Macro for OSPI reset pin */
#if defined (BOARD_RA8T2_EK)
#define OM_RESET                                    (BSP_IO_PORT_12_PIN_07)
#else
#define OM_RESET                                    (BSP_IO_PORT_01_PIN_06)
#endif /* BOARD_RA8T2_EK */

/* Macros for OSPI command code */
#define OSPI_B_COMMAND_ENTER_4_ADDR_SPI             (0xB7)
#define OSPI_B_COMMAND_EXIT_4_ADDR_SPI              (0xB8)
#define OSPI_B_COMMAND_ERASE_CHIP_SPI               (0x60)

#if defined (BOARD_RA8D1_EK)
#define OSPI_B_COMMAND_WRITE_ENABLE_SPI             (0x06)
#define OSPI_B_COMMAND_WRITE_ENABLE_OPI             (0x0606)
#define OSPI_B_COMMAND_WRITE_REGISTER_SPI           (0x71)
#define OSPI_B_COMMAND_WRITE_REGISTER_OPI           (0x7171)
#define OSPI_B_COMMAND_READ_STATUS_SPI              (0x05)
#define OSPI_B_COMMAND_READ_STATUS_OPI              (0x0505)
#define OSPI_B_COMMAND_READ_REGISTER_SPI            (0x65)
#define OSPI_B_COMMAND_READ_REGISTER_OPI            (0x6565)
#define OSPI_B_COMMAND_READ_DEVICE_ID_SPI           (0x9F)
#define OSPI_B_COMMAND_READ_DEVICE_ID_OPI           (0x9F9F)
#elif defined (BOARD_RA8T2_EK) || defined (BOARD_RA8D2_EK)
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
#elif defined (BOARD_RA8M2_EK) || defined (BOARD_RA8P1_EK)
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
#endif /* Specific board */

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

#if defined (BOARD_RA8T2_EK) || defined (BOARD_RA8D2_EK)
#define OSPI_B_DUMMY_CYCLE_READ_STATUS_OPI          (16U)
#else
#define OSPI_B_DUMMY_CYCLE_READ_STATUS_OPI          (4U)
#endif /* Specific board */

#if defined (BOARD_RA8D1_EK)
#define OSPI_B_DUMMY_CYCLE_READ_REGISTER_SPI        (1U)
#elif defined (BOARD_RA8T2_EK) || defined (BOARD_RA8D2_EK)
#define OSPI_B_DUMMY_CYCLE_READ_REGISTER_SPI        (16U)
#elif defined (BOARD_RA8M2_EK) || defined (BOARD_RA8P1_EK)
#define OSPI_B_DUMMY_CYCLE_READ_REGISTER_SPI        (0U)
#endif /* Specific board */

#if defined (BOARD_RA8T2_EK) || defined (BOARD_RA8D2_EK)
#define OSPI_B_DUMMY_CYCLE_READ_REGISTER_OPI        (16U)
#else
#define OSPI_B_DUMMY_CYCLE_READ_REGISTER_OPI        (4U)
#endif /* Specific board */

#if defined (BOARD_RA8D1_EK)
#define OSPI_B_DUMMY_CYCLE_READ_MEMORY_SPI          (3U)
#define OSPI_B_DUMMY_CYCLE_READ_MEMORY_OPI          (10U)
#elif defined (BOARD_RA8T2_EK) || defined (BOARD_RA8D2_EK)
#define OSPI_B_DUMMY_CYCLE_READ_MEMORY_SPI          (11U)
#define OSPI_B_DUMMY_CYCLE_READ_MEMORY_OPI          (11U)
#elif defined (BOARD_RA8M2_EK) || defined (BOARD_RA8P1_EK)
#define OSPI_B_DUMMY_CYCLE_READ_MEMORY_SPI          (8U)
#define OSPI_B_DUMMY_CYCLE_READ_MEMORY_OPI          (10U)
#endif /* Specific board */

#if defined (BOARD_RA8D1_EK)
/* Macros for flash device register address */
#define OSPI_B_ADDRESS_STR1V_REGISTER               (0x00800000)
#define OSPI_B_ADDRESS_STR2V_REGISTER               (0x00800001)
#define OSPI_B_ADDRESS_CFR1V_REGISTER               (0x00800002)
#define OSPI_B_ADDRESS_CFR2V_REGISTER               (0x00800003)
#define OSPI_B_ADDRESS_CFR3V_REGISTER               (0x00800004)
#define OSPI_B_ADDRESS_CFR4V_REGISTER               (0x00800005)
#define OSPI_B_ADDRESS_CFR5V_REGISTER               (0x00800006)

/* Macros for flash device configuration */
#define OSPI_B_DATA_CFR2V_REGISTER                  (0x83)
#define OSPI_B_DATA_CFR3V_REGISTER                  (0x40)
#define OSPI_B_DATA_SET_SPI_CFR5V_REGISTER          (0x40)
#define OSPI_B_DATA_SET_OPI_CFR5V_REGISTER          (0x43)
#elif defined (BOARD_RA8T2_EK) || defined (BOARD_RA8D2_EK)
/* Macros for flash device register address */
#define OSPI_B_ADDRESS_CR_00H_REGISTER              (0x00000000)
#define OSPI_B_ADDRESS_CR_01H_REGISTER              (0x00000001)
#define OSPI_B_ADDRESS_CR_05H_REGISTER              (0x00000005)

/* Macros for flash device configuration */
#define OSPI_B_DATA_SET_SPI_CR_00H                  (0xDF)
#define OSPI_B_DATA_SET_OPI_CR_00H                  (0xE7)
#define OSPI_B_DATA_SET_CR_01H                      (0x10)
#elif defined (BOARD_RA8M2_EK) || defined (BOARD_RA8P1_EK)
/* Macros for flash device register address */
#define OSPI_B_ADDRESS_CR2_000H_REGISTER            (0x00000000)
#define OSPI_B_ADDRESS_CR2_300H_REGISTER            (0x00000300)

/* Macros for flash device configuration */
#define OSPI_B_DATA_SET_SPI_CR2_000H                (0x00)
#define OSPI_B_DATA_SET_OPI_CR2_000H                (0x02)
#define OSPI_B_DATA_SET_CR2_300H                    (0x05)
#endif /* Specific board */

/* Macros for flash device status bit */
#define OSPI_B_WEN_BIT_MASK                         (0x00000002)
#define OSPI_B_BUSY_BIT_MASK                        (0x00000001)

/* Macros for flash device sector */
#define OSPI_B_SECTOR_4K_SIZE                       (0x1000)
#define OSPI_B_SECTOR_4K_NUM                        (32U)
#define OSPI_B_SECTOR_4K_END_ADDRESS                (0x9001FFFF)

#define OSPI_B_32K_SIZE                             (0x8000)

/* Macros for periodic delay and periodic line feed */
#define PERIODIC_PRINT_OUT                          (0x10000U)
#define PERIODIC_LINE_FEED                          (0x400000U)

/* Macros for flash device address space mapping */
#if defined (BOARD_RA8T2_EK)
#define OSPI_B_CS0_START_ADDRESS                    (0x70000000)
#define OSPI_B_CS1_START_ADDRESS                    (0x78000000)
#define OSPI_B_AUTO_CALIB_ADDRESS                   (0x7801F000)
#else
#define OSPI_B_CS0_START_ADDRESS                    (0x80000000)
#define OSPI_B_CS1_START_ADDRESS                    (0x90000000)
#define OSPI_B_AUTO_CALIB_ADDRESS                   (0x9001F000)
#endif

/* Macros for flash device timing */
#define OSPI_B_TIME_UNIT                            (BSP_DELAY_UNITS_MICROSECONDS)
#define OSPI_B_TIME_RESET_SETUP                     (2U)        /* Type 50ns */
#define OSPI_B_TIME_RESET_PULSE                     (1000U)     /* Type 500us */
#define OSPI_B_TIME_ERASE_4K                        (50000U)    /* Type 4KB sector is 95 KBps */
#define OSPI_B_TIME_WRITE                           (10000U)    /* Type 256B page (4KB/256KB sector) is 595/533 KBps */

typedef enum e_ospi_b_transfer
{
    OSPI_B_TRANSFER_ENTER_4_ADDR_SPI = 0,
    OSPI_B_TRANSFER_EXIT_4_ADDR_SPI,
    OSPI_B_TRANSFER_WRITE_ENABLE_SPI,
    OSPI_B_TRANSFER_WRITE_REG_SPI,
    OSPI_B_TRANSFER_WRITE_VOL_REG_SPI,

#if defined (BOARD_RA8D1_EK)
    OSPI_B_TRANSFER_WRITE_CFR5V_SPI,
#elif defined (BOARD_RA8T2_EK) || defined (BOARD_RA8D2_EK)
    OSPI_B_TRANSFER_WRITE_CR_00H_SPI,
    OSPI_B_TRANSFER_WRITE_CR_01H_SPI,
    OSPI_B_TRANSFER_WRITE_CR_05H_SPI,
#elif defined (BOARD_RA8M2_EK) || defined (BOARD_RA8P1_EK)
    OSPI_B_TRANSFER_WRITE_CR2_000H_SPI,
    OSPI_B_TRANSFER_WRITE_CR2_300H_SPI,
#endif /* Specific board */

    OSPI_B_TRANSFER_READ_STATUS_SPI,
    OSPI_B_TRANSFER_READ_VOL_REG_SPI,
    OSPI_B_TRANSFER_READ_NON_VOL_REG_SPI,
    OSPI_B_TRANSFER_READ_DEVICE_ID_SPI,

#if defined (BOARD_RA8T2_EK) || defined (BOARD_RA8D2_EK)
    OSPI_B_TRANSFER_READ_CR_00H_SPI,
    OSPI_B_TRANSFER_READ_CR_01H_SPI,
    OSPI_B_TRANSFER_ENTER_4BYTES_ADDRESS,
#elif defined (BOARD_RA8M2_EK) || defined (BOARD_RA8P1_EK)
    OSPI_B_TRANSFER_READ_CR2_000H_SPI,
    OSPI_B_TRANSFER_READ_CR2_300H_SPI,
#endif /* Specific board */

    OSPI_B_TRANSFER_WRITE_ENABLE_OPI,
    OSPI_B_TRANSFER_WRITE_REG_OPI,
    OSPI_B_TRANSFER_WRITE_VOL_REG_OPI,

#if defined (BOARD_RA8T2_EK) || defined (BOARD_RA8D2_EK)
    OSPI_B_TRANSFER_WRITE_CR_00H_OPI,
#elif defined (BOARD_RA8M2_EK) || defined (BOARD_RA8P1_EK)
    OSPI_B_TRANSFER_WRITE_CR2_000H_OPI,
#endif /* Specific board */
    OSPI_B_TRANSFER_READ_STATUS_OPI,
    OSPI_B_TRANSFER_READ_VOL_REG_OPI,

#if defined (BOARD_RA8D1_EK)
    OSPI_B_TRANSFER_READ_CFR5V_OPI,
#elif defined (BOARD_RA8T2_EK) || defined (BOARD_RA8D2_EK)
    OSPI_B_TRANSFER_READ_CR_00H_OPI,
    OSPI_B_TRANSFER_READ_CR_01H_OPI,
#elif defined (BOARD_RA8M2_EK) || defined (BOARD_RA8P1_EK)
    OSPI_B_TRANSFER_READ_CR2_000H_OPI,
    OSPI_B_TRANSFER_READ_CR2_300H_OPI,
#endif /* Specific board */

    OSPI_B_TRANSFER_READ_DEVICE_ID_OPI,
    OSPI_B_TRANSFER_MAX
} ospi_b_transfer_t;

/* Function declarations */
fsp_err_t ospi_b_init(void);
fsp_err_t erase_ospi_flash(uint32_t erase_size);

#endif /* BOARD_OSPI_H_ */

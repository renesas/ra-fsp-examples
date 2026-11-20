/***********************************************************************************************************************
 * File Name    : board_ospi.h
 * Description  : Contains macros data structures and functions used in board_ospi.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2024 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 ***********************************************************************************************************************/

#ifndef BOARD_OSPI_H_
#define BOARD_OSPI_H_

#include "hal_data.h"
#include "app_thread.h"
#include "filex_ep.h"

/* Macro for OSPI reset pin */
#if defined (BOARD_RA8T2_EK)
#define OM_RESET                             (BSP_IO_PORT_12_PIN_07)
#else
#define OM_RESET                             (BSP_IO_PORT_01_PIN_06)
#endif /* BOARD_RA8T2_EK */

/* Macros for OSPI command code */
#define OSPI_B_COMMAND_ENTER_4_ADDR_SPI      (0xB7)
#define OSPI_B_COMMAND_EXIT_4_ADDR_SPI       (0xB8)

#if defined (BOARD_RA8D1_EK)
#define OSPI_B_COMMAND_WRITE_ENABLE_SPI      (0x06)
#define OSPI_B_COMMAND_WRITE_ENABLE_OPI      (0x0606)

#define OSPI_B_COMMAND_WRITE_REGISTER_SPI    (0x71)
#define OSPI_B_COMMAND_WRITE_REGISTER_OPI    (0x7171)

#define OSPI_B_COMMAND_READ_STATUS_SPI       (0x05)
#define OSPI_B_COMMAND_READ_STATUS_OPI       (0x0505)

#define OSPI_B_COMMAND_READ_REGISTER_SPI     (0x65)
#define OSPI_B_COMMAND_READ_REGISTER_OPI     (0x6565)

#define OSPI_B_COMMAND_READ_DEVICE_ID_SPI    (0x9F)
#define OSPI_B_COMMAND_READ_DEVICE_ID_OPI    (0x9F9F)
#elif defined (BOARD_RA8T2_EK) || defined (BOARD_RA8D2_EK)
#define OSPI_B_COMMAND_WRITE_ENABLE_SPI      (0x06)
#define OSPI_B_COMMAND_WRITE_ENABLE_OPI      (0x0606)
#define OSPI_B_COMMAND_READ_STATUS_SPI       (0x05)
#define OSPI_B_COMMAND_READ_STATUS_OPI       (0x0505)
#define OSPI_B_COMMAND_WCR_SPI               (0x81)
#define OSPI_B_COMMAND_WCR_OPI               (0x8181)
#define OSPI_B_COMMAND_WRITE_REGISTER_SPI    (OSPI_B_COMMAND_WCR_SPI)
#define OSPI_B_COMMAND_WRITE_REGISTER_OPI    (OSPI_B_COMMAND_WCR_OPI)
#define OSPI_B_COMMAND_RCR_SPI               (0x85)
#define OSPI_B_COMMAND_RCR_OPI               (0x8585)
#define OSPI_B_COMMAND_READ_REGISTER_SPI     (OSPI_B_COMMAND_RCR_SPI)
#define OSPI_B_COMMAND_READ_REGISTER_OPI     (OSPI_B_COMMAND_RCR_OPI)
#define OSPI_B_COMMAND_READ_DEVICE_ID_SPI    (0x9F)
#define OSPI_B_COMMAND_READ_DEVICE_ID_OPI    (0x9F9F)
#define OSPI_B_COMMAND_ENTER_4BYTES_ADDR     (0xB7)
#elif defined (BOARD_RA8M2_EK) || defined (BOARD_RA8P1_EK)
#define OSPI_B_COMMAND_WRITE_ENABLE_SPI      (0x06)
#define OSPI_B_COMMAND_WRITE_ENABLE_OPI      (0x06F9)
#define OSPI_B_COMMAND_WCR2_SPI              (0x72)
#define OSPI_B_COMMAND_WCR2_OPI              (0x728D)
#define OSPI_B_COMMAND_WRITE_REGISTER_SPI    (OSPI_B_COMMAND_WCR2_SPI)
#define OSPI_B_COMMAND_WRITE_REGISTER_OPI    (OSPI_B_COMMAND_WCR2_OPI)
#define OSPI_B_COMMAND_READ_STATUS_SPI       (0x05)
#define OSPI_B_COMMAND_READ_STATUS_OPI       (0x05FA)
#define OSPI_B_COMMAND_RCR2_SPI              (0x71)
#define OSPI_B_COMMAND_RCR2_OPI              (0x718E)
#define OSPI_B_COMMAND_READ_REGISTER_SPI     (OSPI_B_COMMAND_RCR2_SPI)
#define OSPI_B_COMMAND_READ_REGISTER_OPI     (OSPI_B_COMMAND_RCR2_OPI)
#define OSPI_B_COMMAND_READ_DEVICE_ID_SPI    (0x9F)
#define OSPI_B_COMMAND_READ_DEVICE_ID_OPI    (0x9F60)
#endif /* Specific board */

/* Macros for OSPI command length */
#define OSPI_B_COMMAND_LENGTH_SPI            (1U)
#define OSPI_B_COMMAND_LENGTH_OPI            (2U)

/* Macros for OSPI transfer address */
#define OSPI_B_ADDRESS_DUMMY                 (0U)
#define OSPI_B_ADDRESS_LENGTH_ZERO           (0U)
#define OSPI_B_ADDRESS_LENGTH_THREE          (3U)
#define OSPI_B_ADDRESS_LENGTH_FOUR           (4U)

/* Macros for OSPI transfer data */
#define OSPI_B_DATA_DUMMY                    (0U)
#define OSPI_B_DATA_LENGTH_ZERO              (0U)
#define OSPI_B_DATA_LENGTH_ONE               (1U)
#define OSPI_B_DATA_LENGTH_TWO               (2U)
#define OSPI_B_DATA_LENGTH_THREE             (3U)
#define OSPI_B_DATA_LENGTH_FOUR              (4U)

/* Macros for OSPI transfer dummy cycles */
#define OSPI_B_DUMMY_CYCLE_WRITE_SPI         (0U)
#define OSPI_B_DUMMY_CYCLE_WRITE_OPI         (0U)
#define OSPI_B_DUMMY_CYCLE_READ_STATUS_SPI   (0U)

#if defined (BOARD_RA8T2_EK) || defined (BOARD_RA8D2_EK)
#define OSPI_B_DUMMY_CYCLE_READ_STATUS_OPI   (16U)
#else
#define OSPI_B_DUMMY_CYCLE_READ_STATUS_OPI   (4U)
#endif /* Specific board */

#if defined (BOARD_RA8D1_EK)
#define OSPI_B_DUMMY_CYCLE_READ_REGISTER_SPI (1U)
#elif defined (BOARD_RA8T2_EK) || defined (BOARD_RA8D2_EK)
#define OSPI_B_DUMMY_CYCLE_READ_REGISTER_SPI (16U)
#elif defined (BOARD_RA8M2_EK) || defined (BOARD_RA8P1_EK)
#define OSPI_B_DUMMY_CYCLE_READ_REGISTER_SPI (0U)
#endif /* Specific board */

#if defined (BOARD_RA8T2_EK) || defined (BOARD_RA8D2_EK)
#define OSPI_B_DUMMY_CYCLE_READ_REGISTER_OPI (16U)
#else
#define OSPI_B_DUMMY_CYCLE_READ_REGISTER_OPI (4U)
#endif /* Specific board */

#if defined (BOARD_RA8D1_EK)
#define OSPI_B_DUMMY_CYCLE_READ_MEMORY_SPI   (3U)
#define OSPI_B_DUMMY_CYCLE_READ_MEMORY_OPI   (10U)
#elif defined (BOARD_RA8T2_EK) || defined (BOARD_RA8D2_EK)
#define OSPI_B_DUMMY_CYCLE_READ_MEMORY_SPI   (11U)
#define OSPI_B_DUMMY_CYCLE_READ_MEMORY_OPI   (11U)
#elif defined (BOARD_RA8M2_EK) || defined (BOARD_RA8P1_EK)
#define OSPI_B_DUMMY_CYCLE_READ_MEMORY_SPI   (8U)
#define OSPI_B_DUMMY_CYCLE_READ_MEMORY_OPI   (10U)
#endif /* Specific board */

#if defined (BOARD_RA8D1_EK)
/* Macro for flash device register address */

#define OSPI_B_ADDRESS_CFR2V_REGISTER        (0x00800003)
#define OSPI_B_ADDRESS_CFR3V_REGISTER        (0x00800004)
#define OSPI_B_ADDRESS_CFR4V_REGISTER        (0x00800005)
#define OSPI_B_ADDRESS_CFR5V_REGISTER        (0x00800006)
#define OSPI_B_ADDRESS_CFR4N_REGISTER        (0x00000005)

/* Macros for configure flash device */
#define OSPI_B_DATA_CFR2V_REGISTER           (0x88)
#define OSPI_B_DATA_DEFAULT_CFR3V_REGISTER   (0x00)
#define OSPI_B_DATA_CFR4V_REGISTER           (0xA0)
#define OSPI_B_DATA_DEFAULT_CFR4V_REGISTER   (0xA8)
#define OSPI_B_DATA_SET_SPI_CFR5V_REGISTER   (0x40)
#define OSPI_B_DATA_SET_OPI_CFR5V_REGISTER   (0x43)
#elif defined (BOARD_RA8T2_EK) || defined (BOARD_RA8D2_EK)
/* Macros for flash device register address */
#define OSPI_B_ADDRESS_CR_00H_REGISTER       (0x00000000)
#define OSPI_B_ADDRESS_CR_01H_REGISTER       (0x00000001)
#define OSPI_B_ADDRESS_CR_05H_REGISTER       (0x00000005)

/* Macros for configure flash device */
#define OSPI_B_DATA_SET_SPI_CR_00H           (0xDF)
#define OSPI_B_DATA_SET_OPI_CR_00H           (0xE7)
#define OSPI_B_DATA_SET_CR_01H               (0x10)
#elif defined (BOARD_RA8M2_EK) || defined (BOARD_RA8P1_EK)
/* Macros for flash device register address */
#define OSPI_B_ADDRESS_CR2_000H_REGISTER     (0x00000000)
#define OSPI_B_ADDRESS_CR2_300H_REGISTER     (0x00000300)

/* Macros for configure flash device */
#define OSPI_B_DATA_SET_OPI_CR2_000H         (0x02)
#define OSPI_B_DATA_SET_CR2_300H             (0x05)
#endif /* Specific board */

/* Macros for flash device status bit */
#define OSPI_B_WEN_BIT_MASK                  (0x00000002)

#if defined (BOARD_RA8D1_EK)
/* Macros for flash device sector size */
#define OSPI_B_SECTOR_4K_SIZE                (0x1000)
#define OSPI_B_SECTOR_256K_SIZE              (0x40000)
#define OSPI_B_SECTOR_256K_END_ADDRESS       (0x93FFFFFF)

/* Macros for flash device sector range */
#define SECTOR_128K                          (32U)
#define FILEX_START_ADDRESS                  (0x90080000)
#define FILEX_TOTAL_MEDIA_SIZE               (G_FX_MEDIA_TOTAL_SECTORS * G_FX_MEDIA_BYTES_PER_SECTOR)
#define LEVELX_SECTOR_RANGE_START            (163U)
#define LEVELX_SECTOR_RANGE_END              (287U)
#define LEVELX_START_ADDRESS                 (FILEX_START_ADDRESS + FILEX_TOTAL_MEDIA_SIZE)
#elif defined (BOARD_RA8T2_EK) || defined (BOARD_RA8D2_EK)
/* Macros for flash device sector size */
#define OSPI_B_SECTOR_4K_SIZE                (0x1000)

/* Macros for flash device sector range */
#if defined (BOARD_RA8T2_EK)
#define FILEX_START_ADDRESS                  (0x78001000)
#else
#define FILEX_START_ADDRESS                  (0x90001000)
#endif /* BOARD_RA8T2_EK */
#define FILEX_TOTAL_MEDIA_SIZE               (G_FX_MEDIA_TOTAL_SECTORS * G_FX_MEDIA_BYTES_PER_SECTOR)
#define LEVELX_SECTOR_RANGE_START            (4096U)
#define LEVELX_SECTOR_RANGE_END              (8292U)
#define LEVELX_START_ADDRESS                 (FILEX_START_ADDRESS + FILEX_TOTAL_MEDIA_SIZE)
#elif defined (BOARD_RA8M2_EK) || defined (BOARD_RA8P1_EK)
/* Macros for flash device sector size */
#define OSPI_B_SECTOR_4K_SIZE                (0x1000)

/* Macros for flash device sector range */
#define FILEX_START_ADDRESS                  (0x90001000)
#define FILEX_TOTAL_MEDIA_SIZE               (G_FX_MEDIA_TOTAL_SECTORS * G_FX_MEDIA_BYTES_PER_SECTOR)
#define LEVELX_SECTOR_RANGE_START            (4096U)
#define LEVELX_SECTOR_RANGE_END              (8292U)
#define LEVELX_START_ADDRESS                 (FILEX_START_ADDRESS + FILEX_TOTAL_MEDIA_SIZE)
#endif /* Specific board */

/* Macros for flash device address space mapping */
#if defined (BOARD_RA8T2_EK)
#define OSPI_B_CS1_START_ADDRESS             (0x78000000)
#else
#define OSPI_B_CS1_START_ADDRESS             (0x90000000)
#endif /* BOARD_RA8T2_EK */

#if defined (BOARD_RA8D1_EK)
#define OSPI_B_AUTO_CALIB_ADDRESS            (0x90040000)
#elif defined (BOARD_RA8T2_EK)
#define OSPI_B_AUTO_CALIB_ADDRESS            (0x78000000)
#else
#define OSPI_B_AUTO_CALIB_ADDRESS            (0x90000000)
#endif /* Specific board */

/* Macros for flash device timing */
#define OSPI_B_TIME_ERASE_4K                 (100000U)  /*  Type total 4KB sector is 95 KBps -> Type 1.347s */
#define OSPI_B_TIME_ERASE_256K               (1500000U) /*  Type 256KB sector is 331 KBps -> Type 0.773s */
#define OSPI_B_TIME_WRITE                    (10000U)   /*  Type 256B page (4KB/256KB) is 595/533 KBps */

typedef enum e_ospi_b_transfer
{
    OSPI_B_TRANSFER_ENTER_4_ADDR_SPI = 0,
    OSPI_B_TRANSFER_EXIT_4_ADDR_SPI,
    OSPI_B_TRANSFER_WRITE_ENABLE_SPI,
    OSPI_B_TRANSFER_WRITE_REG_SPI,

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
fsp_err_t erase_ospi_flash(void);
fsp_err_t erase_ospi_flash_range(uint8_t * p_address, uint32_t size);
fsp_err_t ospi_b_wait_operation(uint32_t timeout);

#endif /* BOARD_OSPI_H_ */

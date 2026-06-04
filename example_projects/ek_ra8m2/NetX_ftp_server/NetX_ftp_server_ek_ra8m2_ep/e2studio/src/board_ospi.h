/***********************************************************************************************************************
 * File Name    : board_ospi.h
 * Description  : Contains macros data structures and functions used in board_ospi.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2023 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef BOARD_OSPI_H_
#define BOARD_OSPI_H_

#include "common_utils.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

/* Macros for OSPI command code */
#if defined (BOARD_RA8D1_EK) || defined (BOARD_RA8M1_EK)
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
#elif defined (BOARD_RA8M2_EK)
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
#endif

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
#define OSPI_B_DUMMY_CYCLE_READ_STATUS_OPI          (4U)

#if defined (BOARD_RA8D1_EK) || defined (BOARD_RA8M1_EK)
#define OSPI_B_DUMMY_CYCLE_READ_REGISTER_SPI        (1U)
#elif defined (BOARD_RA8M2_EK)
#define OSPI_B_DUMMY_CYCLE_READ_REGISTER_SPI        (0U)
#endif
#define OSPI_B_DUMMY_CYCLE_READ_REGISTER_OPI        (4U)
#if defined (BOARD_RA8D1_EK) || defined (BOARD_RA8M1_EK)
#define OSPI_B_DUMMY_CYCLE_READ_MEMORY_SPI          (3U)
#define OSPI_B_DUMMY_CYCLE_READ_MEMORY_OPI          (10U)
#elif defined (BOARD_RA8M2_EK)
#define OSPI_B_DUMMY_CYCLE_READ_MEMORY_SPI          (8U)
#define OSPI_B_DUMMY_CYCLE_READ_MEMORY_OPI          (10U)
#endif

#if defined (BOARD_RA8D1_EK) || defined (BOARD_RA8M1_EK)
/* Macros for flash device register address */
#define OSPI_B_ADDRESS_STR1V_REGISTER               (0x00800000)
#define OSPI_B_ADDRESS_STR2V_REGISTER               (0x00800001)
#define OSPI_B_ADDRESS_CFR1V_REGISTER               (0x00800002)
#define OSPI_B_ADDRESS_CFR2V_REGISTER               (0x00800003)
#define OSPI_B_ADDRESS_CFR3V_REGISTER               (0x00800004)
#define OSPI_B_ADDRESS_CFR4V_REGISTER               (0x00800005)
#define OSPI_B_ADDRESS_CFR5V_REGISTER               (0x00800006)

/* Macros for configure flash device */
#define OSPI_B_DATA_CFR2V_REGISTER                  (0x83)
#define OSPI_B_DATA_CFR3V_REGISTER                  (0x40)
#define OSPI_B_DATA_SET_SPI_CFR5V_REGISTER          (0x40)
#define OSPI_B_DATA_SET_OPI_CFR5V_REGISTER          (0x43)

#elif defined (BOARD_RA8M2_EK)
/* Macros for flash device register address */
#define OSPI_B_ADDRESS_CR2_000H_REGISTER            (0x00000000)
#define OSPI_B_ADDRESS_CR2_300H_REGISTER            (0x00000300)

/* Macros for configure flash device */
#define OSPI_B_DATA_SET_SPI_CR2_000H                (0x00)
#define OSPI_B_DATA_SET_OPI_CR2_000H                (0x02)
#define OSPI_B_DATA_SET_CR2_300H                    (0x05)
#endif

/* Flash device timing */
#define OSPI_B_TIME_UNIT                            (BSP_DELAY_UNITS_MICROSECONDS)
#define OSPI_B_TIME_RESET_SETUP                     (2U)             /* Type 50ns */
#define OSPI_B_TIME_RESET_PULSE                     (1000U)          /* Type 500us */

/* Flash device status bit */
#define OSPI_B_WEN_BIT_MASK                         (0x00000002)
#define OSPI_B_BUSY_BIT_MASK                        (0x00000001)

typedef enum e_ospi_b_transfer
{
    OSPI_B_TRANSFER_WRITE_ENABLE_SPI = 0,

#if defined (BOARD_RA8D1_EK) || defined (BOARD_RA8M1_EK)
    OSPI_B_TRANSFER_WRITE_CFR2V_SPI,
    OSPI_B_TRANSFER_WRITE_CFR3V_SPI,
    OSPI_B_TRANSFER_WRITE_CFR5V_SPI,
#elif defined (BOARD_RA8M2_EK)
    OSPI_B_TRANSFER_WRITE_CR2_000H_SPI,
    OSPI_B_TRANSFER_WRITE_CR2_300H_SPI,
#endif

    OSPI_B_TRANSFER_READ_STATUS_SPI,

#if defined (BOARD_RA8D1_EK) || defined (BOARD_RA8M1_EK)
    OSPI_B_TRANSFER_READ_CFR2V_SPI,
    OSPI_B_TRANSFER_READ_CFR3V_SPI,
    OSPI_B_TRANSFER_READ_CFR5V_SPI,
#elif defined (BOARD_RA8M2_EK)
    OSPI_B_TRANSFER_READ_CR2_000H_SPI,
    OSPI_B_TRANSFER_READ_CR2_300H_SPI,
#endif

    OSPI_B_TRANSFER_READ_DEVICE_ID_SPI,
    OSPI_B_TRANSFER_WRITE_ENABLE_OPI,

#if defined (BOARD_RA8D1_EK) || defined (BOARD_RA8M1_EK)
    OSPI_B_TRANSFER_WRITE_CFR2V_OPI,
    OSPI_B_TRANSFER_WRITE_CFR3V_OPI,
    OSPI_B_TRANSFER_WRITE_CFR5V_OPI,
#elif defined (BOARD_RA8M2_EK)
    OSPI_B_TRANSFER_WRITE_CR2_000H_OPI,
#endif

    OSPI_B_TRANSFER_READ_STATUS_OPI,

#if defined (BOARD_RA8D1_EK) || defined (BOARD_RA8M1_EK)
    OSPI_B_TRANSFER_READ_CFR2V_OPI,
    OSPI_B_TRANSFER_READ_CFR3V_OPI,
    OSPI_B_TRANSFER_READ_CFR5V_OPI,
#elif defined (BOARD_RA8M2_EK)
    OSPI_B_TRANSFER_READ_CR2_000H_OPI,
    OSPI_B_TRANSFER_READ_CR2_300H_OPI,
#endif

    OSPI_B_TRANSFER_READ_DEVICE_ID_OPI,
    OSPI_B_TRANSFER_MAX
} ospi_b_transfer_t;
/***********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/

extern spi_flash_direct_transfer_t g_ospi_b_direct_transfer[OSPI_B_TRANSFER_MAX];

/***********************************************************************************************************************
 * Exported global function (to be accessed by other files)
 **********************************************************************************************************************/
fsp_err_t ospi_b_init();

#endif /* BSP_OSPI_H_ */

/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef _APP_OSPI_OPERATIONS_H_
#define _APP_OSPI_OPERATIONS_H_

#include "hal_data.h"

/* Macro for OSPI command code */
#define OSPI_B_COMMAND_WRITE_ENABLE_SPI             (0x06)
#define OSPI_B_COMMAND_WRITE_ENABLE_OPI             (0x06F9)

#define OSPI_B_COMMAND_WCR2_SPI                     (0x72)
#define OSPI_B_COMMAND_WCR2_OPI                     (0x728D)

#define OSPI_B_COMMAND_RCR2_SPI                     (0x71)
#define OSPI_B_COMMAND_RCR2_OPI                     (0x718E)

#define OSPI_B_COMMAND_READ_STATUS_SPI              (0x05)
#define OSPI_B_COMMAND_READ_STATUS_OPI              (0x05FA)

#define OSPI_B_COMMAND_READ_DEVICE_ID_SPI           (0x9F)

#define OSPI_B_COMMAND_LENGTH_SPI                   (1U)
#define OSPI_B_COMMAND_LENGTH_OPI                   (2U)

/* Macro for OSPI transfer address */
#define OSPI_B_ADDRESS_DUMMY                        (0U)

#define OSPI_B_ADDRESS_CR2_000H_REGISTER            (0x00000000)
#define OSPI_B_ADDRESS_CR2_300H_REGISTER            (0x00000300)

#define OSPI_B_ADDRESS_LENGTH_ZERO                  (0U)
#define OSPI_B_ADDRESS_LENGTH_FOUR                  (4U)

/* Macro for OSPI transfer data */
#define OSPI_B_DATA_DUMMY                           (0U)
#define OSPI_B_DATA_LENGTH_ZERO                     (0U)
#define OSPI_B_DATA_LENGTH_ONE                      (1U)
#define OSPI_B_DATA_LENGTH_TWO                      (2U)
#define OSPI_B_DATA_LENGTH_THREE                    (3U)

/* Macro for OSPI transfer dummy cycles */
#define OSPI_B_DUMMY_CYCLE_WRITE_SPI                (0U)
#define OSPI_B_DUMMY_CYCLE_WRITE_OPI                (0U)

#define OSPI_B_DUMMY_CYCLE_READ_STATUS_SPI          (0U)
#define OSPI_B_DUMMY_CYCLE_READ_STATUS_OPI          (4U)

/* Flash device timing */
#define OSPI_B_TIME_UNIT                            (BSP_DELAY_UNITS_MICROSECONDS)
#define OSPI_B_TIME_RESET_SETUP                     (2U)             //  Type 50ns
#define OSPI_B_TIME_RESET_PULSE                     (1000U)          //  Type 500us

#define OSPI_B_TIME_ERASE_4K                        (25000U)       // Typical: 25ms
#define OSPI_B_TIME_WRITE                           (150U)         // Typical: 0.15ms

/* Flash device status bit */
#define OSPI_B_WEL_BIT_MASK                         (0x00000002)    /* Write enable latch */
#define OSPI_B_WIP_BIT_MASK                         (0x00000001)    /* Write in progress bit */

#define OSPI_B_SECTOR_SIZE_4K                       (0x1000)

/* Macros for configure flash device */
#define OSPI_B_DATA_SET_WRCR2_300H                  (0x07)
#define OSPI_B_DATA_SET_WRCR2_000H                  (0x02)

#define ENCRYPTED_START_ADDRESS                     (BSP_PARTITION_OSPI0_CS1_CPU1_S_START)
#define PLAINTEXT_START_ADDRESS                     (BSP_PARTITION_PLAINTEXT_CPU1_S_START)

#define OSPI_B_ERR_RETURN(err, action) do {             \
    if (FSP_SUCCESS != err) {                           \
        action;                                         \
    }                                                   \
} while (0)

#define OSPI_B_CHECK_IF_NULL(condition, action) do {     \
    if (condition == NULL) {                             \
        action;                                          \
    }                                                    \
} while (0)

typedef enum {
    OSPI_B_MODE_SPI = 0,
    OSPI_B_MODE_OPI,
    OSPI_B_MODE_MAX
} app_ospi_b_mode_t;

typedef enum {
    OSPI_B_TRANSFER_WRITE_ENABLE = 0,

    OSPI_B_TRANSFER_WRITE_WRCR2_300H,
    OSPI_B_TRANSFER_WRITE_WRCR2_000H,

    OSPI_B_TRANSFER_READ_STATUS,
    OSPI_B_TRANSFER_READ_WRCR2_300H,
    OSPI_B_TRANSFER_READ_WRCR2_000H,
    OSPI_B_TRANSFER_READ_DEVICE_ID,

    OSPI_B_TRANSFER_MAX
} ospi_b_transfer_t;

fsp_err_t ospi_b_init(void);

fsp_err_t write_plaintext_data_to_ospi(void);
fsp_err_t write_encrypted_data_to_ospi(void);

#endif

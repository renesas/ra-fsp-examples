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
#define OSPI_B_COMMAND_WRITE_ENABLE_OPI             (0x0606)
#define OSPI_B_COMMAND_WRITE_REGISTER_SPI           (0x71)
#define OSPI_B_COMMAND_WRITE_REGISTER_OPI           (0x7171)
#define OSPI_B_COMMAND_READ_STATUS_SPI              (0x05)
#define OSPI_B_COMMAND_READ_STATUS_OPI              (0x0505)
#define OSPI_B_COMMAND_READ_REGISTER_SPI            (0x65)
#define OSPI_B_COMMAND_READ_REGISTER_OPI            (0x6565)
#define OSPI_B_COMMAND_READ_DEVICE_ID_SPI           (0x9F)
#define OSPI_B_COMMAND_READ_DEVICE_ID_OPI           (0x9F9F)

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
#define OSPI_B_DATA_LENGTH_FOUR                     (4U)

/* Macro for OSPI transfer dummy cycles */
#define OSPI_B_DUMMY_CYCLE_WRITE_SPI                (0U)
#define OSPI_B_DUMMY_CYCLE_WRITE_OPI                (0U)
#define OSPI_B_DUMMY_CYCLE_READ_STATUS_SPI          (0U)
#define OSPI_B_DUMMY_CYCLE_READ_STATUS_OPI          (4U)
#define OSPI_B_DUMMY_CYCLE_READ_REGISTER_SPI        (1U)
#define OSPI_B_DUMMY_CYCLE_READ_REGISTER_OPI        (4U)
#define OSPI_B_DUMMY_CYCLE_READ_MEMORY_SPI          (3U)
#define OSPI_B_DUMMY_CYCLE_READ_MEMORY_OPI          (10U)

/* Macro for flash device register address */
#define OSPI_B_ADDRESS_STR1V_REGISTER               (0x00800000)
#define OSPI_B_ADDRESS_STR2V_REGISTER               (0x00800001)
#define OSPI_B_ADDRESS_CFR1V_REGISTER               (0x00800002)
#define OSPI_B_ADDRESS_CFR2V_REGISTER               (0x00800003)
#define OSPI_B_ADDRESS_CFR3V_REGISTER               (0x00800004)
#define OSPI_B_ADDRESS_CFR4V_REGISTER               (0x00800005)
#define OSPI_B_ADDRESS_CFR5V_REGISTER               (0x00800006)

/* Macros for configure flash device */
#define OSPI_B_DATA_CFR2V_REGISTER                  (0x81)
#define OSPI_B_DATA_CFR3V_REGISTER                  (0x40)
#define OSPI_B_DATA_SET_SPI_CFR5V_REGISTER          (0x40)
#define OSPI_B_DATA_SET_OPI_CFR5V_REGISTER          (0x43)

/* Flash device sector size */
#define OSPI_B_SECTOR_SIZE_4K                       (0x1000)
#define OSPI_B_SECTOR_SIZE_256K                     (0x40000)
#define OSPI_B_SECTOR_4K_END_ADDRESS                (0x9001FFFF)

/* Flash device timing */
#define OSPI_B_TIME_UNIT                            (BSP_DELAY_UNITS_MICROSECONDS)
#define OSPI_B_TIME_RESET_SETUP                     (2U)             //  Type 50ns
#define OSPI_B_TIME_RESET_PULSE                     (1000U)          //  Type 500us
#define OSPI_B_TIME_ERASE_256K                      (1500000U)       //  Type 256KB sector is 331 KBps -> Type 0.773s
#define OSPI_B_TIME_ERASE_4K                        (100000U)        //  Type 4KB sector is 95 KBps -> Type 0.042s
#define OSPI_B_TIME_WRITE                           (10000U)         //  Type 256B page (4KB/256KB) is 595/533 KBps -> Type

#define ENCRYPTED_START_ADDRESS                     (BSP_PARTITION_OSPI0_CS1_CPU0_S_START)
#define PLAINTEXT_START_ADDRESS                     (BSP_PARTITION_PLAINTEXT_DATA_S_START)

/* Flash device status bit */
#define OSPI_B_WEL_BIT_MASK                         (0x00000002)
#define OSPI_B_BUSY_BIT_MASK                        (0x00000001)

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

    OSPI_B_TRANSFER_WRITE_CFR2V,
    OSPI_B_TRANSFER_WRITE_CFR3V,
    OSPI_B_TRANSFER_WRITE_CFR5V,

    OSPI_B_TRANSFER_READ_STATUS,
    OSPI_B_TRANSFER_READ_CFR2V,
    OSPI_B_TRANSFER_READ_CFR3V,
    OSPI_B_TRANSFER_READ_CFR5V,

    OSPI_B_TRANSFER_MAX
} ospi_b_transfer_t;

fsp_err_t ospi_b_init(void);

void execute_encrypted_func_ospi(void);
void execute_plaintext_fun_ospi(void);

#endif

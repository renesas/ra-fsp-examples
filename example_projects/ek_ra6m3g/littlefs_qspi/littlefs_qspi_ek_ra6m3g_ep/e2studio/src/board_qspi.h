/***********************************************************************************************************************
 * File Name    : board_qspi.h
 * Description  : Contains declarations of data structures and functions used in board_qspi.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef BOARD_QSPI_H_
#define BOARD_QSPI_H_

#include "common_utils.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
/* Macros for QSPI flash device pins */
#if defined (BOARD_RA6M5_EK) || defined (BOARD_RA4M3_EK) || defined (BOARD_RA6M3_EK) || defined (BOARD_RA6M3G_EK)\
	|| defined (BOARD_RA6M4_EK)
#define RESET_SIO3_PIN              (BSP_IO_PORT_03_PIN_10)
#elif defined (BOARD_RA4M2_EK)
#define RESET_SIO3_PIN              (BSP_IO_PORT_05_PIN_05)
#elif defined (BOARD_RA6E2_EK)
#define RESET_SIO3_PIN              (BSP_IO_PORT_01_PIN_03)
#endif

/* Flash device Register/Setting Commands */
#define WRITE_ENABLE                (0x06)
#define WRITE_DISABLE               (0x04)
#define READ_STATUS_REG             (0x05)
#define READ_CONFIG_REG             (0x15)
#define WRITE_STATUS_REG            (0x01)
#define ENABLE_QPI                  (0x35)
#define EXIT_QPI                    (0xF5)
#define ENTER_4BYTE_MODE            (0xB7)
#define EXIT_4BYTE_MODE             (0xE9)

#define QE_BIT_ENABLE               (0x40)
#define FOUR_BYTE_BIT_ENABLE        (0x20)

/* Flash device address */
#define QSPI_START_ADDRESS          (0x60000000)

/* Flash device block/sector size */
#define QSPI_SECTOR_SIZE            (0x1000)
#define QSPI_32KB_BLOCK_SIZE        (0x8000)
#define QSPI_64KB_BLOCK_SIZE        (0x10000)

#define DATA_SIZE                   (8U)
#define ZERO                        (0U)
#define ONE                         (1U)
#define TWO                         (2U)
#define LENGTH_ZERO                 (0U)
#define LENGTH_ONE                  (1U)
#define LENGTH_TWO                  (2U)
#define LENGTH_THREE                (3U)
#define LENGTH_FOUR                 (4U)

/* Flash device timing */
#define QSPI_TIME_UNIT              (BSP_DELAY_UNITS_MICROSECONDS)
#define QSPI_TIME_RESET             (10000U)    /* Type 10 ms */
#define QSPI_TIME_SECTOR_ERASE      (400000U)   /* Typical sector erase cycle time is 30 ms (Max: 400 ms) */
#define QSPI_TIME_32KB_BLOCK_ERASE  (1000000U)  /* Typical 32KB block erase cycle time is 0.18 s (Max: 1 s) */
#define QSPI_TIME_64KB_BLOCK_ERASE  (1000000U)  /* Typical 64KB block erase cycle time is 0.38 s (Max: 2 s) */

/* Macros for periodic delay and periodic line feed */
#define PERIODIC_PRINT_OUT          (0x10000U)
#define PERIODIC_LINE_FEED          (0x400000U)

typedef enum qspi_protocol
{
    QSPI_EXTENDED_SPI_PROTOCOL = 0,
    QSPI_DUAL_SPI_PROTOCOL     = 1,
    QSPI_QUAD_SPI_PROTOCOL     = 2,
} qspi_protocol;

/***********************************************************************************************************************
 * Exported global functions (to be accessed by other files)
 **********************************************************************************************************************/
/***********************************************************************************************************************
 *  Function Name: qspi_init
 *  Description  : This function initializes QSPI module and Flash device.
 *  Arguments    : None.
 *  Return Value : FSP_SUCCESS     Upon successful initialization of QSPI module and Flash device.
 *                 FSP_ERR_ABORTED Upon incorrect read data.
 *                 Any other error code apart from FSP_SUCCESS Unsuccessful operation.
 **********************************************************************************************************************/
fsp_err_t qspi_init(void);

/***********************************************************************************************************************
 *  Function Name: erase_qspi_flash
 *  Description  : This function erases the flash memory using lower level flash APIs.
 *  Arguments    : erase_size      The size of flash to be erased.
 *  Return Value : FSP_SUCCESS     Upon successful operation.
 *                 Any other error code apart from FSP_SUCCESS Unsuccessful operation.
 **********************************************************************************************************************/
fsp_err_t erase_qspi_flash(uint32_t erase_size);

#endif /* BOARD_QSPI_H_ */

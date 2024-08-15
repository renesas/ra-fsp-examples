/***********************************************************************************************************************
 * File Name    : setup_qspi.h
 * Description  : Contains declarations of data structures and functions used in setup_qspi.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef SETUP_QSPI_H_
#define SETUP_QSPI_H_

/* Macros for QSPI flash device pins */
#define QSPI_SIO3   (BSP_IO_PORT_03_PIN_10)

#define READ_STASTUS_REGISTER     (0x05)
#define RESET_ENABLE              (0x66)
#define RESET                     (0x99)
#define READ_CONFIG_REG           (0x15)
#define READ_STATUS_REG           (0x05)
#define WRITE_STATUS_REG          (0x01)
#define WRITE_ENABLE              (0x06)
#define WRITE_DISABLE             (0x04)
#define REMS                      (0x90)
#define RDID                      (0x9F)
#define QPIID                     (0xAF)
#define ENABLE_4BYTE              (0xB7)

#define ENABLE_QPI                (0x35)
#define EXIT_QPI                  (0xF5)

#define MX_MANUFACTURER_ID        (0xC2)
#define DEVICE_ID                 (0x18)
#define MEMORY_TYPE               (0x20)
#define MEMORY_DENSITY            (0x19)


typedef enum qspi_protocol
{
    QSPI_EXTENDED_SPI_PROTOCOL = 0,
    QSPI_DUAL_SPI_PROTOCOL     = 1,
    QSPI_QUAD_SPI_PROTOCOL     = 2,
} qspi_protocol;

#define TREADY2_RESET_TIME       (100)       /* worst case value for tREADY2 */

#define NUMBER_CYCLE_RELEASE_XIP_4_BYTE     (10)  //4 Byte addressing, Quad IO, address + performance enhance indicator is 10 clock cycles

#define NUMBER_CYCLE_RELEASE_XIP_3_BYTE     (8)   //3 Byte addressing, Quad IO, address + performance enhance indicator is 8  clock cycles

fsp_err_t setup_qspi(spi_flash_ctrl_t * p_ctrl, spi_flash_cfg_t const * const p_cfg);

#endif /* SETUP_QSPI_H_ */

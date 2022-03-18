/***********************************************************************************************************************
 * File Name    : setup_qspi.h
 * Description  : Contains declarations of data structures and functions used in setup_qspi.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 *
 * This file is part of Renesas RA Flex Software Package (FSP)
 *
 * The contents of this file (the "contents") are proprietary and confidential to Renesas Electronics Corporation
 * and/or its licensors ("Renesas") and subject to statutory and contractual protections.
 *
 * This file is subject to a Renesas FSP license agreement. Unless otherwise agreed in an FSP license agreement with
 * Renesas: 1) you may not use, copy, modify, distribute, display, or perform the contents; 2) you may not use any name
 * or mark of Renesas for advertising or publicity purposes or in connection with your use of the contents; 3) RENESAS
 * MAKES NO WARRANTY OR REPRESENTATIONS ABOUT THE SUITABILITY OF THE CONTENTS FOR ANY PURPOSE; THE CONTENTS ARE PROVIDED
 * "AS IS" WITHOUT ANY EXPRESS OR IMPLIED WARRANTY, INCLUDING THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, AND NON-INFRINGEMENT; AND 4) RENESAS SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, OR
 * CONSEQUENTIAL DAMAGES, INCLUDING DAMAGES RESULTING FROM LOSS OF USE, DATA, OR PROJECTS, WHETHER IN AN ACTION OF
 * CONTRACT OR TORT, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE CONTENTS. Third-party contents
 * included in this file may be subject to different terms.
 **********************************************************************************************************************/

#ifndef SETUP_QSPI_H_
#define SETUP_QSPI_H_

/* Macros for QSPI flash device pins */
#if defined (BOARD_RA6M4_EK) || defined (BOARD_RA6M3_EK) || defined (BOARD_RA6M5_EK) || defined (BOARD_RA4M3_EK)
#define QSPI_SIO3   BSP_IO_PORT_03_PIN_10
#else
#define QSPI_SIO3   BSP_IO_PORT_05_PIN_05      // BOARD_RA4M2_EK
#endif

#define READ_STASTUS_REGISTER     0x05
#define RESET_ENABLE              0x66
#define RESET                     0x99
#define READ_CONFIG_REG           0x15
#define READ_STATUS_REG           0x05
#define WRITE_STATUS_REG          0x01
#define WRITE_ENABLE              0x06
#define WRITE_DISABLE             0x04
#define REMS                      0x90
#define RDID                      0x9F
#define QPIID                     0xAF
#define ENABLE_4BYTE              0xB7

#define ENABLE_QPI                0x35
#define EXIT_QPI                  0xF5

#define MX_MANUFACTURER_ID        0xC2
#define DEVICE_ID                 0x18

typedef enum qspi_protocol
{
    QSPI_EXTENDED_SPI_PROTOCOL = 0,
    QSPI_DUAL_SPI_PROTOCOL     = 1,
    QSPI_QUAD_SPI_PROTOCOL     = 2,
} qspi_protocol;

#define TREADY2_RESET_TIME       100       /* worst case value for tREADY2 */

#define NUMBER_CYCLE_RELEASE_XIP_4_BYTE     10  //4 Byte addressing, Quad IO, address + performance enhance indicator is 10 clock cycles

#define NUMBER_CYCLE_RELEASE_XIP_3_BYTE     8   //3 Byte addressing, Quad IO, address + performance enhance indicator is 8  clock cycles

fsp_err_t setup_qspi(spi_flash_ctrl_t * p_ctrl, spi_flash_cfg_t const * const p_cfg);

#endif /* SETUP_QSPI_H_ */

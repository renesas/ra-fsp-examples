/***********************************************************************************************************************
 * File Name    : sau_spi_master_ep.h
 * Description  : Contains data structures and functions used in sau_spi_master_ep.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef SAU_SPI_MASTER_EP_H_
#define SAU_SPI_MASTER_EP_H_

#define EP_VERSION              ("1.1")
#define MODULE_NAME             "r_sau_spi_master"
#define BANNER_INFO             "\r\n******************************************************************"\
                                "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module      *"\
                                "\r\n*   Example Project Version %s                                  *"\
                                "\r\n*   Flex Software Pack Version %d.%d.%d                             *"\
                                "\r\n******************************************************************"\
                                "\r\nRefer to readme.txt file for more details on Example Project and" \
                                "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

/* Macro for example project information */
#define EP_INFO                 "\r\nThis example project demonstrates the basic usage of SAU SPI driver in"\
                                "\r\nmaster configuration on Renesas RA MCUs. The project writes commands"\
                                "\r\nto the slave digital thermometer module, and reads the temperature"\
                                "\r\nvalues using SAU SPI bus configured as master. The temperature values"\
                                "\r\nare continuously printed on terminal. Status information and error"\
                                "\r\nmessages will be printed on terminal during the execution of the project.\r\n\r\n"

/* Macros for PMOD MAX31723PMB1 register address */
#define CFG_WRITE_ADDR              (0x80)
#define CFG_READ_ADDR               (0x00)
#define DATA_READ_ADDR              (0x01)

/* Macros for PMOD MAX31723PMB1 configuration */
#define SENSOR_RES_CFG              (0x06)
#define CONVERSION_TIME             (200U)
#define CONVERSION_TIME_UNIT        (BSP_DELAY_UNITS_MILLISECONDS)
#define DATA_FRAME_SIZE             (3U)

/* Macros for SPI transfer */
#define SPI_TIMEOUT_UNIT            (BSP_DELAY_UNITS_MILLISECONDS)
#define SPI_TIMEOUT_DIV             (1U)

/* Macros for manipulating and print the temperature values */
#define SENSOR_RESET_VALUE          (0.0F)
#define INTEGER_PART                (2U)
#define FRACTIONAL_PART             (1U)
#define FLOAT_DIVISOR               (16.0F)
#define SHIFT_FOUR_BITS             (4U)
#define DEGREE_SYMBOL               ("\xB0")
#define BUFF_SIZE                   (30U)
#define PRINT_DELAY                 (2U)

/* Function macros to assert and de-assert Slave Select pins */
#define SS_PIN                      (BSP_IO_PORT_01_PIN_03)
#define SS_DE_ASSERT(x)             (R_IOPORT_PinWrite(&g_ioport_ctrl, (x), BSP_IO_LEVEL_LOW))
#define SS_ASSERT(x)                (R_IOPORT_PinWrite(&g_ioport_ctrl, (x), BSP_IO_LEVEL_HIGH))

/* Public function declaration */
void sau_spi_master_entry(void);

#endif /* SAU_SPI_MASTER_EP_H_ */

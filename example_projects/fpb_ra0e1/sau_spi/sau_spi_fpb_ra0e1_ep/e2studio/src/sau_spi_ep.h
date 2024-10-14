/***********************************************************************************************************************
 * File Name    : sau_spi_ep.h
 * Description  : Contains data structures and functions used in sau_spi_ep.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#ifndef SAU_SPI_EP_H_
#define SAU_SPI_EP_H_


/*  Macro for example project information */
#define EP_INFO                "\r\nThis Example Project demonstrates the basic usage of SAU SPI driver."\
                               "\r\nThe project writes commands to the slave digital thermometer module and reads"\
                               "\r\nthe temperature values using SAU SPI bus configured as master. The temperature"\
                               "\r\nvalues are continuously printed on J-Link RTT Viewer. Status information and"\
                               "\r\nerror messages will be printed on J-link RTT Viewer during the execution of the project.\r\n\r\n"

/*  Macro for PMOD MAX31723PMB1 register address */
#define CFG_WRITE_ADDR              (0x80)
#define CFG_READ_ADDR               (0x00)
#define DATA_READ_ADDR              (0x01)

/* Macro for PMOD MAX31723PMB1 configuration */
#define SENSOR_RES_CFG              (0x06)
#define CONVERSION_TIME             (200U)
#define CONVERSION_TIME_UNIT        (BSP_DELAY_UNITS_MILLISECONDS)
#define DATA_FRAME_SIZE             (3U)

/* Macro for SPI transfer */
#define SPI_TIMEOUT_UNIT            (BSP_DELAY_UNITS_MILLISECONDS)
#define SPI_TIMEOUT_DIV             (1U)

/* Macro for manipulating and print the temperature values */
#define SENSOR_RESET_VALUE          (0.0F)
#define TEMPERATURE_INDEX           (2U)
#define FLOAT_DIVISOR               (16.0F)
#define SHIFT_FOUR_BITS             (4U)
#define DEGREE_SYMBOL               ("\xB0")
#define BUFF_SIZE                   (30U)
#define PRINT_DELAY                 (2U)

/* Function macros to assert and de-assert Slave Select pins */
#define CS_PIN                      (BSP_IO_PORT_01_PIN_03)
#define CS_DE_ASSERT(x)             (R_IOPORT_PinWrite(&g_ioport_ctrl, (x), BSP_IO_LEVEL_LOW))
#define CS_ASSERT(x)                (R_IOPORT_PinWrite(&g_ioport_ctrl, (x), BSP_IO_LEVEL_HIGH))

/* Public function declarations */
void sau_spi_entry(void);

#endif /* SAU_SPI_EP_H_ */

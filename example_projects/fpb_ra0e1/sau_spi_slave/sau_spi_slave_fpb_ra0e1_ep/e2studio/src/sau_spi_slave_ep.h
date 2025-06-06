/***********************************************************************************************************************
 * File Name    : sau_spi_slave_ep.h
 * Description  : Contains data structures and functions used in sau_spi_slave_ep.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef SAU_SPI_SLAVE_EP_H_
#define SAU_SPI_SLAVE_EP_H_

/* Macro for example project information */
#define EP_INFO                "\r\nThis example project demonstrates the basic usage of the SAU SPI"\
                               "\r\ndriver in slave configuration on Renesas RA MCUs using Renesas FSP."\
                               "\r\nThe project follows a command protocol similar to that of the"\
                               "\r\nMAX31723 sensor. During the communication, the MCU#1, which is"\
                               "\r\nrunning the sau_spi_slave_ep project uses an IRQ to detect the"\
                               "\r\nslave select pin signal of MCU#2, which is running the"\
                               "\r\nsau_spi_master_ep, and MCU#1 reads commands and data sent from MCU"\
                               "\r\n#2 when the signal transitions from LOW to HIGH. When MCU #2 sends"\
                               "\r\na \"read data\" command, MCU #1 reads the internal die temperature"\
                               "\r\nusing the ADC and sends the temperature back to the master MCU #2."\
                               "\r\nThe temperature values are continuously printed to the terminal to"\
                               "\r\nverify correct data transmission. Additionally, status information"\
                               "\r\nand error messages are displayed on the terminal during execution.\r\n\r\n"

#define EP_VERSION              ("1.0")
#define MODULE_NAME             "r_sau_spi_slave"
#define BANNER_INFO             "\r\n******************************************************************"\
                                "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module       *"\
                                "\r\n*   Example Project Version %s                                  *"\
                                "\r\n*   Flex Software Pack Version  %d.%d.%d                            *"\
                                "\r\n******************************************************************"\
                                "\r\nRefer to readme.txt file for more details on Example Project and" \
                                "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

/* The commands used in this EP are derived from the purpose of sau_spi_master_ep
communicating with the PMOD MAX31723PMB1
* CFG_WRITE_CMD: Write to the configuration register.
* CFG_READ_CMD: Read from the configuration register.
* DATA_READ_CMD: Read from the temperature register.
* CFG_VALUE_EXPECT: The value is set to configuration register. */
#define CFG_WRITE_CMD               (0x80)
#define CFG_READ_CMD                (0x00)
#define DATA_READ_CMD               (0x01)
#define CFG_VALUE_EXPECT            (0x06)

/* Macros for SPI data frame */
#define INDEX_ZERO                  (0U)
#define INDEX_ONE                   (1U)
#define INDEX_TWO                   (2U)
#define FRAME_DATA_SIZE             (3U)

/* Max wait count for time-out operation */
#define MAX_COUNT                   (180000000)

/* Macros for manipulating the temperature values */
#define MCU_TEMP_TO_C(a)            (((((float)(a)) * (-0.220784f)) + 312.671f))
#define DEGREE_SYMBOL               ("\xB0")
#define FLOAT_DIVISOR               (16.0F)
#define SHIFT_FOUR_BITS             (4U)
#define INTERGER_PART               (2U)
#define FRACTIONAL_PART             (1U)

/* Macros for used channels of modules */
#define DETECT_SS_SIGNAL_CHANNEL    (5U)
#define SAU_SPI_00_CHANNEL          (0U)

/* Enumeration for SAU SPI Slave directions */
typedef enum e_spi_direct
{
    SLAVE_READ  = 1,
    SLAVE_WRITE = 2,
} spi_dir_t;

/* User defined function */
void sau_spi_slave_entry(void);

#endif /* SAU_SPI_SLAVE_EP_H_ */

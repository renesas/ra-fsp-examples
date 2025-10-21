/**********************************************************************************************************************
 * File Name    : smbus_ep.h
 * Description  : Contains data structures and functions used in smbus_ep.c.
 *********************************************************************************************************************/
/**********************************************************************************************************************
* Copyright (c) 2024 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
**********************************************************************************************************************/

#ifndef SMBUS_EP_H_
#define SMBUS_EP_H_

#include "common_utils.h"

#define EP_INFO     "\r\nThis example project demonstrates the basic usage of I2C communication conforming to"\
                    "\r\nthe SMBus Specification (version 2.0) on Renesas RA MCUs using the Renesas Flexible"\
                    "\r\nSoftware Package (FSP). The project involves writing commands to the MAX31875 sensor"\
                    "\r\nand reading the resulting temperature values. The SMBus transmission time is monitored"\
                    "\r\nby the GPT module to ensure the MCU acting as a master device complies with the SMBus"\
                    "\r\ntimeout standard. The Packet Error Check (PEC) byte is calculated in CRC-8 format at"\
                    "\r\nboth transmission and reception. These temperature values are continuously printed to a"\
                    "\r\nterminal. Additionally, status information and error messages are displayed on the host PC."\
                    "\r\nThe error LED will turn on if an error occurs.\r\n\r\n"

/* Macros for text color */
#define CTRL_RESET                      "\x1B[0m"
#define CTRL_TEXT_BRIGHT_GREEN          "\x1B[1;32m"

/* Macro for SMBus event */
#define SMBUS_RESET_EVENT               (255)

/* Macros for SMBus transfer */
#define SMBUS_THREE_BYTE                (3U)
#define SMBUS_TWO_BYTE                  (2U)
#define SMBUS_ONE_BYTE                  (1U)

/* Macro for the conversion */
#define CONVERT_SECOND_TO_MILISECOND    (1000)

/* Macros for sensor register address */
#define CFG_ADR                         (0x01)
#define DATA_ADR                        (0x00)

/* Macros for configuration register write value. Set resolution as 12-bits and enable PEC */
#define CFG_8BIT_HIGHER                 (0x00)
#define CFG_8BIT_LOWER                  (0x68)
#define CFG_DATA_SIZE                   (3U)
#define TEMP_DATA_SIZE                  (3U)

/* Macros for manipulating and print the temperature values */
#define FLOAT_RESET_VALUE               (0.0F)
#define TEMPERATURE_INDEX               (0U)
#define FLOAT_DIVISOR                   (16.0F)
#define SHIFT_FOUR_BITS                 (4U)
#define DEGREE_SYMBOL                   ("\xB0")
#define BUFF_SIZE                       (30U)
#define PRINT_DELAY                     (2U)

/* Macro for Error LED */
#if defined (BOARD_RA2A1_EK) || defined (BOARD_RA4M1_EK) || defined (BOARD_RA6M1_EK) || defined (BOARD_RA6M2_EK) \
|| defined (BOARD_RA6M5_CK)
    #define LED_ERROR_STATE             (0U)
#elif defined (BOARD_RA4W1_EK) || defined (BOARD_RA2E3_FPB) || defined (BOARD_RA4E1_FPB) || defined (BOARD_RA6E1_FPB) \
|| defined (BOARD_RA4T1_MCK) || defined (BOARD_RA6T2_MCK) || defined (BOARD_RA6T3_MCK) || defined (BOARD_RA8T1_MCK) \
|| defined (BOARD_RA6T1_RSSK) || defined (BOARD_RA8E1_FPB) || defined (BOARD_RA2T1_FPB)
    #define LED_ERROR_STATE             (1U)
#else
    #define LED_ERROR_STATE             (2U)
#endif

/* Enumeration for led state use in application */
#if defined (BOARD_RA4W1_EK) || defined (BOARD_RA4T1_MCK) || defined (BOARD_RA6T2_MCK) || defined (BOARD_RA6T3_MCK)\
|| defined (BOARD_RA8T1_MCK) || defined (BOARD_RA6T1_RSSK) || defined (BOARD_RA8T2_MCK)
typedef enum e_led_power
{
    LED_POWER_ON = BSP_IO_LEVEL_LOW,
    LED_POWER_OFF = BSP_IO_LEVEL_HIGH,
}led_power_t;
#else
typedef enum e_led_power
{
    LED_POWER_ON = BSP_IO_LEVEL_HIGH,
    LED_POWER_OFF = BSP_IO_LEVEL_LOW,
}led_power_t;
#endif

/* Macro for IIC used version */
#if __has_include("r_iic_master.h")
    #define USE_IIC_MODULE
#elif __has_include("r_iic_b_master.h")
    #define USE_IIC_B_MODULE
#endif

/* Public function declaration */
void smbus_entry(void);

#endif /* SMBUS_EP_H_ */

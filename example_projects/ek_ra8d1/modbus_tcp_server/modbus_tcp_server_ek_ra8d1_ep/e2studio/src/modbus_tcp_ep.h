/***********************************************************************************************************************
* Copyright (c) 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef MODBUS_TCP_EP_H_
#define MODBUS_TCP_EP_H_

#define EP_VERSION      ("1.0")
#define MODULE_NAME     "r_modbus_tcp_server"
#define BANNER_INFO     "\r\n********************************************************************************"\
                        "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module                 *"\
                        "\r\n*   Example Project Version %s                                                *"\
                        "\r\n*   Flex Software Pack Version  %d.%d.%d                                          *"\
                        "\r\n********************************************************************************"\
                        "\r\nRefer to the accompanying .md file for Example Project details and" \
                        "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#define MODBUS_TCP_EP_DESCRIPTION   "\r\nThis example implements a Modbus TCP server on a Renesas RA MCU\r\n"\
                                    "using the Flexible Software Package (FSP) and an RTOS-based TCP/IP stack.\r\n"\
                                    "It maps coils, discrete inputs, holding registers, and input registers\r\n"\
                                    "to on-board LEDs, user switches, and an ADC temperature sensor.\r\n"\
                                    "A Modbus client (e.g., QModMaster on a PC) can control LEDs,\r\n"\
                                    "read switch status, adjust the LED1 blink interval, and monitor temperature\r\n"\
                                    "via standard Modbus function codes. The EP information and error messages are\r\n"\
                                    "output to a terminal, using either the SEGGER J-Link RTT Viewer\r\n"\
                                    "or a UART serial terminal, with UART selected as the default interface.\r\n\r\n"

#define MODBUS_FUNCTION_TABLE   "\r\n============================================\r\n"\
                                "Supported Modbus TCP Function Codes\r\n"\
                                "============================================\r\n"\
                                " FC | Hex  | Description\r\n"\
                                "--------------------------------------------\r\n"\
                                " 1  | 0x01 | Read Coils – Reads LED ON/OFF states\r\n"\
                                " 2  | 0x02 | Read Discrete Inputs – Reads push button status\r\n"\
                                " 3  | 0x03 | Read Holding Registers – Reads LED1 Blink Delay value\r\n"\
                                " 4  | 0x04 | Read Input Registers – Reads temperature value (read-only)\r\n"\
                                " 5  | 0x05 | Write Single Coil – Controls a single LED output\r\n"\
                                " 6  | 0x06 | Write Single Register – Updates LED1 Blink Delay value\r\n"\
                                " 15 | 0x0F | Write Multiple Coils – Controls multiple LEDs simultaneously\r\n"\
                                "============================================\r\n"

void modbus_tcp_entry(void);

/* Modbus function codes */
#define MODBUS_FC_READ_COILS                 (0x01U)
#define MODBUS_FC_READ_DISCRETE_INPUTS       (0x02U)
#define MODBUS_FC_READ_HOLDING_REGISTERS     (0x03U)
#define MODBUS_FC_READ_INPUT_REGISTERS       (0x04U)
#define MODBUS_FC_WRITE_SINGLE_COIL          (0x05U)
#define MODBUS_FC_WRITE_SINGLE_REGISTER      (0x06U)
#define MODBUS_FC_WRITE_MULTIPLE_COILS       (0x0FU)
#define MODBUS_FC_WRITE_MULTIPLE_REGISTERS   (0x10U)

#define MODBUS_COIL_LED1_ADDR   (0U)
#define MODBUS_COIL_LED2_ADDR   (1U)
#define MODBUS_COIL_LED3_ADDR   (2U)
#define MODBUS_COIL_COUNT       (3U)

#define MODBUS_INPUT_SW1_ADDR   (0U)
#define MODBUS_INPUT_SW2_ADDR   (1U)
#define MODBUS_INPUT_COUNT      (2U)

#define MODBUS_INPUT_REGISTER_START_ADDR   (0U)
#define MODBUS_INPUT_REGISTER_END_ADDR     (1U)
#define MODBUS_HOLDING_REGISTER_ADDR       (0U)
#define MODBUS_REGISTER_COUNT              (1U)

/* MCU Temperature Conversion Coefficients (RA8D1 calibration) */
#define ADCTEMP_AS_C(adc_val)     ((((float)(adc_val)) * 0.196551f) - 277.439f)
#define ADCTEMP_AS_F(adc_val)     ((((float)(adc_val)) * 0.353793f) - 467.39f)

#endif /* MODBUS_TCP_EP_H_ */

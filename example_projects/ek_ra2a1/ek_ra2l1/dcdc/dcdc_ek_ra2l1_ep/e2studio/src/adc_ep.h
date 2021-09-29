/***********************************************************************************************************************
 * File Name    : adc_ep.h
 * Description  : Contains data structures and functions used in adc_ep.c/.h.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/
#ifndef ADC_EP_H_
#define ADC_EP_H_

/*******************************************************************************************************************//**
 * @ingroup adc_ep
 * @{
 **********************************************************************************************************************/

/* Macros for Commands to be received through RTT inputs */
#define SCAN_START       0x01
#define SCAN_STOP        0x02

/* Macro for default buff size for reading through RTT */
#define BUFF_SIZE         0x0F

/* Macros for checking the deviation in adc values */
#define TEMPERATURE_DEVIATION_LIMIT       0x04

/* Macro for indexing buffer. used to read the bytes received from RTT input */
#define BUFF_INDEX       0x00

/* Macro to provide delay in read adc data*/
#define ADC_READ_DELAY        0x01

/* Macro to set value to output voltage control*/
#define VREFADCG_VALUE    0x03

/* Macro to set value to enable VREFADC output*/
#define VREFADCG_ENABLE   0x03

#define SHIFT_BY_ONE     0x01
#define SHIFT_BY_THREE   0x03

#define DCDC_DISABLED            0X00
#define DCDC_ENABLE_NOT_STARTED  0X01
#define DCDC_ENABLE_AT_STARTUP   0X02
#define LDO_MODE                 0x04
#define LDO_BOOST_MODE           0x05
#define TOTAL_MODES              0x06
#define MODE_STRING_LENGTH       0x32

/* Macros for menu options to be displayed */
#define MENU_OPTION       "\r\nMENU to Select\r\n"\
                          "Press 1 to Start ADC Scan\r\n"\
                          "Press 2 to Stop ADC Scan(Only for Continuous mode)\r\n"\
						  "User Input :"


#define EP_INFO           "\nThis example project demonstrates the typical use of the DC-DC Regulator using ADC module HAL"\
                          "\nAPI's provided by FSP. The project enables DC-DC regulator from BSP properties and calls"\
                          "\nR_BSP_PowerModeSet API at run time to change the MCU power input from Linear Regulator (LDO) mode"\
                          "\nto Switching Regulator (DC-DC) mode with Voltage range selected as 2.7V - 3.6V."\
                          "\nThis example project can be easily modified to support varying the input voltage range at run time as well."\
                          "\nThis can be done by calling above mentioned API with required Voltage range as an input."\
                          "\nThe API will return the previous mode value.\n"\
                          "\nUser can initiate the ADC scan using JLinkRTTViewer by sending commands."\
                          "\nResult and ADC status is displayed on the JLinkRTTViewer.\n\n"

/* Reading the commands from RTT input and process it*/
fsp_err_t read_process_input_from_RTT(void);

/* Read the adc data available */
fsp_err_t adc_read_data(void);

/* close the open adc module  */
void deinit_adc_module(void);

/** @} */
#endif /* ADC_EP_H_ */

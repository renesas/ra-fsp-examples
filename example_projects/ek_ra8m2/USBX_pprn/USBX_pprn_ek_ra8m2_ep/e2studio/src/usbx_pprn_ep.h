/***********************************************************************************************************************
 * File Name    : usbx_pprn_ep.h
 * Description  : Contains macros used in usbx_pprn_thread_entry.c and usbx_pprn_read_thread_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2022 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef USBX_PPRN_EP_H_
#define USBX_PPRN_EP_H_

#include "ux_api.h"
#include "ux_system.h"
#include "ux_device_class_printer.h"

/*******************************************************************************************************************//**
 * @addtogroup usbx_pprn_ep
 * @{
 **********************************************************************************************************************/

/******************************************************************************
 Macro definitions
 ******************************************************************************/
/* To display EP info on the RTT Viewer */
#define EP_INFO        "\r\nThis project demonstrates the basic functionality of USBX PPRN Module running on\r\n"\
                       "Renesas RA MCUs based on Renesas FSP using AzureRTOS. The user will be able to use the\r\n"\
                       "RA board as a printer device by connecting it to the host PC. Status of application (print\r\n"\
                       "information and error messages) will be displayed on J-Link RTT Viewer.\r\n"\


/* Macros to print info, error and trap the error */
#define PRINT_INFO_STR(str)         (app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_INFO_STR, sizeof(str), (str)))
#define PRINT_ERR_STR(str)          (app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_ERR_STR, sizeof(str), (str)))
#define PRINT_NEXT_LINE(n)          (app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_NEXT_LINE, sizeof(n), &(n)))
#define PRINT_DATA(data)            (app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_PRINT_DATA, sizeof(data), &(data)))
#define ERROR_TRAP(err)             (app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_ERR_TRAP, sizeof(UINT *), &(err)))

/* USBX device configuration settings */
#define DEMO_STACK_SIZE                         (4096U)
/* Protocol: 1 - Unidirectional, 2 - Bidirectional, 3 - Bidirectional (1284-compatible, not recommended) */
#define DEMO_PROTOCOL                           (1U)
/* Length of g_device_framework_hi_speed[] */
#define DEVICE_FRAME_LENGTH_HIGH_SPEED          ((53U) + ((DEMO_PROTOCOL > 1) ? 7 : 0))
/* Length of g_device_framework_full_speed[] */
#define DEVICE_FRAME_LENGTH_FULL_SPEED          ((43U) + ((DEMO_PROTOCOL > 1) ? 7 : 0))
/* Length of g_string_framework[]. If edit g_string_framework[], need to change this value */
#define STRING_FRAMEWORK_LENGTH                 (53U)
#define LANGUAGE_ID_FRAME_WORK_LENGTH           (2U)
#define CONFIG_NUMB                             (1U)
#define INTERFACE_NUMB0                         (0x00)
#define INTERFACE_NUMB1                         (0x01)
#define MEMPOOL_SIZE                            (18432U)
#define BYTE_SIZE                               (4U)
#define DATA_LEN                                (512U)
#define MAX_PACKET_SIZE_HS                      (512U)
#define MAX_PACKET_SIZE_FS                      (64U)
#define MAX_PRINT_LINE                          (0xFF)
#define BUFFER_SIZE_MIN                         (30U)
#define PRINT_REQUEST_SIZE                      (4096U)
#define VALUE_ONE                               (1U)
#define VALUE_THREE                             (3U)
#define SLEEP_TIME_10                           (10U)
#define TIME_DELAY                              (0xFFFFFFFF)
/* Length of printer_device_id[]. If edit printer_device_id[], need to change this value */
#define PRINTER_DEVICE_ID_LENGTH                (91U)

/* HIGH_SPEED_CONFIG
 * When enabled, this macro uses the HIGH-SPEED interface
 * on EK-RA6M3, EK-RA6M5, EK-RA8M1, EK-RA8D1, EK-RA8P1, EK-RA8M2
 *
 * When disabled, it uses the FULL-SPEED interface
 * on EK-RA4M2, EK-RA4M3, EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M4, EK-RA6M5, EK-RA8M1, EK-RA8D1,
 * MCK-RA8T1, EK-RA4L1, EK-RA8E2, EK-RA8P1, MCK-RA8T2, EK-RA8M2 */
#define HIGH_SPEED_CONFIG

/* Uncomment this macro to use the actual printer.
 * If this macro is commented out, the EP will print data to the RTT Viewer. */
//#define USE_PRINTER

#endif /* USBX_PPRN_EP_H_ */

/*******************************************************************************************************************//**
 * @} (end defgroup usbx_pprn_ep)
 **********************************************************************************************************************/

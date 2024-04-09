/***********************************************************************************************************************
 * File Name    : usbx_pprn_ep.h
 * Description  : Contains macros used in usbx_pprn_thread_entry.c and usbx_pprn_read_thread_entry.c
 ***********************************************************************************************************************/
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
 * Copyright (C) 2022 Renesas Electronics Corporation. All rights reserved.
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
/* To display EP info on the RTT viewer */
#define EP_INFO        "\r\nThis project demonstrates the basic functionality of USBX PPRN Module running on\r\n"\
                       "Renesas RA MCUs based on Renesas FSP using AzureRTOS. User will be able to use the\r\n"\
                       "RA board as a printer device by connecting it to the HOST PC (Windows 10). Status of \r\n"\
                       "application (Print information and error messages) will be displayed on J-Link RTT Viewer.\r\n"\


/* macros to print info, error and trap the error.*/
#define PRINT_INFO_STR(str)         (app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_INFO_STR, sizeof(str), (str)))
#define PRINT_ERR_STR(str)          (app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_ERR_STR, sizeof(str), (str)))
#define PRINT_NEXT_LINE(n)          (app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_NEXT_LINE, sizeof(n), &(n)))
#define PRINT_DATA(data)            (app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_PRINT_DATA, sizeof(data), &(data)))
#define ERROR_TRAP(err)             (app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_ERR_TRAP, sizeof(UINT *), &(err)))

/* USBx device configuration settings */
#define DEMO_STACK_SIZE                         (4096U)
#define DEMO_PROTOCOL                           (1U)    /* 1-Uni-dir, 2-Bi-dir, 3-Bi-dir-1284. Do not set 3 (uncare) */
#define DEVICE_FRAME_LENGTH_HIGH_SPEED          ((53U) + ((DEMO_PROTOCOL > 1) ? 7 : 0))   /* Length of g_device_framework_hi_speed[] */
#define DEVICE_FRAME_LENGTH_FULL_SPEED          ((43U) + ((DEMO_PROTOCOL > 1) ? 7 : 0))   /* Length of g_device_framework_full_speed[] */
#define STRING_FRAMEWORK_LENGTH                 (53U)   /* Length of g_string_framework[]. If edit g_string_framework[], need to change this value. */
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
/* Length of printer_device_id[]. If edit printer_device_id[], need to change this value. */
#define PRINTER_DEVICE_ID_LENGTH                (91U)

/* HIGH_SPEED_CONFIG
 * macro when enabled uses the HIGH SPEED Interface
 * on EK-RA6M3 and EK-RA6M5
 *
 * macro when disabled uses the FULL SPEED Interface
 * on EK-RA4M2, EK-RA4M3, EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M4, EK-RA6M5, EK-RA8M1 */
#define HIGH_SPEED_CONFIG

/* Un-comment this Macro for the use of actual printer.
 * The EP will print data on the RTT viewer if this macro is commented.  */
//#define USE_PRINTER

#endif /* USBX_PPRN_EP_H_ */
/*******************************************************************************************************************//**
 * @} (end defgroup usbx_pprn_ep)
 **********************************************************************************************************************/


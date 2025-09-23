/***********************************************************************************************************************
 * File Name    : usbx_pcdc_acm_ep.h
 * Description  : Contains macros, data structures and functions used in the EP
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef USBX_PCDC_ACM_EP_H_
#define USBX_PCDC_ACM_EP_H_

/*******************************************************************************************************************//**
 * @addtogroup usbx_pcdc_acm_ep
 * @{
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 Macro definitions
 **********************************************************************************************************************/
/* To display EP info on the RTT Viewer */
#define EP_INFO     "\r\nThis project demonstrates the basic functionality of USBX Device CDC ACM module"\
		            "\r\non Renesas RA MCUs based on Renesas FSP using AzureRTOS. The user will use an RA board"\
		            "\r\nas a CDC device which will be enumerated as a COM port when connected to the host PC."\
					"\r\nOnce the board is connected, the user can provide input from the serial terminal."\
					"\r\nThe user input will be echoed on the serial terminal. The application status messages"\
					"\r\nwill be displayed on the J-Link RTT Viewer.\r\n"\

/* Macros to print info, error and trap the error */
#define PRINT_INFO_STR(str)  ({app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_INFO_STR, sizeof(str), (str));})
#define PRINT_ERR_STR(str)   ({app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_ERR_STR, sizeof(str), (str));})
#define ERROR_TRAP(err)      ({app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_ERR_TRAP, sizeof(UINT *), &(err));})

/* USBX device configuration settings */
#define DEVICE_FRAME_LENGTH_HIGH_SPEED              (103U)
#define DEVICE_FRAME_LENGTH_FULL_SPEED              (93U)
#define STRING_FRAMEWORK_LENGTH                     (105U)
#define LANGUAGE_ID_FRAME_WORK_LENGTH               (2U)
#define CONFIG_NUMB                                 (1U)
#define INTERFACE_NUMB0                             (0x00)
#define INTERFACE_NUMB1                             (0x01)
#define MEMPOOL_SIZE                                (18432U)
#define BYTE_SIZE                                   (4U)
#define DATA_LEN                                    (2048U)
#define WRITE_DATA_LEN                              (59U)
#define MAX_PACKET_SIZE_HS                          (512U)
#define MAX_PACKET_SIZE_FS                          (64U)

#endif /* USBX_PCDC_ACM_EP_H_ */

/*******************************************************************************************************************//**
 * @} (end defgroup usbx_pcdc_acm_ep)
 **********************************************************************************************************************/

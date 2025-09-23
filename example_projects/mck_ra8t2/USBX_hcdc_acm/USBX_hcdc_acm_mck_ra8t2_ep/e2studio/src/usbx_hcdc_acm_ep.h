/***********************************************************************************************************************
 * File Name    : usbx_hcdc_acm_ep.h
 * Description  : Contains data structures and functions used in usbx_hcdc_acm_ep.h.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef USBX_HCDC_ACM_EP_H_
#define USBX_HCDC_ACM_EP_H_

/* To display EP info on the RTT Viewer */
#define EP_INFO     "\r\nThis project demonstrates the basic functionality of USBX Host CDC-ACM Module\r\n"\
                    "running on Renesas RA MCUs based on Renesas FSP using AzureRTOS. Board 1 acts as Host device\r\n"\
                    "running USBX HCDC-ACM example project and Board 2 acts as Peripheral device running\r\n"\
                    "USBX PCDC-ACM example project and connected with Type-C USB cable via OTG.\r\n"\
                    "The host can send data to the device and device will echo the data back to host. Status of\r\n"\
                    "application will be displayed on J-Link RTT Viewer\r\n"\

/* Macros to print info, error and trap the error */
#define PRINT_INFO_STR(str)  ({app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_INFO_STR, sizeof(str), (str));})
#define PRINT_ERR_STR(str)   ({app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_ERR_STR, sizeof(str), (str));})
#define ERROR_TRAP(err)      ({app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_ERR_TRAP, sizeof(UINT *), &(err));})

#define DATA_LEN                   (1030U)
#define MAX_PACKET_SIZE            (2048U)
#define MEMPOOL_SIZE               (18432)
#define CDCACM_FLAG                ((uint32_t) 0x0001)
#define VALUE_4                    (4)
#define MOD_VAL                    (50)
#define READ_LEN                   (64)
#define WAIT_TIME                  (50)
#define SUCCESS                    (0U)
#define UX_FSP_DEVICE_INSERTION    (0x01U)
#define UX_FSP_DEVICE_REMOVAL      (0x02U)

#endif /* USBX_HCDC_ACM_EP_H_ */

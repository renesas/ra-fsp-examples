/***********************************************************************************************************************
 * File Name    : usbx_pmsc_ep.h
 * Description  : Contains macros, data structures and functions used in the pmsc_thread_entry.c
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef USBX_PMSC_EP_H_
#define USBX_PMSC_EP_H_

#include "ux_api.h"
#include "ux_system.h"

/* EP Banner Info. */
#define EP_INFO        "\r\nThis project demonstrates the basic functionality of USBX Device Mass Storage Class Module running\r\n"\
                       "on Renesas RA MCUs based on Renesas FSP using AzureRTOS. RA Board will act as a device MSC peripheral.\r\n"\
                       "The USBX PMSC driver internally configures memory device (block media with sdmmc) to enumerate as a memory\r\n"\
                       "drive on host PC. User can read, write and perform various file operations on the disk.\r\n"\
                       "Application status and any errors will be printed on the JLink RTTViewer.\r\n\n"

/* macros to print info, error and trap the error.*/
#define PRINT_INFO_STR(str)  (app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_INFO_STR, sizeof(str), (str)))
#define PRINT_ERR_STR(str)   (app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_ERR_STR, sizeof(str), (str)))
#define ERROR_TRAP(err)      (app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_ERR_TRAP, sizeof(UINT *), &(err)))

/* USBX device configuration settings */
#define DEVICE_FRAME_LENGTH_HIGH_SPEED              (60U)
#define DEVICE_FRAME_LENGTH_FULL_SPEED              (50U)
#define STRING_FRAMEWORK_LENGTH                     (93U)
#define LANGUAGE_ID_FRAME_WORK_LENGTH               (2U)
#define MEMPOOL_SIZE                                (14336U)
#define BYTE_SIZE                                   (4U)
#define USB_MSC_PLUG_IN                             (1UL << 0)
#define USB_MSC_PLUG_OUT                            (1UL << 1)


#endif /* USBX_PMSC_EP_H_ */

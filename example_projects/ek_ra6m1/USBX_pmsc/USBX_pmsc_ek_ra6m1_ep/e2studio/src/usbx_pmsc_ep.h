/***********************************************************************************************************************
 * File Name    : usbx_pmsc_ep.h
 * Description  : Contains macros, data structures and functions used in the pmsc_thread_entry.c
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
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
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
#define PRINT_INFO_STR(str)  app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_INFO_STR, sizeof(str), str);
#define PRINT_ERR_STR(str)   app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_ERR_STR, sizeof(str), str);
#define ERROR_TRAP(err)      app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_ERR_TRAP, sizeof(UINT *), &err);

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

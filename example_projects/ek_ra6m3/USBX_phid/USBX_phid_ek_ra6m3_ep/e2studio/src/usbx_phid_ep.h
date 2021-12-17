/***********************************************************************************************************************
 * File Name    : usbx_phid_ep.h
 * Description  : Contains macros, data structures and functions used in the usbx_phid source code
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


#ifndef USBX_PHID_EP_H_
#define USBX_PHID_EP_H_

#include "phid_thread.h"
#include "r_usb_basic.h"
#include "r_usb_basic_cfg.h"

#include "ux_api.h"
#include "ux_system.h"
#include "ux_device_class_hid.h"
/*******************************************************************************************************************//**
 * @addtogroup usbx_phid_ep
 * @{
 **********************************************************************************************************************/

/******************************************************************************
 Macro definitions
 ******************************************************************************/
/* To display EP info on the RTT viewer */
#define EP_INFO        "\r\nThis project demonstrates the basic functionality of USBX Peripheral Human Interface \r\n"\
                       "Device (HID) module running on Renesas RA MCUs based on Renesas FSP using AzureRTOS. \r\n"\
                       "RA board will act as HID device. The project configures RA board as a Keyboard. The RA board \r\n"\
                       "sends the host PC character from a-z & numbers 0-9 as per user's request from RTT viewer.\r\n"\
                       "USB status and any error messages are displayed on JlinkRTT viewer\r\n"\

/* macros to print info, error and trap the error.*/
#define PRINT_INFO_STR(str)  app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_INFO_STR, sizeof(str), str);
#define PRINT_ERR_STR(str)   app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_ERR_STR, sizeof(str), str);
#define ERROR_TRAP(err)      app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_ERR_TRAP, sizeof(UINT *), &err);

#define MEMPOOL_SIZE                    (18432)
#define REPORT_DESCRIPTOR_LENGTH        (76)
#define VALUE_4                         (4)
#define DELAY_100_MS                    (100)
#define DELAY_3000_MS                   (3000)
#define DELAY_5000_MS                   (5000)
#define VALUE_10000_MS                  (10000)
#define HID_NUM_LOCK_MASK               (1)
#define HID_CAPS_LOCK_MASK              (2)
#define DEVICE_FRAME_LENGTH_SPEED       (52U)
#define STRING_FRAMEWORK_LENGTH         (53U)
#define LANGUAGE_ID_FRAME_WORK_LENGTH   (2U)
#define CONFIG_NUMB                     (1U)
#define INTERFACE_NUMB0                 (0x00)
#define HID_FLAG                        ((ULONG) 0x0001)
#define HID_ACTIVATE_FLAG               ((ULONG) 0x0004)
#define HID_DEACTIVATE_FLAG             ((ULONG) 0x0008)
#define FIRST_KEY                       (0x04)  //First Key to pass as starting character 'a'
#define END_KEY                         (35)

#define EVENT_LENGTH                    (8U)
#define ZERO                            (0U)
#define ONE                             (1U)
#define TWO                             (2U)
/*******************************************************************************************************************//**
 * @} (end defgroup usbx_phid_ep)
 **********************************************************************************************************************/

#endif /* USBX_PHID_EP_H_ */

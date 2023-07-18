/***********************************************************************************************************************
 * File Name    : usbx_cdc_dfu_peri_device_common.h
 * Description  : Contains macros used in usbx_cdc_dfu_peripheral_device_entry.c and usbx_dfu_peripheral_device_entry.c
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

/*******************************************************************************************************************//**
 * @addtogroup usbx_dfu_ep
 * @{
 **********************************************************************************************************************/

#ifndef USBX_CDC_DFU_PERI_DEVICE_COMMON_H_
#define USBX_CDC_DFU_PERI_DEVICE_COMMON_H_

#include <usbx_dfu_peripheral_device.h>
#include "ux_device_class_cdc_acm.h"
#include "ux_device_class_dfu.h"

/* Macro definitions */
#define UX_ZERO                                 (0U)
#define INTERFACE_NUMB0                         (0x00)
#define INTERFACE_NUMB1                         (0x01)
#define INTERFACE_NUMB2                         (0x02)
#define CONFIG_NUMB                             (1U)
#define BYTE_SIZE                               (4U)
#define MEMPOOL_SIZE                            (2*18432U)
#define BUFFER_SIZE                             (512*100U)
#define BUFFER_SIZE_50                          (50U)

#define ZERO                                    (0U)
#define ONE                                     (1U)

#define ON                                      (1U)
#define OFF                                     (0U)
#define RA_NULL                                 ((UCHAR)(0))
#define NULL_0                                    ('\0')

#define WORK_AROUND                             (0U)
#define WORK_AROUND2                            (0U)

#define DEVICE_FRAME_LENGTH_CDC_FULL_SPEED      (119U)
#define DEVICE_FRAME_LENGTH_CDC_HI_SPEED        (129U)   /* CDC(103) + IAD(8) + DFU(9*2) */
#define STRING_FRAMEWORK_CDC_LENGTH             (105U)
#define CDC_LANGUAGE_ID_FRAME_WORK_LENGTH       (2U)

#define DFU_DEVICE_FRAME_LENGTH_HIGH_SPEED      (55U)
#define DFU_DEVICE_FRAME_LENGTH_FULL_SPEED      (45U)
#define DFU_STRING_FRAMEWORK_LENGTH             (94U)
#define DFU_LANGUAGE_ID_FRAME_WORK_LENGTH       (2U)

#define CDC_DATA_LENGTH                         (1024U)
#define CDC_ACTUAL_LENGTH                       (15U)

#define THREAD_SLEEP_10                         (10U)
#define THREAD_SLEEP_20                         (20U)
#define THREAD_SLEEP_50                         (50U)
#define THREAD_SLEEP_100                        (100U)
#define THREAD_SLEEP_200                        (200U)

#define DETACH_EVENT_FLAG                       ((ULONG) 0x01)
#define ATTACH_EVENT_FLAG                       ((ULONG) 0x02)

#define EP_INFO        "\r\nThe sample code accompanying this file shows the operation of a USBX Device Firmware Update \r\n"\
                       "(DFU) driver on Renesas  RA MCUs based on Renesas FSP using Azure RTOS. The RA MCU board will be\r\n"\
                       "connected to the host PC, where it acts as a peripheral device. Prepare a file (.txt,.srec,.xml,.hex, etc.)\r\n"\
                       "to download to the RA MCU which has both CDC and DFU interfaces. In a real-world application, a new \r\n"\
                       "device firmware can be downloaded. In this example project, a set of data is used to demonstrate the \r\n"\
                       "download functionality.\r\n"\
                       "\r\nThe DFU example application works in four steps:\r\n"\
                       "\r\na. First, the system is initialized with support of the Interface Association Descriptor. The CDC \r\n"\
                       "   and DFU classes are then initialized with separate interfaces.\r\n"\
                       "\r\nb. Secondly, the device mounts as a CDC class and an application on the host (Windows PC) will exercise\r\n"\
                       "   the DFU class and send a download request. Upon reception of this command, the system un-initializes the CDC\r\n"\
                       "   class and initializes the DFU class.\r\n"\
                       "\r\nc. Then, the device will disappear from the bus for a short time (enough for the host and the device to \r\n"\
                       "   detect a reset sequence). Upon restarting, the device will be exclusively in DFU mode, downloading the data \r\n"\
                       "   sent from the host.\r\n"\
                       "\r\nd. Finally, when the new data has been downloaded, the host application resets the device, and upon \r\n"\
                       "   re-enumeration, the device will revert to its normal operation with the new firmware. The RA MCU will display\r\n"\
                       "   the USB enumeration status on the J-Link RTT Viewer.\r\n"\

/* macros to print info, error and trap the error.*/
#define PRINT_INFO_STR(str) (app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_INFO_STR, sizeof(str), (str)))
#define PRINT_ERR_STR(str)  (app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_ERR_STR, sizeof(str), (str)))
#define ERROR_TRAP(err)     (app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_ERR_TRAP, sizeof(UINT *), &(err)))


/******************************************************************************
 * Exported global variable (to be accessed by other files)
 ******************************************************************************/

extern uint8_t g_device_framework_full_speed[];
extern uint8_t g_device_framework_hi_speed[];
extern uint8_t g_string_framework[];
extern uint8_t g_language_id_framework[];


extern UCHAR g_dfu_device_framework_full_speed[];
extern UCHAR g_dfu_string_framework[];
extern UX_SLAVE_CLASS_DFU_PARAMETER g_ux_dfu_peri_class_parameter;

extern volatile _Bool g_download_done;
extern volatile _Bool g_dfuManifestation_done;

extern UCHAR g_dfu_data[BUFFER_SIZE];
extern UCHAR g_pre_dfu_data[BUFFER_SIZE];

extern ULONG g_data_cnt;
extern ULONG g_block_no_cnt;


/* Function prototypes */
void usbx_peri_cdc_dfu_init(void);
void usbx_peri_dfu_un_init(void);
void uvc_err_status(void);
#endif /* USBX_CDC_DFU_PERI_DEVICE_COMMON_H_ */

/*******************************************************************************************************************//**
 * @} (end addtogroup  usbx_dfu_ep)
 **********************************************************************************************************************/

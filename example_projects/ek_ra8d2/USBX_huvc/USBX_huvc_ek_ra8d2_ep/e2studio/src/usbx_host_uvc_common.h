/***********************************************************************************************************************
 * File Name    : usbx_host_uvc_common.h
 * Description  : Contains macros, data structures and functions used in usbcamera_thread_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2023 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef USBX_HOST_UVC_COMMON_H_
#define USBX_HOST_UVC_COMMON_H_

/* Macros to print info, error and trap the error */
#define PRINT_INFO_STR(str) (app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_INFO_STR, sizeof(str), (str)))
#define PRINT_ERR_STR(str)  (app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_ERR_STR, sizeof(str), (str)))
#define ERROR_TRAP(err)     (app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_ERR_TRAP, sizeof(UINT *), (void *)&(err)))

/* To display EP info on the RTT Viewer */
#define EP_INFO         "\r\nThis project demonstrates the basic functionality of USBX Host UVC Module running\r\n"\
                        "on Renesas RA MCUs based on Renesas FSP using AzureRTOS. The USB Camera will be\r\n"\
                        "connected as a peripheral device to the RA board which will act as a host device.\r\n"\
                        "A user menu will be provided over the RTT Viewer. The user can select option 1 to\r\n"\
                        "start video streaming and select option 2 to stop video streaming. After stopping\r\n"\
                        "the video streaming, the last captured image can be displayed on the e2studio's\r\n"\
                        "memory window. Error and info messages will be printed on J-link RTT Viewer.\r\n"

/* To display EP menu on the RTT Viewer */
#define USBX_HUVC_MENU  "\r\nUSBX Host UVC Main Menu options:"\
                        "\r\n1. Input '1' to start the streaming video."\
                        "\r\n2. Input '2' to stop the video channel and capture a JPEG image."\
                        "\r\nThe user input:  "

/* Macros for Menu Options */
#define START_VIDEO                     (1)
#define STOP_VIDEO_AND_CAPTURE_IMAGE    (2)

/* Macros constant value */
#define ZERO                            (0)
#define BUFFER_ZERO                     (0)
#define BUFFER_ONE                      (1)
#define EIGHT_BYTES                     (8)
#define STOP                            (1)
#define START                           (2)
#define CAPTURE_IMAGE                   (3)

#define WAIT_10_TICKS                   (10)
#define WAIT_100_TICKS                  (100)

#endif /* USBX_HOST_UVC_COMMON_H_ */

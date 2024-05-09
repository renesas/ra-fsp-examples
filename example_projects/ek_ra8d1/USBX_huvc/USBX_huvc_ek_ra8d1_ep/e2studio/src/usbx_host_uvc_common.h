/***********************************************************************************************************************
 * File Name    : usbx_host_uvc_common.h
 * Description  : Contains macros, data structures and functions used in usbcamera_thread_entry.c.
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
 * Copyright (C) 2023 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#ifndef USBX_HOST_UVC_COMMON_H_
#define USBX_HOST_UVC_COMMON_H_
/* Macros to print info, error and trap the error.*/
#define PRINT_INFO_STR(str) (app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_INFO_STR, sizeof(str), (str)))
#define PRINT_ERR_STR(str)  (app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_ERR_STR, sizeof(str), (str)))
#define ERROR_TRAP(err)     (app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_ERR_TRAP, sizeof(UINT *), (void *)&(err)))

/* To display EP info on the RTT viewer */
#define EP_INFO         "\r\nThis project demonstrates the basic functionality of USBX Host UVC Module running\r\n"\
                        "on Renesas RA MCUs based on Renesas FSP using AzureRTOS. The USB Camera will be\r\n"\
                        "connected as a peripheral device to the RA board which will act as a host device.\r\n"\
                        "A user menu will be provided over the RTT Viewer.The User can select option 1 to\r\n"\
                        "start video streaming and select option 2 to stop video streaming. After stopping\r\n"\
                        "the video streaming, the last captured image can be displayed on the e2studio's\r\n"\
                        "memory window. Error and info messages will be printed on J-link RTT Viewer.\r\n"

/* To display EP menu on the RTT viewer */
#define USBX_HUVC_MENU  "\r\nUSBX Host UVC Main Menu options:"\
                        "\r\n1. Input '1' to start the streaming video."\
                        "\r\n2. Input '2' to stop the video channel and capture a JPEG image."\
                        "\r\nThe User Input:  "

/* Macros for Menu Options */
#define    START_VIDEO                       (1)
#define    STOP_VIDEO_AND_CAPTURE_IMAGE      (2)

/* Macros constant value */
#define ZERO             (0)
#define BUFFER_ZERO      (0)
#define BUFFER_ONE       (1)
#define EIGHT_BYTES      (8)
#define STOP             (1)
#define START            (2)
#define CAPTURE_IMAGE    (3)

#define WAIT_10_TICKS    (10)
#define WAIT_100_TICKS   (100)


#endif /* USBX_HOST_UVC_COMMON_H_ */

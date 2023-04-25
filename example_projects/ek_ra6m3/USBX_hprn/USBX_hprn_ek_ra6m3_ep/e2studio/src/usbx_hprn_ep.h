/***********************************************************************************************************************
 * File Name    : usbx_hprn_ep.h
 * Description  : Contains macros used in usbx_hprn_entry.c
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

#ifndef USBX_HPRN_EP_H_
#define USBX_HPRN_EP_H_

#include "ux_api.h"
#include "ux_system.h"

/*******************************************************************************************************************//**
 * @addtogroup usbx_hprn_ep
 * @{
 **********************************************************************************************************************/

/******************************************************************************
 Macro definitions
 ******************************************************************************/
/* To display EP info on the RTT viewer */
#define EP_INFO        "\r\nThis project demonstrates the basic functionality of USBX HPRN Module running on\r\n"\
                       "Renesas RA MCUs based on Renesas FSP using AzureRTOS. User will be able to use the\r\n"\
                       "RA board as a Host device by connecting it to the Printer Device. Status of \r\n"\
                       "application (Printer information and error messages) will be displayed on J-Link RTT Viewer.\r\n"\

#define DATA_LEN                   (1030U)
#define MAX_REQUEST_SIZE           (2048U)
#define MEMPOOL_SIZE               (18432)
#define HPRN_FLAG                  ((uint32_t) 0x0001)
#define VALUE_4                    (4)
#define MOD_VAL                    (50)
#define READ_LEN                   (64)
#define WAIT_TIME                  (50)
#define SUCCESS                    (0U)
#define UX_FSP_DEVICE_INSERTION    (0x01U)
#define UX_FSP_DEVICE_REMOVAL      (0x02U)
#define RESET_VALUE                (0x00)
#define THREAD_WAIT                (2)

/* macros to print info, error and trap the error.*/
#define PRINT_INFO_STR(str)         {app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_INFO_STR, sizeof(str), str);}
#define PRINT_ERR_STR(str)          {app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_ERR_STR, sizeof(str), str);}
#define PRINT_NEXT_LINE(n)          {app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_NEXT_LINE, sizeof(n), &n);}
#define PRINT_DATA(data)            {app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_PRINT_DATA, sizeof(data), &data);}
#define ERROR_TRAP(err)             {app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_ERR_TRAP, sizeof(UINT *), &err);}

#define DATA_MIN                    (2U)
#define DATA_MAX                    (32U)
#define SEMI_COLON                  (0x3B)

#endif /* USBX_HPRN_EP_H_ */
/*******************************************************************************************************************//**
 * @} (end defgroup usbx_hprn_ep)
 **********************************************************************************************************************/


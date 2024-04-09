/***********************************************************************************************************************
 * File Name    : sau_uart_ep.h
 * Description  : Contains data structures and functions used in sau_uart_ep.c
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
 * Copyright (C) 2024 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#ifndef SAU_UART_EP_H_
#define SAU_UART_EP_H_

/* Macro for RTT Viewer */
#define RTT_DELAY_VALUE             (200U)

/* Macro for SAU UART receive */
#define NULL_CHAR                   ('\0')
#define CR_ASCII                    (0x0D)
#define LF_ASCII                    (0x0A)
#define BUFFER_LEN                  (256U)

/* Macro for SAU UART transmit */
#define TIME_OUT_FOR_ONE_BYTE       (1000)
#define TIME_OUT_VALUE              (1U)
#define TIME_OUT_UNIT               (BSP_DELAY_UNITS_MICROSECONDS)

/* Public function declarations */
void sau_uart_ep_entry(void);

#endif /* SAU_UART_EP_H_ */

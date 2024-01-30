/***********************************************************************************************************************
 * File Name    : uart_terminal.h
 * Description  : Contains macros, data structures and functions used  common to the EP
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
 * Copyright (C) 2023 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#ifndef UART_TRANSFER_H_
#define UART_TRANSFER_H_

#include "common_utils.h"

/**********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#define UART_EP_INFO_SIZE          (512U)
#define UART_ERROR_CODE_SIZE       (32U)
#define UART_ERROR_INFO_SIZE       (128U)
#define UART_CHAR_TIMEOUT_VALUE    (1U)
#define UART_CHAR_TIMEOUT_UNIT     (BSP_DELAY_UNITS_MILLISECONDS)

/**********************************************************************************************************************
 * Public functions declarations
 **********************************************************************************************************************/
fsp_err_t uart_init(void);
void      uart_deinit(void);
fsp_err_t uart_print(uint8_t * p_buf);
fsp_err_t uart_ep_info_print(void);
fsp_err_t uart_error_info_print(uint8_t * p_func, uint8_t * p_info);
fsp_err_t uart_error_code_print(fsp_err_t err);

#endif                                 /* UART_TRANSFER_H_ */

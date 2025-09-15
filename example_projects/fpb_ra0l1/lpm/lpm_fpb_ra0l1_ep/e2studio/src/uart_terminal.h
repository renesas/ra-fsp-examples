/***********************************************************************************************************************
 * File Name    : uart_terminal.h
 * Description  : Contains macros, data structures and functions commonly used in the EP
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2023 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef UART_TRANSFER_H_
#define UART_TRANSFER_H_

#include "common_utils.h"

/***********************************************************************************************************************
* Macro definitions
***********************************************************************************************************************/
#define UART_EP_INFO_SIZE               (512U)
#define UART_ERROR_CODE_SIZE            (32U)
#define UART_ERROR_INFO_SIZE            (128U)
#define UART_CHAR_TIMEOUT_VALUE         (1U)
#define UART_CHAR_TIMEOUT_UNIT          (BSP_DELAY_UNITS_MILLISECONDS)

/***********************************************************************************************************************
* Public functions declarations
***********************************************************************************************************************/
fsp_err_t uart_init(void);
void uart_deinit(void);
fsp_err_t uart_print(uint8_t * p_buf);
fsp_err_t uart_ep_info_print(void);
fsp_err_t uart_error_info_print(uint8_t * p_func, uint8_t * p_info);
fsp_err_t uart_error_code_print(fsp_err_t err);

#endif /* UART_TRANSFER_H_ */

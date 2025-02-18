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
* Copyright (C) 2019-2020 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
 * File Name: r_ble_console.h
 * Description : Console Library.
 * History : DD.MM.YYYY Version Description           
 *         : 15.12.2020 1.00    First Release
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @file
 * @defgroup cli_console Console Library
 * @{
 * @ingroup cli
 * @brief Console Library.
***********************************************************************************************************************/
#include <stdint.h>
#include <stdbool.h>

#ifndef R_BLE_CONSOLE_H
#define R_BLE_CONSOLE_H

/* Ascii code */
#define KEY_UP    (0x41)
#define KEY_DOWN  (0x42)
#define KEY_RIGHT (0x43)
#define KEY_LEFT  (0x44)

/* Escape sequence code only valid p_escape=true. */
#define KEY_CTRLC (0x03)
#define KEY_CTRLD (0x04)
#define KEY_BS    (0x08)
#define KEY_TAB   (0x09)
#define KEY_LF    (0x0A)
#define KEY_CR    (0x0D)
#define KEY_ESC   (0x1B)
#define KEY_DEL   (0x7F)

/*******************************************************************************************************************//**
 * The callback invoked when the CLI event (SCI interrupt) trigger
***********************************************************************************************************************/
typedef void (*ble_cli_event_cb_t)(void *);

/*********************************************************************************************************************
 * Function Name: console_init
 * Description  : Initialize console.
 * Arguments    : None
 * Return Value : None
 ********************************************************************************************************************/
void console_init(void);

/*********************************************************************************************************************
 * Function Name: console_terminate
 * Description  : Terminate console.
 * Arguments    : None
 * Return Value : None
 ********************************************************************************************************************/
void console_terminate(void);

/*********************************************************************************************************************
 * Function Name: console_getc
 * Description  : Get a character from the console.
 * Arguments    : p_c      - A character to get.
 *              : p_escape - The character is escape sequence or not.
 * Return Value : true if got a character, otherwise false.
 ********************************************************************************************************************/
bool console_getc(uint8_t *p_c, bool *p_escape);

/*********************************************************************************************************************
 * Function Name: console_putsf
 * Description  : Print string on the console.
 * Arguments    : Same as printf
 * Return Value : None
 ********************************************************************************************************************/
void console_putsf(const char *p_format, ...);

/*********************************************************************************************************************
 * Function Name: get_uart_reception
 * Description  : Get uart reception on-going flag.
 * Arguments    : None
 * Return Value : True  : on-going
 *                False : Not perform UART reception
 ********************************************************************************************************************/
bool get_uart_reception(void);

/*********************************************************************************************************************
 * Function Name: set_uart_reception
 * Description  : Set uart reception on-going flag.
 * Arguments    : True  : on-going
 *                False : Not perform UART reception
 * Return Value : None
 ********************************************************************************************************************/
void set_uart_reception(bool flag);

/*********************************************************************************************************************
 * Function Name: get_uart_transmssion
 * Description  : Get uart transmission on-going flag.
 * Arguments    : None
 * Return Value : True  : on-going
 *                False : Not perform UART transmission
 ********************************************************************************************************************/
bool get_uart_transmission(void);

/*********************************************************************************************************************
 * Function Name: set_uart_transmission
 * Description  : Set uart transmission on-going flag.
 * Arguments    : True  : on-going
 *                False : Not perform UART transmission
 * Return Value : None
 ********************************************************************************************************************/
void set_uart_transmission(bool flag);

#endif /* R_BLE_CONSOLE_H */
/*@}*/

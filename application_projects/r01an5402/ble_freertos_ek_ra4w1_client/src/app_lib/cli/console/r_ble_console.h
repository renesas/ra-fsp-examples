/**********************************************************************************************************************
* Copyright (c) 2019 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*********************************************************************************************************************/
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

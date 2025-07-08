/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
/**********************************************************************************************************************
 * File Name    : jlink_console.h
 * Description  : Entry function.
 *********************************************************************************************************************/

#include "hal_data.h"
#include "r_typedefs.h"

#ifndef JLINK_CONSOLE_ENTRY_H_
#define JLINK_CONSOLE_ENTRY_H_

#define CONNECTION_ABORT_CRTL    (0x00)
#define MENU_RETURN_CRTL         (0x0A)
#define MENU_VTAB_CRTL           (0x0D)
#define MENU_EXIT_CRTL           (0x20)

/*******************************************************************************************************************//**
 * @ingroup hal_entry
 * @{
 **********************************************************************************************************************/
extern void jlink_console_init (void);

extern bool_t jlink_configured (void);

extern bool_t jlink_set_echo_mode (bool_t mode);

extern void jlink_restore_echo_mode (bool_t mode);

extern void start_key_check (void);

extern uint8_t get_detected_key (void);

extern bool_t key_pressed (void);

extern fsp_err_t print_to_console (char_t * p_data);

extern uint8_t * input_from_console_fifo (void);

/**********************************************************************************************************************
 * Function Name: get_new_chars
 * Description  : .
 * Argument     : pBuf
 * Return Value : .
 *********************************************************************************************************************/
uint8_t get_new_chars (uint8_t * pBuf);

/** @} */
#endif /* JLINK_CONSOLE_ENTRY_H_ */

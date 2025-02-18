/**********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO
 * THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2025 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
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

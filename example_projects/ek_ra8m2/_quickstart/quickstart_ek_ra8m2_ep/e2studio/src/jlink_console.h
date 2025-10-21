/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/**********************************************************************************************************************
 * File Name    : jlink_console.h
 * Description  : Entry function.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 14/10/2025 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "hal_data.h"
#include "r_typedefs.h"
#include "r_sci_b_uart.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#ifndef JLINK_CONSOLE_ENTRY_H_
#define JLINK_CONSOLE_ENTRY_H_

#define JLINK_SHORT_TIIMEOUT (16368)

/**********************************************************************************************************************
 Global Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 External global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/
extern void jlink_console_init (void);
extern bool_t jlink_configured (void);

extern void start_key_check (void);
extern uint8_t get_detected_key (void);
extern bool_t key_pressed (void);
extern uint8_t* input_from_console_fifo (void);
extern uint32_t get_new_chars (uint8_t* pBuf);

extern int8_t* input_from_console_multi (void);

extern fsp_err_t print_to_console (char_t * p_data);
extern int8_t input_from_console (void);
extern void   input_from_any_console (int8_t *c);

extern bool_t jlink_set_mode(bool_t mode);
extern void   jlink_restore_mode(bool_t mode);

#endif /* JLINK_CONSOLE_ENTRY_H_ */

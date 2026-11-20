/***********************************************************************************************************************
 * File Name    : terminal.h
 * Description  : Contains variables and functions used in terminal.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef TERMINAL_H_
#define TERMINAL_H_

#include "terminal_thread.h"

/* Macro for the null character */
#define NULL_CHAR               ('\0')

/* Macros for USB HID key table dimensions */
#define INDEX                   (2U)
#define ROW_SIZE                (40U)
#define COL_SIZE                (25U)
#define BUFF_SIZE               (25U)

/* Macros for keyboard modifier keys */
#define KEY_MOD_LSHIFT          (0x02U)
#define KEY_MOD_RSHIFT          (0x20U)

/* Single entry-point called in terminal_thread_entry.c */
void terminal_thread_entry_run(void);

#endif /* TERMINAL_H_ */

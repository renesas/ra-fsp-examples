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

#include <stdbool.h>
#include "terminal_thread.h"
#include "usb_phid_example_app.h"

/* External variable - Defined in terminal.c */
extern volatile bool g_write_flag;

/* Single entry-point called in terminal_thread_entry.c */
void terminal_thread_entry_run(void);

#endif /* TERMINAL_H_ */

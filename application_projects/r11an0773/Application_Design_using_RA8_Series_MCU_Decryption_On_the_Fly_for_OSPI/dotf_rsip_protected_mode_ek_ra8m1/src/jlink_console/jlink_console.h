/*
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/


#ifndef JLINK_CONSOLE_H_
#define JLINK_CONSOLE_H_

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "hal_data.h"
#include "common_util.h"
#include "stdio.h"


/**********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#define BUFFER_LINE_LENGTH (1024)

/**********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/
extern char sprintf_buffer[];
extern fsp_err_t jlink_console_init (void);
extern app_err_t print_to_console(char * p_data);
extern int8_t input_from_console (void);

#endif /* JLINK_CONSOLE_H_ */

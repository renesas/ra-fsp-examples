/***********************************************************************************************************************
 * File Name    : console.h
 * Description  : Contains data structures and function declarations for console
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/ 
#ifndef CONSOLE_CONSOLE_H_
#define CONSOLE_CONSOLE_H_


#define MINIMUM_TOKEN_LENGTH      ( 5)   // minimum length of a colour token
#define MAXIMUM_TOKEN_LENGTH      (10)   // maximum length of a colour token
#define PRINT_BUFFER              (2*1024)
#define TRANSFER_LENGTH           (2048)

void console_write(const char *buffer);
void printf_colour(const char *format, ...);
char wait_for_keypress(void);
void clear_receive_buffer (void);

#endif /* CONSOLE_CONSOLE_H_ */

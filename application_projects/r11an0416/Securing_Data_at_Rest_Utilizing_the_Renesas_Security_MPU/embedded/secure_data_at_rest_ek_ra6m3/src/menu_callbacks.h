/***********************************************************************************************************************
 * File Name    : menu_callbacks.h
 * Description  : Contains macros, function definitions used for user command processing
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef MENU_CALLBACKS_H_
#define MENU_CALLBACKS_H_

void read_secure_settings(void);
void test_secure_code(void);
void test_non_secure_code(void);


void secure_sram_code_read(void);
void secure_code_read(void);
void non_secure_code_read(void);

void secure_sram_code_write(void);
void secure_code_write(void);
void non_secure_code_write(void);

void non_secure_code_leak_test(void);
void setup_faw(void);
void reset_faw(void);
#endif /* MENU_CALLBACKS_H_ */

/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/**********************************************************************************************************************
 * File Name    : menu_aes.h
 * Version      : 1.0
 * Description  : The encryption / FAT32 demo.
 *********************************************************************************************************************/

#include <stdbool.h>


#ifndef MENU_AES_H_
#define MENU_AES_H_

extern test_fn enc_display_menu (void);

extern void initialise_crypto_demo (void);
extern void deinitialise_crypto_demo (void);

extern void encrypt_demo (void);
extern void decrypt_demo (void);

extern bool_t initialise_crypto_key (void);
extern bool_t get_key_as_str (char_t * target_str, size_t max_len);

#endif /* MENU_AES_H_ */

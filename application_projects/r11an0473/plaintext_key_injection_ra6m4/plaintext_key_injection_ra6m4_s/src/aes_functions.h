/***********************************************************************************************************************
 * File Name    : aes_functions.c
 * Description  : contains prototypes for aes key generation, encryption, decryption functions
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/
#ifndef AES_FUNCTIONS_H_
#define AES_FUNCTIONS_H_

typedef struct aes_decryption_args
{
    const uint8_t  * c_input;       /* data to be decrypted */
    size_t input_size2;             /* size of c_input */
    uint8_t *output;                /* decryption result */
    size_t output_size;             /* size of output */
    size_t  * output_len;           /* lengh of decrypted data */

}non_secure_decryption_args_t;

typedef struct aes_encryption_args
{
    const uint8_t  * c_input;       /* data to be decrypted */
    size_t input_size;             /* size of c_input */
    uint8_t *output;                /* decryption result */
    size_t output_size;             /* size of output */
    size_t  * output_len;           /* lengh of decrypted data */

}non_secure_encryption_args_t;

BSP_CMSE_NONSECURE_ENTRY bool init_lfs(void);
BSP_CMSE_NONSECURE_ENTRY bool wrap_inject_aes_key(size_t key_id);
BSP_CMSE_NONSECURE_ENTRY bool destroy_the_key(void);
BSP_CMSE_NONSECURE_ENTRY bool encryption_operation(non_secure_encryption_args_t *args);
BSP_CMSE_NONSECURE_ENTRY bool decryption_operation(non_secure_decryption_args_t *args);

#endif /* AES_FUNCTIONS_H_ */

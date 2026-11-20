/***********************************************************************************************************************
 * File Name    : aes_example_app.h
 * Description  : Public interface for the NetX Crypto example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef AES_EXAMPLE_APP_H_
#define AES_EXAMPLE_APP_H_

#include "aes_thread.h"
#include "hal_data.h"
#include "nx_crypto_hmac_sha2.h"
#include "hw_sce_trng_private.h"

/* External variables */
extern NX_CRYPTO_METHOD crypto_method_aes_cbc_256;
extern NX_CRYPTO_METHOD crypto_method_hmac_sha256;
extern NX_CRYPTO_METHOD crypto_method_aes_cbc_128;
extern NX_CRYPTO_METHOD crypto_method_aes_256_gcm_16;
extern void * gp_sce_resource;

/* Single entry-point called in aes_thread_entry.c */
void aes_example_app_run(void);

#endif /* AES_EXAMPLE_APP_H_ */

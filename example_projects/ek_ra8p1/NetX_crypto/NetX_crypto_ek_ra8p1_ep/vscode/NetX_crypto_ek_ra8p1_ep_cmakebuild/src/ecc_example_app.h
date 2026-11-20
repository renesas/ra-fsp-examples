/***********************************************************************************************************************
 * File Name    : ecc_example_app.h
 * Description  : Public interface for the NetX Crypto example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef ECC_EXAMPLE_APP_H_
#define ECC_EXAMPLE_APP_H_

#include "ecc_thread.h"
#include "hal_data.h"
#include "rm_netx_secure_crypto.h"

/* Macros for metadata size */
#define ECDSA_SIZE      (sizeof(NX_CRYPTO_ECDSA))
#define ECDH_SIZE       (sizeof(NX_CRYPTO_ECDH))

/* External variables */
extern NX_CRYPTO_METHOD crypto_method_ecdh;
extern NX_CRYPTO_METHOD crypto_method_ecdsa;
extern NX_CRYPTO_METHOD crypto_method_ec_secp256;
extern NX_CRYPTO_METHOD crypto_method_sha256;

/* Single entry-point called in ecc_thread_entry.c */
void ecc_example_app_run(void);

#endif /* ECC_EXAMPLE_APP_H_ */

/***********************************************************************************************************************
 * File Name    : rsa_example_app.h
 * Description  : Public interface for the NetX Crypto example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef RSA_EXAMPLE_APP_H_
#define RSA_EXAMPLE_APP_H_

#include "hal_data.h"
#include "rm_netx_secure_crypto.h"
#include "rsa_thread.h"

/* External variables */
extern NX_CRYPTO_METHOD crypto_method_rsa;
extern NX_CRYPTO_METHOD crypto_method_sha256;

/* Single entry-point called in rsa_thread_entry.c */
void rsa_example_app_run(void);

#endif /* RSA_EXAMPLE_APP_H_ */

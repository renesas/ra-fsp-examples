/* gcm_mode.h - TinyCrypt interface to a GCM mode implementation */

/*
 *  Copyright (C) 2017 by Intel Corporation, All Rights Reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *    - Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *
 *    - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 *    - Neither the name of Intel Corporation nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 * @brief Interface to a GCM mode implementation.
 *
 *  Overview: GCM (for "Galois/Counter Mode") mode is a NIST approved mode of
 *            operation defined in SP 800-38D.
 *
 *  Security: 
 *
 *  Requires: AES-128, AES-192, AES-256
 *
 *  Usage:    1) call tc_gcm_config to configure.
 *
 *            2) call tc_gcm_generation_encryption to encrypt data and generate tag.
 *
 *            3) call tc_gcm_decryption_verification to decrypt data and verify tag.
 */

#ifndef __TC_GCM_MODE_H__
#define __TC_GCM_MODE_H__

#include <tinycrypt/aes.h>
#include <stddef.h>
#include "rm_tinycrypt_port_cfg.h"

#ifdef __cplusplus
extern "C" {
#endif

/* max additional authenticated size in bits: 2^64 - 1*/
#define TC_GCM_AAD_MAX_BITS 0xFFFFFFFFFFFFFFFF

/* max initial vector size in bits: 2^64 - 1*/
#define TC_GCM_IV_MAX_BITS 0xFFFFFFFFFFFFFFFF

/* max message size in bytes: (2^39 - 2^8)/8 = 68719476704 */
#define TC_GCM_PAYLOAD_MAX_BYTES 0xFFFFFFFE0

typedef struct tc_gcm_mode_struct {
	TCAesKeySched_t sched; /* AES key schedule */
	unsigned int tlen; /* tag length in bytes (parameter t in SP-800 38D) */
} *TCGcmMode_t;

/**
 * @brief initialize the GCM mode encryption procedure
 * @return returns TC_CRYPTO_SUCCESS (1)
 *          returns TC_CRYPTO_FAIL (0) if:
 *                sched == NULL or
 *                iv == NULL or
 *                aad == NULL or
 *                additional_len == 0
 * @param sched IN -- AES key schedule
 * @param iv IN - Pointer to Initial Vector
 * @param aad IN - Pointer to additional data
 * @param additional_len -- Length of additional data
 */
int tc_gcm_encryption_init(const TCAesKeySched_t sched, uint8_t * iv, uint8_t * aad, uint32_t additional_len);

/**
 * @brief updates data for GCM encryption procedure
 * @return returns TC_CRYPTO_SUCCESS (1)
 *          returns TC_CRYPTO_FAIL (0) if:
 *                sched == NULL or
 *                input == NULL or
 *                output == NULL or
 *                length == 0
 * @param sched IN -- AES key schedule
 * @param input IN - plaintext to encrypt
 * @param output OUT -- encrypted data
 * @param length IN -- intput length
 */
int tc_gcm_encryption_update(const TCAesKeySched_t sched, const uint8_t * input, uint8_t * output,
                             uint32_t length);

/**
 * @brief calculates TAG for GCM mode encryption procedure
 * @return returns TC_CRYPTO_SUCCESS (1)
 *          returns TC_CRYPTO_FAIL (0) if:
 *                sched == NULL or
 *                input == NULL or
 *                output == NULL or
 *                tag == NULL or
 *                input_len == 0
 *                aad_len == 0
 * @param sched IN -- AES key schedule
 * @param input IN - plaintext to encrypt
 * @param input_len IN -- intput length
 * @param aad_len IN -- additional data length
 * @param output OUT - encrypted data
 * @param tag OUT - encrypted tag
 */
int tc_gcm_encryption_final(const TCAesKeySched_t sched,
                            uint8_t             * input,
                            uint32_t              input_len,
                            uint32_t              aad_len,
                            uint8_t             * output,
                            uint8_t             * tag);

/**
 * @brief initialize the GCM mode decryption procedure
 * @return returns TC_CRYPTO_SUCCESS (1)
 *          returns TC_CRYPTO_FAIL (0) if:
 *                sched == NULL or
 *                iv == NULL or
 *                aad == NULL or
 *                additional_len == 0
 * @param sched IN -- AES key schedule
 * @param iv IN - Pointer to Initial Vector
 * @param aad IN - Pointer to additional data
 * @param additional_len -- Length of additional data
 */
int tc_gcm_decryption_init(const TCAesKeySched_t sched, uint8_t * iv, uint8_t * aad, uint32_t additional_len);

/**
 * @brief updates data for GCM decryption procedure
 * @return returns TC_CRYPTO_SUCCESS (1)
 *          returns TC_CRYPTO_FAIL (0) if:
 *                sched == NULL or
 *                input == NULL or
 *                output == NULL or
 *                length == 0
 * @param sched IN -- AES key schedule
 * @param input IN - ciphertext to decrypt
 * @param output OUT -- decrypted data
 * @param length IN -- intput length
 */
int tc_gcm_decryption_update(const TCAesKeySched_t sched, const uint8_t * input, uint8_t * output,
                             uint32_t length);

/**
 * @brief calculates TAG for GCM mode decryption procedure
 * @return returns TC_CRYPTO_SUCCESS (1)
 *          returns TC_CRYPTO_FAIL (0) if:
 *                sched == NULL or
 *                input == NULL or
 *                tag == NULL or
 *                aad_len == 0
 *                input_len == 0
 *                tag_len == 0
 *                output == NULL or
 * @param sched IN -- AES key schedule
 * @param input IN - ciphertext to decrypt
 * @param tag IN - pointer to TAG buffer
 * @param aad_len IN -- additional data length
 * @param input_len IN -- intput length
 * @param tag_len IN -- tag length
 * @param output OUT - decrypted data
 */
int tc_gcm_decryption_final(const TCAesKeySched_t sched,
                            uint8_t             * input,
                            uint8_t             * tag,
                            uint32_t              aad_len,
                            uint32_t              input_len,
                            uint8_t               tag_len,
                            uint8_t             * output);

/**
 * @brief GCM configuration procedure
 * @return returns TC_CRYPTO_SUCCESS (1)
 *          returns TC_CRYPTO_FAIL (0) if:
 *                context == NULL or
 *                sched == NULL or
 *                tlen != {128, 120, 112, 104, 96, 64, 32}
 * @param context -- GCM state
 * @param sched IN -- AES key schedule
 * @param tlen -- tag length in bytes (parameter t in SP-800 38D)
 */
int tc_gcm_config(TCGcmMode_t context, TCAesKeySched_t sched, uint8_t tlen);

/**
 * @brief GCM tag generation and encryption procedure
 * @return returns TC_CRYPTO_SUCCESS (1)
 *         returns TC_CRYPTO_FAIL (0) if:
 *                out == NULL or
 *                c == NULL or
 *                ((plen < 0) and (payload == NULL)) or
 *                ((alen < 0) and (aad == NULL))
 *
 * @param out OUT -- encrypted data
 * @param olen IN -- output length in bytes
 * @param tag OUT - pointer to TAG buffer
 * @param aad IN -- associated data
 * @param alen IN -- associated data length in bytes
 * @param iv IN -- Pointer to Initial Vector
 * @param ivlen IN -- Length of Initial Vector
 * @param payload IN -- payload
 * @param plen IN -- payload length in bytes
 * @param g IN -- GCM state
 */
int tc_gcm_generation_encryption(uint8_t *out, unsigned int olen,
                 uint8_t *tag,
				 const uint8_t *aad,
				 unsigned int alen, const uint8_t *iv,
				 unsigned int ivlen, const uint8_t *payload,
				 unsigned int plen, TCGcmMode_t g);

/**
 * @brief GCM decryption and tag verification procedure
 * @return returns TC_CRYPTO_SUCCESS (1)
 *         returns TC_CRYPTO_FAIL (0) if:
 *                out == NULL or
 *                c == NULL or
 *                ((plen < 0) and (payload == NULL)) or
 *                ((alen < 0) and (aad == NULL))
 *
 * @param out OUT -- encrypted data
 * @param olen IN -- output length in bytes
 * @param tag IN - pointer to TAG buffer
 * @param aad IN -- associated data
 * @param alen IN -- associated data length in bytes
 * @param iv IN -- Pointer to Initial Vector
 * @param ivlen IN -- Length of Initial Vector
 * @param payload IN -- payload
 * @param plen IN -- payload length in bytes
 * @param g IN -- GCM state
 */
int tc_gcm_decryption_verification(uint8_t *out, unsigned int olen,
                   uint8_t *tag,
				   const uint8_t *aad,
				   unsigned int alen, const uint8_t *iv,
				   unsigned int ivlen,const uint8_t *payload,
				   unsigned int plen, TCGcmMode_t g);
#endif /* __TC_GCM_MODE_H__ */

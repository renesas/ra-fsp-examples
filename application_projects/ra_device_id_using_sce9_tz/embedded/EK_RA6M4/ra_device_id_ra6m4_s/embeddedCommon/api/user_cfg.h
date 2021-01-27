/***********************************************************************************************************************
 * File Name    : user_cfg.h
 * Description  : application specific definitions
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

#ifndef DEVCERT_H_
#define DEVCERT_H_

#include "cryptoTypes.h"
#include "psa/crypto.h"

#define ECC_PRIVATE_KEY_HNDL_SIZE 52

typedef uint8_t EccPrivateKeyHandle[ECC_PRIVATE_KEY_HNDL_SIZE];

/* Memory Map of device certificate and device key */

//#define ADDR_DEV_CERT       (0x000F8000)
#define ADDR_DEV_CERT       (0x00040000)
#define SIZE_DEV_CERT       0x00001000
/** Max size of PEM format certificate.
 * This keeps the Frame Protocol message size under the max of 1030.
 * NOTE: As a result of the Frame Protocol message size limit of
 * 1030 bytes, the CA signing cert used to sign the OEM cert
 * is limited to an RSA key size of 1024 bytes.  A larger key
 * will cause the certificate size to exceed the frame protocol
 * limit. This can be addressed by breaking the PEM into 2
 * messages or increasing the frame size allowed. */
#define MAX_PEM_LENGTH   (1022)
#define DEV_CERT_PEM_LENGTH 1024  // Size of the OEMCertificatePEM structure
#define DEV_CERT_PTR       (*((volatile uint32_t *)ADDR_DEV_CERT))


/* #define ENABLE_FAW          1 */

typedef uint8_t PEM_CERT_BUFF[MAX_PEM_LENGTH];

/* Device Certificate */
typedef struct
{
    uint16_t            certPEMLen;       /* Length of PEM Cert */
    PEM_CERT_BUFF       certPEM;          /* PEM formatted X509 certificate generated using public key */
} DevCertificatePEM_t;

/* Device key structure as stored in firmware */
typedef struct
{
    EccPublicKey  pubKey;           /* public key buffer */
    psa_key_handle_t key_handle;    /* ECC key handle */
}DevKey_t;


#endif /* DEVCERT_H_ */

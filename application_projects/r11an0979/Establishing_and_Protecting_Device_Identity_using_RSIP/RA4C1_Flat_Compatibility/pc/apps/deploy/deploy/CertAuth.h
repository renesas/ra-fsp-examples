/***********************************************************************************************************************
* Copyright (c) 2017 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/*******************************************************************************
* File Name    : CertAuth.h
* Description  : A simple example Certificate Authority providing rudimentary
*				 functionality
******************************************************************************/

/*****************************************************************************
* History : DD.MM.YYYY Version  Description
*
*         : 25.04.2017 1.00     First Release
******************************************************************************/

/******************************************************************************
Macro definitions
******************************************************************************/
#pragma once

/******************************************************************************
Includes
******************************************************************************/

#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/rsa.h>
#include <openssl/ec.h>
#include <stdint.h>
/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Exported global variables
******************************************************************************/

/******************************************************************************
Exported global functions (to be accessed by other files)
******************************************************************************/

class CertAuth
{
public:
	CertAuth(void);
	~CertAuth(void);

	int CreateCert(RSA *rootKeyPair, EC_KEY *deviceKey, char *outputFile, char *returnCert, int returnCertMaxLength, uint16_t *pemLength);
    int CreateSelfSignedECDSACert(char *outputFile, char *returnCert, int returnCertMaxLength, uint16_t *pemLength);
	int convertPEMFileToCerFile(char *pemFile, char *cerFile);
	int nextDeviceSerialNumber();
	int validateCert(char *batchFile);
	int extractPublicKey(char *certFile, unsigned char *publicKey);
};

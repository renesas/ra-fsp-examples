/*******************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only
* intended for use with Renesas products. No other uses are authorized. This
* software is owned by Renesas Electronics Corporation and is protected under
* all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
* AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software
* and to discontinue the availability of this software. By using this software,
* you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
* Copyright 2017 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/

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

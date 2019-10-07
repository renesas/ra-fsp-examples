/***********************************************************************************************************************
* Copyright [2015] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
*
* The contents of this file (the "contents") are proprietary and confidential to Renesas Electronics Corporation
* and/or its licensors ("Renesas") and subject to statutory and contractual protections.
*
* Unless otherwise expressly agreed in writing between Renesas and you: 1) you may not use, copy, modify, distribute,
* display, or perform the contents; 2) you may not use any name or mark of Renesas for advertising or publicity
* purposes or in connection with your use of the contents; 3) RENESAS MAKES NO WARRANTY OR REPRESENTATIONS ABOUT THE
* SUITABILITY OF THE CONTENTS FOR ANY PURPOSE; THE CONTENTS ARE PROVIDED ÅeAS ISÅf WITHOUT ANY EXPRESS OR IMPLIED
* WARRANTY, INCLUDING THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND
* NON-INFRINGEMENT; AND 4) RENESAS SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, OR CONSEQUENTIAL DAMAGES,
* INCLUDING DAMAGES RESULTING FROM LOSS OF USE, DATA, OR PROJECTS, WHETHER IN AN ACTION OF CONTRACT OR TORT, ARISING
* OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE CONTENTS. Third-party contents included in this file may
* be subject to different terms.
**********************************************************************************************************************/

/* Set of functions to encode Update messages according to version 3.0.7 of the Synergy Security Architecture
 *
 * Extensions to the documented protocol are :-
 * 1) Add/Modify messsages contain the module signature in the body of the message, rather than hidden in the payload.
 * 2) Add/Modify messsages contain the  new version number for the module.
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include "crypto.h"
#include "deviceCertificate.h"
#include "memoryChecks.h"
#include "staticAssert.h"
#include "moduleInfo.h"
#include "secureUpdateMsg.h"
#if defined _WIN32
#include "srec_file.h"
 /* MOD REA START - Add SBMCrypto API header */
#ifdef ENABLE_WINDOWS_KEYSTORE 
#include "SBMCryptoApi.h"
#else
#include "keyDbApi.h"
#endif
#ifdef ENABLE_DATA_FLASH_SUPPORT
#include "srec_wrapper.h"
#endif
 /* MOD REA END */
#endif


#if defined _WIN32
static void dump(uint8_t *pB, uint32_t length)
{
    for (uint32_t i=0; i<length; i++)
    {
        if (0 == i%16)
        {
            printf("\n");
        }
        printf("%02X ",pB[i]);
    }
    printf("\n");
}

static void quadWrite(uint8_t *pB, uint32_t value)
{
    pB[0] = (value >> 24) & 0xFF;
    pB[1] = (value >> 16) & 0xFF;
    pB[2] = (value >> 8) & 0xFF;
    pB[3] = (value >> 0) & 0xFF;
}

static bool secureUpdateValidate(const SecureUpdateInput *pInput)
{
    const SecureUpdateEntry *pEntry;
/* MOD REA START - Use Windows Keystore APIs if enabled */
#ifdef ENABLE_WINDOWS_KEYSTORE
#else
	KeyDbRecord keyDbRecord;
#endif
/* MOD REA END */

/* MOD REA START - Use APIs which support data flash */
#ifdef ENABLE_DATA_FLASH_SUPPORT
	SrecRegion regions[2] = { { 0, NULL, 0 },{ 0, NULL, 0 } };
	SrecImage srecImage = { 2, regions };
#else
	SrecReaderImage srecImage;
#endif
/* MOD REA END */
    bool foundHeader = false;
    bool foundTrailer = false;

    if (pInput->numEntries > MAX_NUM_CMDS_IN_UPDATE)
    {
        printf("invalid number entries %d\n", pInput->numEntries);
        return false;
    }

    ModuleId masterSigModule = MODULE_ID_INVALID_ID;
    for (uint32_t i = 0; i < pInput->numEntries; i++)
    {
        pEntry = &pInput->entries[i];

        if (pEntry->numSig > MAX_NUM_SIG)
        {
            printf("Too many sigs for module %d\n", i);
            return false;
        }

        // Header, Trailer, Add, Delete are all signed by the masterModule which is the module specified in the header.
        if (SECURE_UPDATE_HEADER == pEntry->cmd)
        {
            masterSigModule = pEntry->module;
        }

        ModuleId signingModule = (SECURE_UPDATE_MODIFY == pEntry->cmd) ? pEntry->module : masterSigModule;
        for (uint32_t s = 0; s < pEntry->numSig; s++)
        {
			/* MOD REA START */
#ifdef ENABLE_WINDOWS_KEYSTORE
			if (!SBMCryptoKeyStoreHasKey(signingModule, pEntry->sigVersion[s], signing_key) &&
				(!SBMCryptoKeyStoreHasKey(signingModule, pEntry->sigVersion[s], update_key)))
			{
				printf("Failed to find keyDB entry for signing module(%d), version(%d)\n", signingModule, pEntry->sigVersion[s]);
				return false;
			}
#else
			/* MOD REA END */
			if (!keyDbGetRecord((uint8_t)signingModule, pEntry->sigVersion[s], &keyDbRecord))
			{
				printf("Failed to find keyDB entry for signing module(%d), version(%d)\n", signingModule, pEntry->sigVersion[s]);
				return false;
			}
#endif
        }

        switch (pEntry->cmd)
        {
        case SECURE_UPDATE_HEADER:
            if (0 != i)
            {
                printf("found a header in the midst of a command table\n");
                return false;
            }
            foundHeader = true;
            break;
        case SECURE_UPDATE_ADD:
            if (0 != pEntry->newSigKey)
            {
                printf("Add of module(%d) must include a new signature key\n", i);
                return false;
            }
            if (0 != pEntry->newMUK)
            {
                printf("Add of module(%d) must include a new module update key\n", i);
                return false;
            }
        
/* MOD REA START - Use APIs which support data flash */
#ifdef ENABLE_DATA_FLASH_SUPPORT
			if (!srecFileRead(pEntry->filename, &srecImage))
			{
				printf("Failed to parse srec file(%s) for module(%d).\n", pEntry->filename, i);
				return false;
			}
			else
			{
				if (NULL != srecImage.psRegions[code_memory].pu1Data)
				{
					free(srecImage.psRegions[code_memory].pu1Data);
				}
				if (NULL != srecImage.psRegions[data_memory].pu1Data)
				{
					free(srecImage.psRegions[data_memory].pu1Data);
				}
			}
#else
			if (!srecReadFile(pEntry->filename, &srecImage))
			{
				printf("Failed to parse srec file(%s) for module(%d).\n", pEntry->filename, i);
				return false;
			}
			else
			{
				free(srecImage.pBuffer);
			}
#endif
/* MOD REA END */
            break;
        case SECURE_UPDATE_MODIFY:
/* MOD REA START */
#ifdef ENABLE_WINDOWS_KEYSTORE
			if (0 != pEntry->newSigKey)
			{
				if ((!SBMCryptoKeyStoreHasKey((uint8_t)signingModule, pEntry->newSigKey, signing_key)) || 
					(!SBMCryptoKeyStoreHasKey((uint8_t)signingModule, pEntry->newSigKey, update_key)))
				{
					printf("Modify of module(%d) is attempting to reuse signing key from unknown version(%d)\n", signingModule, pEntry->newSigKey);
					return false;
				}
			}
			if (0 != pEntry->newMUK)
			{
				if ((!SBMCryptoKeyStoreHasKey((uint8_t)signingModule, pEntry->newMUK, signing_key)) ||
					(!SBMCryptoKeyStoreHasKey((uint8_t)signingModule, pEntry->newMUK, update_key)))
				{
					printf("Modify of module(%d) is attempting to reuse MUK from unknown version(%d)\n", signingModule, pEntry->newMUK);
					return false;
				}
			}
#else
			if (0 != pEntry->newSigKey)
			{
				if (!keyDbGetRecord((uint8_t)signingModule, pEntry->newSigKey, &keyDbRecord))
				{
					printf("Modify of module(%d) is attempting to reuse signing key from unknown version(%d)\n", signingModule, pEntry->newSigKey);
					return false;
				}
			}
			if (0 != pEntry->newMUK)
			{
				if (!keyDbGetRecord((uint8_t)signingModule, pEntry->newMUK, &keyDbRecord))
				{
					printf("Modify of module(%d) is attempting to reuse MUK from unknown version(%d)\n", signingModule, pEntry->newMUK);
					return false;
				}
			}
#endif
/* MOD REA END */

/* MOD REA START - Use APIs which support data flash */
#ifdef ENABLE_DATA_FLASH_SUPPORT
			if (!srecFileRead(pEntry->filename, &srecImage))
			{
				printf("Failed to parse srec file(%s) for module(%d).\n", pEntry->filename, i);
				return false;
			}
			else
			{
				if (NULL != srecImage.psRegions[code_memory].pu1Data)
				{
					free(srecImage.psRegions[code_memory].pu1Data);
				}
				if (NULL != srecImage.psRegions[data_memory].pu1Data)
				{
					free(srecImage.psRegions[data_memory].pu1Data);
				}
			}
#else
			if (!srecReadFile(pEntry->filename, &srecImage))
			{
				printf("Failed to parse srec file(%s) for module(%d).\n", pEntry->filename, i);
				return false;
			}
			else
			{
				free(srecImage.pBuffer);
			}
#endif
			/* MOD REA END */
            break;
        case SECURE_UPDATE_DELETE:
            // nothing in delete can be validated here.
            break;
        case SECURE_UPDATE_TRAILER:
            if (i != pInput->numEntries - 1)
            {
                printf("Trailer must be the last entry in the command table\n");
            }
            foundTrailer = true;
            break;
        default:
            printf("Unknown command(%d) found in update command table\n", pEntry->cmd);
            return false;
            break;

        }
    }

    if (foundHeader && foundTrailer)
    {
        //ok.
    }
    else
    {
        printf("Header or trailer for update is missing\n");
        return false;
    }
    return true;
}


static bool secureUpdateEncodeHeader(const SecureUpdateInput *pInput, uint8_t *pBuffer, uint32_t *pBufLength, ModuleId *pMasterModuleId)
{
    // assumes that update has been validated.
    const SecureUpdateEntry  *pEntry;

/* MOD REA START */
#ifdef ENABLE_WINDOWS_KEYSTORE
#else
	KeyDbRecord keyDbRecord;
#endif
/* MOD REA END */

    // Record the moduleId contained in the Header command. This is the master module for signing the header, trailer, adds and deletes.
    *pMasterModuleId = pInput->entries[0].module;

    // encode prefix.
    pBuffer[0]              = (uint8_t)SECURE_UPDATE_HEADER;
    uint8_t *pLength        = &pBuffer[1];
    pBuffer[5]              = SECURE_UPDATE_MSG_VERSION;
    uint8_t *pModuleCount   = &pBuffer[6];

    *pModuleCount = 0;

    uint32_t bufIndex = 7;

    // encode the key list for each module that is being modified.
    for (uint32_t i = 0; i < pInput->numEntries; i++)
    {
        pEntry = &pInput->entries[i];
        if (SECURE_UPDATE_MODIFY == pEntry->cmd)
        {
            *pModuleCount = *pModuleCount + 1;
            pBuffer[bufIndex++] = pEntry->module;
            pBuffer[bufIndex++] = pEntry->numSig;

            for (uint32_t s = 0; s < pEntry->numSig; s++)
            {
/* MOD REA START */
#ifdef ENABLE_WINDOWS_KEYSTORE
				if (!SBMCryptoGetPublickey(pEntry->module, pEntry->sigVersion[s], update_key, &pBuffer[bufIndex], sizeof(EccPublicKey)))
				{
					printf("secureUpdateEncodeHeader failed to get db record for module=%d, version=%d\n", pEntry->module, pEntry->sigVersion[s]);
					return false;
				}
#else
                if (!keyDbGetRecord(pEntry->module, pEntry->sigVersion[s], &keyDbRecord))
                {
                    printf("secureUpdateEncodeHeader failed to get db record for module=%d, version=%d\n", pEntry->module, pEntry->sigVersion[s]);
                    return false;
                }
                memcpy(&pBuffer[bufIndex], &keyDbRecord.updatePublicKey, sizeof(EccPublicKey));
#endif
                bufIndex += sizeof(EccPublicKey);
            }
        }
    }

    // Sign the header.
    pEntry = &pInput->entries[0];
    pBuffer[bufIndex++] = pEntry->module;
    pBuffer[bufIndex++] = pEntry->numSig;
    uint32_t sigLength = bufIndex;

    quadWrite(pLength, bufIndex + pEntry->numSig * (sizeof(EccPublicKey) + sizeof(EccSignature)));

    for (uint32_t s = 0; s < pEntry->numSig; s++)
    {

/* MOD REA START */
#ifdef ENABLE_WINDOWS_KEYSTORE
		if (!SBMCryptoGetPublickey(pEntry->module, pEntry->sigVersion[s], update_key, &pBuffer[bufIndex], sizeof(EccPublicKey)))
		{
			printf("secureUpdateEncodeHeader failed to get db record for module=%d, version=%d\n", pEntry->module, pEntry->sigVersion[s]);
			return false;
		}
		bufIndex += sizeof(EccPublicKey);

		if (!SBMCryptoHashAndSign(pEntry->module, pEntry->sigVersion[s], update_key, pBuffer, sigLength, NULL, 0, &pBuffer[bufIndex], sizeof(EccSignature)))
		{
			printf("secureUpdateEncodeHeader failed to get sign for module=%d, version=%d\n", pEntry->module, pEntry->sigVersion[s]);
			return false;
		}
		bufIndex += sizeof(EccSignature);
#else
		if (!keyDbGetRecord(pEntry->module, pEntry->sigVersion[s], &keyDbRecord))
		{
			printf("secureUpdateEncodeHeader failed to get db record for module=%d, version=%d\n", pEntry->module, pEntry->sigVersion[s]);
			return false;
		}
		memcpy(&pBuffer[bufIndex], &keyDbRecord.updatePublicKey, sizeof(EccPublicKey));
		bufIndex += sizeof(EccPublicKey);

		if (!cryptoEccHashAndSign(pBuffer, sigLength, NULL, 0, &keyDbRecord.updatePrivateKey, (EccSignature*)&pBuffer[bufIndex]))
		{
			printf("secureUpdateEncodeHeader failed to get sign for module=%d, version=%d\n", pEntry->module, pEntry->sigVersion[s]);
			return false;
		}
		bufIndex += sizeof(EccSignature);
#endif
/* MOD REA END */
    }

    // record overall length;
    if (*pBufLength < bufIndex)
    {
        // oops.... we've overshot the buffer length....
        printf("Header too large for buffer\n");
        return false;
    }

    *pBufLength = bufIndex;

    return true;
}

static bool secureUpdateEncodeAddModify(const SecureUpdateEntry  *pEntry, uint8_t *pBuffer, uint32_t *pBufLength, ModuleId masterModuleId)
{
    // assumes that update has been validated.

/* MOD REA START */
#ifndef ENABLE_WINDOWS_KEYSTORE
	KeyDbRecord keyDbRecord;
#endif
/* MOD REA END */

    Sha256				*pHash;
    uint32_t            bufIndex = 0;
    ModuleId            signingModule;

	/* MOD REA START - Use APIs which support data flash */
#ifdef ENABLE_DATA_FLASH_SUPPORT
	SrecRegion regions[2] = { { 0, NULL, 0 },{ 0, NULL, 0 } };
	SrecImage srecImage = { 2, regions };
	if (!srecFileRead(pEntry->filename, &srecImage))
	{
		printf("Failed to load srec file %s\n", pEntry->filename);
		return false;
	}
#else
    // read Srec file.
    SrecReaderImage  image[2];
    if (!srecReadFile(pEntry->filename, &image))
    {
        printf("Failed to load srec file %s\n", pEntry->filename);
        return false;
    }
#endif
/* MOD REA END*/

    // encode prefix.
    pBuffer[bufIndex++] = (uint8_t)pEntry->cmd;

    uint8_t *pLength = &pBuffer[bufIndex];
    bufIndex += 4;

    pHash = (Sha256*)&pBuffer[bufIndex];
    bufIndex += sizeof(Sha256);

    if (SECURE_UPDATE_ADD == pEntry->cmd)
    {
        // The only difference between add and modify the is the sigMod field in the message.
        // Adds are signed by the master module of the update.
        pBuffer[bufIndex++] = masterModuleId;
        signingModule = masterModuleId;
    }
    else
    {
        // updates are signed by the moduel being updated.
        signingModule = pEntry->module;
    }

    pBuffer[bufIndex++] = pEntry->numSig;

    // make space for the signatures.
    uint32_t sigSpace = pEntry->numSig * (sizeof(EccPublicKey) + sizeof(EccSignature));
    uint32_t startIndexOfSigs = bufIndex;
    uint32_t startIndexOfStuffThatIsSigned = bufIndex + sigSpace;

    bufIndex = startIndexOfStuffThatIsSigned;

    // encode the module.
    pBuffer[bufIndex++] = pEntry->module;
    quadWrite(&pBuffer[bufIndex], pEntry->newVersion);
    bufIndex += 4;
	/* MOD REA START - Use APIs which support data flash */
#ifdef ENABLE_DATA_FLASH_SUPPORT
	quadWrite(&pBuffer[bufIndex], srecImage.psRegions[code_memory].u4Address);
#else
    quadWrite(&pBuffer[bufIndex], image.lowestAddr);
#endif
    bufIndex += 4;
    quadWrite(&pBuffer[bufIndex], 0);  // not encrypted.
    bufIndex += 4;

/* MOD REA START */
#ifdef ENABLE_WINDOWS_KEYSTORE

	/* Make new key pairs */
	if (!SBMCryptoMakeCompositeKeys(pEntry->module, pEntry->newVersion, pEntry->newSigKey, pEntry->newMUK))
	{
		printf("Failed to make new keys for add of module(%d), version(%d)\n", pEntry->module, pEntry->newVersion);
		return false;
	}

	/* Copy the signing public key to the message */ 
	if (!SBMCryptoGetPublickey(pEntry->module, pEntry->newVersion, signing_key, &pBuffer[bufIndex], sizeof(EccPublicKey)))
	{
		printf("Failed to get signing public key for add of module(%d), version(%d)\n", pEntry->module, pEntry->newVersion);
		return false;
	}
	bufIndex += sizeof(EccPublicKey);

	/*Extract the MUK public key */
	EccPublicKey updatePublicKey;
	if (!SBMCryptoGetPublickey(pEntry->module, pEntry->newVersion, update_key, updatePublicKey, sizeof(EccPublicKey)))
	{
		printf("Failed to get update public key for add of module(%d), version(%d)\n", pEntry->module, pEntry->newVersion);
		return false;
	}
/* MOD REA - Enable support for data flash programming */
#ifdef ENABLE_DATA_FLASH_SUPPORT
	uint32_t imageBodyLength = srecImage.psRegions[code_memory].u4Size;
	if (!SBMCryptoHashAndSign(pEntry->module, pEntry->newVersion, signing_key,
		srecImage.psRegions[code_memory].pu1Data, imageBodyLength, updatePublicKey, sizeof(EccPublicKey),
		&pBuffer[bufIndex], sizeof(EccSignature)))
	{
		printf("encode addModify - failed to sign the add\n");
		return false;
	}
#else
	/* Sign the add operation (Sign over the hash of image and update public key) and copy the signature to message */
	uint32_t imageBodyLength = image.highestAddr - image.lowestAddr + 1;
	if (!SBMCryptoHashAndSign(pEntry->module, pEntry->newVersion, signing_key, 
							image.pBuffer, imageBodyLength, updatePublicKey, sizeof(EccPublicKey),
							&pBuffer[bufIndex], sizeof(EccSignature)))
	{
		printf("encode addModify - failed to sign the add\n");
		return false;
	}
#endif
	bufIndex += sizeof(EccSignature);

	/* Put the MUK public key in the message. */
	memcpy(&pBuffer[bufIndex], updatePublicKey, sizeof(EccPublicKey));
	bufIndex += sizeof(EccPublicKey);
	
#else
    // Make keys for signature.....
    if (!keyDbMakeRecord(pEntry->module, pEntry->newVersion, pEntry->newSigKey, pEntry->newMUK, &keyDbRecord))
    {
        printf("Failed to make new keys for add of module(%d), version(%d)\n", pEntry->module, pEntry->newVersion);
        return false;
    }

    // sign the module
    uint32_t imageBodyLength = image.highestAddr - image.lowestAddr + 1;

    memcpy(&pBuffer[bufIndex], &keyDbRecord.sigPublicKey, sizeof(EccPublicKey));
    bufIndex += sizeof(EccPublicKey);

    if (!cryptoEccHashAndSign(
        image.pBuffer,
        imageBodyLength,
        (uint8_t*)&keyDbRecord.updatePublicKey,
        sizeof(EccPublicKey),
        &keyDbRecord.sigPrivateKey,
        (EccSignature*)&pBuffer[bufIndex]))
    {
        printf("encode addModify - failed to sign the add\n");
        return false;
    }

    bufIndex += sizeof(EccSignature);

    // put the MUK in the message.
    memcpy(&pBuffer[bufIndex], &keyDbRecord.updatePublicKey, sizeof(EccPublicKey));
    bufIndex += sizeof(EccPublicKey);
#endif
/* MOD REA END */

    // put the image in the message.
    quadWrite(&pBuffer[bufIndex], imageBodyLength);
    bufIndex += 4;
/* MOD REA - Enable support for data flash programming */
#ifdef ENABLE_DATA_FLASH_SUPPORT
	memcpy(&pBuffer[bufIndex], srecImage.psRegions[code_memory].pu1Data, imageBodyLength);
#else
	memcpy(&pBuffer[bufIndex], image.pBuffer, imageBodyLength);
#endif
/* MOD REA END */

    bufIndex += imageBodyLength;

/* MOD REA - Enable support for data flash programming */
#ifdef ENABLE_DATA_FLASH_SUPPORT

	/* Check wether we have valid data flash memory*/
	uint32_t dataBodyLength = srecImage.psRegions[data_memory].u4Size;
	if (0 != dataBodyLength)
	{
		/*Add the data length */
		quadWrite(&pBuffer[bufIndex], dataBodyLength);
		bufIndex += 4;

		/* Add start address */
		quadWrite(&pBuffer[bufIndex], srecImage.psRegions[data_memory].u4Address);
		bufIndex += 4;

		/* Add data*/
		memcpy(&pBuffer[bufIndex], srecImage.psRegions[data_memory].pu1Data, srecImage.psRegions[data_memory].u4Size);
		bufIndex += srecImage.psRegions[data_memory].u4Size;
	}

#endif
/* MOD REA END */

    // encode the length fields.
    if (*pBufLength < bufIndex)
    {
        // oops.... we've overshot the buffer length....
        printf("Add too large for buffer %d<%d\n", *pBufLength, bufIndex);
        return false;
    }
    quadWrite(pLength, bufIndex);
    *pBufLength = bufIndex;

    // Now need to go back and sign the add by the authorising module.
    bufIndex = startIndexOfSigs;
    for (uint32_t s = 0; s < pEntry->numSig; s++)
    {

/* MOD REA START */
#ifdef ENABLE_WINDOWS_KEYSTORE

		/* Copy the public key of the update key pair */
		if (!SBMCryptoGetPublickey(signingModule, pEntry->sigVersion[s], update_key, &pBuffer[bufIndex], sizeof(EccPublicKey)))
		{
			printf("encode addModify - failed to get record for %d %d\n", signingModule, pEntry->sigVersion[s]);
			return false;
		}
		bufIndex += sizeof(EccPublicKey);

		/* Sign the add */
		if (!SBMCryptoHashAndSign(signingModule, pEntry->sigVersion[s], update_key, &pBuffer[startIndexOfStuffThatIsSigned],
			*pBufLength - (startIndexOfStuffThatIsSigned), NULL, 0, &pBuffer[bufIndex], sizeof(EccSignature)))
		{
			printf("encode addModify - failed to sign the add\n");
			return false;
		}
#else
        if (!keyDbGetRecord(signingModule, pEntry->sigVersion[s], &keyDbRecord))
        {
            printf("encode addModify - failed to get record for %d %d\n", signingModule, pEntry->sigVersion[s]);
            return false;
        }

        memcpy(&pBuffer[bufIndex], &keyDbRecord.updatePublicKey, sizeof(EccPublicKey));
        bufIndex += sizeof(EccPublicKey);

        if (!cryptoEccHashAndSign(
            &pBuffer[startIndexOfStuffThatIsSigned],
            *pBufLength - (startIndexOfStuffThatIsSigned),
            NULL,
            0,
            &keyDbRecord.updatePrivateKey,
            (EccSignature*)&pBuffer[bufIndex]))
        {
            printf("encode addModify - failed to sign the add\n");
            return false;
        }
#endif
/* MOD REA END */
        bufIndex += sizeof(EccSignature);
    }

    // And insert the hash field.
    cryptoCalculateSha256Start(SEGMENTED_HASH_ADD_MODIFY, &pBuffer[startIndexOfStuffThatIsSigned], *pBufLength - (startIndexOfStuffThatIsSigned));
    cryptoCalculateSha256MiddleEnd(SEGMENTED_HASH_ADD_MODIFY, NULL, 0, true, pHash);

    return true;
}

static bool secureUpdateEncodeDelete(const SecureUpdateEntry  *pEntry, uint8_t *pBuffer, uint32_t *pBufLength, ModuleId masterModuleId)
{
/* MOD REA START - Use Windows Keystore APIs if enabled */
#ifdef ENABLE_WINDOWS_KEYSTORE
#else
	KeyDbRecord keyDbRecord;
#endif
/* MOD REA END */

    uint32_t            bufIndex = 0;
    
	// encode prefix.
    pBuffer[bufIndex++] = (uint8_t)SECURE_UPDATE_DELETE;

    uint8_t *pLength = &pBuffer[bufIndex];
    bufIndex += 4;

    pBuffer[bufIndex++] = pEntry->module;
    pBuffer[bufIndex++] = masterModuleId;
    pBuffer[bufIndex++] = pEntry->numSig;

    uint32_t sigLength = bufIndex;
    uint32_t lengthValue = bufIndex + pEntry->numSig * (sizeof(EccPublicKey) + sizeof(EccSignature));

    // encode the length fields.
    if (*pBufLength < lengthValue)
    {
        // oops.... we've overshot the buffer length....
        printf("Delete too large for buffer %d<%d\n", *pBufLength, lengthValue);
        return false;
    }
    quadWrite(pLength, lengthValue);
    *pBufLength = lengthValue;

    // And sign.
    for (uint32_t s = 0; s < pEntry->numSig; s++)
    {

/* MOD REA START */
#ifdef ENABLE_WINDOWS_KEYSTORE
		/* Copy the public key of the update key pair */
		if (!SBMCryptoGetPublickey(masterModuleId, pEntry->sigVersion[s], update_key, &pBuffer[bufIndex], sizeof(EccPublicKey)))
		{
			printf("encodeDelete - failed to get record for %d %d\n", masterModuleId, pEntry->sigVersion[s]);
			return false;
		}
		bufIndex += sizeof(EccPublicKey);

		/* Sign the delete  */
		if (!SBMCryptoHashAndSign(masterModuleId, pEntry->sigVersion[s], update_key, pBuffer, sigLength, NULL, 0, &pBuffer[bufIndex], sizeof(EccSignature)))
		{
			printf("encodeDelete - failed to sign the delete\n");
			return false;
		}
#else
        if (!keyDbGetRecord(masterModuleId, pEntry->sigVersion[s], &keyDbRecord))
        {
            printf("encodeDelete - failed to get record for %d %d\n", masterModuleId, pEntry->sigVersion[s]);
            return false;
        }

        memcpy(&pBuffer[bufIndex], &keyDbRecord.updatePublicKey, sizeof(EccPublicKey));
        bufIndex += sizeof(EccPublicKey);

        if (!cryptoEccHashAndSign(
            pBuffer,
            sigLength,
            NULL,
            0,
            &keyDbRecord.updatePrivateKey,
            (EccSignature*)&pBuffer[bufIndex]))
        {
            printf("encodeDelete - failed to sign the delete\n");
            return false;
        }
#endif
/* MOD REA END */
        bufIndex += sizeof(EccSignature);
    }
    return true;
}


static bool secureUpdateEncodeTrailer(const SecureUpdateEntry  *pEntry, uint8_t *pBuffer, const uint32_t lengthUsed, uint32_t *pSubBufLength, ModuleId masterModuleId)
{
/* MOD REA START - Use Windows Keystore APIs if enabled */
#ifdef ENABLE_WINDOWS_KEYSTORE
#else
	KeyDbRecord keyDbRecord;
#endif
/* MOD REA END */

    uint32_t bufIndex = lengthUsed;

    // encode the prefix of the trailer.
    pBuffer[bufIndex++] = SECURE_UPDATE_TRAILER;
    uint8_t  *pLength = &pBuffer[bufIndex];
    bufIndex += 4;
    pBuffer[bufIndex++] = masterModuleId;
    pBuffer[bufIndex++] = pEntry->numSig;

    if (masterModuleId != pEntry->module)
    {
        printf("Trailer is NOT allowed to use a different moduleId to the header\n");
        return false;
    }

    // calculate all the lengths....
    uint32_t sigLength = bufIndex;

    *pSubBufLength = bufIndex + pEntry->numSig * (sizeof(EccPublicKey) + sizeof(EccSignature)) - lengthUsed;
    quadWrite(pLength, *pSubBufLength);

    // Sign the whole update.
    for (uint32_t s = 0; s < pEntry->numSig; s++)
    {
/* MOD REA START */
#ifdef ENABLE_WINDOWS_KEYSTORE
		/* Copy the public key of the update key pair */
		if (!SBMCryptoGetPublickey(masterModuleId, pEntry->sigVersion[s], update_key, &pBuffer[bufIndex], sizeof(EccPublicKey)))
		{
			printf("encodeTrailer - failed to get record for %d %d\n", masterModuleId, pEntry->sigVersion[s]);
			return false;
		}
		bufIndex += sizeof(EccPublicKey);

		/* Sign the delete  */
		if (!SBMCryptoHashAndSign(masterModuleId, pEntry->sigVersion[s], update_key, pBuffer, sigLength, NULL, 0, &pBuffer[bufIndex], sizeof(EccSignature)))
		{
			printf("encodeTrailer - failed to sign the trailer\n");
			return false;
		}
#else
        if (!keyDbGetRecord(masterModuleId, pEntry->sigVersion[s], &keyDbRecord))
        {
            printf("encodeTrailer - failed to get record for %d %d\n", masterModuleId, pEntry->sigVersion[s]);
            return false;
        }

        memcpy(&pBuffer[bufIndex], &keyDbRecord.updatePublicKey, sizeof(EccPublicKey));
        bufIndex += sizeof(EccPublicKey);

        if (!cryptoEccHashAndSign(
            pBuffer,
            sigLength,
            NULL,
            0,
            &keyDbRecord.updatePrivateKey,
            (EccSignature*)&pBuffer[bufIndex]))
        {
            printf("encodeTrailer - failed to sign the add\n");
            return false;
        }
#endif
/* MOD REA END */
        bufIndex += sizeof(EccSignature);
    }

    return true;
}


static bool secureUpdateEncodeMessage(SecureUpdateInput *pInput, uint8_t *pStartBuffer, uint32_t *pFullBufferLength)
{
    uint32_t subBufferSize = *pFullBufferLength;
    uint8_t *pBuffer = pStartBuffer;
    uint32_t bufferUsed = 0;
    ModuleId masterModuleId; // This is the module coordinating the update.

    memset(pStartBuffer, 0, *pFullBufferLength);

    for (uint32_t i = 0; i < pInput->numEntries; i++)
    {
        switch (pInput->entries[i].cmd)
        {
        case SECURE_UPDATE_HEADER:
            if (!secureUpdateEncodeHeader(pInput, pBuffer, &subBufferSize, &masterModuleId))
            {
                printf("Encode of header failed\n");
                return false;
            }
            break;
        case SECURE_UPDATE_ADD:
            if (!secureUpdateEncodeAddModify(&pInput->entries[i], pBuffer, &subBufferSize, masterModuleId))
            {
                printf("Encode of add failed\n");
                return false;
            }
            break;
        case SECURE_UPDATE_MODIFY:
            if (!secureUpdateEncodeAddModify(&pInput->entries[i], pBuffer, &subBufferSize, masterModuleId))
            {
                printf("Encode of modify failed\n");
                return false;
            }
            break;
        case SECURE_UPDATE_DELETE:
            if (!secureUpdateEncodeDelete(&pInput->entries[i], pBuffer, &subBufferSize, masterModuleId))
            {
                printf("Encode of delete failed\n");
                return false;
            }
            break;
        case SECURE_UPDATE_TRAILER:
            if (!secureUpdateEncodeTrailer(&pInput->entries[i], pStartBuffer, bufferUsed, &subBufferSize, masterModuleId))
            {
                printf("Encode of trailer failed\n");
                return false;
            }
            break;
        default:
            printf("Unsupport command %d\n", pInput->entries[i].cmd);
            return false;
        }
        pBuffer += subBufferSize;
        bufferUsed += subBufferSize;
        subBufferSize = *pFullBufferLength - bufferUsed;
    }

    // Record the length of the final message.
    *pFullBufferLength = bufferUsed;

    return true;
}

static bool secureUpdateEncryptUpdate(uint8_t *pBuffer, uint32_t *pLength, AesKey *pAesKey, HmacKey *pHmacKey)
{
    uint32_t tmpBufferLen = *pLength + sizeof(HmacResult) + sizeof(AesIv) + 16;  // 16 for aes padding.
    uint8_t *pTmpBuffer = (uint8_t*)malloc(tmpBufferLen);
    bool retval = false;

    if (!cryptoMakeAesKey(pAesKey))
    {

    }
    else if (!cryptoMakeHmacKey(pHmacKey))
    {
    
    }
    else if (NULL == pTmpBuffer)
    {

    }
    else if (!cryptoDataEncrypt(pBuffer, *pLength, pAesKey, pHmacKey, pTmpBuffer, &tmpBufferLen))
    {

    }
    else
    {
        memcpy(pBuffer, pTmpBuffer, tmpBufferLen);
        *pLength = tmpBufferLen;
        free(pTmpBuffer);
        retval = true;
    }

    return retval;
}

static bool secureUpdateMakeUPK(SecureUpdateInput *pInput, AesKey *pAesKey, HmacKey *pHmacKey, uint8_t *pUpkBuffer, uint32_t *pUpkLength)
{
    uint32_t bufIndex = 0;
/* MOD REA START - Use Windows Keystore APIs if enabled */
#ifdef ENABLE_WINDOWS_KEYSTORE
#else
	KeyDbRecord keyDbRecord;
#endif
/* MOD REA END */
    
    pUpkBuffer[bufIndex++] = SECURE_UPDATE_MSG_VERSION;
    
    memcpy(&pUpkBuffer[bufIndex], pAesKey, sizeof(AesKey));
    bufIndex += sizeof(AesKey);

    memcpy(&pUpkBuffer[bufIndex], pHmacKey, sizeof(HmacKey));
    bufIndex += sizeof(HmacKey);

    pUpkBuffer[bufIndex++] = pInput->entries[0].module;
    pUpkBuffer[bufIndex++] = pInput->entries[0].numSig;

    uint32_t sigLength = bufIndex;

    for (uint32_t s = 0; s < pInput->entries[0].numSig; s++)
    {
		/* MOD REA START */
#ifdef ENABLE_WINDOWS_KEYSTORE
		printf("Signing UPK with module(%d), version(%d)\n", pInput->entries[0].module, pInput->entries[0].sigVersion[s]);

		/* Copy the public key of the UPK signing key pair */
		if (!SBMCryptoGetPublickey(pInput->entries[0].module, pInput->entries[0].sigVersion[s], update_key, &pUpkBuffer[bufIndex], sizeof(EccPublicKey)))
		{
			printf("failed to find key to sign UPK\n");
			return false;
		}
		bufIndex += sizeof(EccPublicKey);

		/* Sign the delete  */
		if (!SBMCryptoHashAndSign(pInput->entries[0].module, pInput->entries[0].sigVersion[s], update_key, pUpkBuffer, sigLength, NULL, 0, &pUpkBuffer[bufIndex], sizeof(EccSignature)))
		{
			printf("encodeTrailer - failed to sign the trailer\n");
			return false;
		}
#else
        printf("Signing UPK with module(%d), version(%d)\n", pInput->entries[0].module, pInput->entries[0].sigVersion[s]);
        if (!keyDbGetRecord(pInput->entries[0].module, pInput->entries[0].sigVersion[s], &keyDbRecord))
        {
            printf("failed to find key to sign UPK\n");
            return false;
        }
        memcpy(&pUpkBuffer[bufIndex], keyDbRecord.updatePublicKey, sizeof(EccPublicKey));
        bufIndex += sizeof(EccPublicKey);
        if (!cryptoEccHashAndSign(pUpkBuffer, sigLength, NULL, 0, &keyDbRecord.updatePrivateKey, (EccSignature*)&pUpkBuffer[bufIndex]))
        {
            printf("sign of upk failed\n");
            return false;
        }
#endif
/* MOD REA END */
        bufIndex += sizeof(EccSignature);
    }

    if (bufIndex > *pUpkLength)
    {
        printf("overflowed the UPK sig buffer\n");
        return false;
    }
    *pUpkLength = bufIndex;
    return true;
}

bool secureUpdateMakeUpdate(SecureUpdateInput *pInput, uint8_t *pStartBuffer, uint32_t *pFullBufferLength, uint8_t *pUpkBuffer, uint32_t *upkBufferLength)
{
/* MOD REA START - These variables are not used if encryption is not enabled */
#ifdef ENABLE_ENCRYPTION
    AesKey aesKey;
    HmacKey hmacKey;
#endif
/* MOD REA END */

    if (!secureUpdateValidate(pInput))
    {
        printf("MakeUpdate - validate failed\n");
    }
    else if (!secureUpdateEncodeMessage(pInput, pStartBuffer, pFullBufferLength))
    {
        printf("MakeUpdate - encode failed\n");
    }
/* MOD REA START - Encrypt and create UPK only if encryption is enabled */
#ifdef ENABLE_ENCRYPTION
    else if (!secureUpdateEncryptUpdate(pStartBuffer, pFullBufferLength, &aesKey, &hmacKey))
    {
        printf("MakeUpdate - failed to encrypt update\n");
    }
    else if (!secureUpdateMakeUPK(pInput, &aesKey, &hmacKey, pUpkBuffer, upkBufferLength))
    {
        printf("MakeUpdate - failed to make UPK\n");
    }
#endif
/* MOD REA END */
    else
    {
        return true;
    }
    return false;
}

#endif




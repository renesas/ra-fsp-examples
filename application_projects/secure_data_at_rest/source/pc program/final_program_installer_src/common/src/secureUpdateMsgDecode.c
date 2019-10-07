/***********************************************************************************************************************
* Copyright [2015] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
*
* The contents of this file (the "contents") are proprietary and confidential to Renesas Electronics Corporation
* and/or its licensors ("Renesas") and subject to statutory and contractual protections.
*
* Unless otherwise expressly agreed in writing between Renesas and you: 1) you may not use, copy, modify, distribute,
* display, or perform the contents; 2) you may not use any name or mark of Renesas for advertising or publicity
* purposes or in connection with your use of the contents; 3) RENESAS MAKES NO WARRANTY OR REPRESENTATIONS ABOUT THE
* SUITABILITY OF THE CONTENTS FOR ANY PURPOSE; THE CONTENTS ARE PROVIDED eAS ISf WITHOUT ANY EXPRESS OR IMPLIED
* WARRANTY, INCLUDING THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND
* NON-INFRINGEMENT; AND 4) RENESAS SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, OR CONSEQUENTIAL DAMAGES,
* INCLUDING DAMAGES RESULTING FROM LOSS OF USE, DATA, OR PROJECTS, WHETHER IN AN ACTION OF CONTRACT OR TORT, ARISING
* OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE CONTENTS. Third-party contents included in this file may
* be subject to different terms.
**********************************************************************************************************************/

 /* Set of functions to decode Update messages
 *
 * Extensions to the documented protocol are :-
 * 1) Add/Modify messages contain the module signature in the body of the message, rather than hidden in the payload.
 * 2) Add/Modify messages contain the  new version number for the module.
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
#include "moduleTable.h"
#include "secureUpdateMsg.h"
#include "printfMapping.h"

#if !defined _WIN32
#include "flash.h"
#endif

// The ARM requires words/quadwords to be aligned on word/quadword boundaries.
// To efficiently cast byte arrays we need them to be so aligned.
#if defined _WIN32
#define ALIGN_MEMORY(aLIGN) 
#else
#define ALIGN_MEMORY(aLIGN)  __attribute__((aligned(aLIGN)))
#endif


#if defined _WIN32
static void dump(uint8_t *pB, uint32_t length)
{
    for (uint32_t i = 0; i<length; i++)
    {
        if (0 == i % 16)
        {
            printf("\n");
        }
        printf("%02X ", pB[i]);
    }
    printf("\n");
}
#endif


// The code flash size is considerably larger than the ram size.
// Hence it is impossible to decrypt a complete update into ram and then parse it.
// Hence, we need to stream the update through the parser.
//
// Code flash must be programmed a page at a time.
// The larger code flash page size 32k.
// Hence the streaming quanta size is 32k.
//
// However, AES includes upto 16 bytes of padding.
// In advance, we don't know how much padding there will be on the end of the buffer.
// Hence to ensure we get 32k of payload we need to decrypt 32k + 16 bytes.
// If there are trailing bytes they will be moved to the front of the buffer in the next iteration.
//
// When the decrypt buffer contains code to be flashed, it is passed directly to the flash programmer.
// The flash programmer requires the buffer to be duobyte aligned so we use a duobyte buffer.
//
// Initially 32k+16 bytes are decrypted into the decrypt buffer.
// The header is processed, and the unprocessed decrypted payload is then moved to the front of the buffer.
// Additional data is decrypted to fill the decrypt buffer.
// This is repeated for each command with the update message.

#define FLASH_PAGE_SIZE  (32*1024)
#define DECRYPT_BUFFER_SIZE (FLASH_PAGE_SIZE + 16)
#define DATA_FLASH_PAGE_SIZE 4
static uint16_t decryptBufferWordAligned[DECRYPT_BUFFER_SIZE / 2];
static uint8_t  *pDecryptBuffer = (uint8_t*)decryptBufferWordAligned;
static uint32_t decryptedBufferLength;    // Amount of active data in the decrypt buffer.

static uint8_t  *pEncryptedBuffer;  // Pointer to the head of the encrypted data buffer.
static uint32_t encryptedLength;    // Total length of the encrypted data buffer.
static uint32_t encryptedUsed;      // Count of the number of bytes that have been decrypted.

// Return true if the whole key is zero.
// A zero key is invalid.
static bool isKeyZero(EccPublicKey *pKey)
{
    uint8_t *pB = (uint8_t*)pKey;

    for (uint32_t i = 0; i < sizeof(EccPublicKey); i++)
    {
        if (0 != *pB++)
        {
            return false;
        }
    }
    return true;
}

// Initialise the decryption of a message
static bool decryptInitialise(uint8_t *pEncryptedBufferSource, uint32_t encryptedLengthSource, const AesKey *const pAesKey)
{
    // Store the context into the file global variables.
    pEncryptedBuffer = pEncryptedBufferSource;
    encryptedLength = encryptedLengthSource;
    encryptedUsed = 0;

    decryptedBufferLength = 0;

    // Start the hash of the whole update.
    cryptoCalculateSha256Start(SEGMENTED_HASH_WHOLE_UPDATE, NULL, 0);

    // Calculate the length of the first segment, and whether the whole update fits in a single segment.
    bool finalSegment;
    uint32_t segmentLength;
    if (encryptedLengthSource < DECRYPT_BUFFER_SIZE)
    {
        finalSegment = true;
        segmentLength = encryptedLengthSource;
    }
    else
    {
        finalSegment = false;
        segmentLength = DECRYPT_BUFFER_SIZE;
    }
	
/*MOD REA START -If encryption is not enabled, this reduces to a simple buffer copy */
#if ENABLE_ENCRYPTION
    // Decrypt the first segment.
    uint32_t tmpDecryptLength = DECRYPT_BUFFER_SIZE;
    if (!cryptoDataDecryptInitial(&pEncryptedBuffer[encryptedUsed], segmentLength, pAesKey, &pDecryptBuffer[decryptedBufferLength], &tmpDecryptLength, finalSegment))
    {
        ERROR("decryptInitialise - decrypt failed\n");
        return false;
    }

    // Record in the file globals how much has been processed.
    decryptedBufferLength += tmpDecryptLength;
    encryptedUsed += (encryptedLengthSource < DECRYPT_BUFFER_SIZE) ? encryptedLengthSource : DECRYPT_BUFFER_SIZE;;
#else
    memcpy(&pDecryptBuffer[decryptedBufferLength], &pEncryptedBuffer[encryptedUsed], segmentLength);
    decryptedBufferLength += segmentLength;
    encryptedUsed += segmentLength;
    (void)finalSegment;
    (void)pAesKey;
#endif
/* MOD REA END */

    return true;
}

// Called after each command has been processed to
// (a) consume part of the decrypt buffer and
// (b) fill the decrypt buffer with more decrypted info.
static bool decryptNextSegment(uint32_t consumed)
{
    if (0 != consumed)
    {
        // Move the remaining data to the front of the buffer.
        if (consumed > decryptedBufferLength)
        {
            ERROR("decryptNextSegment - consumed(%d) more than whole segment(%d)\n", (int)consumed, (int)decryptedBufferLength);
            return false;
        }

        // Add the consumed data to the hash of the whole update.
        if (!cryptoCalculateSha256MiddleEnd(SEGMENTED_HASH_WHOLE_UPDATE, pDecryptBuffer, consumed, false, NULL))
        {
            ERROR("decryptNextSegment - failed to update hash\n");
            return false;
        }

        // Move data to the front of the decrypt buffer.
        // overlapping regions so memcpy is unsafe... copy in a simple loop.
        uint8_t *pSrc = &pDecryptBuffer[consumed];
        uint8_t *pDst = pDecryptBuffer;
        for (uint32_t i = 0; i < decryptedBufferLength - consumed; i++)
        {
            *pDst++ = *pSrc++;
        }
        decryptedBufferLength -= consumed;
    }


    // work out how much more to decrypt....
    int32_t signedAmountToDecrypt = (int32_t)encryptedLength - (int32_t)encryptedUsed;
    uint32_t amountToDecrypt = (uint32_t) signedAmountToDecrypt;
    bool finalSegment;

    if (signedAmountToDecrypt < 0)
    {
        ERROR("decryptNextSegment - amountToDecrypt(%d) < 0 - something gone badly wrong....\n", (int)signedAmountToDecrypt);
        return false;
    }
    if (0 == amountToDecrypt)
    {
        // decrypt has been completed.
        // return how much is left in the decrypt buffer.
        return true;
    }
    if (amountToDecrypt > (DECRYPT_BUFFER_SIZE - decryptedBufferLength))
    {
        // Can't decrypt everything this time.
        // partial decrypts must be 'mod 16' to match the AES block size.
        amountToDecrypt = (DECRYPT_BUFFER_SIZE - decryptedBufferLength) & 0xFFFFFFF0;
        finalSegment = false;
    }
    else
    {
        // can decrypt all the remaining.
        finalSegment = true;
    }

    if (amountToDecrypt > (DECRYPT_BUFFER_SIZE - decryptedBufferLength))
    {
        ERROR("decryptNextSegment - amountToDecrypt(%d) > space in buffer(%d)\n", (int)amountToDecrypt,(int)(DECRYPT_BUFFER_SIZE - decryptedBufferLength));
        return false;
    }

/*MOD REA START -If encryption is not enabled, this reduces to a simple buffer copy */
#if ENABLE_ENCRYPTION
    uint32_t tmpDecryptLength = amountToDecrypt;
    if (!cryptoDataDecryptBlock(&pEncryptedBuffer[encryptedUsed], amountToDecrypt, &pDecryptBuffer[decryptedBufferLength], &tmpDecryptLength, finalSegment))
    {
        ERROR("decryptNextSegment - decrypt failed\n");
        return false;
    }
    decryptedBufferLength += tmpDecryptLength;
    encryptedUsed += amountToDecrypt;
#else
    memcpy(&pDecryptBuffer[decryptedBufferLength], &pEncryptedBuffer[encryptedUsed], amountToDecrypt);
    decryptedBufferLength += amountToDecrypt;
    encryptedUsed += amountToDecrypt;
    (void)finalSegment;
#endif
/* MOD REA END */
    
    return true;
}

// The whole update has been successfully processed.
// Finalise the hash of the whole buffer and return it.
static bool decryptFinalise(Sha256 *pHash)
{
    return cryptoCalculateSha256MiddleEnd(SEGMENTED_HASH_WHOLE_UPDATE, NULL, 0, true, pHash);
}



// Read unaligned bytes into a quadword.
static uint32_t quadRead(const uint8_t *pB)
{
    return (uint32_t) ((pB[0] << 24) | (pB[1] << 16) | (pB[2] << 8) | (pB[3]));
}

/* MOD REA START -This function is required only if encryption is used */
#if ENABLE_ENCRYPTION
// validate and decode a decrypted UPK.
static bool secureUpdateDecodeUpk(uint8_t * const pUpkBuffer, const uint32_t upkLength, AesKey *pAesKey, HmacKey *pHmacKey)
{
    // see definition of UPK structure.
    // sum of all fields before the signatures.
    if (upkLength < 1 + sizeof(AesKey) + sizeof(HmacKey) + 2)
    {
        ERROR("UPK - too short\n");
        return false;
    }

    uint32_t bufIndex = 1 + sizeof(AesKey) + sizeof(HmacKey);
    const uint8_t modSig = pUpkBuffer[bufIndex++];
    const uint8_t nSig = pUpkBuffer[bufIndex++];

    if (nSig >= MAX_NUM_SIG)
    {
        ERROR("decodeUpk - too many sigs(%d)\n", nSig);
        return false;
    }

    if (upkLength < 1 + sizeof(AesKey) + sizeof(HmacKey) + 2 + nSig * (sizeof(EccPublicKey) + sizeof(EccSignature)))
    {
        ERROR("UPK - too short\n");
        return false;
    }

    uint32_t sigLength = bufIndex;
    bool sigValid = false;

    for (uint32_t s = 0; s < nSig; s++)
    {
        // UPK may have multiple signatures
        // Find the one that matches a MUK used on this device.
        if (moduleTableCheckModuleUpdateKey(modSig, (EccPublicKey * const)&pUpkBuffer[bufIndex]))
        {
            // found the correct MUK.
            if (cryptoEccHashAndVerify(
                pUpkBuffer, sigLength,
                NULL, 0,
                (const EccPublicKey*)&pUpkBuffer[bufIndex],
                (const EccSignature*)&pUpkBuffer[bufIndex + sizeof(EccPublicKey)]))
            {
                sigValid = true;
            }
        }
        bufIndex += sizeof(EccPublicKey) + sizeof(EccSignature);
    }

    if (sigValid)
    {
        TRACE("UPK - valid\n");
        memcpy(pAesKey, &pUpkBuffer[1], sizeof(AesKey));
        memcpy(pHmacKey, &pUpkBuffer[1 + sizeof(AesKey)], sizeof(HmacKey));
    }
    else
    {
        ERROR("UPK - not found a valid signature\n");
    }

    return sigValid;
}
#endif
/* MOD REA END */

// Decode a header command. (see v0.3.0.7 of the Renesas Security Architecture document).
// Returns the id of the master module that is signing the whole update.
static bool secureUpdateDecodeHeader(ModuleId *pSigMod)
{
    uint32_t bufIndex = 0;

    // Verify that the first command in the update is a header!
    if (SECURE_UPDATE_HEADER != pDecryptBuffer[bufIndex++])
    {
        ERROR("decodeHeader - message does not start with a header!\n");
        return false;
    }

    uint32_t readLength = quadRead(&pDecryptBuffer[bufIndex]);
    bufIndex += 4;

    if (readLength > decryptedBufferLength)
    {
        ERROR("decodeHeader - buf too short %d<%d \n", (int)decryptedBufferLength, (int)readLength);
        return false;
    }

    if (SECURE_UPDATE_MSG_VERSION != pDecryptBuffer[bufIndex++])
    {
        ERROR("decodeHeader - wrong msg version %d\n", pDecryptBuffer[--bufIndex]);
        return false;
    }

    // Check the MUKs of the modules being updated....
    uint8_t count = pDecryptBuffer[bufIndex++];   // Number of modules who's MUKs need checking.
    ModuleId updatedModule;
    uint8_t  nkey;
    bool foundValidKey;
    TRACE("count=%d\n", count);

    while (0 != count--)
    {
        updatedModule = pDecryptBuffer[bufIndex++];
        nkey = pDecryptBuffer[bufIndex++];

        TRACE("updatedModule=%d, nkey=%d\n", updatedModule, nkey);

        foundValidKey = false;
        for (uint32_t i = 0; i < nkey; i++)
        {
            // Each module may have multiple sigs, each with a different MUKs.
            // check each MUK so see if there is a match.
            if (moduleTableCheckModuleUpdateKey(updatedModule, (EccPublicKey *)&pDecryptBuffer[bufIndex]))
            {
                foundValidKey = true;
            }
            bufIndex += sizeof(EccPublicKey);
        }

        if (!foundValidKey)
        {
            ERROR("decodeHeader - failed to find a valid MUK for module(%d)\n", updatedModule);
            return false;
        }
    }

    // Now need to check the header signature
    // It also has a module+MUK.
    *pSigMod = pDecryptBuffer[bufIndex++];
    uint8_t nSig = pDecryptBuffer[bufIndex++];
    uint32_t sigLength = bufIndex;
    bool sigValid = false;

    if (nSig >= MAX_NUM_SIG)
    {
        ERROR("decodeHeader - too many sigs(%d)\n", nSig);
        return false;
    }

    if (decryptedBufferLength < bufIndex + (nSig * (sizeof(EccPublicKey) + sizeof(EccSignature))))
    {
        ERROR("decodeHeader - message too small2\n");
        return false;
    }

    for (uint32_t s = 0; s < nSig; s++)
    {
        // Find the correct MUK for the signature of the header.
        if (moduleTableCheckModuleUpdateKey(*pSigMod, (EccPublicKey * const)&pDecryptBuffer[bufIndex]))
        {
            // found the correct MUK.
            if (cryptoEccHashAndVerify(
                pDecryptBuffer, sigLength,
                NULL, 0,
                (const EccPublicKey * const)&pDecryptBuffer[bufIndex],
                (const EccSignature * const )&pDecryptBuffer[bufIndex + sizeof(EccPublicKey)]))
            {
                sigValid = true;
            }
        }
        bufIndex += sizeof(EccPublicKey) + sizeof(EccSignature);
    }

    if (sigValid)
    {
        // Consume this message, move the decrypt buffer forward and fill the space in the buffer.
        if (!decryptNextSegment(bufIndex))
        {
            ERROR("decodeHeader - decrypt failed\n");
            return false;
        }
    }

    TRACE("decodeHeader %s\n", sigValid ? "PASSED" : "FAILED");

    return sigValid;
}

// Process an update add or modify command. (see v0.3.0.7 of the Renesas Security Architecture document).
static bool secureUpdateDecodeAddModify(bool isAdd, ModuleId sigMod)
{
    uint32_t bufIndex = 1;  // skip the type field.
    uint32_t totalAddLength = quadRead(&pDecryptBuffer[bufIndex]);
    uint32_t totalAddConsumed = totalAddLength;
    bufIndex += 4;

    // This is the expected Hash for the payload of the add/modify.
    // The signature over this hash is checked early in the decode.
    // As the payload of the add/modify is streamed through the decoder the hash is re-calculated, and finally checked against
    // the expected hash.
    Sha256 expectedHash;
    memcpy(expectedHash, &pDecryptBuffer[bufIndex], sizeof(Sha256));
    bufIndex += sizeof(Sha256);

    if (isAdd)
    {
        // additions must be signed by the module controlling the whole update.
        if (pDecryptBuffer[bufIndex++] != sigMod)
        {
            ERROR("decodeAddModify - addition signed by wrong module %d!=%d\n", (int)pDecryptBuffer[--bufIndex], (int)sigMod);
            return false;
        }
    }

    uint8_t nSig = pDecryptBuffer[bufIndex++];
    bool sigValid = false;

    if (nSig >= MAX_NUM_SIG)
    {
        ERROR("decodeAddModify - too many sigs(%d)\n", nSig);
        return false;
    }

    if (!isAdd)
    {
        // Modify commands are signed by the module being updated....so we need to find out who it is...
        sigMod = pDecryptBuffer[bufIndex + nSig * (sizeof(EccPublicKey) + sizeof(EccSignature))];
    }

    for (uint32_t s = 0; s < nSig; s++)
    {
        if (moduleTableCheckModuleUpdateKey(sigMod, (EccPublicKey * const )&pDecryptBuffer[bufIndex]))
        {
            // found the correct MUK.
            if (cryptoEccVerifyAHash(
                (const Sha256 *const) &expectedHash,
                (const EccPublicKey * const )&pDecryptBuffer[bufIndex],
                (const EccSignature * const )&pDecryptBuffer[bufIndex + sizeof(EccPublicKey)]))
            {
                sigValid = true;
            }
        }
        bufIndex += sizeof(EccPublicKey) + sizeof(EccSignature);
    }

    // If the signature over the hash supplied in the command is correct, then
    // we decode the rest of the command.
    // Note - we still need to calculate the hash over the payload and
    // verify it matches the hash in the command.
    if (sigValid)
    {
        uint32_t hashStartIndex;
        hashStartIndex = bufIndex;

        uint8_t mod = pDecryptBuffer[bufIndex++];

        uint32_t newVersionNumber = quadRead(&pDecryptBuffer[bufIndex]);
        bufIndex += 4;

        uint32_t modAddr = quadRead(&pDecryptBuffer[bufIndex]);
        bufIndex += 4;

        uint32_t decryptAddr = quadRead(&pDecryptBuffer[bufIndex]);
        bufIndex += 4;

        EccPublicKey *pNewModSigKey = (EccPublicKey*)&pDecryptBuffer[bufIndex];
        bufIndex += sizeof(EccPublicKey);

        EccSignature *pNewModSig = (EccSignature*)&pDecryptBuffer[bufIndex];
        bufIndex += sizeof(EccSignature);

        EccPublicKey *pNewModMUK = (EccPublicKey*)&pDecryptBuffer[bufIndex];
        bufIndex += sizeof(EccPublicKey);

        uint32_t payloadLength = quadRead(&pDecryptBuffer[bufIndex]);
        bufIndex += 4;

        if (bufIndex > decryptedBufferLength)
        {
            ERROR("decodeAddModify - decrypt buffer too small 3\n");
            return false;
        }

        if (isKeyZero(pNewModSigKey))
        {
            // todo - for modify need to allow unchanged keys.
            ERROR("decodeAddModify - module sig key must be non zero\n");
            return false;
        }

        if (isKeyZero(pNewModMUK))
        {
            // todo - for modify need to allow unchanged keys.
            ERROR("decodeAddModify - module update key must be non zero\n");
            return false;
        }

        if (totalAddLength < bufIndex)
        {
            ERROR("decodeAddModify - buffer to short\n");
            return false;
        }
        


        // We have decided that the module is good to be added.
        // 1. Add it to the pending module table.
        // 2. Start the module hash check across the header and update the decrypt buffer to contain just the payload.
        // 3. Erase the relevant flash pages to store the module.
        // 4. loop over each flash page
        //       - Add to module hash check.
        //       - program flash
        //       - update decrypt buffer.
        // 5. If pending length, there is data flash that needs update repeat the steps detailed in step 4
        //    without any hash checks
        // 6. finalise and check hash.


        TRACE("decodeAddModify - module(%d) is valid. Adding it to the module table\n", mod);

        // 1. Add it to the pending module table.
        if (!moduleTableUpdateEntry(mod, newVersionNumber, modAddr, payloadLength, decryptAddr, pNewModSigKey, pNewModSig, pNewModMUK))
        {
            ERROR("decodeAddModify - failed to add module to module table\n");
            return false;
        }

        // 2. Start the module hash check across the header and update the decrypt buffer to contain just the payload.
        if (!cryptoCalculateSha256Start(SEGMENTED_HASH_ADD_MODIFY, &pDecryptBuffer[hashStartIndex], bufIndex - hashStartIndex))
        {
            ERROR("decodeAddModify - hash calculation failed\n");
            return false;
        }
        /* Keep track of consumption */
        totalAddConsumed -= bufIndex;
        if (!decryptNextSegment(bufIndex))
        {
            ERROR("decodeAddModify - decryptNextSegment failed\n");
            return false;
        }

        // 4. loop over each flash page
        //       - Add to module hash check.
        //       - Erase flash block.
        //       - program flash block
        //       - update decrypt buffer.
        uint32_t segmentLength;
        while (payloadLength)
        {
            segmentLength = (payloadLength > FLASH_PAGE_SIZE) ? FLASH_PAGE_SIZE : payloadLength;
            payloadLength -= segmentLength;
            totalAddConsumed -= segmentLength;

            TRACE("decodeAddModify - programming segment of size %d at 0x%x\n", (int)segmentLength, (int)modAddr);

            if (segmentLength > decryptedBufferLength)
            {
                ERROR("decodeAddModify - not enough data in decryptBuffer to program segment.\n");
                return false;
            }

            if (!cryptoCalculateSha256MiddleEnd(SEGMENTED_HASH_ADD_MODIFY, pDecryptBuffer, segmentLength, false, NULL))
            {
                ERROR("decodeAddModify - hash calculation failed\n");
                return false;
            }
#if !defined _WIN32
            else if (!flash_code_erase(modAddr, modAddr + ((1 + segmentLength) / 2)))
            {
                ERROR("decodeAddModify - failed to erase flash block at 0x%x\n", (int)modAddr);
                return false;
            }
            else if (!flash_code_program(modAddr, decryptBufferWordAligned, (1 + segmentLength) / 2))
            {
                ERROR("decodeAddModify - failed to program flash\n");
                return false;
            }
            else if (!flash_code_verify(modAddr, decryptBufferWordAligned, (1 + segmentLength) / 2))
            {
                ERROR("decodeAddModify - failed to verify flash\n");
                return false;
            }
            modAddr += segmentLength;
#endif
            if (!decryptNextSegment(segmentLength))
            {
                ERROR("decodeAddModify - decryptNextSegment failed\n");
                return false;
            }
        }

/* REA MOD START - Add support for data flash */
        /* 5. Buffer is not empty, means we have data flash that needs update */
        if(totalAddConsumed > 0)
        {
        	bufIndex = 0;

        	/* Get the length */
        	payloadLength = quadRead(&pDecryptBuffer[bufIndex]);
        	bufIndex += 4;

        	/* Get the start address */
        	modAddr = quadRead(&pDecryptBuffer[bufIndex]);
        	bufIndex += 4;

#if !defined _WIN32
        	/* Erase the flash that needs to be programmed */
            if (!flash_code_erase(modAddr, modAddr + payloadLength))
            {
                ERROR("decodeAddModify - failed to erase flash\n");
                return false;
            }
#endif

            if (!cryptoCalculateSha256MiddleEnd(SEGMENTED_HASH_ADD_MODIFY, pDecryptBuffer, bufIndex, false, NULL))
            {
            	ERROR("decodeAddModify - hash calculation failed\n");
            	return false;
            }

            totalAddConsumed -= bufIndex;

            /* Make the decrypt buffer point to the data */
            if (!decryptNextSegment(bufIndex))
            {
            	ERROR("decodeAddModify - decryptNextSegment failed\n");
            	return false;
            }

            /* Loop through the pages and program them */
            while (payloadLength)
            {
                segmentLength = (payloadLength > DATA_FLASH_PAGE_SIZE) ? DATA_FLASH_PAGE_SIZE : payloadLength;
                payloadLength -= segmentLength;
                totalAddConsumed -= segmentLength;

                TRACE("decodeAddModify - programming data flash segment of size %d at 0x%x\n", (int)segmentLength, (int)modAddr);

                if (segmentLength > decryptedBufferLength)
                {
                    ERROR("decodeAddModify - not enough data in decryptBuffer to program segment.\n");
                    return false;
                }

                if (!cryptoCalculateSha256MiddleEnd(SEGMENTED_HASH_ADD_MODIFY, pDecryptBuffer, segmentLength, false, NULL))
                {
                    ERROR("decodeAddModify - hash calculation failed\n");
                    return false;
                }

    #if !defined _WIN32
                else if (!flash_code_program(modAddr, decryptBufferWordAligned, (1 + segmentLength) / 2))
                {
                    ERROR("decodeAddModify - failed to program flash\n");
                    return false;
                }
                else if (!flash_code_verify(modAddr, decryptBufferWordAligned, (1 + segmentLength) / 2))
                {
                    ERROR("decodeAddModify - failed to verify flash\n");
                    return false;
                }
                modAddr += segmentLength;
    #endif
                if (!decryptNextSegment(segmentLength))
                {
                    ERROR("decodeAddModify - decryptNextSegment failed\n");
                    return false;
                }
            }
        }
/* REA MOD END */

        // 6. finalise and check hash.
        Sha256 calculatedHash;
        if (!cryptoCalculateSha256MiddleEnd(SEGMENTED_HASH_ADD_MODIFY, NULL, 0, true, &calculatedHash))
        {
            ERROR("decodeAddModify - hash calculation failed\n");
            return false;
        }
        if (0 != memcmp(calculatedHash, expectedHash, sizeof(Sha256)))
        {
            ERROR("decodeAddModify - calculated hash does NOT match hash in message\n");
            return false;
        }

        TRACE("decodeAddModify - PASSED\n");
        return true;
    }
    else
    {
        ERROR("decodeAddModify - failed to find valid sig for module\n");
        return false;
    }
}

// Process an update delete command. (see v0.3.0.7 of the Renesas Security Architecture document).
static bool secureUpdateDecodeDelete(const ModuleId sigMod)
{
    uint32_t bufIndex = 1;  // skip the type field.

    uint32_t readLength = quadRead(&pDecryptBuffer[bufIndex]);
    bufIndex += 4;

    if (readLength > decryptedBufferLength)
    {
        ERROR("decodeDelete - buf too short %d<%d \n", (int)decryptedBufferLength, (int)readLength);
        return false;
    }

    uint8_t moduleToDelete = pDecryptBuffer[bufIndex++];

    if (pDecryptBuffer[bufIndex++] != sigMod)
    {
        ERROR("decodeDelete - delete signed by wrong module %d!=%d\n", (int)pDecryptBuffer[--bufIndex], (int)sigMod);
        return false;
    }

    uint8_t nSig = pDecryptBuffer[bufIndex++];
    if (nSig >= MAX_NUM_SIG)
    {
        ERROR("decodeDelete - too many sigs(%d)\n", nSig);
        return false;
    }

    // check the signatures. 
    bool sigValid = false;
    uint32_t sigLength = bufIndex;

    for (uint32_t s = 0; s < nSig; s++)
    {
        if (moduleTableCheckModuleUpdateKey(sigMod, (EccPublicKey * const)&pDecryptBuffer[bufIndex]))
        {
            // found the correct MUK.
            if (cryptoEccHashAndVerify(
                pDecryptBuffer,
                sigLength,
                NULL,
                0,
                (const EccPublicKey * const)&pDecryptBuffer[bufIndex],
                (const EccSignature * const)&pDecryptBuffer[bufIndex + sizeof(EccPublicKey)]))
            {
                sigValid = true;
            }
        }
        bufIndex += sizeof(EccPublicKey) + sizeof(EccSignature);
    }

    if (sigValid)
    {
        ERROR("decodeDelete - deleting module %d\n", moduleToDelete);
        moduleTableDeleteEntry(moduleToDelete);
    }

    if (!decryptNextSegment(bufIndex))
    {
        ERROR("decodeDelete - decryptNextSegment failed\n");
        return false;
    }

    TRACE("decodeDelete - %s\n", sigValid ? "PASSED" : "FAILED");
    return sigValid;
}


// Process an update trailer command. (see v0.3.0.7 of the Renesas Security Architecture document).
static bool secureUpdateDecodeTrailer(const ModuleId sigMod)
{
    uint32_t bufIndex = 1;  // skip the type field.

    uint32_t readLength = quadRead(&pDecryptBuffer[bufIndex]);
    bufIndex += 4;

    if (readLength > decryptedBufferLength)
    {
        ERROR("decodeTrailer - buf too short %d<%d \n", (int)decryptedBufferLength, (int)readLength);
        return false;
    }

    if (pDecryptBuffer[bufIndex++] != sigMod)
    {
        ERROR("decodeTrailer - signed by wrong module %d!=%d\n", (int)pDecryptBuffer[--bufIndex], (int)sigMod);
        return false;
    }

    uint8_t nSig = pDecryptBuffer[bufIndex++];
    bool sigValid = false;
    Sha256 calculatedHash;

    if (nSig >= MAX_NUM_SIG)
    {
        ERROR("decodeTrailer - too many sigs(%d)\n", nSig);
        return false;
    }

    // finalise the hash over the whole buffer.
    if (!decryptNextSegment(bufIndex))
    {
        ERROR("decodeTrailer - decryptNextSegment failed\n");
        return false;
    }
    if (!decryptFinalise(&calculatedHash))
    {
        ERROR("decodeTrailer - decryptFinalise failed\n");
        return false;
    }
    bufIndex = 0;   // since decryptNextSegment call has consumed the start of the header.

    for (uint32_t s = 0; s < nSig; s++)
    {
        if (moduleTableCheckModuleUpdateKey(sigMod, (EccPublicKey * const)&pDecryptBuffer[bufIndex]))
        {
            // found the correct MUK.
            if (cryptoEccVerifyAHash(
                (const Sha256 *const)&calculatedHash,
                (const EccPublicKey * const)&pDecryptBuffer[bufIndex],
                (const EccSignature * const)&pDecryptBuffer[bufIndex + sizeof(EccPublicKey)]))
            {
                sigValid = true;
            }
        }
        bufIndex += sizeof(EccPublicKey) + sizeof(EccSignature);
    }

    TRACE("decodeTrailer - %s\n", sigValid ? "PASSED" : "FAILED");
    return sigValid;
}


// Process a complete update message.
static bool secureUpdateDecodeMessage(uint8_t * const pBuffer, const uint32_t length, const AesKey *const pAesKey)
{
    ModuleId sigMod;
    bool processedTrailer = false;

    if (!decryptInitialise(pBuffer, length, pAesKey))
    {
        ERROR("decodeMsg - failed to init decrypt\n");
        return false;
    }

    if (!secureUpdateDecodeHeader(&sigMod))
    {
        ERROR("decodeMsg - failed to decode header\n");
        return false;
    }
    
    while ((!processedTrailer) && (decryptedBufferLength > 0))  // todo - needs more robust handling of length overruns....
    {
        switch (pDecryptBuffer[0])
        {
        case SECURE_UPDATE_ADD:
            if (!secureUpdateDecodeAddModify(true, sigMod))
            {
                ERROR("decodeMsg - failed to add module\n");
                return false;
            }
            break;
        case SECURE_UPDATE_MODIFY:
            if (!secureUpdateDecodeAddModify(false, sigMod))
            {
                ERROR("decodeMsg - failed to modify module\n");
                return false;
            }
            break;
        case SECURE_UPDATE_DELETE:
            if (!secureUpdateDecodeDelete(sigMod))
            {
                ERROR("decodeMsg - failed to delete module\n");
                return false;
            }
            break;
        case SECURE_UPDATE_TRAILER:
            if (!secureUpdateDecodeTrailer(sigMod))
            {
                ERROR("decodeMsg - failed to decode trailer\n");
                return false;
            }
            processedTrailer = true;
            break;
        default:
            ERROR("decodeMsg - unsupported command=%d\n", pBuffer[0]);
            return false;
            break;
        }
    }
    TRACE("Decoded update.\n");
    return true;
}


// Process a complete secure update.
// The update is provided by the application as an opaque buffer.
// We know it should be a UPK, followed by an Update.
bool secureUpdateDecodeUpdate(uint8_t * const pBuffer, const uint32_t length)
{
    // contents of buffer is :-
    //      u32 upkLength
    //      u8  upk[upkLength]
    //      u32 appLength
    //      u8  update[appLength]
    //  Buffer is tightly packed. U32's are not quad aligned.

/* MOD REA START No need to decrypt the UPK and the sources if encryption is not enabled */
    bool retval = false;
#if ENABLE_ENCRYPTION
#define SIZE_OF_DECRYPT_BUFFER (1024) // todo - calculate max size of UPK.
    static uint8_t decryptBuffer[SIZE_OF_DECRYPT_BUFFER] ALIGN_MEMORY(4);
    uint32_t  upkDecryptLength = SIZE_OF_DECRYPT_BUFFER;

    uint32_t upkLength = quadRead(pBuffer);
    uint32_t appLength = quadRead(&pBuffer[4 + upkLength]);
    AesKey aesKey;
    HmacKey hmacKey;
    EccPublicKey  upkPublicKey;
    EccPrivateKey upkPrivateKey;
    
    if (length < (appLength + upkLength + 8))
    {
        ERROR("invalid length fields\n");
    }
    else if (!moduleTableStartUpdate())
    {
        ERROR("failed to start MT update\n");
    }
    else if (!cryptoDeriveEccKey((uint8_t*)&synergySeed, CRYPTO_DERIVATION_STRING_DEV_ENC, &upkPublicKey, &upkPrivateKey))
    {
        ERROR("failed to derive the keys to decrypto the UPK\n");
    }
    else if (!cryptoECIESDecrypt(&pBuffer[4], upkLength, (const EccPrivateKey *)&upkPrivateKey, decryptBuffer, &upkDecryptLength))
    {
        ERROR("failed to decrypt the UPK\n");
    }
    else if (!secureUpdateDecodeUpk(decryptBuffer, upkDecryptLength, &aesKey, &hmacKey))
    {
        ERROR("decodeUpdate - failed to decode UPK\n");
    }
    else if (!cryptoDataCheckHmac(&pBuffer[8 + upkLength], appLength, (const HmacKey *const)&hmacKey))
    {
        ERROR("decodeUpdate - hmac failed\n");
    }
    else if (!secureUpdateDecodeMessage(&pBuffer[8 + upkLength], appLength, (const AesKey * const)&aesKey))
#else
    uint32_t appLength = quadRead(pBuffer);
    if(length <(appLength + 4))
    {
    	ERROR("invalid length fields\n");
    }
    else if (!moduleTableStartUpdate())
    {
    	ERROR("failed to start MT update\n");
    }
    else if(!secureUpdateDecodeMessage(&pBuffer[4], appLength, NULL))
#endif
/* MOD REA END*/
    {
        ERROR("decodeUpdate - failed to decode message\n");
    }
    else if (!moduleTableFinaliseUpdate())
    {
        ERROR("decodeUpdate - failed to commit module table\n");
    }
    else
    {
        retval = true;
    }
    return retval;
}



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

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <winscard.h>
/* MOD REA START - Add SBMCrypto API header */
#ifdef ENABLE_WINDOWS_KEYSTORE 
#include "SBMCryptoApi.h"
#endif
/* MOD REA END */
#include "crypto.h"
#include "keyDbApi.h"
#include "moduleInfo.h"
#include "moduleTable.h"
#include "synergy_upk.h"
#include "deviceCertificate.h"
#include "javacardCommon.h"

#define MAX_JAVACARD_MSG_LENGTH 255    // think length is limited to 255 bytes?
#define ADPU_HEADER_LENGTH 5

// Macros to read/write 16 bit quantities from/to Javacard message buffers
#define GET16(bUFFER)  ((bUFFER[0] << 8) + bUFFER[1]); bUFFER+=2
#define PUT16(bUFFER, vALUE)  *bUFFER++ = vALUE >> 8; *bUFFER++ = vALUE & 0xFF

// Context info for connection to the card.
static bool connectionEstablished = false;
static SCARDHANDLE card;
static SCARD_IO_REQUEST pioSendPci;

// Dumps the contents of a buffer to the console for debugging.
static void dump(uint8_t *p, uint32_t length)
{
    for (uint32_t i = 0; i < length; i++)
    {
        if (0 == i % 16)
        {
            printf("\n");
        }
        printf(" 0x%02X", *p++);
    }
    printf("\n");
}

typedef enum
{
    OP_SELECT_APPLET,
    OP_GET_INFO,
    OP_GET_DEVICE_ID_LIST,

    OP_PERSONALISE,
    OP_UPDATE_PERSONALISE,

    OP_START_AUTHORISE,
    OP_LOAD_UNENCRYPTED_UPK,
    OP_COMPLETE_AUTHORISE,

    OP_SEND_DEV_CERT,
    OP_ENCRYPT_UPK,
    OP_ENCRYPT_UPK_GET_ENCRYTPED_UPK

}JavacardOperation;

// The last two bytes of every messages received from the card must be 0x90, 0x00.
// Also checks the length is the expected length.
static bool javacardCheckReply(uint8_t *p, uint32_t length, uint32_t expectedLength, JavacardOperation operation)
{
    if ((0x90 == p[length - 2]) || (0x00 == p[length - 1]))
    {
        if (expectedLength != length)
        {
            printf("Unexpected length of reply to command(%d) to javacard. length(%d)!=expecteLength(%d). \n", operation, length, expectedLength);
            printf("Full contents of the reply are :-");
            dump(p, length);
            return false;
        }
        else
        {
            // This reply means all is good!
            return true;
        }
    }
    else if ((0x69 == p[length - 2]) || (0x85 == p[length - 1]))
    {
        printf("Javacard error : Command(%d) invoked in incorrect state.\n", operation);
        switch (operation)
        {
        case OP_UPDATE_PERSONALISE:
            printf("   - is personalisation update being attempted on an unpersonalised card?\n");
            break;
        case OP_START_AUTHORISE:
        case OP_LOAD_UNENCRYPTED_UPK:
        case OP_COMPLETE_AUTHORISE:
            printf("   - has authorisation been attempted on an unprovisioned card?\n");
            break;
        case OP_SEND_DEV_CERT:
        case OP_ENCRYPT_UPK:
        case OP_ENCRYPT_UPK_GET_ENCRYTPED_UPK:
            printf("   - has UPK encryption been attempted on an unauthorised card?\n");
            printf("   - is the DeviceCertificate invalid?\n");
            break;
        default:
            break;
        }
        return false;
    }
    else if ((0x69 == p[length - 2]) || (0x84 == p[length - 1]))
    {
        printf("Javacard error : Commmand=%d. Data out of range\n", operation);
        switch (operation)
        {
        case OP_START_AUTHORISE:
        case OP_LOAD_UNENCRYPTED_UPK:
        case OP_COMPLETE_AUTHORISE:
            printf("   - are you attempting to authorise more installations than the lifetime limit of the card?\n");
            break;
        case OP_SEND_DEV_CERT:
        case OP_ENCRYPT_UPK:
        case OP_ENCRYPT_UPK_GET_ENCRYTPED_UPK:
            printf("   - are you attempting to deploy to more cards than allowed by the authorisation?\n");
            break;
        default:
            break;
        }
        return false;
    }
    else
    {
        printf("Unexpected reply to command(%d) to javacard. Contents of reply are :-", operation);
        dump(p, length);
        return false;
    }
}

// All the Javacard messages are command and reponse.
// Most of the reponses are a simple ack that the command has been performed.
// This function sends a supplied commmand and checks that the simple 2 byte ack is correct.
// (Commands with complex responses are handled elsewhere).
static bool javacardSendAndCheckReply(const uint8_t *pMsg, uint32_t length, const char *pName, JavacardOperation operation)
{
    DWORD replyLength = MAX_JAVACARD_MSG_LENGTH;
    uint8_t replyBuffer[MAX_JAVACARD_MSG_LENGTH];
    uint32_t status;

    if (SCARD_S_SUCCESS != (status = SCardTransmit(card, &pioSendPci, pMsg, length, NULL, replyBuffer, &replyLength)))
    {
        printf("send of %s failed. Status=0x%x, msgLength=%d\n", pName, status, length);
        return false;
    }
    if (!javacardCheckReply(replyBuffer, replyLength, 2, operation))
    {
        printf("Reply to %s was invalid\n", pName);
        return false;
    }
    return true;
}


// Check that the string will fit into the javacard message.
static bool javacardCheckStringLength(const char *p, const char *pName)
{
    uint32_t length = strlen(p);

    if ((0 == length) || (length > MAX_JAVACARD_STRING_LENGTH))
    {
        printf("%s name must be between 1 and %d characters in length - you supplied '%s'\n", pName, MAX_JAVACARD_STRING_LENGTH, p);
        return false;
    }
    return true;
}





// Opens a connection to a card.
bool javacardOpenCard(bool useSecondReader, bool debug)
{
    uint32_t status;
    SCARDCONTEXT context;
    DWORD activeProtocol;
    DWORD readerInfoSize = 0;
    char *pReaders;

    if (connectionEstablished)
    {
        connectionEstablished = false;
        SCardDisconnect(card, SCARD_LEAVE_CARD);
    }

    if (debug)
    {
        printf("Opening connection to card\n");
    }

    // open the card handling system.
    if (SCARD_S_SUCCESS != (status = SCardEstablishContext(SCARD_SCOPE_SYSTEM, 0, 0, &context)))
    {
        printf("openCard SCardEstablishContext failed with code=0x%x\n", status);
        return false;
    }

    // discover what cards are connected.
    if (SCARD_S_SUCCESS != (status = SCardListReadersA(context, NULL, NULL, &readerInfoSize)))
    {
        switch (status)
        {
        case 0x8010002e:
            printf("Unable to detect a smart card reader.\n");
            printf("Please check that it is connected to the PC.\n");
            break;
        case 0x80100069:
            printf("Unable to detect smart card in the card reader.\n");
            printf("Please check that the card is fully inserted and the correct way up.\n");
            break;
        case 0x8010000B:
            printf("Unable to connect to card. Another program is using it\n");
            break;
        default:
            printf("openCard SCardListReadersA failed with code=0x%x\n", status);
            break;
        }
        return false;
    }
    if (NULL == (pReaders = (char*)malloc(readerInfoSize)))
    {
        printf("openCard - malloc failed\n");
        return false;
    }
    if (SCARD_S_SUCCESS != (status = SCardListReadersA(context, NULL, pReaders, &readerInfoSize)))
    {
        switch (status)
        {
        case 0x8010002e:
            printf("Unable to detect a smart card reader.\n");
            printf("Please check that it is connected to the PC.\n");
            break;
        case 0x80100069:
            printf("Unable to detect smart card in the card reader.\n");
            printf("Please check that the card is fully inserted and the correct way up.\n");
            break;
        case 0x8010000B:
            printf("Unable to connect to card. Another program is using it\n");
            break;
        default:
            printf("openCard SCardListReadersA failed with code=0x%x\n", status);
            break;
        }
        return false;
    }

    // pReaders is a multi-string....
    char *pDisplayNames = pReaders;
    uint32_t numReaders = 0;
    while (0 != *pDisplayNames)
    {
        if (debug)
        {
            printf("Card readers = :%s:\n", pDisplayNames);
        }
        pDisplayNames += strlen(pDisplayNames) + 1;
        numReaders++;
    }

    // locate the right card
    switch (numReaders)
    {
    case 0:
    default:
        printf("Invalid number of card readers = %d\n", numReaders);
        printf("Only 1 or 2 readers is supported\n");
        return false;
        break;
    case 1:
        // Do nothing. Just use the first reader.
        break;
    case 2:
        if (useSecondReader)
        {
            pReaders += strlen(pReaders) + 1;
        }
        break;
    }

    // connect to the card
    if (SCARD_S_SUCCESS != (status = SCardConnectA(context, pReaders, SCARD_SHARE_EXCLUSIVE, SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1, &card, &activeProtocol)))
    {
        switch (status)
        {
        case 0x8010002e:
            printf("Unable to detect a smart card reader.\n");
            printf("Please check that it is connected to the PC.\n");
            break;
        case 0x80100069:
        case 0x80100066:
            if (debug)
            {
                printf("Unable to detect smart card in the card reader.\n");
                printf("Please check that the card is fully inserted and the correct way up.\n");
            }
            break;
        case 0x8010000B:
            printf("Unable to connect to card. Another program is using it\n");
            break;
        default:
            if (debug)
            {
                printf("openCard SCardConnectA failed with code=0x%x\n", status);
            }
            break;
        }
        return false;
    }

    switch (activeProtocol)
    {
    case SCARD_PROTOCOL_T0:
        pioSendPci = *SCARD_PCI_T0;
        break;

    case SCARD_PROTOCOL_T1:
        pioSendPci = *SCARD_PCI_T1;
        break;

    default:
        printf("unknown protocol type for card connection: %ld\n", activeProtocol);
        return false;
        break;
    }

    // select our applet.
    static const uint8_t select_command[] = { 0x00, 0xA4, 0x04, 0x00, 0x10, 0x46, 0x1F, 0x0A, 0x45, 0xE3, 0x70, 0xDF, 0xF0, 0x7C, 0xAC, 0xBB, 0x8E, 0x2F, 0x6A, 0xD8, 0x37 };
    if (!javacardSendAndCheckReply(select_command, sizeof(select_command), "openCard", OP_SELECT_APPLET ))
    {
        printf("Unable to launch the STZ Secure Deploy applet. Has the card been personalised?\n");
        return false;
    }
    
    printf("Opened connection to Javacard and selected our applet\n");
    connectionEstablished = true;
    return true;

}


// Get the applet info structure from the card.
bool javacardGetInfo(JavacardGetInfoReply *pDecodedInfoReply, bool debug)
{
    static const uint8_t info_command[] = { 0xB0, 0x40, 0x00, 0x00, 0x3e };
    uint8_t *pReplyBuffer = (uint8_t*)malloc(MAX_JAVACARD_MSG_LENGTH);
    DWORD replyLength = MAX_JAVACARD_MSG_LENGTH;
    uint32_t status;

    if (debug)
    {
        printf("Getting info from card\n");
    }

    if (!connectionEstablished)
    {
        if (debug)
        {
            printf("Connection has not been established to the card. FAIL.\n");
        }
        return false;
    }

    memset(pDecodedInfoReply, 0, sizeof(JavacardGetInfoReply));

    if (NULL == pReplyBuffer)
    {
        printf("getInfo - malloc failed\n");
        return false;
    }
    if (SCARD_S_SUCCESS != (status = SCardTransmit(card, &pioSendPci, info_command, sizeof(info_command), NULL, pReplyBuffer, &replyLength)))
    {
        printf("getInfo - failed to send command\n");
        return false;
    }

    // Length of the reply received from the Javacard.
    #define GET_INFO_REPLY_BYTE_LEN  (6*2 + 4 + MAX_JAVACARD_STRING_LENGTH*3)

    if (!javacardCheckReply(pReplyBuffer, replyLength, GET_INFO_REPLY_BYTE_LEN, OP_GET_INFO))
    {
        printf("getInfo - reply length(%d) != expected length(%d)\n", replyLength, GET_INFO_REPLY_BYTE_LEN);
        return false;
    }

    //parse the reply.
    pDecodedInfoReply->numDevicesInstalled = GET16(pReplyBuffer);
    pDecodedInfoReply->numDevicesRemaining = GET16(pReplyBuffer);
    pDecodedInfoReply->numDevicesFailed = GET16(pReplyBuffer);
    pDecodedInfoReply->savingInstalledLog = (1 == *pReplyBuffer++);

    pDecodedInfoReply->versionNameLength = *pReplyBuffer++;
    memcpy(pDecodedInfoReply->verionName, pReplyBuffer, MAX_JAVACARD_STRING_LENGTH);
    pReplyBuffer += MAX_JAVACARD_STRING_LENGTH;

    pDecodedInfoReply->oemNameLength = *pReplyBuffer++;
    memcpy(pDecodedInfoReply->oemName, pReplyBuffer, MAX_JAVACARD_STRING_LENGTH);
    pReplyBuffer += MAX_JAVACARD_STRING_LENGTH;

    pDecodedInfoReply->lifetimeLimit = GET16(pReplyBuffer);
    pDecodedInfoReply->lifetimeUsed = GET16(pReplyBuffer);

    pDecodedInfoReply->cardNameLength = *pReplyBuffer++;
    memcpy(pDecodedInfoReply->cardName, pReplyBuffer, MAX_JAVACARD_STRING_LENGTH);
    pReplyBuffer += MAX_JAVACARD_STRING_LENGTH;

    if (debug)
    {
        printf("Got info from card.\n");
    }
    return true;
}



// Function used by STZ to personalise a card to a particular OEM.
//
// Involves assigning an OEM name and a card name to the card.
// Also assigns a STZ controled lifetime limit on the number of deployments the card can perform.
// The OEM's public key is deployed to the card so that UPKs provided by the OEM can be authenticated.
// Personalisation is signed with the STZsigning key. Public part of the key is hardcoded into the javacard applet.
bool javacardPersonaliseCard(const char *pOemName, const char *pCardName, uint32_t lifetimeLimit, EccPublicKey *pOemPublicKey, EccPrivateKey *pStzSigningPrivateKey)
{
#define PERSONALISE_MSG_LEN (5 + MAX_JAVACARD_STRING_LENGTH + 64 + 2 + MAX_JAVACARD_STRING_LENGTH + 64)
    uint8_t  msg[PERSONALISE_MSG_LEN];
    uint8_t  *pMsg = msg;

    printf("Personalising card\n");

    if (!connectionEstablished)
    {
        printf("Connection has not been established to the card. FAIL.\n");
        return false;
    }

    bool retval = true;
    retval &= javacardCheckStringLength(pOemName, "OEM");
    retval &= javacardCheckStringLength(pCardName, "Card");
    if (!retval)
    {
        return false;
    }

    memset(msg, 0, sizeof(msg));

    *pMsg++ = 0xB0;
    *pMsg++ = 0x50;
    *pMsg++ = (uint8_t) strlen(pOemName);
    *pMsg++ = (uint8_t) strlen(pCardName);
    *pMsg++ = PERSONALISE_MSG_LEN - ADPU_HEADER_LENGTH;  // payload length


    memcpy(pMsg, pOemName, strlen(pOemName));
    pMsg += MAX_JAVACARD_STRING_LENGTH;

    memcpy(pMsg, pOemPublicKey, sizeof(EccPublicKey));
    pMsg += 64;

    if ((0xFFFF == lifetimeLimit) ||
        ((lifetimeLimit > 0) && (lifetimeLimit < 32768)))
    {
        PUT16(pMsg, lifetimeLimit);
    }
    else
    {
        printf("Lifetime limit must be 0xFFFF or between 1 and 32767. Supplied value was %d\n", lifetimeLimit);
        return false;
    }

    memcpy(pMsg, pCardName, strlen(pCardName));
    pMsg += MAX_JAVACARD_STRING_LENGTH;

    if (64 + pMsg - msg != PERSONALISE_MSG_LEN)
    {
        printf("personalise msg length calc wrong. actual=%d, expected=%d\n", 64 + pMsg - msg, PERSONALISE_MSG_LEN);
        return false;
    }

    uint8_t *pBuffer = (uint8_t*)msg;
    cryptoEccHashAndSign(&pBuffer[ADPU_HEADER_LENGTH], PERSONALISE_MSG_LEN - (sizeof(EccPublicKey) + ADPU_HEADER_LENGTH), NULL, 0, pStzSigningPrivateKey, (EccSignature*) pMsg);
    
    if (!javacardSendAndCheckReply(msg, PERSONALISE_MSG_LEN, "personalise", OP_PERSONALISE))
    {
        printf("Failed to personalise the card\n");
        return false;
    }
    printf("Personalised the card for OEM :%s:\n", pOemName);
    return true;
}


// Function used by STZ to update the personalisation of the javacard.
//
// The increase to the lifetime limit of the card is provided.
bool javacardUpdatePersonaliseCard(uint32_t addedLimit, EccPrivateKey *pStzSigningPrivateKey)
{
#define UPDATE_PERSONALISE_MSG_LEN (5 + 2 + MAX_JAVACARD_STRING_LENGTH + 64)
    uint8_t  msg[UPDATE_PERSONALISE_MSG_LEN];
    uint8_t  *pMsg = msg;
    memset(msg, 0, sizeof(msg));

    printf("Updating personalisation of card\n");

    if (!connectionEstablished)
    {
        printf("Connection has not been established to the card. FAIL.\n");
        return false;
    }

    JavacardGetInfoReply getInfoReply;
    memset(&getInfoReply, 0, sizeof(JavacardGetInfoReply));

    if (!javacardGetInfo(&getInfoReply, true))
    {
        printf("updatePersonaliseCard FAILED - Unable to get info from card. Has it be commisioned with the STZ app?\n");
        return false;
    }
    if (0 == getInfoReply.oemNameLength)
    {
        printf("updatePersonaliseCard FAILED - card is NOT personalised\n");
        return false;
    }
    
    *pMsg++ = 0xB0;
    *pMsg++ = 0x32;
    *pMsg++ = 0;
    *pMsg++ = getInfoReply.cardNameLength;
    *pMsg++ = UPDATE_PERSONALISE_MSG_LEN - ADPU_HEADER_LENGTH;  // payload length

    if (0xFFFF == addedLimit)
    {
        PUT16(pMsg, addedLimit);
    }
    else
    {
        addedLimit += getInfoReply.lifetimeLimit;
        if ((addedLimit > 0) && (addedLimit < 32768))
        {
            PUT16(pMsg, addedLimit);
        }
        else
        {
            printf("Lifetime limit must be 0xFFFF or between 1 and 32767. Supplied value was %d\n", addedLimit);
            return false;
        }
    }

    memcpy(pMsg, getInfoReply.cardName, MAX_JAVACARD_STRING_LENGTH);
    pMsg += MAX_JAVACARD_STRING_LENGTH;

    if (64 + pMsg - msg != UPDATE_PERSONALISE_MSG_LEN)
    {
        printf("personalise msg length calc wrong. actual=%d, expected=%d\n", 64 + pMsg - msg, PERSONALISE_MSG_LEN);
        return false;
    }

    uint8_t *pBuffer = (uint8_t*)msg;
    cryptoEccHashAndSign(&pBuffer[ADPU_HEADER_LENGTH], UPDATE_PERSONALISE_MSG_LEN - (sizeof(EccPublicKey) + ADPU_HEADER_LENGTH), NULL, 0, pStzSigningPrivateKey, (EccSignature*)pMsg);

    if (!javacardSendAndCheckReply(msg, UPDATE_PERSONALISE_MSG_LEN, "Update personalise", OP_UPDATE_PERSONALISE))
    {
        printf("FAILED To update personalisation of card\n");
        return false;
    }
    printf("Updated personalisation of card\n");
    return true;
}

// Used by the OEM to authorize the card to install a version of the software.
// OEM provides
// (a) a short name for the release version, 
// (b) how many devices the card is allowed to deploy 
// (c) the unencrypted UPK for the deployment.
// (d) the OEM's private key needed to sign the authorisation.

/* MOD REA START - Function takes signing key arguments instead of the key itself */
#ifdef ENABLE_WINDOWS_KEYSTORE
bool javacardAuthorizeCard(const char *versionName, const uint32_t numInstallations, uint8_t moduleId, uint8_t version, eSBMKeyPairType keyType, UpdateProtectionKey *pUpk)
#else
bool javacardAuthorizeCard(const char *versionName, const uint32_t numInstallations, EccPrivateKey *pOemPrivateKey, UpdateProtectionKey *pUpk)
#endif
{
#define AUTHORISE_HEADER_LENGTH (5 + 2 + MAX_JAVACARD_STRING_LENGTH)
#define AUTHORISE_TRAILER_LENGTH (5 + 64)
#define UPK_MAX_PAYLOAD_LENGTH (128)

    // Authorisation is a 3 step process.
    // 1) Send a header (which contains the name for the version and number of allowed deployments.
    // 2) Send the UPK.  [Since the UPK is large it needs to be sent as multiple segments].
    // 3) Send the signature (that was calculated across the header + UPK)
    
    printf("Authorising card to install version :%s: %d times\n", versionName, numInstallations);

    if (!connectionEstablished)
    {
        printf("Connection has not been established to the card. FAIL.\n");
        return false;
    }

    // TODO - calculate whether numInstallations will be allowed based on the number that have already been performed......


    if (pUpk->nsig > 4)
    {
        printf("Only UPKs with upto 4 signatures are supported. This UPK has %d sigs\n", pUpk->nsig);
        return false;
    }
    uint32_t upkRemaining = offsetof(UpdateProtectionKey, sigs[pUpk->nsig]);


    uint8_t  msg[MAX_JAVACARD_MSG_LENGTH];
    uint8_t  *pMsg = msg;
    memset(msg, 0, MAX_JAVACARD_MSG_LENGTH);

    // check parameters
    bool retval = true;
    retval &= javacardCheckStringLength(versionName, "versionName");
    if (!retval)
    {
        return false;
    }

    if (numInstallations > 65535)
    {
        printf("Maximum number of installations is 65535. %d requested.\n", numInstallations);
        return false;
    }

    // build header
    *pMsg++ = 0xB0;
    *pMsg++ = 0x52;
    *pMsg++ = pUpk->nsig;
    *pMsg++ = (uint8_t)strlen(versionName);
    *pMsg++ = AUTHORISE_HEADER_LENGTH - ADPU_HEADER_LENGTH;

    uint8_t *sigStartAddr = pMsg;
    PUT16(pMsg, numInstallations);
    memcpy(pMsg, versionName, strlen(versionName));

    // Calculate signature that will be sent in the trailer.
    EccSignature sig;
#ifdef ENABLE_WINDOWS_KEYSTORE
	if (!SBMCryptoHashAndSign(moduleId, version, keyType, sigStartAddr, (uint32_t)(AUTHORISE_HEADER_LENGTH - ADPU_HEADER_LENGTH), (uint8_t*)pUpk, upkRemaining, (uint8_t*)&sig, sizeof(sig)))
	{
		return false;
	}
#else
    cryptoEccHashAndSign(sigStartAddr, AUTHORISE_HEADER_LENGTH - ADPU_HEADER_LENGTH, (uint8_t*)pUpk, upkRemaining, pOemPrivateKey, &sig);
#endif
    // send the header.
    if (!javacardSendAndCheckReply(msg, AUTHORISE_HEADER_LENGTH, "Authorise header", OP_START_AUTHORISE))
    {
        printf("FAILED to start authorisation of card\n");
        return false;
    }


    // Now send the UPK.
    // prepare the header for sending the blocks of UPK.
    pMsg = msg;
    *pMsg++ = 0xB0;
    *pMsg++ = 0x54;
    uint8_t *pIndex = pMsg++;  // location for the index field.
    *pMsg++ = 0x00;
    uint8_t *pLength = pMsg++;  // location for the length field.

    *pIndex = 0;  // First block is block zero.

    uint8_t  *pSrc = (uint8_t *)pUpk;
    uint32_t segmentLength;

    while (upkRemaining)
    {
        segmentLength = (upkRemaining > UPK_MAX_PAYLOAD_LENGTH) ? UPK_MAX_PAYLOAD_LENGTH : upkRemaining;
        memcpy(pMsg, pSrc, segmentLength);
        *pLength = segmentLength;

        if (!javacardSendAndCheckReply(msg, ADPU_HEADER_LENGTH + segmentLength, "UPK", OP_LOAD_UNENCRYPTED_UPK))
        {
            printf("FAILED to send UPK to card.\n");
            return false;
        }

        *pIndex = *pIndex + 1;
        pSrc += segmentLength;
        upkRemaining -= segmentLength;
    }

    // Now send the trailer.
    pMsg = msg;
    *pMsg++ = 0xB0;
    *pMsg++ = 0x56;
    *pMsg++ = 0x00;
    *pMsg++ = 0x00;
    *pMsg++ = 0x40;
    memcpy(pMsg, &sig, sizeof(EccSignature));

    if (!javacardSendAndCheckReply(msg, AUTHORISE_TRAILER_LENGTH, "authorise trailer", OP_COMPLETE_AUTHORISE))
    {
        printf("FAILED to sign authorisation of card\n");
        return false;
    }

    return true;
}


// For each deployment an encrypted UPK has to be provided to the device.
//
// 1) The device certificate is read from the device.
// 2) The device certificate is sent to the javacard.
// 3) The javacard validates the device certificate and hence encrypts the UPK.
// 4) The encrypted UPK is extracted from the javacard.
// 5) The encrypted UPK is sent to the device, followed by the encrypted update.
bool javacardEncryptUPK(DeviceCertificate *pDevCert, uint8_t **ppEncryptedUpk, uint32_t *pEncryptedUPKlength)
{
    // The detailed procedure is
    // 1) send the Device Certificate. [Due to its size it has to be sent as multiple segments].
    // 2) send the command to encrypt the UPK.
    // 3) extract the encrypted UPK. [Due to its size the extraction has to be performed in multiple segments].

#define DEV_CERT_MAX_PAYLOAD_LENGTH (128)
    *pEncryptedUPKlength = 0;

    printf("Encrypting a UPK\n");

    if (!connectionEstablished)
    {
        printf("Connection has not been established to the card. FAIL.\n");
        return false;
    }
    
    uint8_t  msg[MAX_JAVACARD_MSG_LENGTH];
    uint8_t  *pMsg = msg;
    memset(msg, 0, MAX_JAVACARD_MSG_LENGTH);

    // send the DevCert
    *pMsg++ = 0xB0;
    *pMsg++ = 0x58;
    uint8_t *pIndex = pMsg++;
    *pMsg++ = 0x00;
    uint8_t *pLength = pMsg++;

    *pIndex = 0;  // First block is block zero.

    uint8_t  *pSrc = (uint8_t *)pDevCert;
    uint32_t segmentLength;
    uint32_t devCertRemaining = sizeof(DeviceCertificate);

    while (devCertRemaining)
    {
        segmentLength = (devCertRemaining > UPK_MAX_PAYLOAD_LENGTH) ? UPK_MAX_PAYLOAD_LENGTH : devCertRemaining;
        memcpy(pMsg, pSrc, segmentLength);
        *pLength = segmentLength;


        if (!javacardSendAndCheckReply(msg, ADPU_HEADER_LENGTH + segmentLength, "DevCert", OP_SEND_DEV_CERT))
        {
            printf("FAILED to send DevCert to card\n");
            return false;
        }

        *pIndex = *pIndex + 1;
        pSrc += segmentLength;
        devCertRemaining -= segmentLength;
    }

    // send command to request encryption of the UPK
    pMsg = msg;
    *pMsg++ = 0xB0;
    *pMsg++ = 0x5A;
    *pMsg++ = 0x00;
    *pMsg++ = 0x00;
    *pMsg++ = 2 + sizeof(EccPublicKey) + sizeof(HmacResult);

    DWORD replyLength = MAX_JAVACARD_MSG_LENGTH;
    uint8_t replyBuffer[MAX_JAVACARD_MSG_LENGTH];
    uint8_t *pReplyBuffer = replyBuffer;
    uint32_t status;


    if (SCARD_S_SUCCESS != (status = SCardTransmit(card, &pioSendPci, msg, ADPU_HEADER_LENGTH, NULL, replyBuffer, &replyLength)))
    {
        printf("send of request to encrypt UPK failed. Status=0x%x, msgLength=%d\n", status, ADPU_HEADER_LENGTH);
        return false;
    }
    if (!javacardCheckReply(replyBuffer, replyLength, 2 + 2 + sizeof(HmacResult) + sizeof(EccPublicKey), OP_ENCRYPT_UPK))
    {
        printf("Reply to request to encrypt UPK was invalid\n");
        return false;
    }

    // format of reply is :-
    // 2 bytes - length of the encrypted UPK that follows in the next messages.
    // 64 bytes ECDH public key.
    // 32 bytes HMAC for the encrypted UPK

    uint32_t encryptedUpkLength = GET16(pReplyBuffer);
    uint8_t *pEncryptedUpk = (uint8_t *)malloc(encryptedUpkLength + sizeof(HmacResult) + sizeof(EccPublicKey));

    if (NULL == pEncryptedUpk)
    {
        printf("Failed to alloc buffer for encrypted UPK\n");
        return false;
    }

    if ((sizeof(HmacResult) + sizeof(EccPublicKey)) != (replyLength - (2 + 2)))
    {
        printf("UPK header length incorrect %d!=%d\n", sizeof(HmacResult) + sizeof(EccPublicKey), replyLength - (2 + 2));
        return false;
    }

    // Copy the header (ECDH public key + HMAC) into the encrypted UPK buffer.
    memcpy(pEncryptedUpk, &replyBuffer[2], replyLength - (2 + 2));

    // Now get the encrypted UPK.
    pMsg = msg;
    *pMsg++ = 0xB0;
    *pMsg++ = 0x5C;
    pIndex = pMsg++;
    *pMsg++ = 0x00;
    pLength = pMsg++;

    *pIndex = 0;  // First block is block zero.

    uint8_t  *pDst = (uint8_t *)&pEncryptedUpk[sizeof(HmacResult) + sizeof(EccPublicKey)];
    uint32_t upkRemaining = encryptedUpkLength;

    while (upkRemaining)
    {
        segmentLength = (upkRemaining > UPK_MAX_PAYLOAD_LENGTH) ? UPK_MAX_PAYLOAD_LENGTH : upkRemaining;
        *pLength = segmentLength;
        replyLength = MAX_JAVACARD_MSG_LENGTH;

        if (SCARD_S_SUCCESS != (status = SCardTransmit(card, &pioSendPci, msg, ADPU_HEADER_LENGTH, NULL, replyBuffer, &replyLength)))
        {
            printf("send of request to get encrypted UPK failed (segment=%d). Status=0x%x, msgLength=%d\n", *pIndex, status, ADPU_HEADER_LENGTH);
            return false;
        }
        if (!javacardCheckReply(replyBuffer, replyLength, 2 + segmentLength, OP_ENCRYPT_UPK_GET_ENCRYTPED_UPK))
        {
            printf("Reply to request to get encrypted UPK failed (segment=%d)\n", *pIndex);
            return false;
        }
        memcpy(pDst, replyBuffer, segmentLength);
        
        *pIndex = *pIndex + 1;
        pDst += segmentLength;
        upkRemaining -= segmentLength;
    }

    printf("Encrypted the UPK\n");
    *ppEncryptedUpk = pEncryptedUpk;
    *pEncryptedUPKlength = encryptedUpkLength + sizeof(HmacResult) + sizeof(EccPublicKey);

    return true;
}


bool javacardGetInstalledDeviceList(uint32_t numInstalled, UniqueId *pInstalledDeviceList)
{
    uint8_t msg[] = { 0xB0, 0x42, 0, 0, 0x80 };
    uint8_t *pIndex = &msg[2];

    uint8_t *pReplyBuffer = (uint8_t*)malloc(MAX_JAVACARD_MSG_LENGTH);
    DWORD replyLength = MAX_JAVACARD_MSG_LENGTH;
    uint32_t status;
    uint32_t expectedReplyLength;
    uint32_t numRequested;
    uint32_t offset = 0;

#define MAX_NUM_DEVICE_ID_IN_ONE_MESSAGE    8

    memset(pInstalledDeviceList, 0, sizeof(UniqueId) * numInstalled);

    printf("Getting installed devices list from card. Num devices=%d\n", numInstalled);

    if (!connectionEstablished)
    {
        printf("Connection has not been established to the card. FAIL.\n");
        return false;
    }
    if (NULL == pReplyBuffer)
    {
        printf("getInstalledDevices - malloc failed\n");
        return false;
    }
    while (numInstalled)
    {
        numRequested = numInstalled > MAX_NUM_DEVICE_ID_IN_ONE_MESSAGE ? MAX_NUM_DEVICE_ID_IN_ONE_MESSAGE : numInstalled;
        numInstalled -= numRequested;
        printf("getInstalledDevices - getting block of size (%d)\n", numRequested);

        // Always replies with 8 ids irrespective of how many are valid....
        expectedReplyLength = MAX_NUM_DEVICE_ID_IN_ONE_MESSAGE * sizeof(UniqueId) + 2;  // 2 for the status field.

        pIndex = &msg[2];
        PUT16(pIndex, offset);
        offset += numRequested;

        replyLength = MAX_JAVACARD_MSG_LENGTH;

        if (SCARD_S_SUCCESS != (status = SCardTransmit(card, &pioSendPci, msg, sizeof(msg), NULL, pReplyBuffer, &replyLength)))
        {
            printf("getInstalledDevices - failed to send command\n");
            return false;
        }
        if (!javacardCheckReply(pReplyBuffer, replyLength, expectedReplyLength, OP_GET_DEVICE_ID_LIST))
        {
            printf("getInstalledDevices - reply length(%d) != expected length(%d)\n", replyLength, expectedReplyLength);
            return false;
        }
        memcpy(pInstalledDeviceList, pReplyBuffer, numRequested * sizeof(UniqueId));
        pInstalledDeviceList += numRequested;
    }

    return true;
}

void javacardDisplayInfo(void)
{
    JavacardGetInfoReply jcInfo;

    javacardGetInfo(&jcInfo, false);
    printf("[JCInfo]%s:%s:%s:%d:%d:%d\n",
        jcInfo.oemName,
        jcInfo.cardName,
        jcInfo.verionName,
        jcInfo.numDevicesInstalled,
        jcInfo.numDevicesRemaining,
        jcInfo.numDevicesFailed);

    UniqueId *pDeviceIdList = (UniqueId*)malloc(sizeof(UniqueId) * jcInfo.numDevicesInstalled);
    if (javacardGetInstalledDeviceList(jcInfo.numDevicesInstalled, pDeviceIdList))
    {
        uint8_t *pOneId;
        for (uint32_t i = 0; i < jcInfo.numDevicesInstalled; i++)
        {
            pOneId = (uint8_t*)&pDeviceIdList[i];
            printf("[JCDeviceId]%d:", i);
            for (uint32_t j = 0; j < sizeof(UniqueId); j++)
            {
                printf("%02x", pOneId[j]);
            }
            printf("\n");
        }
    }
}


bool javacardDetectPresenceChange(bool useSecondReader, bool currentPresence)
{
    JavacardGetInfoReply jcInfo;

    if (currentPresence)
    {
        if (javacardGetInfo(&jcInfo, false))
        {
            return false;
        }
        else
        {
            // card has just disappeared....
            printf("[JCremoved]\n");
            return true;
        }
    }
    else
    {
        if (javacardOpenCard(useSecondReader, false))
        {
            printf("[JCinserted]\n");
            return true;
        }
        else
        {
            return false;
        }
    }
}
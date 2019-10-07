

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



#ifndef JAVARCARD_COMMON_H_
#define JAVARCARD_COMMON_H_

#ifdef ENABLE_WINDOWS_KEYSTORE
#include "SBMCryptoTypes.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
		
#define MAX_JAVACARD_STRING_LENGTH 16   // card names are upto 16 characters.


    typedef struct
    {
        uint8_t     oemNameLength;          // Name of the OEM to which this card is assigned.
        char        oemName[MAX_JAVACARD_STRING_LENGTH + 1];  // +1 for terminating \0

        uint8_t     cardNameLength;         // Name of this particular card.
        char        cardName[MAX_JAVACARD_STRING_LENGTH + 1];

        uint16_t    lifetimeLimit;          // Total number of installations allowed by STZ for this card.
        uint16_t    lifetimeUsed;           // Total number of installations used.



        uint16_t    numDevicesInstalled;    // Number of succesfull UPK encryptions within the current authorisation.
        uint16_t    numDevicesRemaining;    // Number of UPK encryptions remaining within the curent authorisation.
        uint16_t    numDevicesFailed;       // Number of UPK encryption failures within the current authorisation.

        bool        savingInstalledLog;     // True if an installation log is being saved.
        
        uint8_t     versionNameLength;      // Textual name for the SW version being installed.
        char        verionName[MAX_JAVACARD_STRING_LENGTH + 1];  // +1 for the \0
        
    } JavacardGetInfoReply;


    // Opens a connection to a card and selects the STZ applet.
    //
    // useSecondReader - if True and there are 2 readers connected, then the 2nd is used. Otherwise 1st (or only) reader is used.
    extern bool javacardOpenCard(bool useSecondReader, bool debug);

    // Gets (and decodes) the info structure from a card.
    // Caller must provide a pointer to a structure (which the function populates).
    extern bool javacardGetInfo(JavacardGetInfoReply *pDecodedInfoReply, bool debug);

    // Function used by STZ to personalise a card to a particular OEM.
    //
    // Involves assigning an OEM name and a card name to the card.
    // Also assigns a STZ controled lifetime limit on the number of deployments the card can perform.
    // The OEM's public key is deployed to the card so that UPKs provided by the OEM can be authenticated.
    // Personalisation is signed with the STZsigning key. Public part of the key is hardcoded into the javacard applet.
    extern bool javacardPersonaliseCard(const char *pOemName, const char *pCardName, uint32_t lifetimeLimit, EccPublicKey *pOemPublicKey, EccPrivateKey *pStzSigningPrivateKey);

    // Function used by STZ to update the personalisation of the javacard.
    //
    // The increase to the lifetime limit of the card is provided.
    extern bool javacardUpdatePersonaliseCard(uint32_t addedLimit, EccPrivateKey *pStzSigningPrivateKey);

    // Used by the OEM to authorize the card to install a version of the software.
    // OEM provides
    // (a) a short name for the release version, 
    // (b) how many devices the card is allowed to deploy 
    // (c) the unencrypted UPK for the deployment.
    // (d) the OEM's private key needed to sign the authorisation.
#ifdef ENABLE_WINDOWS_KEYSTORE
	extern bool javacardAuthorizeCard(const char *versionName, const uint32_t numInstallations, uint8_t moduleId, uint8_t version, eSBMKeyPairType keyType, UpdateProtectionKey *pUpk);
#else
    extern bool javacardAuthorizeCard(const char *versionName, const uint32_t numInstallations, EccPrivateKey *pOemPrivateKey, UpdateProtectionKey *pUpk);
#endif
        
    // For each deployment an encrypted UPK has to be provided to the device.
    //
    // 1) The device certificate is read from the device.
    // 2) The device certificate is sent to the javacard.
    // 3) The javacard validates the device certificate and hence encrypts the UPK.
    // 4) The encrypted UPK is extracted from the javacard.
    // 5) The encrypted UPK is sent to the device, followed by the encrypted update.
    extern bool javacardEncryptUPK(DeviceCertificate *pDevCert, uint8_t **ppEncryptedUpk, uint32_t *pEncryptedUPKlength);

    // Provide a list of the UniqueId's of the installed devices.
    //
    // Caller must ensure that pInstalledDeviceList points to a buffer of numInstalled*sizeof(UniqueId) bytes.
    extern bool javacardGetInstalledDeviceList(uint32_t numInstalled, UniqueId *pInstalledDeviceList);

    // Displays info on the current selected javacard.
    //
    // Info is rendered by the GUI.
    extern void javacardDisplayInfo(void);

    // Reports on the insert and removal of cards.
    // 
    // CurrentPresence - input regarding the current state of the card.
    // Return true if the new state is different from the current state.
    extern bool javacardDetectPresenceChange(bool useSecondReader, bool currentPresence);

#ifdef __cplusplus
}
#endif

#endif

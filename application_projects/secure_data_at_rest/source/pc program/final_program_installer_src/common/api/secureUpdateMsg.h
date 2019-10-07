

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


/*
 * secureUpdateMsg.h
 *
 *  Created on: 3 Mar 2016
 *      Author: paul
 */

#ifndef SECURE_UPDATE_MSG_H_
#define SECURE_UPDATE_MSG_H_



// For use in C++
#ifdef __cplusplus
extern "C" {
#endif

//#define SPECIAL_COMBO_TESTING


#define MAX_NUM_SIG              10
#define MAX_FILENAME_LEN        500
#define MAX_NUM_CMDS_IN_UPDATE  100

#define SECURE_UPDATE_MSG_VERSION  1  // This is version 1 of the Secure Update message protocol.

typedef enum
{
    SECURE_UPDATE_HEADER,
    SECURE_UPDATE_MODIFY,
    SECURE_UPDATE_ADD,
    SECURE_UPDATE_DELETE,
    SECURE_UPDATE_TRAILER
} SecureUpdateCommands;

typedef struct
{
    SecureUpdateCommands cmd;               // Type of operation for this element.

    ModuleId  module;                       // module to be affected.
    uint32_t  newVersion;                   // new version number for the module.

    uint32_t  numSig;                       // number of versions of the authorising module
    uint32_t  sigVersion[MAX_NUM_SIG];      // which versions of the sigModule are authorising.

    bool      encrypted;                    // Whether the module should be encrypted.
    uint32_t  newSigKey;                    // 0=>new key is required. !=0 is the version number of module whose keys should be copied.
    uint32_t  newMUK;                       // 0=>new key is required. !=0 is the version number of module whose keys should be copied.

    char      filename[MAX_FILENAME_LEN];   // srec file for the module.
} SecureUpdateEntry;

typedef struct
{
    uint32_t            numEntries;
    SecureUpdateEntry   entries[MAX_NUM_CMDS_IN_UPDATE];
} SecureUpdateInput;


extern bool secureUpdateMakeUpdate(SecureUpdateInput *pInput, uint8_t *pStartBuffer, uint32_t *pFullBufferLength, uint8_t *pUpkBuffer, uint32_t *upkBufferLength);
extern bool secureUpdateDecodeUpdate(uint8_t * const pBuffer, const uint32_t length);


#ifdef __cplusplus
}
#endif

#endif /* SRC_MODULETABLE_H_ */

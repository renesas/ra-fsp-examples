/***********************************************************************************************************************
 * Copyright [2015] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
 *
 * The contents of this file (the "contents") are proprietary and confidential to Renesas Electronics Corporation
 * and/or its licensors ("Renesas") and subject to statutory and contractual protections.
 *
 * Unless otherwise expressly agreed in writing between Renesas and you: 1) you may not use, copy, modify, distribute,
 * display, or perform the contents; 2) you may not use any name or mark of Renesas for advertising or publicity
 * purposes or in connection with your use of the contents; 3) RENESAS MAKES NO WARRANTY OR REPRESENTATIONS ABOUT THE
 * SUITABILITY OF THE CONTENTS FOR ANY PURPOSE; THE CONTENTS ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY, INCLUDING THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND
 * NON-INFRINGEMENT; AND 4) RENESAS SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, OR CONSEQUENTIAL DAMAGES,
 * INCLUDING DAMAGES RESULTING FROM LOSS OF USE, DATA, OR PROJECTS, WHETHER IN AN ACTION OF CONTRACT OR TORT, ARISING
 * OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE CONTENTS. Third-party contents included in this file may
 * be subject to different terms.
 **********************************************************************************************************************/
/**********************************************************************************************************************
* File Name    : crypto_cfg.h
* Description  : Configuration options for the cryptographic modules
***********************************************************************************************************************/


#ifndef R_CRYPTO_CFG_H
#define R_CRYPTO_CFG_H

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/


/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#ifndef _WIN32

// These defines controls use of the Synergy SCE Hardware acceleration for crypto operations.
// ENABLE_HW_CRYPTO must be defined if ANY of the individual HW crypto functions are defined.
// Each of the individual functions (RNG, SHA256, etc.) can be individually enabled/disabled.
#define ENABLE_HW_CRYPTO

#ifdef ENABLE_HW_CRYPTO
#define ENABLE_HW_SHA256
#define ENABLE_HW_RNG
#define ENABLE_HW_ECC
//#define SCE_HW_AES128 // NOT YET implemented

#endif  // #ifdef ENABLE_HW_CRYPTO

#endif //#ifndef _WIN32


/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/**********************************************************************************************************************
Exported global variables
***********************************************************************************************************************/


/**********************************************************************************************************************
Function Prototypes
***********************************************************************************************************************/

#endif // R_CRYPTO_CFG_H


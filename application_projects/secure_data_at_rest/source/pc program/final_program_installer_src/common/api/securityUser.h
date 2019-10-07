/***********************************************************************************************************************
 * Copyright [2015-2017] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
 * 
 * The contents of this file (the "contents") are proprietary and confidential to Renesas Electronics Corporation
 * and/or its licensors ("Renesas") and subject to statutory and contractual protections.
 *
 * This file is subject to a Renesas SSP license agreement. Unless otherwise agreed in an SSP license agreement with
 * Renesas: 1) you may not use, copy, modify, distribute, display, or perform the contents; 2) you may not use any name
 * or mark of Renesas for advertising or publicity purposes or in connection with your use of the contents; 3) RENESAS
 * MAKES NO WARRANTY OR REPRESENTATIONS ABOUT THE SUITABILITY OF THE CONTENTS FOR ANY PURPOSE; THE CONTENTS ARE PROVIDED
 * "AS IS" WITHOUT ANY EXPRESS OR IMPLIED WARRANTY, INCLUDING THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, AND NON-INFRINGEMENT; AND 4) RENESAS SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, OR
 * CONSEQUENTIAL DAMAGES, INCLUDING DAMAGES RESULTING FROM LOSS OF USE, DATA, OR PROJECTS, WHETHER IN AN ACTION OF
 * CONTRACT OR TORT, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE CONTENTS. Third-party contents
 * included in this file may be subject to different terms.
 **********************************************************************************************************************/

/**********************************************************************************************************************
 * File Name    : securityUser.h
 * Description  : Prototypes for functions that can be defined by the user to customize the behavior of the SBM
 **********************************************************************************************************************/

#ifndef SECURITY_USER_H
#define SECURITY_USER_H
/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "buttons.h"
#include "moduleInfo.h"
/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/


/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/
/** Enumeration for Security Kernel Status */
typedef enum e_security_status
{
    SECURE_KERNEL_BRICKED,        ///< Device is bricked
    SECURE_KERNEL_CRYPTO_FAILED,  ///< Crypto test failed. Only used in Crypto Testing mode.
    SECURE_KERNEL_DEFAULT_HANDLER,///< Code is in default handler; system exception occurred since there are no interrupts
} security_status_t;

/***********************************************************************************************************************
 * Functions
 **********************************************************************************************************************/

void SecurityUserStatusNotify (security_status_t status);
void SecurityUserStatusInitialize (void);
void SecurityUserControlsInitialize (void);
bool SecurityUserForceExecution (ModuleName module);

#endif //SECURITY_USER_H

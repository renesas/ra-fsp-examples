

/***********************************************************************************************************************
* Copyright [2015] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
*
* The contents of this file (the "contents") are proprietary and confidential to Renesas Electronics Corporation
* and/or its licensors ("Renesas") and subject to statutory and contractual protections.
*
* Unless otherwise expressly agreed in writing between Renesas and you: 1) you may not use, copy, modify, distribute,
* display, or perform the contents; 2) you may not use any name or mark of Renesas for advertising or publicity
* purposes or in connection with your use of the contents; 3) RENESAS MAKES NO WARRANTY OR REPRESENTATIONS ABOUT THE
* SUITABILITY OF THE CONTENTS FOR ANY PURPOSE; THE CONTENTS ARE PROVIDED �eAS IS�f WITHOUT ANY EXPRESS OR IMPLIED
* WARRANTY, INCLUDING THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND
* NON-INFRINGEMENT; AND 4) RENESAS SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, OR CONSEQUENTIAL DAMAGES,
* INCLUDING DAMAGES RESULTING FROM LOSS OF USE, DATA, OR PROJECTS, WHETHER IN AN ACTION OF CONTRACT OR TORT, ARISING
* OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE CONTENTS. Third-party contents included in this file may
* be subject to different terms.
**********************************************************************************************************************/


#ifndef PRINTF_MAPPING_H
#define PRINTF_MAPPING_H


// For security kernel development builds ERRORs and TRACEing are routed via printf to PMOD-D.
// For production builds all debug tracing is disabled (resulting in a smaller kernel).

#if defined (DISABLE_PRINTF)
#define ERROR(...)
#define TRACE(...)
#define SEMI_HOSTED_PRINTF(...)
#define ASSERT_OBITUARY(...)

#else
#define ERROR               printf
#define TRACE               printf
#define SEMI_HOSTED_PRINTF  st_semihosting_print
#define ASSERT_OBITUARY     sprintf
#endif

#define UNUSED(x) (void)(x)

#endif


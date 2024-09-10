/***********************************************************************************************************************
* Copyright (c) 2019 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/


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


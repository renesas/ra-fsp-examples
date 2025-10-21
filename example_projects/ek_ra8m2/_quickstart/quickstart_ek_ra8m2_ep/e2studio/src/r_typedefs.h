/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/**********************************************************************************************************************
 * File Name    : r_typedefs.h
 * Version      : 1.0
 * Description  : Contains macros, data structures and functions used  common to the EP
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 14/10/2025 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include <stdint.h>
#include <stdbool.h>

#include "common_data.h"

 /**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#ifndef RENESAS_APPLICATION_SYSTEM_INC_R_TYPEDEFS_H_
#define RENESAS_APPLICATION_SYSTEM_INC_R_TYPEDEFS_H_


/* Suppress compiler warning: unused parameter 'xxx' [-Wunused-parameter] */
#define     UNUSED_PARAM(param)             ((void)(param))

#define R_UNINITIALISED_U8    (0xFF)
#define R_UNINITIALISED_U16   (0xFFFF)
#define R_UNINITIALISED_U32   (0xFFFFFFFF)

/**********************************************************************************************************************
 Global Typedef definitions
 *********************************************************************************************************************/
typedef int ( * test_fn)(void);

typedef int          int_t;
typedef unsigned int bool_t;
typedef char         char_t;

typedef float       float32_t;
typedef double      float64_t;
typedef long double float128_t;

/**********************************************************************************************************************
 External global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/

#endif /* RENESAS_APPLICATION_SYSTEM_INC_R_TYPEDEFS_H_ */


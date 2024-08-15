/**********************************************************************************************************************
 * File Name    : r_typedefs.h
 * Description  : Contains macros, data structures and functions used  common to the EP
 *********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/* generic headers */

#ifndef R_TYPEDEFS_H_
#define R_TYPEDEFS_H_

/*
 * Typedefs
 * Other typedefs are defined in <stdint.h>
 */

/* typedef int ( * test_fn)(void); */
/**********************************************************************************************************************
 * Function Name: test_fn
 * Description  : Common prototype for sub-menu functions
 * Argument     : pvParameters
 * Return Value : User defined
 *********************************************************************************************************************/
typedef int ( * test_fn)(void);

typedef int int_t;
typedef unsigned int bool_t;
typedef char         char_t;

typedef float float32_t;
typedef double float64_t;
typedef long double float128_t;

#endif /* R_TYPEDEFS_H_ */

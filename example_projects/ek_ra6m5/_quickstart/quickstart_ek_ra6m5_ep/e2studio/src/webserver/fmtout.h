/***********************************************************************************************************************
* Copyright (c) 2010 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/******************************************************************************
* File Name    : fmtOut.h
* Version      : 1.0
* Device(s)    : Renesas
* Tool-Chain   : GNUARM-NONE-EABI v14.02
* OS           : None
* H/W Platform : RSK+
* Description  : The driver for a file on a disk.
*******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 18.11.2010 1.00    First Release
******************************************************************************/

/******************************************************************************
  WARNING!  IN ACCORDANCE WITH THE USER LICENCE THIS CODE MUST NOT BE CONVEYED
  OR REDISTRIBUTED IN COMBINATION WITH ANY SOFTWARE LICENSED UNDER TERMS THE
  SAME AS OR SIMILAR TO THE GNU GENERAL PUBLIC LICENCE
******************************************************************************/
#ifndef FMTOUT_H_INCLUDED
#define FMTOUT_H_INCLUDED

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include <stdarg.h>
#include <stdint.h>
/* Remove if float support not required */
#define _FMTOUT_FLOAT_SUPPORT_

/******************************************************************************
Typedefs
******************************************************************************/
/* Define the type of the low level put function */
typedef int32_t (* PFNPUTCHAR)(char, void *);

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
Function Name: fmtOut
Description:   Function to perform ANSI formatted output
Arguments:     IN  pszFormat - Pointer to the format string
               IN  pfnPutChar - Pointer to a function to output a int8_t
               IN  pvGenericPointer - Pointer passed to pfnPutChar
               IN  ap - The argument pointer
Return value:  The number of characters printed
******************************************************************************/
extern  int32_t fmtOut(const char     *pszFormat,
                       PFNPUTCHAR     pfnPutChar,
                       void *         pvGenericPointer,
                       va_list        ap);
#ifdef __cplusplus
}
#endif

/* FMTOUT_H_INCLUDED */
#endif 

/******************************************************************************
End  Of File
******************************************************************************/

/******************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only
* intended for use with Renesas products. No other uses are authorized.
* This software is owned by Renesas Electronics Corporation and is  protected
* under all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES
* REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY,
* INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR  A
* PARTICULAR PURPOSE AND NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE  EXPRESSLY
* DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE  LIABLE
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES
* FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS
* AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this
* software and to discontinue the availability of this software.
* By using this software, you agree to the additional terms and
* conditions found by accessing the following link:
* http://www.renesas.com/disclaimer
*******************************************************************************
* Copyright (C) 2010 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************
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

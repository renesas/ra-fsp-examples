/******************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only
* intended for use with Renesas products. No other uses are authorized. This
* software is owned by Renesas Electronics Corporation and is protected under
* all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
* AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software
* and to discontinue the availability of this software. By using this software,
* you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
*******************************************************************************
* Copyright (C) 2012 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************
* File Name    : trace.h
* Version      : 1.01
* Device(s)    : Renesas
* Tool-Chain   : GNUARM-NONE-EABI v14.02
* OS           : FreeRTOS
* H/W Platform : RSK+
* Description  : Debug formatted output routine
*                TRACE print function enabled with define _TRACE_ON_
*******************************************************************************
* History      : DD.MM.YYYY Version Description
*              : 05.08.2010 1.00    First Release
*              : 14.12.2010 1.01    Added ASSERT definition
******************************************************************************/

/******************************************************************************
  WARNING!  IN ACCORDANCE WITH THE USER LICENCE THIS CODE MUST NOT BE CONVEYED
  OR REDISTRIBUTED IN COMBINATION WITH ANY SOFTWARE LICENSED UNDER TERMS THE
  SAME AS OR SIMILAR TO THE GNU GENERAL PUBLIC LICENCE
******************************************************************************/

#ifndef TRACE_H_INCLUDED_
#define TRACE_H_INCLUDED_

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/******************************************************************************
Macro definitions
******************************************************************************/

/* Some function macros for TRACE output
   NOTE: debugging TRACE statements require double braces
   so the debug strings can be removed from the output load module:
   TRACE(("My Variable = %u\r\n", uiMyVariable));
   See ANSI C formatted output for more detail on the format specifiers */

#ifdef _TRACE_ON_                   /* Trace ON */
#define TRACE(_x_)                  Trace _x_
#define TRACE_VALUE(_x_)            Trace("0x%.8lX FILE %s LINE %d\r\n",\
                                    (long)_x_, __FILE__, __LINE__);
extern void abort(void);
#define ASSERT(x)                   (x ? (void)0 :(Trace("ASSERT: "#x" "\
                                    "%s, %d \r\n", __FILE__,__LINE__),\
                                     abort(),(void)0))
#else                               /* _NO_TRACE_ON_ */
#define TRACE(_x_)                  /* TRACE REMOVED */
#define VALUE(_x_)                  /* VALUE REMOVED */
#define ASSERT(x)                   ((void)0)
#endif                              /* _TRACE_ON_ */

/******************************************************************************
Function Prototypes
******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
Function Name: Trace
Description:   Function to perform a formatted print output for debugging
Arguments:     IN  pszFormat - Pointer to a null terminated format string
               I/O ... - The parameters
Return value:  The number of chars output
******************************************************************************/
#ifdef _TRACE_ON_                   /* Trace ON */
extern  int Trace(const char_t *pszFormat, ...);
/* Same as above but replaces '\n' with "\r\n" */
extern  int _Trace_(const char_t *pszFormat, ...);
#endif

/******************************************************************************
Function Name: dbgPrintBuffer
Description:   Function to print a data buffer in hex format
Arguments:     IN  pbyBuffer - Pointer to the buffer
               IN  stLength - The length of the buffer
Return value:  none
******************************************************************************/
#ifdef _TRACE_ON_                   /* Trace ON */
extern  void dbgPrintBuffer(uint8_t *pbyBuffer, size_t stLength);
#endif

#ifdef __cplusplus
}
#endif

#endif /* TRACE_H_INCLUDED */

/******************************************************************************
End  Of File
******************************************************************************/

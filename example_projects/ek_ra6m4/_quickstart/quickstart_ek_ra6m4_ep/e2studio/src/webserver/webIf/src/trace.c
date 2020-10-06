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
* File Name    : trace.c
* Version      : 1.01
* Device(s)    : Renesas
* Tool-Chain   : GNUARM-NONE-EABI v14.02
* OS           : FreeRTOS
* H/W Platform : Renesas
* Description  : Debug formatted output routine for SCIx
*                TRACE print function enabled with define _TRACE_ON_
*******************************************************************************
* History      : DD.MM.YYYY Ver. Description
*              : 04.02.2010 1.00 First Release
*              : 10.06.2010 1.01 Updated type definitions
******************************************************************************/

/******************************************************************************
  WARNING!  IN ACCORDANCE WITH THE USER LICENCE THIS CODE MUST NOT BE CONVEYED
  OR REDISTRIBUTED IN COMBINATION WITH ANY SOFTWARE LICENSED UNDER TERMS THE
  SAME AS OR SIMILAR TO THE GNU GENERAL PUBLIC LICENCE
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include <stdio.h>
#include <stdarg.h>

#include <stdint.h>
#include <stddef.h>
#include "fmtOut.h"

/******************************************************************************
Typedef definitions
******************************************************************************/

typedef char                char_t;

typedef struct _PRINTBUFF
{
    uint8_t *   pbyBuffer;
    uint32_t    uiCount;
    uint32_t    uiLength;

} PRINTBUFF,
*PPRINTBUFF;

/******************************************************************************
Macro definitions
******************************************************************************/

/* Define the TRACE output buffer size. This is allocated on the stack when 
   the Trace function is called so it is advantageous to keep it small to
   miminise stack usage */
#define TRACE_OUTPUT_BUFFER_SIZE    64
#define TRACE_DATA_LINE_LENGTH      16

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/

/******************************************************************************
* Function Name: sciOutputDebugString
* Description  : Function to write a debug string (for TRACE((""));)
* Arguments    : IN  pbyBuffer - Pointer to the source memory
*                IN  uiCount - The number of bytes to write
* Return Value : 0 for success -1 on error
******************************************************************************/

extern int scifOutputDebugString(uint8_t *pbyBuffer, uint32_t uiCount);

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Function Prototypes
******************************************************************************/

static int32_t hPutChar(char iChar, void *pFileStream);
/* Same as above but it replaces '\n' with "\r\n" */
static int32_t _hPutChar_(char iChar, void *pFileStream);

int Trace(const char_t *pszFormat, ...);
int _Trace_(const char_t *pszFormat, ...);
void dbgPrintBuffer(uint8_t *pbyBuffer, size_t stLength);

/******************************************************************************
Public Functions
******************************************************************************/

/******************************************************************************
* Function Name: Trace
* Description  : Function to perform a formatted print output for debugging 
* Arguments    : IN  pszFormat - Pointer to a null terminated format string
*                I/O ... - The parameters
* Return Value : The number of chars output
******************************************************************************/
static uint8_t     pbyFmtOut[TRACE_OUTPUT_BUFFER_SIZE];
static PRINTBUFF   printBuffer;
#ifdef _TRACE_ON_
int Trace(const char_t *pszFormat, ...)
{

    int32_t         rc = 0;
    va_list     ap;

    /* Initialise the print buffer */
    printBuffer.pbyBuffer = pbyFmtOut;
    printBuffer.uiCount = 0;
    printBuffer.uiLength = TRACE_OUTPUT_BUFFER_SIZE;
    /* Perform the formatted write */
    va_start(ap, pszFormat);
    rc = fmtOut(pszFormat, hPutChar, (void *)&printBuffer, ap);
    va_end(ap);
    /* Write the data */
    if (printBuffer.uiCount)
    {
        /* Write to our debug output */
        scifOutputDebugString(printBuffer.pbyBuffer, printBuffer.uiCount);
    }
    return (int)rc;
}

int _Trace_(const char_t *pszFormat, ...)
{
    //uint8_t     pbyFmtOut[TRACE_OUTPUT_BUFFER_SIZE];
    //PRINTBUFF   printBuffer;
    int32_t         rc = 0;
    va_list     ap;
    /* Initialise the print buffer */
    printBuffer.pbyBuffer = pbyFmtOut;
    printBuffer.uiCount = 0;
    printBuffer.uiLength = TRACE_OUTPUT_BUFFER_SIZE;
    /* Perform the formatted write */
    va_start(ap, pszFormat);
    rc = fmtOut(pszFormat, _hPutChar_, (void *)&printBuffer, ap);
    va_end(ap);
    /* Write the data */
    if (printBuffer.uiCount)
    {
        /* Write to our debug output */
        scifOutputDebugString(printBuffer.pbyBuffer, printBuffer.uiCount);
    }
    return (int)rc;
}
#endif
/******************************************************************************
End of function Trace
******************************************************************************/

/******************************************************************************
* Function Name: dbgPrintBuffer
* Description  : Function to print a data buffer in hex format
* Arguments    : IN  pbyBuffer - Pointer to the buffer
*                IN  stLength - The length of the buffer
* Return Value : none
******************************************************************************/
#ifdef _TRACE_ON_
void dbgPrintBuffer(uint8_t *pbyBuffer, size_t stLength)
{
   char pchBuffer[(TRACE_DATA_LINE_LENGTH * 3) + 5];
   uint32_t uiLineCount = TRACE_DATA_LINE_LENGTH;
   char *pchHexTable = "0123456789ABCDEF", *pchDest = pchBuffer;
   uint32_t uiCharCount = 0;

   while (stLength--)
   {
        uint8_t    byData = *pbyBuffer++;
        /* Look up the ascii */
        *pchDest++ = pchHexTable[(byData >> 4)];
        *pchDest++ = pchHexTable[(byData & 0x0F)];
        uiLineCount--;
        /* Check for end of line */
        if (!uiLineCount)
        {
            /* Add new line */
            *pchDest++ = '\r';
            *pchDest++ = '\n';
            *pchDest++ = '\0';
            uiLineCount = TRACE_DATA_LINE_LENGTH;
            /* Send to TRACE stream */
            uiCharCount += 4;
            scifOutputDebugString((uint8_t *)pchBuffer, uiCharCount);
            uiCharCount = 0;
            pchDest = pchBuffer;
        }
        else
        {
            /* Add space */
            *pchDest++ = ' ';
            uiCharCount += 3;
        }
   }
   /* Terminate any incomplete line */
   if (uiLineCount != TRACE_DATA_LINE_LENGTH)
   {
        *pchDest++ = '\r';
        *pchDest++ = '\n';
        *pchDest++ = '\0';
        uiCharCount += 2;
        /* Send to TRACE stream */
        scifOutputDebugString((uint8_t *)pchBuffer, uiCharCount);
   }
}
#endif
/******************************************************************************
End of function  dbgPrintBuffer
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/

/******************************************************************************
* Function Name: hPutChar
* Description  : Function to put characters to the debug output stream
* Arguments    : IN iChar - The character to put
*                IN pFileStream - Pointer to the ANSI File Stream to put the
*                                 character to
* Return Value : 0 for success otherwise error code
******************************************************************************/
#ifdef _TRACE_ON_
static int32_t hPutChar(char iChar, void *pFileStream)
{
    PPRINTBUFF pPrintBuff = (PPRINTBUFF)pFileStream;
    /* Write the data if the buffer becomes full */
    if (pPrintBuff->uiCount == pPrintBuff->uiLength)
    {
        /* Write to our debug output */
        scifOutputDebugString(pPrintBuff->pbyBuffer, pPrintBuff->uiCount);
        pPrintBuff->uiCount = 0;
    }
    /* Put the byte in the buffer */
    *(pPrintBuff->pbyBuffer + pPrintBuff->uiCount++) = (uint8_t)iChar;

    return 0;
}
/******************************************************************************
End of function hPutChar
******************************************************************************/

/******************************************************************************
* Function Name: hPutChar
* Description  : Function to put characters to the debug output stream
*                A modified version to make debug statements from lwIP readable
* Arguments    : IN iChar - The character to put
*                IN pFileStream - Pointer to the ANSI File Stream to put the
*                                 character to
* Return Value : 0 for success otherwise error code
******************************************************************************/
static int32_t _hPutChar_(char iChar, void *pFileStream)
{
    PPRINTBUFF pPrintBuff = (PPRINTBUFF)pFileStream;
    /* Write the data if the buffer becomes full */
    if (pPrintBuff->uiCount == pPrintBuff->uiLength)
    {
        /* Write to our debug output */
        scifOutputDebugString(pPrintBuff->pbyBuffer, pPrintBuff->uiCount);
        pPrintBuff->uiCount = 0;
    }
    if ((char)iChar == '\n')
    {
        *(pPrintBuff->pbyBuffer + pPrintBuff->uiCount++) = (uint8_t)'\r';
        if (pPrintBuff->uiCount == pPrintBuff->uiLength)
        {
            /* Write to our debug output */
            scifOutputDebugString(pPrintBuff->pbyBuffer, pPrintBuff->uiCount);
            pPrintBuff->uiCount = 0;
        }
    }
    /* Put the byte in the buffer */
    *(pPrintBuff->pbyBuffer + pPrintBuff->uiCount++) = (uint8_t)iChar;

    return 0;
}

#endif
/******************************************************************************
End of function _hPutChar_
******************************************************************************/

/******************************************************************************
End  Of File
******************************************************************************/

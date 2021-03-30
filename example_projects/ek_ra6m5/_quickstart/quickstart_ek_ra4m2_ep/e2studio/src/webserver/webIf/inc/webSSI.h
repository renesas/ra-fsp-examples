/*******************************************************************************
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
 * and to discontinue the availability of this software. By using this
 * software, you agree to the additional terms and conditions found by
 * accessing the following link:
 * http://www.renesas.com/disclaimer
*******************************************************************************
* Copyright (C) 2018 Renesas Electronics Corporation. All rights reserved.
 *****************************************************************************/
/******************************************************************************
 * @headerfile     webSSI.h
 * @brief          Server Side Include file handler functions
 * @version        1.00
 * @date           27.06.2018
 * H/W Platform    RZ/A1LU
 *****************************************************************************/
 /*****************************************************************************
 * History      : DD.MM.YYYY Ver. Description
 *              : 30.06.2018 1.00 First Release
 *****************************************************************************/
/******************************************************************************
  WARNING!  IN ACCORDANCE WITH THE USER LICENCE THIS CODE MUST NOT BE CONVEYED
  OR REDISTRIBUTED IN COMBINATION WITH ANY SOFTWARE LICENSED UNDER TERMS THE
  SAME AS OR SIMILAR TO THE GNU GENERAL PUBLIC LICENCE
******************************************************************************/
/* Multiple inclusion prevention macro */
#ifndef WEBSSI_H_INCLUDED
#define WEBSSI_H_INCLUDED

/**************************************************************************//**
 * @ingroup R_SW_PKG_93_WEBIF_API
 * @defgroup R_SW_PKG_93_WEBIF_SSI Server Side Interface
 * @brief Server Side Handler Functions
 * 
 * @anchor R_SW_PKG_93_WEB_SSI_API_SUMMARY
 * @par Summary
 *
 * This middleware module contains an API Abstraction for the Server Side
 * Include file handler functions.
 * 
 * @anchor R_SW_PKG_93_WEB_SSI_API_INSTANCES
 * @par Known Implementations:
 * This driver is used in the RZA1LU Software Package.
 * @see RENESAS_APPLICATION_SOFTWARE_PACKAGE
 *
 * @see RENESAS_OS_ABSTRACTION  Renesas OS Abstraction interface
 * @{
 *****************************************************************************/
/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/

#include <stdlib.h>
#include <string.h>

#include "webio.h"
#include "webfs.h"

/******************************************************************************
Typedefs
******************************************************************************/

/* Define a structure to associate the command string with function */
typedef struct _SSIFNASS
{
    uint8_t  *pszSsiFileName;
    PCSVRFN pSsiFunction;
} SSIFNASS,
*PSSIFNASS,
* const PCSSIFNASS;

/* Define a structure to describe a table of SSI handlers */
typedef struct _SSITAB
{
    PSSIFNASS   pSsiList;
    size_t      stNumber;
} SSITAB,
*PSSITAB;

/******************************************************************************
Public Functions
******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief         Function to check the table of embedded SSI functions for the
 *                given file name
 * 
 * @param[in]     pszSsiFileName: Pointer to the SSI file name
 * 
 * @retval        p_func: Pointer to the function to handle the request 
 * @retval        NULL:   On error
 */
extern  PSVRFN ssiGetFunction(int8_t *pszSsiFileName);

#ifdef __cplusplus
}
#endif

#endif /* WEBSSI_H_INCLUDED */
/**************************************************************************//**
 * @} (end addtogroup)
 *****************************************************************************/
/******************************************************************************
End  Of File
******************************************************************************/

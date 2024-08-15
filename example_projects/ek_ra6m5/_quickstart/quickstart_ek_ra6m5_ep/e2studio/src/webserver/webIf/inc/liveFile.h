/***********************************************************************************************************************
* Copyright (c) 2018 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/******************************************************************************
 * @headerfile     liveFile.h
 * @brief          Functions to create "live" files which return 
 *                 measurement data
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
#ifndef LIVEFILE_H_INCLUDED
#define LIVEFILE_H_INCLUDED

/**************************************************************************//**
 * @ingroup R_SW_PKG_93_WEBIF_API 
 * @defgroup R_SW_PKG_93_LIVE_FILE_API Live File API
 * @brief Functions to create "live" files which return measurement data
 *
 * @anchor R_SW_PKG_93_LIVE_FILE_API_SUMMARY
 * @par Summary
 *
 * Live file API allows user to extract measurement data from files. 
 * 
 * @anchor R_SW_PKG_93_LIVE_FILE_API_INSTANCES
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
#include "efsFile.h"

/******************************************************************************
Typedefs
******************************************************************************/

typedef int (*PLIFN)(const int8_t *pszFileName, PEFS pEfsFile);

/* Define a structure to associate the name string with function */
typedef struct _LIFNASS
{
    int8_t  *pszLiveFileName;
    PLIFN   pLiveFunction;
} LIFNASS,
*PLIFNASS,
* const PCLIFNASS;

/* Define a structure to describe a table of live data handlers */
typedef struct _LITAB
{
    PLIFNASS    pLiveFileList;
    size_t      stNumber;
} LITAB,
*PLITAB;

/*******************************************************************************
Public Functions
******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief         Function to open data from a live file
 * 
 * @param[in]     pszFileName: Pointer to the file path and name
 * @param[out]    pEfsFile:    Pointer to the encapsulated file information
 * 
 * @retval        0: For success
 * @retval        ER_CODE:  Error code
 */
extern  EFSERR liveFindFile(int8_t *pszFileName, PEFS pEfsFile);

#ifdef __cplusplus
}
#endif

#endif /* LIVEFILE_H_INCLUDED */
/**************************************************************************//**
 * @} (end addtogroup)
 *****************************************************************************/
/*******************************************************************************
End  Of File
******************************************************************************/

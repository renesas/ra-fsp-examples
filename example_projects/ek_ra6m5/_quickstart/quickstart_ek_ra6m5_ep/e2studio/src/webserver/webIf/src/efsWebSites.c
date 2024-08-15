/***********************************************************************************************************************
* Copyright (c) 2012 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/******************************************************************************
* File Name    : efsWebSites.c
* Version      : 1.00
* Description  : The data that refers to all the embedded file systems used
*                in the application
*******************************************************************************
* History      : DD.MM.YYYY Version Description
*              : 04.04.2011 1.00    First Release
******************************************************************************/

/******************************************************************************
  WARNING!  IN ACCORDANCE WITH THE USER LICENCE THIS CODE MUST NOT BE CONVEYED
  OR REDISTRIBUTED IN COMBINATION WITH ANY SOFTWARE LICENSED UNDER TERMS THE
  SAME AS OR SIMILAR TO THE GNU GENERAL PUBLIC LICENCE
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/

#include "efsWebSites.h"

/*****************************************************************************
External Variables
******************************************************************************/
#ifndef _NO_EMBEDDED_WEB_
/* The web site encapsulated file systems:
   TODO: Document */
extern  size_t gFsWebSite;
/* The web site encapsulated file systems:
   TODO: Document */
extern  size_t gFsWebData;
/* TODO: Add additional file systems here */
#endif
/*****************************************************************************
Constant Data
******************************************************************************/

static const void *ppFileSystemList[] = 
{
    #ifdef _NO_EMBEDDED_WEB_
    /* Knock the website out of the debug build to save download time */
    NULL
    #elif defined(_REDUCED_EMBEDDED_WEB_)
    /* Optimised debug build, keep in the web site but exclude the large
       files to save download time */
    &gFsWebSite,
    // TODO: knock out...
    &gFsWebData
    /* TODO: Add additional file systems here */
    #else
    &gFsWebSite,
    /* TODO: Add additional file systems here */
    #endif
};

const EFSL gEFSL = 
{
    /* Pointer to the array of embedded file system pointers */
    (void**)ppFileSystemList,
    /* The number of embedded file systems in the array */
    (sizeof(ppFileSystemList) / sizeof(void*))
};

/******************************************************************************
End  Of File
******************************************************************************/

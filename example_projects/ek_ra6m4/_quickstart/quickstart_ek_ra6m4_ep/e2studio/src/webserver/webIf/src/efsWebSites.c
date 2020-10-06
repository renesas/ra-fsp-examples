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
//#include "version.h"

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

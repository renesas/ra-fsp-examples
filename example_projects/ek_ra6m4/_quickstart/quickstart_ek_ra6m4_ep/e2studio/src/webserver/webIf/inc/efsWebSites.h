/***********************************************************************************************************************
* Copyright (c) 2018 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/******************************************************************************
 * @headerfile     efsWebSites.h
 * @brief          The data that refers to all the embedded file systems used
 *                 in the application
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
#ifndef EFSWEBSITES_H
#define EFSWEBSITES_H

/**************************************************************************//**
 * @ingroup R_SW_PKG_93_EFS_FILE
 * @{
 *****************************************************************************/
/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/

#include <stdlib.h>

/*****************************************************************************
Typedefs
******************************************************************************/

/* Not to be confused with the Embedded File System Library */
typedef struct _EFSL
{
    
    void    **ppvEfs;  /*!< Pointer to the array of embedded file system pointers */
    
    size_t  stNumberOfElements; /*!< The number of embeddes file systems in the array */
} EFSL,
*PEFSL;

/*****************************************************************************
Constant Data
******************************************************************************/

extern const EFSL gEFSL;

#endif /* EFSWEBSITES_H */
/**************************************************************************//**
 * @} (end addtogroup)
 *****************************************************************************/
/******************************************************************************
End  Of File
******************************************************************************/

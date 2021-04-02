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

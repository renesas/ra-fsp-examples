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
 * @headerfile     Webif.h
 * @brief          Interface functions for John Bartas's Webio
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
#ifndef WEBIOIF_H_INCLUDED
#define WEBIOIF_H_INCLUDED

/**************************************************************************//**
 * @ingroup R_SW_PKG_93_NONOS_MIDDLEWARE
 * @defgroup R_SW_PKG_93_WEBIF_API Webif
 * @brief Web interface API
 *
 * @anchor R_SW_PKG_93_WEB_IF_API_SUMMARY
 * @par Summary
 *
 * The Interface allows the user to start and stop the WebIO service. 
 * 
 * @anchor R_SW_PKG_93_WEB_IF_API_INSTANCES
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

/******************************************************************************
Public Functions
******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief         Function to start the Webio service
 * 
 * @param[in]     usPortNumber:  The port number to use
 * 
 * @retval        True:  If the service was started
 * @retval        False: If service not started
 */
extern  _Bool wsStart(uint16_t usPortNumber);

/**
 * @brief         Function to stop the Webio service
 * @return        None.
 */
extern  void wsStop(void);

#ifdef __cplusplus
}
#endif

#endif /* WEBIOIF_H_INCLUDED */
/**************************************************************************//**
 * @} (end addtogroup)
 *****************************************************************************/
/******************************************************************************
End  Of File
******************************************************************************/

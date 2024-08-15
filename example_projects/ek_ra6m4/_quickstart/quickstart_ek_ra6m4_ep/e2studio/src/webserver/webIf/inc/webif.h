/***********************************************************************************************************************
* Copyright (c) 2018 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

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

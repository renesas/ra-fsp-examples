/*********************************************************************
*                    SEGGER Microcontroller GmbH                     *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2025  SEGGER Microcontroller GmbH                *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V6.50 - Graphical user interface for embedded applications **
emWin is protected by international copyright laws.   Knowledge of the
source code may not be used to write a similar product.  This file may
only  be used  in accordance  with  a license  and should  not be  re-
distributed in any way. We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : GUIDRV_SLinEPD.h
Purpose     : Interface definition for GUIDRV_SLinEPD driver
---------------------------END-OF-HEADER------------------------------
*/

#ifndef GUIDRV_SLINEPD_H
#define GUIDRV_SLINEPD_H

#include "GUI.h"

#if defined(__cplusplus)
extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

/*********************************************************************
*
*       Configuration structure
*/
typedef struct {
  U32 AutoUpdatePeriod;
} CONFIG_SLINEPD;

/*********************************************************************
*
*       Display drivers
*/
//
// Addresses
//
extern const GUI_DEVICE_API GUIDRV_SLinEPD_1_API;
extern const GUI_DEVICE_API GUIDRV_SLinEPD_OY_1_API;
extern const GUI_DEVICE_API GUIDRV_SLinEPD_OX_1_API;
extern const GUI_DEVICE_API GUIDRV_SLinEPD_OXY_1_API;
extern const GUI_DEVICE_API GUIDRV_SLinEPD_OS_1_API;
extern const GUI_DEVICE_API GUIDRV_SLinEPD_OSY_1_API;
extern const GUI_DEVICE_API GUIDRV_SLinEPD_OSX_1_API;
extern const GUI_DEVICE_API GUIDRV_SLinEPD_OSXY_1_API;

//
// Macros to be used in configuration files
//
#if defined(WIN32) && !defined(LCD_SIMCONTROLLER)

  #define GUIDRV_SLINEPD_1      &GUIDRV_Win_API
  #define GUIDRV_SLINEPD_OY_1   &GUIDRV_Win_API
  #define GUIDRV_SLINEPD_OX_1   &GUIDRV_Win_API
  #define GUIDRV_SLINEPD_OXY_1  &GUIDRV_Win_API
  #define GUIDRV_SLINEPD_OS_1   &GUIDRV_Win_API
  #define GUIDRV_SLINEPD_OSY_1  &GUIDRV_Win_API
  #define GUIDRV_SLINEPD_OSX_1  &GUIDRV_Win_API
  #define GUIDRV_SLINEPD_OSXY_1 &GUIDRV_Win_API

#else

  #define GUIDRV_SLINEPD_1      &GUIDRV_SLinEPD_1_API
  #define GUIDRV_SLINEPD_OY_1   &GUIDRV_SLinEPD_OY_1_API
  #define GUIDRV_SLINEPD_OX_1   &GUIDRV_SLinEPD_OX_1_API
  #define GUIDRV_SLINEPD_OXY_1  &GUIDRV_SLinEPD_OXY_1_API
  #define GUIDRV_SLINEPD_OS_1   &GUIDRV_SLinEPD_OS_1_API
  #define GUIDRV_SLINEPD_OSY_1  &GUIDRV_SLinEPD_OSY_1_API
  #define GUIDRV_SLINEPD_OSX_1  &GUIDRV_SLinEPD_OSX_1_API
  #define GUIDRV_SLINEPD_OSXY_1 &GUIDRV_SLinEPD_OSXY_1_API

#endif

/*********************************************************************
*
*       Public routines
*/
void GUIDRV_SLinEPD_Config           (GUI_DEVICE * pDevice, CONFIG_SLINEPD * pConfig);
void GUIDRV_SLinEPD_SetBus8          (GUI_DEVICE * pDevice, GUI_PORT_API * pHW_API);
void GUIDRV_SLinEPD_SetSSD1673       (GUI_DEVICE * pDevice);
void GUIDRV_SLinEPD_SetUC8451        (GUI_DEVICE * pDevice);
void GUIDRV_SLinEPD_EnableFastMode   (int OnOff);
void GUIDRV_SLinEPD_EnablePartialMode(int OnOff);

#if defined(__cplusplus)
}
#endif

#endif

/*************************** End of file ****************************/

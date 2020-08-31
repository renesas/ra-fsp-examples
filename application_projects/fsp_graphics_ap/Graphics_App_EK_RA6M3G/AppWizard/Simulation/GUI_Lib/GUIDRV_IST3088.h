/*********************************************************************
*                    SEGGER Microcontroller GmbH                     *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2020  SEGGER Microcontroller GmbH                *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V6.14 - Graphical user interface for embedded applications **
emWin is protected by international copyright laws.   Knowledge of the
source code may not be used to write a similar product.  This file may
only  be used  in accordance  with  a license  and should  not be  re-
distributed in any way. We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : GUIDRV_IST3088.h
Purpose     : Interface definition for GUIDRV_IST3088 driver
---------------------------END-OF-HEADER------------------------------
*/

#ifndef GUIDRV_IST3088_H
#define GUIDRV_IST3088_H

#if defined(__cplusplus)
extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

/*********************************************************************
*
*       Display drivers
*/
//
// Addresses
//
extern const GUI_DEVICE_API GUIDRV_IST3088_4_API;
extern const GUI_DEVICE_API GUIDRV_IST3088_OY_4_API;
extern const GUI_DEVICE_API GUIDRV_IST3088_OX_4_API;
extern const GUI_DEVICE_API GUIDRV_IST3088_OXY_4_API;
extern const GUI_DEVICE_API GUIDRV_IST3088_OS_4_API;
extern const GUI_DEVICE_API GUIDRV_IST3088_OSY_4_API;
extern const GUI_DEVICE_API GUIDRV_IST3088_OSX_4_API;
extern const GUI_DEVICE_API GUIDRV_IST3088_OSXY_4_API;

//
// Macros to be used in configuration files
//
#if defined(WIN32) && !defined(LCD_SIMCONTROLLER)

  #define GUIDRV_IST3088_4       &GUIDRV_Win_API
  #define GUIDRV_IST3088_OY_4    &GUIDRV_Win_API
  #define GUIDRV_IST3088_OX_4    &GUIDRV_Win_API
  #define GUIDRV_IST3088_OXY_4   &GUIDRV_Win_API
  #define GUIDRV_IST3088_OS_4    &GUIDRV_Win_API
  #define GUIDRV_IST3088_OSY_4   &GUIDRV_Win_API
  #define GUIDRV_IST3088_OSX_4   &GUIDRV_Win_API
  #define GUIDRV_IST3088_OSXY_4  &GUIDRV_Win_API

#else

  #define GUIDRV_IST3088_4       &GUIDRV_IST3088_4_API
  #define GUIDRV_IST3088_OY_4    &GUIDRV_IST3088_OY_4_API
  #define GUIDRV_IST3088_OX_4    &GUIDRV_IST3088_OX_4_API
  #define GUIDRV_IST3088_OXY_4   &GUIDRV_IST3088_OXY_4_API
  #define GUIDRV_IST3088_OS_4    &GUIDRV_IST3088_OS_4_API
  #define GUIDRV_IST3088_OSY_4   &GUIDRV_IST3088_OSY_4_API
  #define GUIDRV_IST3088_OSX_4   &GUIDRV_IST3088_OSX_4_API
  #define GUIDRV_IST3088_OSXY_4  &GUIDRV_IST3088_OSXY_4_API

#endif

/*********************************************************************
*
*       Public routines
*/
void GUIDRV_IST3088_SetBus16(GUI_DEVICE * pDevice, GUI_PORT_API * pHW_API);

#if defined(__cplusplus)
}
#endif

#endif

/*************************** End of file ****************************/

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
File        : GUIDRV_Page1bpp.h
Purpose     : Interface definition for GUIDRV_Page1bpp driver
---------------------------END-OF-HEADER------------------------------
*/

#ifndef GUIDRV_Page1bpp_H
#define GUIDRV_Page1bpp_H

/*********************************************************************
*
*       Display drivers
*/
//
// Addresses
//
extern const GUI_DEVICE_API GUIDRV_Win_API;

extern const GUI_DEVICE_API GUIDRV_Page1bpp_API;

//
// Macros to be used in configuration files
//
#if defined(WIN32) && !defined(LCD_SIMCONTROLLER)

  #define GUIDRV_PAGE1BPP         &GUIDRV_Win_API

#else

  #define GUIDRV_PAGE1BPP         &GUIDRV_Page1bpp_API

#endif

#endif

/*************************** End of file ****************************/

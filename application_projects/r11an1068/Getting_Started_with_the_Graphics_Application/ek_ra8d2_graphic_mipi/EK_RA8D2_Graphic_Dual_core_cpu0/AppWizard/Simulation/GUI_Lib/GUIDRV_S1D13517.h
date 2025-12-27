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
File        : GUIDRV_S1D13517.h
Purpose     : Interface definition for GUIDRV_S1D13517 driver
---------------------------END-OF-HEADER------------------------------
*/

#ifndef GUIDRV_S1D13517_H
#define GUIDRV_S1D13517_H

#if defined(__cplusplus)
extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

/*********************************************************************
*
*       Configuration structure
*/
typedef struct {
  //
  // Driver specific configuration items
  //
  U32 TransColorIndex;
} CONFIG_S1D13517;

/*********************************************************************
*
*       Display drivers
*/
//
// Addresses
//
extern const GUI_DEVICE_API GUIDRV_S1D13517_16C0_API;
extern const GUI_DEVICE_API GUIDRV_S1D13517_OY_16C0_API;
extern const GUI_DEVICE_API GUIDRV_S1D13517_OX_16C0_API;
extern const GUI_DEVICE_API GUIDRV_S1D13517_OXY_16C0_API;
extern const GUI_DEVICE_API GUIDRV_S1D13517_OS_16C0_API;
extern const GUI_DEVICE_API GUIDRV_S1D13517_OSY_16C0_API;
extern const GUI_DEVICE_API GUIDRV_S1D13517_OSX_16C0_API;
extern const GUI_DEVICE_API GUIDRV_S1D13517_OSXY_16C0_API;

//
// Macros to be used in configuration files
//
#if defined(WIN32) && !defined(LCD_SIMCONTROLLER)

  #define GUIDRV_S1D13517_16C0       &GUIDRV_Win_API
  #define GUIDRV_S1D13517_OY_16C0    &GUIDRV_Win_API
  #define GUIDRV_S1D13517_OX_16C0    &GUIDRV_Win_API
  #define GUIDRV_S1D13517_OXY_16C0   &GUIDRV_Win_API
  #define GUIDRV_S1D13517_OS_16C0    &GUIDRV_Win_API
  #define GUIDRV_S1D13517_OSY_16C0   &GUIDRV_Win_API
  #define GUIDRV_S1D13517_OSX_16C0   &GUIDRV_Win_API
  #define GUIDRV_S1D13517_OSXY_16C0  &GUIDRV_Win_API

#else

  #define GUIDRV_S1D13517_16C0       &GUIDRV_S1D13517_16C0_API
  #define GUIDRV_S1D13517_OY_16C0    &GUIDRV_S1D13517_OY_16C0_API
  #define GUIDRV_S1D13517_OX_16C0    &GUIDRV_S1D13517_OX_16C0_API
  #define GUIDRV_S1D13517_OXY_16C0   &GUIDRV_S1D13517_OXY_16C0_API
  #define GUIDRV_S1D13517_OS_16C0    &GUIDRV_S1D13517_OS_16C0_API
  #define GUIDRV_S1D13517_OSY_16C0   &GUIDRV_S1D13517_OSY_16C0_API
  #define GUIDRV_S1D13517_OSX_16C0   &GUIDRV_S1D13517_OSX_16C0_API
  #define GUIDRV_S1D13517_OSXY_16C0  &GUIDRV_S1D13517_OSXY_16C0_API

#endif

/*********************************************************************
*
*       Public routines
*/
#if defined(WIN32) && !defined(LCD_SIMCONTROLLER)
  #define GUIDRV_S1D13517_SetBus16(pDevice, pHW_API)
  #define GUIDRV_S1D13517_Config(pDevice, pConfig)
#else
  void GUIDRV_S1D13517_SetBus16(GUI_DEVICE * pDevice, GUI_PORT_API * pHW_API);
  void GUIDRV_S1D13517_Config  (GUI_DEVICE * pDevice, CONFIG_S1D13517 * pConfig);
#endif

#if defined(__cplusplus)
}
#endif

#endif

/*************************** End of file ****************************/

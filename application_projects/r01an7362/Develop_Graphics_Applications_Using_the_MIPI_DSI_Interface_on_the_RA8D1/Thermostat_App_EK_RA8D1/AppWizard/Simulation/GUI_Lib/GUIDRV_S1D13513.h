/*********************************************************************
*                    SEGGER Microcontroller GmbH                     *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2024  SEGGER Microcontroller GmbH                *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V6.44 - Graphical user interface for embedded applications **
emWin is protected by international copyright laws.   Knowledge of the
source code may not be used to write a similar product.  This file may
only  be used  in accordance  with  a license  and should  not be  re-
distributed in any way. We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : GUIDRV_S1D13513.h
Purpose     : Interface definition for GUIDRV_S1D13513 driver
---------------------------END-OF-HEADER------------------------------
*/

#ifndef GUIDRV_S1D13513_H
#define GUIDRV_S1D13513_H

#if defined(__cplusplus)
extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

#define GUIDRV_S1D13513_USE_MAIN 0
#define GUIDRV_S1D13513_USE_PIP1 1
#define GUIDRV_S1D13513_USE_PIP2 2

/*********************************************************************
*
*       Configuration structure
*/
typedef struct {
  //
  // Driver specific configuration items
  //
  U32 BufferOffset;
  int UseLayer;
  int WriteBufferSize;
} CONFIG_S1D13513;

/*********************************************************************
*
*       Display drivers
*/
//
// Addresses
//
extern const GUI_DEVICE_API GUIDRV_S1D13513_32_API;
extern const GUI_DEVICE_API GUIDRV_S1D13513_OY_32_API;
extern const GUI_DEVICE_API GUIDRV_S1D13513_OX_32_API;
extern const GUI_DEVICE_API GUIDRV_S1D13513_OXY_32_API;
extern const GUI_DEVICE_API GUIDRV_S1D13513_OS_32_API;
extern const GUI_DEVICE_API GUIDRV_S1D13513_OSY_32_API;
extern const GUI_DEVICE_API GUIDRV_S1D13513_OSX_32_API;
extern const GUI_DEVICE_API GUIDRV_S1D13513_OSXY_32_API;

//
// Macros to be used in configuration files
//
#if defined(WIN32) && !defined(LCD_SIMCONTROLLER)

  #define GUIDRV_S1D13513_32       &GUIDRV_Win_API
  #define GUIDRV_S1D13513_OY_32    &GUIDRV_Win_API
  #define GUIDRV_S1D13513_OX_32    &GUIDRV_Win_API
  #define GUIDRV_S1D13513_OXY_32   &GUIDRV_Win_API
  #define GUIDRV_S1D13513_OS_32    &GUIDRV_Win_API
  #define GUIDRV_S1D13513_OSY_32   &GUIDRV_Win_API
  #define GUIDRV_S1D13513_OSX_32   &GUIDRV_Win_API
  #define GUIDRV_S1D13513_OSXY_32  &GUIDRV_Win_API

#else

  #define GUIDRV_S1D13513_32       &GUIDRV_S1D13513_32_API
  #define GUIDRV_S1D13513_OY_32    &GUIDRV_S1D13513_OY_32_API
  #define GUIDRV_S1D13513_OX_32    &GUIDRV_S1D13513_OX_32_API
  #define GUIDRV_S1D13513_OXY_32   &GUIDRV_S1D13513_OXY_32_API
  #define GUIDRV_S1D13513_OS_32    &GUIDRV_S1D13513_OS_32_API
  #define GUIDRV_S1D13513_OSY_32   &GUIDRV_S1D13513_OSY_32_API
  #define GUIDRV_S1D13513_OSX_32   &GUIDRV_S1D13513_OSX_32_API
  #define GUIDRV_S1D13513_OSXY_32  &GUIDRV_S1D13513_OSXY_32_API

#endif

/*********************************************************************
*
*       Public routines
*/
void GUIDRV_S1D13513_Config  (GUI_DEVICE * pDevice, CONFIG_S1D13513 * pConfig);
void GUIDRV_S1D13513_SetBus16(GUI_DEVICE * pDevice, GUI_PORT_API * pHW_API);

#if defined(__cplusplus)
}
#endif

#endif

/*************************** End of file ****************************/

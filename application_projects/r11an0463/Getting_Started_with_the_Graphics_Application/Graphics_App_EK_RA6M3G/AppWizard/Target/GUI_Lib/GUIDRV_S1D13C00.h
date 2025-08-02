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

** emWin V6.48 - Graphical user interface for embedded applications **
emWin is protected by international copyright laws.   Knowledge of the
source code may not be used to write a similar product.  This file may
only  be used  in accordance  with  a license  and should  not be  re-
distributed in any way. We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : GUIDRV_S1D13C00.h
Purpose     : Interface definition for GUIDRV_S1D13C00 driver
---------------------------END-OF-HEADER------------------------------
*/

#ifndef GUIDRV_S1D13C00_H
#define GUIDRV_S1D13C00_H

/*********************************************************************
*
*       Configuration structure
*/
typedef struct {
  void (* pfUpdate)(int yStart, int yEnd);
  void (* pfFillRect) (int x0, int y0, int x1, int y1, U32 PixelIndex);
} CONFIG_S1D13C00;


#define S1D13C00_ADDR_BYTES  (sizeof(U32))

/*********************************************************************
*
*       Display drivers
*/
//
// Addresses
//
extern const GUI_DEVICE_API GUIDRV_Win_API;

extern const GUI_DEVICE_API GUIDRV_S1D13C00_1_0_API;
extern const GUI_DEVICE_API GUIDRV_S1D13C00_1_OX_API;
extern const GUI_DEVICE_API GUIDRV_S1D13C00_1_OY_API;
extern const GUI_DEVICE_API GUIDRV_S1D13C00_1_OXY_API;
extern const GUI_DEVICE_API GUIDRV_S1D13C00_1_OS_API;
extern const GUI_DEVICE_API GUIDRV_S1D13C00_1_OSX_API;
extern const GUI_DEVICE_API GUIDRV_S1D13C00_1_OSY_API;
extern const GUI_DEVICE_API GUIDRV_S1D13C00_1_OSXY_API;

extern const GUI_DEVICE_API GUIDRV_S1D13C00_6_0_API;
extern const GUI_DEVICE_API GUIDRV_S1D13C00_6_OX_API;
extern const GUI_DEVICE_API GUIDRV_S1D13C00_6_OY_API;
extern const GUI_DEVICE_API GUIDRV_S1D13C00_6_OXY_API;
extern const GUI_DEVICE_API GUIDRV_S1D13C00_6_OS_API;
extern const GUI_DEVICE_API GUIDRV_S1D13C00_6_OSX_API;
extern const GUI_DEVICE_API GUIDRV_S1D13C00_6_OSY_API;
extern const GUI_DEVICE_API GUIDRV_S1D13C00_6_OSXY_API;

//
// Macros to be used in configuration files
//
#if defined(WIN32) && !defined(LCD_SIMCONTROLLER)

  #define GUIDRV_S1D13C00_1           &GUIDRV_Win_API
  #define GUIDRV_S1D13C00_6           &GUIDRV_Win_API

#else

  #define GUIDRV_S1D13C00_1_0             &GUIDRV_S1D13C00_1_0_API
  #define GUIDRV_S1D13C00_1_OX            &GUIDRV_S1D13C00_1_OX_API
  #define GUIDRV_S1D13C00_1_OY            &GUIDRV_S1D13C00_1_OY_API
  #define GUIDRV_S1D13C00_1_OXY           &GUIDRV_S1D13C00_1_OXY_API
  #define GUIDRV_S1D13C00_1_OS            &GUIDRV_S1D13C00_1_OS_API
  #define GUIDRV_S1D13C00_1_OSX           &GUIDRV_S1D13C00_1_OSX_API
  #define GUIDRV_S1D13C00_1_OSY           &GUIDRV_S1D13C00_1_OSY_API
  #define GUIDRV_S1D13C00_1_OSXY          &GUIDRV_S1D13C00_1_OSXY_API

  #define GUIDRV_S1D13C00_6_0             &GUIDRV_S1D13C00_6_0_API
  #define GUIDRV_S1D13C00_6_OX            &GUIDRV_S1D13C00_6_OX_API
  #define GUIDRV_S1D13C00_6_OY            &GUIDRV_S1D13C00_6_OY_API
  #define GUIDRV_S1D13C00_6_OXY           &GUIDRV_S1D13C00_6_OXY_API
  #define GUIDRV_S1D13C00_6_OS            &GUIDRV_S1D13C00_6_OS_API
  #define GUIDRV_S1D13C00_6_OSX           &GUIDRV_S1D13C00_6_OSX_API
  #define GUIDRV_S1D13C00_6_OSY           &GUIDRV_S1D13C00_6_OSY_API
  #define GUIDRV_S1D13C00_6_OSXY          &GUIDRV_S1D13C00_6_OSXY_API

#endif

void GUIDRV_S1D13C00_Config         (GUI_DEVICE * pDevice, CONFIG_S1D13C00 * pConfig);
void GUIDRV_S1D13C00_SetBus         (GUI_DEVICE * pDevice, GUI_PORT_API * pPortAPI);
void GUIDRV_S1D13C00_SetBaseAddress (GUI_DEVICE * pDevice, U32 BaseAddr);

#endif

/*************************** End of file ****************************/

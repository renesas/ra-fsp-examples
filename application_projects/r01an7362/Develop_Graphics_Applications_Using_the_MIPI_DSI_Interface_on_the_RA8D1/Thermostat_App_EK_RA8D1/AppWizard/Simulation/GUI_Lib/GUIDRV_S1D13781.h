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
File        : GUIDRV_S1D13781.h
Purpose     : Interface definition for GUIDRV_S1D13781 driver
---------------------------END-OF-HEADER------------------------------
*/

#ifndef GUIDRV_S1D13781_H
#define GUIDRV_S1D13781_H

#define GUIDRV_S1D13781_USE_MAIN 0
#define GUIDRV_S1D13781_USE_PIP1 1
#define GUIDRV_S1D13781_USE_PIP2 2

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
  int WaitUntilVNDP;
} CONFIG_S1D13781;

/*********************************************************************
*
*       Display drivers
*/
//
// Addresses
//
extern const GUI_DEVICE_API GUIDRV_S1D13781_8C0_API;
extern const GUI_DEVICE_API GUIDRV_S1D13781_OXY_8C0_API;
extern const GUI_DEVICE_API GUIDRV_S1D13781_OSY_8C0_API;
extern const GUI_DEVICE_API GUIDRV_S1D13781_OSX_8C0_API;

extern const GUI_DEVICE_API GUIDRV_S1D13781_16C0_API;
extern const GUI_DEVICE_API GUIDRV_S1D13781_OXY_16C0_API;
extern const GUI_DEVICE_API GUIDRV_S1D13781_OSY_16C0_API;
extern const GUI_DEVICE_API GUIDRV_S1D13781_OSX_16C0_API;

//
// Macros to be used in configuration files
//
#if defined(WIN32) && !defined(LCD_SIMCONTROLLER)

  #define GUIDRV_S1D13781_8C0       &GUIDRV_Win_API
  #define GUIDRV_S1D13781_OXY_8C0   &GUIDRV_Win_API
  #define GUIDRV_S1D13781_OSY_8C0   &GUIDRV_Win_API
  #define GUIDRV_S1D13781_OSX_8C0   &GUIDRV_Win_API

  #define GUIDRV_S1D13781_16C0      &GUIDRV_Win_API
  #define GUIDRV_S1D13781_OXY_16C0  &GUIDRV_Win_API
  #define GUIDRV_S1D13781_OSY_16C0  &GUIDRV_Win_API
  #define GUIDRV_S1D13781_OSX_16C0  &GUIDRV_Win_API

#else

  #define GUIDRV_S1D13781_8C0       &GUIDRV_S1D13781_8C0_API
  #define GUIDRV_S1D13781_OXY_8C0   &GUIDRV_S1D13781_OXY_8C0_API
  #define GUIDRV_S1D13781_OSY_8C0   &GUIDRV_S1D13781_OSY_8C0_API
  #define GUIDRV_S1D13781_OSX_8C0   &GUIDRV_S1D13781_OSX_8C0_API

  #define GUIDRV_S1D13781_16C0      &GUIDRV_S1D13781_16C0_API
  #define GUIDRV_S1D13781_OXY_16C0  &GUIDRV_S1D13781_OXY_16C0_API
  #define GUIDRV_S1D13781_OSY_16C0  &GUIDRV_S1D13781_OSY_16C0_API
  #define GUIDRV_S1D13781_OSX_16C0  &GUIDRV_S1D13781_OSX_16C0_API

#endif

/*********************************************************************
*
*       Public routines
*/
#if defined(WIN32) && !defined(LCD_SIMCONTROLLER)
  #define GUIDRV_S1D13781_Config(pDevice, pConfig)
  #define GUIDRV_S1D13781_SetBusSPI(pDevice, pHW_API)
  #define GUIDRV_S1D13781_SetOrientation(Orientation, LayerIndex)
#else
  void GUIDRV_S1D13781_Config        (GUI_DEVICE * pDevice, CONFIG_S1D13781 * pConfig);
  void GUIDRV_S1D13781_SetBusSPI     (GUI_DEVICE * pDevice, GUI_PORT_API * pHW_API);
  int  GUIDRV_S1D13781_SetOrientation(int Orientation, int LayerIndex);
#endif

#endif

/*************************** End of file ****************************/

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
File        : GUIDRV_SLin.h
Purpose     : Interface definition for GUIDRV_SLin driver
---------------------------END-OF-HEADER------------------------------
*/

#ifndef GUIDRV_SLIN_H
#define GUIDRV_SLIN_H

#if defined(__cplusplus)
extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

/*********************************************************************
*
*       Configuration structure
*/
typedef struct {
  //
  // Function pointers of hardware access routines
  //
  //HW_API_SLIN HW_API;
  //
  // Driver specific configuration items
  //
  int FirstSEG;
  int FirstCOM;
  int UseCache;
  int UseMirror;   // Only used for SSD1848
  int CheckBusy;
  U8  UseDualScan; // Used for T6963
} CONFIG_SLIN;

/*********************************************************************
*
*       Display drivers
*/
//
// Addresses
//
extern const GUI_DEVICE_API GUIDRV_SLin_1_API;
extern const GUI_DEVICE_API GUIDRV_SLin_OY_1_API;
extern const GUI_DEVICE_API GUIDRV_SLin_OX_1_API;
extern const GUI_DEVICE_API GUIDRV_SLin_OXY_1_API;
extern const GUI_DEVICE_API GUIDRV_SLin_OS_1_API;
extern const GUI_DEVICE_API GUIDRV_SLin_OSY_1_API;
extern const GUI_DEVICE_API GUIDRV_SLin_OSX_1_API;
extern const GUI_DEVICE_API GUIDRV_SLin_OSXY_1_API;

extern const GUI_DEVICE_API GUIDRV_SLin_2_API;
extern const GUI_DEVICE_API GUIDRV_SLin_OY_2_API;
extern const GUI_DEVICE_API GUIDRV_SLin_OX_2_API;
extern const GUI_DEVICE_API GUIDRV_SLin_OXY_2_API;
extern const GUI_DEVICE_API GUIDRV_SLin_OS_2_API;
extern const GUI_DEVICE_API GUIDRV_SLin_OSY_2_API;
extern const GUI_DEVICE_API GUIDRV_SLin_OSX_2_API;
extern const GUI_DEVICE_API GUIDRV_SLin_OSXY_2_API;

extern const GUI_DEVICE_API GUIDRV_SLin_4_API;
extern const GUI_DEVICE_API GUIDRV_SLin_OY_4_API;
extern const GUI_DEVICE_API GUIDRV_SLin_OX_4_API;
extern const GUI_DEVICE_API GUIDRV_SLin_OXY_4_API;
extern const GUI_DEVICE_API GUIDRV_SLin_OS_4_API;
extern const GUI_DEVICE_API GUIDRV_SLin_OSY_4_API;
extern const GUI_DEVICE_API GUIDRV_SLin_OSX_4_API;
extern const GUI_DEVICE_API GUIDRV_SLin_OSXY_4_API;

//
// Macros to be used in configuration files
//
#if defined(WIN32) && !defined(LCD_SIMCONTROLLER)

  #define GUIDRV_SLIN_1       &GUIDRV_Win_API
  #define GUIDRV_SLIN_OY_1    &GUIDRV_Win_API
  #define GUIDRV_SLIN_OX_1    &GUIDRV_Win_API
  #define GUIDRV_SLIN_OXY_1   &GUIDRV_Win_API
  #define GUIDRV_SLIN_OS_1    &GUIDRV_Win_API
  #define GUIDRV_SLIN_OSY_1   &GUIDRV_Win_API
  #define GUIDRV_SLIN_OSX_1   &GUIDRV_Win_API
  #define GUIDRV_SLIN_OSXY_1  &GUIDRV_Win_API

  #define GUIDRV_SLIN_2       &GUIDRV_Win_API
  #define GUIDRV_SLIN_OY_2    &GUIDRV_Win_API
  #define GUIDRV_SLIN_OX_2    &GUIDRV_Win_API
  #define GUIDRV_SLIN_OXY_2   &GUIDRV_Win_API
  #define GUIDRV_SLIN_OS_2    &GUIDRV_Win_API
  #define GUIDRV_SLIN_OSY_2   &GUIDRV_Win_API
  #define GUIDRV_SLIN_OSX_2   &GUIDRV_Win_API
  #define GUIDRV_SLIN_OSXY_2  &GUIDRV_Win_API

  #define GUIDRV_SLIN_4       &GUIDRV_Win_API
  #define GUIDRV_SLIN_OY_4    &GUIDRV_Win_API
  #define GUIDRV_SLIN_OX_4    &GUIDRV_Win_API
  #define GUIDRV_SLIN_OXY_4   &GUIDRV_Win_API
  #define GUIDRV_SLIN_OS_4    &GUIDRV_Win_API
  #define GUIDRV_SLIN_OSY_4   &GUIDRV_Win_API
  #define GUIDRV_SLIN_OSX_4   &GUIDRV_Win_API
  #define GUIDRV_SLIN_OSXY_4  &GUIDRV_Win_API

#else

  #define GUIDRV_SLIN_1       &GUIDRV_SLin_1_API
  #define GUIDRV_SLIN_OY_1    &GUIDRV_SLin_OY_1_API
  #define GUIDRV_SLIN_OX_1    &GUIDRV_SLin_OX_1_API
  #define GUIDRV_SLIN_OXY_1   &GUIDRV_SLin_OXY_1_API
  #define GUIDRV_SLIN_OS_1    &GUIDRV_SLin_OS_1_API
  #define GUIDRV_SLIN_OSY_1   &GUIDRV_SLin_OSY_1_API
  #define GUIDRV_SLIN_OSX_1   &GUIDRV_SLin_OSX_1_API
  #define GUIDRV_SLIN_OSXY_1  &GUIDRV_SLin_OSXY_1_API

  #define GUIDRV_SLIN_2       &GUIDRV_SLin_2_API
  #define GUIDRV_SLIN_OY_2    &GUIDRV_SLin_OY_2_API
  #define GUIDRV_SLIN_OX_2    &GUIDRV_SLin_OX_2_API
  #define GUIDRV_SLIN_OXY_2   &GUIDRV_SLin_OXY_2_API
  #define GUIDRV_SLIN_OS_2    &GUIDRV_SLin_OS_2_API
  #define GUIDRV_SLIN_OSY_2   &GUIDRV_SLin_OSY_2_API
  #define GUIDRV_SLIN_OSX_2   &GUIDRV_SLin_OSX_2_API
  #define GUIDRV_SLIN_OSXY_2  &GUIDRV_SLin_OSXY_2_API

  #define GUIDRV_SLIN_4       &GUIDRV_SLin_4_API
  #define GUIDRV_SLIN_OY_4    &GUIDRV_SLin_OY_4_API
  #define GUIDRV_SLIN_OX_4    &GUIDRV_SLin_OX_4_API
  #define GUIDRV_SLIN_OXY_4   &GUIDRV_SLin_OXY_4_API
  #define GUIDRV_SLIN_OS_4    &GUIDRV_SLin_OS_4_API
  #define GUIDRV_SLIN_OSY_4   &GUIDRV_SLin_OSY_4_API
  #define GUIDRV_SLIN_OSX_4   &GUIDRV_SLin_OSX_4_API
  #define GUIDRV_SLIN_OSXY_4  &GUIDRV_SLin_OSXY_4_API

#endif

/*********************************************************************
*
*       Public routines
*/
#if defined(WIN32) && !defined(LCD_SIMCONTROLLER)

  #define GUIDRV_SLin_Config(pDevice, pConfig)
  #define GUIDRV_SLin_SetBus8(pDevice, pHW_API)
  #define GUIDRV_SLin_SetS1D13700(pDevice)
  #define GUIDRV_SLin_SetSSD1848(pDevice)
  #define GUIDRV_SLin_SetT6963(pDevice)
  #define GUIDRV_SLin_SetUC1617(pDevice)

#else

  void GUIDRV_SLin_Config     (GUI_DEVICE * pDevice, CONFIG_SLIN  * pConfig);
  void GUIDRV_SLin_SetBus8    (GUI_DEVICE * pDevice, GUI_PORT_API * pHW_API);
  void GUIDRV_SLin_SetS1D13700(GUI_DEVICE * pDevice);
  void GUIDRV_SLin_SetSSD1325 (GUI_DEVICE * pDevice);
  void GUIDRV_SLin_SetSSD1848 (GUI_DEVICE * pDevice);
  void GUIDRV_SLin_SetT6963   (GUI_DEVICE * pDevice);
  void GUIDRV_SLin_SetUC1617  (GUI_DEVICE * pDevice);

#endif

#if defined(__cplusplus)
}
#endif

#endif

/*************************** End of file ****************************/

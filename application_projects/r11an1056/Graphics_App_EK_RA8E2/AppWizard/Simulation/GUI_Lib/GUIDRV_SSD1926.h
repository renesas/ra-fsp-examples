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
File        : GUIDRV_SSD1926.h
Purpose     : Interface definition for GUIDRV_SSD1926 driver
---------------------------END-OF-HEADER------------------------------
*/

#ifndef GUIDRV_SSD1926_H
#define GUIDRV_SSD1926_H

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
  int FirstSEG;
  int FirstCOM;
  int UseCache;
} CONFIG_SSD1926;

/*********************************************************************
*
*       Display drivers
*/
//
// Addresses
//
extern const GUI_DEVICE_API GUIDRV_SSD1926_8_API;
extern const GUI_DEVICE_API GUIDRV_SSD1926_OY_8_API;
extern const GUI_DEVICE_API GUIDRV_SSD1926_OX_8_API;
extern const GUI_DEVICE_API GUIDRV_SSD1926_OXY_8_API;
extern const GUI_DEVICE_API GUIDRV_SSD1926_OS_8_API;
extern const GUI_DEVICE_API GUIDRV_SSD1926_OSY_8_API;
extern const GUI_DEVICE_API GUIDRV_SSD1926_OSX_8_API;
extern const GUI_DEVICE_API GUIDRV_SSD1926_OSXY_8_API;

//
// Macros to be used in configuration files
//
#if defined(WIN32) && !defined(LCD_SIMCONTROLLER)

  #define GUIDRV_SSD1926_8       &GUIDRV_Win_API
  #define GUIDRV_SSD1926_OY_8    &GUIDRV_Win_API
  #define GUIDRV_SSD1926_OX_8    &GUIDRV_Win_API
  #define GUIDRV_SSD1926_OXY_8   &GUIDRV_Win_API
  #define GUIDRV_SSD1926_OS_8    &GUIDRV_Win_API
  #define GUIDRV_SSD1926_OSY_8   &GUIDRV_Win_API
  #define GUIDRV_SSD1926_OSX_8   &GUIDRV_Win_API
  #define GUIDRV_SSD1926_OSXY_8  &GUIDRV_Win_API

#else

  #define GUIDRV_SSD1926_8       &GUIDRV_SSD1926_8_API
  #define GUIDRV_SSD1926_OY_8    &GUIDRV_SSD1926_OY_8_API
  #define GUIDRV_SSD1926_OX_8    &GUIDRV_SSD1926_OX_8_API
  #define GUIDRV_SSD1926_OXY_8   &GUIDRV_SSD1926_OXY_8_API
  #define GUIDRV_SSD1926_OS_8    &GUIDRV_SSD1926_OS_8_API
  #define GUIDRV_SSD1926_OSY_8   &GUIDRV_SSD1926_OSY_8_API
  #define GUIDRV_SSD1926_OSX_8   &GUIDRV_SSD1926_OSX_8_API
  #define GUIDRV_SSD1926_OSXY_8  &GUIDRV_SSD1926_OSXY_8_API

#endif

/*********************************************************************
*
*       Public routines
*/
void GUIDRV_SSD1926_Config  (GUI_DEVICE * pDevice, CONFIG_SSD1926 * pConfig);
void GUIDRV_SSD1926_SetBus16(GUI_DEVICE * pDevice, GUI_PORT_API * pHW_API);

#if defined(__cplusplus)
}
#endif

#endif

/*************************** End of file ****************************/

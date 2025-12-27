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
File        : GUIDRV_SSD1322.h
Purpose     : Interface definition for GUIDRV_SSD1322 driver
---------------------------END-OF-HEADER------------------------------
*/

#ifndef GUIDRV_SSD1322_H
#define GUIDRV_SSD1322_H

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
} CONFIG_SSD1322;

/*********************************************************************
*
*       Interface
*/
void GUIDRV_SSD1322_Config (GUI_DEVICE * pDevice, CONFIG_SSD1322 * pConfig);
void GUIDRV_SSD1322_SetBus8(GUI_DEVICE * pDevice, GUI_PORT_API * pHW_API);

/*********************************************************************
*
*       Display drivers
*/
//
// Addresses
//
extern const GUI_DEVICE_API GUIDRV_Win_API;

extern const GUI_DEVICE_API GUIDRV_SSD1322_API;

//
// Macros to be used in configuration files
//
#if defined(WIN32) && !defined(LCD_SIMCONTROLLER)

  #define GUIDRV_SSD1322 &GUIDRV_Win_API
  #define GUIDRV_SSD1322_Config
  #define GUIDRV_SSD1322_SetBus8

#else

  #define GUIDRV_SSD1322 &GUIDRV_SSD1322_API

#endif

#if defined(__cplusplus)
}
#endif

#endif

/*************************** End of file ****************************/

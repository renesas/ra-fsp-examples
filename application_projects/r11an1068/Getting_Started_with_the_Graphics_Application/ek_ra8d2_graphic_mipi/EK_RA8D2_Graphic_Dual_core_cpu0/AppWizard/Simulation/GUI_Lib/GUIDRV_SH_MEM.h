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
File        : GUIDRV_SH_MEM.h
Purpose     : Interface definition for GUIDRV_SH_MEM driver
---------------------------END-OF-HEADER------------------------------
*/

#ifndef GUIDRV_SH_MEM_H
#define GUIDRV_SH_MEM_H

/*********************************************************************
*
*       Display drivers
*/
//
// Addresses
//
extern const GUI_DEVICE_API GUIDRV_Win_API;

extern const GUI_DEVICE_API GUIDRV_SH_MEM_API;
extern const GUI_DEVICE_API GUIDRV_SH_MEM_API_OXY;
extern const GUI_DEVICE_API GUIDRV_SH_MEM_API_OSX;
extern const GUI_DEVICE_API GUIDRV_SH_MEM_API_OSY;

extern const GUI_DEVICE_API GUIDRV_SH_MEM_3_API;
extern const GUI_DEVICE_API GUIDRV_SH_MEM_3_API_OXY;
extern const GUI_DEVICE_API GUIDRV_SH_MEM_3_API_OSX;
extern const GUI_DEVICE_API GUIDRV_SH_MEM_3_API_OSY;

//
// Macros to be used in configuration files
//
#if defined(WIN32) && !defined(LCD_SIMCONTROLLER)

  #define GUIDRV_SH_MEM            &GUIDRV_Win_API
  #define GUIDRV_SH_MEM_OXY        &GUIDRV_Win_API
  #define GUIDRV_SH_MEM_OSX        &GUIDRV_Win_API
  #define GUIDRV_SH_MEM_OSY        &GUIDRV_Win_API

  #define GUIDRV_SH_MEM_3          &GUIDRV_Win_API
  #define GUIDRV_SH_MEM_3_OXY      &GUIDRV_Win_API
  #define GUIDRV_SH_MEM_3_OSX      &GUIDRV_Win_API
  #define GUIDRV_SH_MEM_3_OSY      &GUIDRV_Win_API

#else

  #define GUIDRV_SH_MEM            &GUIDRV_SH_MEM_API
  #define GUIDRV_SH_MEM_OXY        &GUIDRV_SH_MEM_API_OXY
  #define GUIDRV_SH_MEM_OSX        &GUIDRV_SH_MEM_API_OSX
  #define GUIDRV_SH_MEM_OSY        &GUIDRV_SH_MEM_API_OSY

  #define GUIDRV_SH_MEM_3          &GUIDRV_SH_MEM_3_API
  #define GUIDRV_SH_MEM_3_OXY      &GUIDRV_SH_MEM_3_API_OXY
  #define GUIDRV_SH_MEM_3_OSX      &GUIDRV_SH_MEM_3_API_OSX
  #define GUIDRV_SH_MEM_3_OSY      &GUIDRV_SH_MEM_3_API_OSY

#endif

#define GUIDRV_SH_MEM_8BITMODE  0
#define GUIDRV_SH_MEM_9BITMODE  1
#define GUIDRV_SH_MEM_10BITMODE 2

typedef struct {
  unsigned Period;           // Period used for toggling VCOM
  unsigned ExtMode;          // Setting of EXTMODE configuration pin
  unsigned BitMode;          // 8- or 10-bit line addressing
  unsigned SendEnd;          // Send end command 0 - no, 1 - yes
  void (* pfToggleVCOM)(void);
} CONFIG_SH_MEM;

/*********************************************************************
*
*       Prototypes
*
**********************************************************************
*/
void GUIDRV_SH_MEM_SetBus8  (GUI_DEVICE * pDevice, GUI_PORT_API * pHW_API);
void GUIDRV_SH_MEM_Config   (GUI_DEVICE * pDevice, CONFIG_SH_MEM * pConfig);
void GUIDRV_SH_MEM_3_SetBus8(GUI_DEVICE * pDevice, GUI_PORT_API * pHW_API);
void GUIDRV_SH_MEM_3_Config (GUI_DEVICE * pDevice, CONFIG_SH_MEM * pConfig);

#endif

/*************************** End of file ****************************/

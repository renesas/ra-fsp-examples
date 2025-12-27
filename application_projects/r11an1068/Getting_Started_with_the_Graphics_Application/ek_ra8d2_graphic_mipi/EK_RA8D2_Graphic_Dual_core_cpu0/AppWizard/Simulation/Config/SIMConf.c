/*********************************************************************
*                     SEGGER Microcontroller GmbH                    *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2019  SEGGER Microcontroller GmbH                *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************
----------------------------------------------------------------------
File        : LCDConf.h
Purpose     : SEGGER internal standard configuration file
----------------------------------------------------------------------
*/

#ifdef WIN32

#include <windows.h>
#include <stdio.h>

#include "LCD_SIM.h"
#include "GUI_SIM_Win32.h"

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       _cbInfo
*
* Purpose:
*   Called during the initialization process in order to get the
*   window handles of the simulation.
*/
#ifdef SPY
static int _cbInfo(SIM_GUI_INFO * pInfo) {
  HWND hDT;
  RECT RectDT;
  RECT RectApp;
  int  xSizeApp, ySizeApp;
  int  xSizeDT, ySizeDT;
  int  xPosApp, yPosApp;
  
  hDT = GetDesktopWindow();
  GetWindowRect(hDT, &RectDT);
  xSizeDT = RectDT.right  - RectDT.left;
  ySizeDT = RectDT.bottom - RectDT.top;
  GetWindowRect(SIM_GUI_hWndMain, &RectApp);
  xSizeApp = RectApp.right  - RectApp.left;
  ySizeApp = RectApp.bottom - RectApp.top;
  xPosApp = ((xSizeDT - xSizeApp) * 2) / 3;
  yPosApp = ((ySizeDT - ySizeApp) * 1) / 3;
  SetWindowPos(SIM_GUI_hWndMain, HWND_TOPMOST, xPosApp, yPosApp, xSizeApp, ySizeApp, 0);
  return 0;
}
#endif

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       SIM_X_Config
*/
void SIM_X_Config() {
#ifdef SPY
  SIM_GUI_SetCallback(_cbInfo);
#endif
}

#else

void SIMConf_C(void); // Avoid empty object files
void SIMConf_C(void) {}

#endif

/*************************** End of file ****************************/

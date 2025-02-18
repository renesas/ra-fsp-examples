/*********************************************************************
*                     SEGGER Microcontroller GmbH                    *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2022  SEGGER Microcontroller GmbH                *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************
----------------------------------------------------------------------
File        : LCDConf.c
Purpose     : ...
---------------------------END-OF-HEADER------------------------------
*/

#include "GUI.h"
#include "AppWizard.h"

#include "GUIDRV_Win32R.h"

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
static int _xSize, _ySize;

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       _ConfigDriver
*/
static void _ConfigDriver(GUI_DEVICE * pDevice, int Index, int LayerIndex) {
  GUI_USE_PARA(pDevice);
  GUI_USE_PARA(Index);
  GUI_USE_PARA(LayerIndex);

  //
  // Set size of 1st layer
  //
  if (LCD_GetSwapXY()) {
    LCD_SetSizeEx (0, _ySize, _xSize);
    LCD_SetVSizeEx(0, _ySize, _xSize);
  } else {
    LCD_SetSizeEx (0, _xSize, _ySize);
    LCD_SetVSizeEx(0, _xSize, _ySize);
  }
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       LCD_X_Config
*/
void LCD_X_Config(void) {
  APPW_X_Config();
  //
  // Get size of display
  //
  _xSize = LCD_GetXSize();
  _ySize = LCD_GetYSize();
  //
  // Create additional drivers and add them to the display rotation module
  //
  LCD_ROTATE_AddDriverExOrientation(GUIDRV_WIN32R_OSY, 0, GUI_ROTATION_CCW);
  LCD_ROTATE_AddDriverExOrientation(GUIDRV_WIN32R_OXY, 0, GUI_ROTATION_180);
  LCD_ROTATE_AddDriverExOrientation(GUIDRV_WIN32R_OSX, 0, GUI_ROTATION_CW);
  //
  // Set callback function to be used for layer 0
  //
  LCD_ROTATE_SetCallback(_ConfigDriver, 0);
}

/*********************************************************************
*
*       LCD_X_DisplayDriver
*/
int LCD_X_DisplayDriver(unsigned LayerIndex, unsigned Cmd, void * pData) {
  return 0;
}

/*************************** End of file ****************************/

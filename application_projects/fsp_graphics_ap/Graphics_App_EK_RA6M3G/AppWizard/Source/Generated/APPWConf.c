/*********************************************************************
*                     SEGGER Microcontroller GmbH                    *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2020  SEGGER Microcontroller GmbH                *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************
----------------------------------------------------------------------
File        : APPWConf.c
Purpose     : Generated file do NOT edit!
---------------------------END-OF-HEADER------------------------------
*/

#include "AppWizard.h"
#include "Resource.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define XSIZE_PHYS 480
#define YSIZE_PHYS 272
#define COLOR_CONVERSION GUICC_M565
#define DISPLAY_DRIVER GUIDRV_WIN32
#define NUM_BUFFERS   2

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
/*********************************************************************
*
*       _apRootList
*/
static APPW_ROOT_INFO * _apRootList[] = {
  &ID_SCREEN_MAIN_RootInfo,
  &ID_SCREEN_LOG_RootInfo,
};

/*********************************************************************
*
*       _NumScreens
*/
static unsigned _NumScreens = GUI_COUNTOF(_apRootList);

/*********************************************************************
*
*       _aVarList
*/
static APPW_VAR_OBJECT _aVarList[] = {
  { ID_VAR_TARGET_TEMP, 0, 0 },
  { ID_VAR_TIME_UPDATE, 0, 0 },
  { ID_VAR_DARKMODE, 0, 0 },
  { ID_VAR_BACKLIGHT, 0, 0 },
  { ID_VAR_TEXT_COLOR_CHANGE, 0, 0 },
};

/*********************************************************************
*
*       _NumVars
*/
static unsigned _NumVars = GUI_COUNTOF(_aVarList);

static const char * _apLang[] = {
  (const char *)acAPPW_Language_0,
};

static const APPW_TEXT_INIT _TextInit = {
  _apLang,
  GUI_COUNTOF(_apLang),
};

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       _InitText
*/
static void _InitText(void) {
  APPW_TextInitMem(&_TextInit);
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       APPW_X_Setup
*/
void APPW_X_Setup(void) {
  APPW_SetpfInitText(_InitText);
  APPW_X_FS_Init();
  APPW_SetData(_apRootList, _NumScreens, _aVarList, _NumVars);
}

/*********************************************************************
*
*       APPW_X_Config
*/
#ifdef WIN32
void APPW_X_Config(void) {
  GUI_MULTIBUF_Config(NUM_BUFFERS);
  GUI_DEVICE_CreateAndLink(DISPLAY_DRIVER, COLOR_CONVERSION, 0, 0);
  if (LCD_GetSwapXY()) {
    LCD_SetSizeEx (0, YSIZE_PHYS, XSIZE_PHYS);
    LCD_SetVSizeEx(0, YSIZE_PHYS, XSIZE_PHYS);
  } else {
    LCD_SetSizeEx (0, XSIZE_PHYS, YSIZE_PHYS);
    LCD_SetVSizeEx(0, XSIZE_PHYS, YSIZE_PHYS);
  }
}
#endif

/*************************** End of file ****************************/

/*********************************************************************
*                     SEGGER Microcontroller GmbH                    *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2024  SEGGER Microcontroller GmbH                *
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

#ifdef WIN32
  #include "GUIDRV_Win32R.h"
#endif

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define XSIZE_PHYS       480
#define YSIZE_PHYS       854
#define COLOR_CONVERSION GUICC_M565
#define DISPLAY_DRIVER   GUIDRV_WIN32R
#define NUM_BUFFERS      2

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
/*********************************************************************
*
*       Multibuffering
*/
static U8 _MultibufEnable = 1;

/*********************************************************************
*
*       _ShowMissingCharacters
*/
static U8 _ShowMissingCharacters = 0;

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
  GUI_CONST_STORAGE APPW_TEXT_INIT * pTextInit;

  APPW__GetTextInit(&pTextInit);
  APPW_TextInitMem(pTextInit);
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
  APPW_ROOT_INFO         ** ppRootInfo;
  APPW_VAR_OBJECT         * paVarList;
  const APPW_SCROLLER_DEF * paScrollerDef;
  APPW_DRAWING_ITEM      ** ppDrawingList;
  int                       NumScreens;
  int                       NumVars;
  int                       NumScrollers;
  int                       NumDrawings;
  int                       CreateFlags;

  APPW_SetpfInitText(_InitText);
  APPW_X_FS_Init();
  APPW_MULTIBUF_Enable(_MultibufEnable);
  APPW__GetResource(&ppRootInfo,
                    &NumScreens,
                    &paVarList,
                    &NumVars,
                    &paScrollerDef,
                    &NumScrollers,
                    &ppDrawingList,
                    &NumDrawings,
                    &CreateFlags);
  WM_SetCreateFlags(CreateFlags);
  APPW_SetData(ppRootInfo, NumScreens, paVarList, NumVars, paScrollerDef, NumScrollers, ppDrawingList, NumDrawings);
  APPW_SetSupportScroller(0);
  GUI_ShowMissingCharacters(_ShowMissingCharacters);
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

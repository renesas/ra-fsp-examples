/*********************************************************************
*                     SEGGER Microcontroller GmbH                    *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2025  SEGGER Microcontroller GmbH                *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************
----------------------------------------------------------------------
File        : Resource.c
Purpose     : Generated file do NOT edit!
---------------------------END-OF-HEADER------------------------------
*/

#include "Resource.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define _appDrawing      NULL
#define _NumDrawings     0
#define _aScrollerList   NULL
#define _NumScrollers    0
#define _CreateFlags     0

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
  { ID_VAR_TARGET_TEMP, 0, 0, NULL },
  { ID_VAR_TIME_UPDATE, 0, 0, NULL },
  { ID_VAR_DARKMODE, 0, 0, NULL },
  { ID_VAR_BACKLIGHT, 0, 0, NULL },
  { ID_VAR_TEXT_COLOR_CHANGE, 0, 0, NULL },
};

/*********************************************************************
*
*       _NumVars
*/
static unsigned _NumVars = GUI_COUNTOF(_aVarList);

/*********************************************************************
*
*       _aID_ANIM_CLOUDY_MOTION_Items
*/
static GUI_CONST_STORAGE APPW_ANIM_ITEM _aID_ANIM_CLOUDY_MOTION_Items[] = {
  { ANIM_LINEAR,
    { 0x0000, 0x7fff },
    { { 0x18000000, -178, ATOM_SCREEN_GEO, ATOM_DETAIL_X0 },
      { 0x18000000, 178, ATOM_SCREEN_GEO, ATOM_DETAIL_X0 },
      { 0x18000818, 0, ATOM_OBJECT_GEO, ATOM_DETAIL_X0 }
    }
  }
};

/*********************************************************************
*
*       ID_ANIM_CLOUDY_MOTION_Data
*/
GUI_CONST_STORAGE APPW_ANIM_DATA ID_ANIM_CLOUDY_MOTION_Data = { ID_ANIM_CLOUDY_MOTION, 4000, 1, 1, GUI_COUNTOF(_aID_ANIM_CLOUDY_MOTION_Items), _aID_ANIM_CLOUDY_MOTION_Items };

/*********************************************************************
*
*       _apLang
*/
static GUI_CONST_STORAGE char * _apLang[] = {
  (GUI_CONST_STORAGE char *)acAPPW_Language_0,
};

/*********************************************************************
*
*       _TextInit
*/
static GUI_CONST_STORAGE APPW_TEXT_INIT _TextInit = {
  (const char **)_apLang,
  GUI_COUNTOF(_apLang),
};

/*********************************************************************
*
*       Private data
*
**********************************************************************
*/
/*********************************************************************
*
*       Font data
*/
APPW_FONT APPW__aFont[2];

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       APPW__GetTextInit
*/
void APPW__GetTextInit(GUI_CONST_STORAGE APPW_TEXT_INIT ** ppTextInit) {
  *ppTextInit = &_TextInit;
}

/*********************************************************************
*
*       APPW__GetResource
*/
void APPW__GetResource(APPW_ROOT_INFO         *** pppRootInfo,    int * pNumScreens,
                       APPW_VAR_OBJECT         ** ppaVarList,     int * pNumVars,
                       const APPW_SCROLLER_DEF ** ppaScrollerDef, int * pNumScrollers,
                       APPW_DRAWING_ITEM      *** pppDrawingList, int * pNumDrawings,
                                                                  int * pCreateFlags) {
  *pppRootInfo    = _apRootList;
  *ppaVarList     = _aVarList;
  *ppaScrollerDef = _aScrollerList;
  *pppDrawingList = (APPW_DRAWING_ITEM **)_appDrawing;
  *pNumScreens    = _NumScreens;
  *pNumVars       = _NumVars;
  *pNumScrollers  = _NumScrollers;
  *pNumDrawings   = _NumDrawings;
  *pCreateFlags   = _CreateFlags;
}

/*************************** End of file ****************************/

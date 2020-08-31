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
File        : ID_SCREEN_LOG.c
Purpose     : Generated file do NOT edit!
---------------------------END-OF-HEADER------------------------------
*/

#include "Resource.h"
#include "ID_SCREEN_LOG.h"

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
/*********************************************************************
*
*       _aCreate
*/
static APPW_CREATE_ITEM _aCreate[] = {
  { WM_OBJECT_WINDOW_Create,
    ID_SCREEN_LOG, 0,
    { { { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
      },
      0, 0, 0, 0, 0, 0
    },
  },
  { WM_OBJECT_IMAGE_Create,
    ID_IMAGE_BRG, ID_SCREEN_LOG,
    { { { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      480, 272, 0, 0, 0, 0
    },
  },
  { WM_OBJECT_TEXT_Create,
    ID_TEXT_APP_EVENT, ID_SCREEN_LOG,
    { { { DISPOSE_MODE_REL_PARENT, 11, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 61, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      212, 32, 0, 0, 0, 0
    },
  },
  { WM_OBJECT_BUTTON_Create,
    ID_BUTTON_BACK, ID_SCREEN_LOG,
    { { { DISPOSE_MODE_REL_PARENT, 2, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 2, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      337, 54, 0, 0, 0, 0
    },
  },
  { WM_OBJECT_TEXT_Create,
    ID_TEXT_0GO_BACK, ID_SCREEN_LOG,
    { { { DISPOSE_MODE_REL_PARENT, 350, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 4, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      123, 48, 0, 0, 0, 0
    },
  },
  { WM_OBJECT_WINDOW_Create,
    ID_WINDOW_CONTROL, ID_SCREEN_LOG,
    { { { DISPOSE_MODE_REL_PARENT, 231, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 54, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 3, 0, 0 },
      },
      0, 0, 0, 0, 0, 0
    },
  },
  { WM_OBJECT_BOX_Create,
    ID_BOX_CONTROL, ID_WINDOW_CONTROL,
    { { { DISPOSE_MODE_REL_PARENT, 9, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 7, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 7, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 8, 0, 0 },
      },
      0, 0, 0, 0, 0, 0
    },
  },
  { WM_OBJECT_SLIDER_Create,
    ID_SLIDER_BACKLIGHT, ID_WINDOW_CONTROL,
    { { { DISPOSE_MODE_REL_PARENT, 28, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 22, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      49, 158, 0, 0, 0, 0
    },
  },
  { WM_OBJECT_SWITCH_Create,
    ID_SWITCH_DARKMODE, ID_WINDOW_CONTROL,
    { { { DISPOSE_MODE_REL_PARENT, 139, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 22, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      96, 30, 0, 0, 0, 0
    },
  },
  { WM_OBJECT_TEXT_Create,
    ID_TEXT_TEXT_COLOR, ID_WINDOW_CONTROL,
    { { { DISPOSE_MODE_REL_PARENT, 156, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 189, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      71, 18, 0, 0, 0, 0
    },
  },
  { WM_OBJECT_TEXT_Create,
    ID_TEXT_BACKLIGHT, ID_WINDOW_CONTROL,
    { { { DISPOSE_MODE_REL_PARENT, 19, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 189, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      100, 18, 0, 0, 0, 0
    },
  },
  { WM_OBJECT_TEXT_Create,
    ID_TEXT_DARKMODE, ID_WINDOW_CONTROL,
    { { { DISPOSE_MODE_REL_PARENT, 155, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 55, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      77, 18, 0, 0, 0, 0
    },
  },
  { WM_OBJECT_ROTARY_Create,
    ID_ROTARY_TEXT_COLOR, ID_WINDOW_CONTROL,
    { { { DISPOSE_MODE_REL_PARENT, 135, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 77, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      101, 104, 0, 0, 0, 0
    },
  },
};

/*********************************************************************
*
*       _aSetup
*/
static const APPW_SETUP_ITEM _aSetup[] = {
  { ID_IMAGE_BRG,         APPW_SET_PROP_JPEG,         { ARG_VP(0, acSimple_blue_green_gradient_480x272),
                                                        ARG_V(4666), } },
  { ID_TEXT_APP_EVENT,    APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_TEXT_APP_EVENT,    APPW_SET_PROP_BKCOLOR,      { ARG_V(0xff637b9a) } },
  { ID_TEXT_APP_EVENT,    APPW_SET_PROP_FONT,         { ARG_VP(0, acNettoOT_16_Normal_EXT_AA4) } },
  { ID_TEXT_APP_EVENT,    APPW_SET_PROP_TEXTID,       { ARG_V(ID_TEXT_LOG_LABEL_TXT) } },
  { ID_BUTTON_BACK,       APPW_SET_PROP_SBITMAPS,     { ARG_VP(0, ac_20px_Renesas_Electronics_logo_svg),
                                                        ARG_VP(0, ac_20px_Renesas_Electronics_logo_svg), } },
  { ID_BUTTON_BACK,       APPW_SET_PROP_ALIGNBITMAP,  { ARG_V(GUI_ALIGN_HCENTER | GUI_ALIGN_VCENTER),
                                                        ARG_V(0),
                                                        ARG_V(0) } },
  { ID_TEXT_0GO_BACK,     APPW_SET_PROP_COLOR,        { ARG_V(GUI_BLACK) } },
  { ID_TEXT_0GO_BACK,     APPW_SET_PROP_FONT,         { ARG_VP(0, acNettoOT_16_Normal_EXT_AA4) } },
  { ID_TEXT_0GO_BACK,     APPW_SET_PROP_TEXTID,       { ARG_V(ID_TEXT_GO_BACK_TXT) } },
  { ID_TEXT_0GO_BACK,     APPW_SET_PROP_WRAP,         { ARG_V(0) } },
  { ID_WINDOW_CONTROL,    APPW_SET_PROP_MOTIONV,      { ARG_VP(0, NULL),
                                                        ARG_VP(0, NULL),
                                                        ARG_V(300) } },
  { ID_BOX_CONTROL,       APPW_SET_PROP_COLOR,        { ARG_V(0x96728190) } },
  { ID_SLIDER_BACKLIGHT,  APPW_SET_PROP_VERTICAL,     { ARG_V(0) } },
  { ID_SLIDER_BACKLIGHT,  APPW_SET_PROP_SBITMAPS,     { ARG_VP(0, acSliderBottomGray_20x40),
                                                        ARG_VP(0, acSliderTopGray_20x40),
                                                        ARG_VP(0, acSliderThumbGray_40x20), } },
  { ID_SLIDER_BACKLIGHT,  APPW_SET_PROP_RANGE,        { ARG_V(5),
                                                        ARG_V(100) } },
  { ID_SWITCH_DARKMODE,   APPW_SET_PROP_SBITMAPS,     { ARG_VP(0, acLeftS_80x30),
                                                        ARG_VP(0, acRightS_80x30),
                                                        ARG_VP(0, NULL),
                                                        ARG_VP(0, acThumbLeft_30x30),
                                                        ARG_VP(0, acThumbRight_30x30), } },
  { ID_SWITCH_DARKMODE,   APPW_SET_PROP_PERIOD,       { ARG_V(60) } },
  { ID_TEXT_TEXT_COLOR,   APPW_SET_PROP_COLOR,        { ARG_V(GUI_BLACK) } },
  { ID_TEXT_TEXT_COLOR,   APPW_SET_PROP_TEXTID,       { ARG_V(ID_TEXT_TEXT_COLOR_TXT) } },
  { ID_TEXT_TEXT_COLOR,   APPW_SET_PROP_FONT,         { ARG_VP(0, acRoboto_16_Normal_EXT_AA4) } },
  { ID_TEXT_BACKLIGHT,    APPW_SET_PROP_COLOR,        { ARG_V(GUI_BLACK) } },
  { ID_TEXT_BACKLIGHT,    APPW_SET_PROP_TEXTID,       { ARG_V(ID_TEXT_BACKLIGHT_TXT) } },
  { ID_TEXT_BACKLIGHT,    APPW_SET_PROP_FONT,         { ARG_VP(0, acRoboto_16_Normal_EXT_AA4) } },
  { ID_TEXT_DARKMODE,     APPW_SET_PROP_COLOR,        { ARG_V(GUI_BLACK) } },
  { ID_TEXT_DARKMODE,     APPW_SET_PROP_TEXTID,       { ARG_V(ID_TEXT_DARKMODE_TXT) } },
  { ID_TEXT_DARKMODE,     APPW_SET_PROP_FONT,         { ARG_VP(0, acRoboto_16_Normal_EXT_AA4) } },
  { ID_ROTARY_TEXT_COLOR, APPW_SET_PROP_SBITMAPS,     { ARG_VP(0, acrotary_silver_100),
                                                        ARG_VP(0, acMarker_white_semicircular_11x11), } },
  { ID_ROTARY_TEXT_COLOR, APPW_SET_PROP_POS,          { ARG_V(40) } },
  { ID_ROTARY_TEXT_COLOR, APPW_SET_PROP_RANGE,        { ARG_V(1350),
                                                        ARG_V(1350) } },
  { ID_ROTARY_TEXT_COLOR, APPW_SET_PROP_SPAN,         { ARG_V(43199),
                                                        ARG_V(0) } },
  { ID_ROTARY_TEXT_COLOR, APPW_SET_PROP_VALUE,        { ARG_V(0) } },
  { ID_ROTARY_TEXT_COLOR, APPW_SET_PROP_PERIOD,       { ARG_V(60) } },
  { ID_ROTARY_TEXT_COLOR, APPW_SET_PROP_OFFSET,       { ARG_V(900) } },
};

/*********************************************************************
*
*       _aAction
*/
static const APPW_ACTION_ITEM _aAction[] = {
  { ID_SCREEN_LOG,        APPW_NOTIFICATION_INITDIALOG,     ID_SLIDER_BACKLIGHT,  APPW_JOB_SETVALUE,       ID_SCREEN_LOG__APPW_NOTIFICATION_INITDIALOG__ID_SLIDER_BACKLIGHT__APPW_JOB_SETVALUE,
    { ARG_V(50),
    }, 0,
  },
  { ID_SWITCH_DARKMODE,   WM_NOTIFICATION_CLEAR,            ID_VAR_DARKMODE,      APPW_JOB_SETVALUE,       ID_SCREEN_LOG__ID_SWITCH_DARKMODE__WM_NOTIFICATION_CLEAR,
    { ARG_V(0),
    }, 0,
  },
  { ID_SWITCH_DARKMODE,   WM_NOTIFICATION_SET,              ID_VAR_DARKMODE,      APPW_JOB_SETVALUE,       ID_SCREEN_LOG__ID_SWITCH_DARKMODE__WM_NOTIFICATION_SET,
    { ARG_V(1),
    }, 0,
  },
  { ID_ROTARY_TEXT_COLOR, WM_NOTIFICATION_VALUE_CHANGED,    ID_VAR_TEXT_COLOR_CHANGE, APPW_JOB_SETVALUE,       ID_SCREEN_LOG__ID_ROTARY_TEXT_COLOR__WM_NOTIFICATION_VALUE_CHANGED,
    { ARG_V(0),
    }, 0,
  },
  { ID_VAR_TEXT_COLOR_CHANGE, WM_NOTIFICATION_VALUE_CHANGED,    0,                    APPW_JOB_NULL,           ID_SCREEN_LOG__WM_NOTIFICATION_VALUE_CHANGED,
  },
  { ID_SLIDER_BACKLIGHT,  WM_NOTIFICATION_VALUE_CHANGED,    ID_VAR_BACKLIGHT,     APPW_JOB_SETVALUE,       ID_SCREEN_LOG__ID_SLIDER_BACKLIGHT__WM_NOTIFICATION_VALUE_CHANGED,
    { ARG_V(0),
    }, 0,
  },
  { ID_VAR_BACKLIGHT,     WM_NOTIFICATION_VALUE_CHANGED,    0,                    APPW_JOB_NULL,           ID_SCREEN_LOG__WM_NOTIFICATION_VALUE_CHANGED_0,
  },
  { ID_BUTTON_BACK,       WM_NOTIFICATION_CLICKED,          0,                    APPW_JOB_SWAPSCREEN,     ID_SCREEN_LOG__ID_BUTTON_BACK__WM_NOTIFICATION_CLICKED,
    { ARG_V(ID_SCREEN_MAIN),
    }, 0,
  },
};

/*********************************************************************
*
*       Public data
*
**********************************************************************
*/
/*********************************************************************
*
*       ID_SCREEN_LOG_RootInfo
*/
APPW_ROOT_INFO ID_SCREEN_LOG_RootInfo = {
  ID_SCREEN_LOG,
  _aCreate, GUI_COUNTOF(_aCreate),
  _aSetup,  GUI_COUNTOF(_aSetup),
  _aAction, GUI_COUNTOF(_aAction),
  cbID_SCREEN_LOG,
  0
};

/*************************** End of file ****************************/

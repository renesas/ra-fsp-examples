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
    { 0, 0 }
  },
  { WM_OBJECT_IMAGE_Create,
    ID_IMAGE_BGD, ID_SCREEN_LOG,
    { { { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      480, 854, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_BUTTON_Create,
    ID_BUTTON_LOGO_BACK, ID_SCREEN_LOG,
    { { { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      480, 97, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_TEXT_GO_BACK, ID_SCREEN_LOG,
    { { { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 90, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      480, 63, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_TEXT_APP_EVENT, ID_SCREEN_LOG,
    { { { DISPOSE_MODE_REL_PARENT, 18, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 629, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      445, 69, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_BUTTON_Create,
    ID_BUTTON_APP_EVENT_OUTLINE, ID_SCREEN_LOG,
    { { { DISPOSE_MODE_REL_PARENT, 18, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 629, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      445, 69, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_WINDOW_Create,
    ID_WINDOW_CONTROL, ID_SCREEN_LOG,
    { { { DISPOSE_MODE_REL_PARENT, 18, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 206, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      445, 379, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_BOX_Create,
    ID_BOX_CONTROL, ID_WINDOW_CONTROL,
    { { { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
      },
      0, 0, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_SLIDER_Create,
    ID_SLIDER_BACKLIGHT, ID_WINDOW_CONTROL,
    { { { DISPOSE_MODE_REL_PARENT, 50, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 40, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      87, 280, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_SWITCH_Create,
    ID_SWITCH_DARKMODE, ID_WINDOW_CONTROL,
    { { { DISPOSE_MODE_REL_PARENT, 245, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 40, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      170, 53, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_ROTARY_Create,
    ID_ROTARY_TEXT_COLOR, ID_WINDOW_CONTROL,
    { { { DISPOSE_MODE_REL_PARENT, 240, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 134, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      180, 184, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_TEXT_DARKMODE, ID_WINDOW_CONTROL,
    { { { DISPOSE_MODE_REL_PARENT, 264, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 97, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      136, 32, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_TEXT_TEXT_COLOR, ID_WINDOW_CONTROL,
    { { { DISPOSE_MODE_REL_PARENT, 272, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 320, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      128, 32, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_TEXT_BACKLIGHT, ID_WINDOW_CONTROL,
    { { { DISPOSE_MODE_REL_PARENT, 8, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 325, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      169, 32, 0, 0, 0, 0
    },
    { 0, 0 }
  },
};

/*********************************************************************
*
*       _aSetup
*/
static GUI_CONST_STORAGE APPW_SETUP_ITEM _aSetup[] = {
  { ID_IMAGE_BGD,                APPW_SET_PROP_BKCOLOR,      { ARG_V(0xff89d0ff) } },
  { ID_BUTTON_LOGO_BACK,         APPW_SET_PROP_COLORS,       { ARG_V(GUI_INVALID_COLOR),
                                                               ARG_V(GUI_INVALID_COLOR),
                                                               ARG_V(GUI_INVALID_COLOR) } },
  { ID_BUTTON_LOGO_BACK,         APPW_SET_PROP_SBITMAPS,     { ARG_VP(0, ac_20px_Renesas_Electronics_logo_svg),
                                                               ARG_VP(0, ac_20px_Renesas_Electronics_logo_svg), } },
  { ID_TEXT_GO_BACK,             APPW_SET_PROP_COLOR,        { ARG_V(0xff2f2f2f) } },
  { ID_TEXT_GO_BACK,             APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_HCENTER | GUI_ALIGN_VCENTER),
                                                               ARG_V(0),
                                                               ARG_V(0) } },
  { ID_TEXT_GO_BACK,             APPW_SET_PROP_TEXTID,       { ARG_V(ID_TEXT_GO_BACK_TXT) } },
  { ID_TEXT_GO_BACK,             APPW_SET_PROP_FONT,         { ARG_VPF(0, acNettoOT_32_Normal_EXT_AA4, &APPW__aFont[2]) } },
  { ID_TEXT_GO_BACK,             APPW_SET_PROP_WRAP,         { ARG_V(0) } },
  { ID_TEXT_APP_EVENT,           APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_TEXT_APP_EVENT,           APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_LEFT | GUI_ALIGN_VCENTER),
                                                               ARG_V(16),
                                                               ARG_V(0) } },
  { ID_TEXT_APP_EVENT,           APPW_SET_PROP_BKCOLOR,      { ARG_V(0xff708caf) } },
  { ID_TEXT_APP_EVENT,           APPW_SET_PROP_TEXTID,       { ARG_V(ID_TEXT_LOG_LABEL_TXT) } },
  { ID_TEXT_APP_EVENT,           APPW_SET_PROP_FONT,         { ARG_VPF(0, acNettoOT_24_Normal_EXT_AA4, &APPW__aFont[0]) } },
  { ID_BUTTON_APP_EVENT_OUTLINE, APPW_SET_PROP_SBITMAPS,     { ARG_VP(0, NULL), } },
  { ID_BUTTON_APP_EVENT_OUTLINE, APPW_SET_PROP_COLORS,       { ARG_V(GUI_INVALID_COLOR),
                                                               ARG_V(GUI_INVALID_COLOR),
                                                               ARG_V(GUI_INVALID_COLOR) } },
  { ID_BUTTON_APP_EVENT_OUTLINE, APPW_SET_PROP_FRAME,        { ARG_V(1) } },
  { ID_BUTTON_APP_EVENT_OUTLINE, APPW_SET_PROP_COLOR,        { ARG_V(GUI_BLACK) } },
  { ID_BOX_CONTROL,              APPW_SET_PROP_COLOR,        { ARG_V(0xff708caf) } },
  { ID_SLIDER_BACKLIGHT,         APPW_SET_PROP_VERTICAL,     { ARG_V(0) } },
  { ID_SLIDER_BACKLIGHT,         APPW_SET_PROP_SBITMAPS,     { ARG_VP(0, acDARK_Slider_Shaft_V_Bottom_Gray_16x30),
                                                               ARG_VP(0, acDARK_Slider_Shaft_V_Top_Blue_16x30),
                                                               ARG_VP(0, acDARK_Slider_Thumb_Up_22x22), } },
  { ID_SLIDER_BACKLIGHT,         APPW_SET_PROP_RANGE,        { ARG_V(5),
                                                               ARG_V(100) } },
  { ID_SLIDER_BACKLIGHT,         APPW_SET_PROP_INVERT,       { ARG_V(0) } },
  { ID_SWITCH_DARKMODE,          APPW_SET_PROP_SBITMAPS,     { ARG_VP(0, acSwitch_Body_Blue_80x30),
                                                               ARG_VP(0, acSwitch_Body_WhiteBlackBorder_80x30),
                                                               ARG_VP(0, NULL),
                                                               ARG_VP(0, acSwitch_Thumb_WhiteBlackBorder_30x30),
                                                               ARG_VP(0, acSwitch_Thumb_WhiteBlackBorder_30x30), } },
  { ID_SWITCH_DARKMODE,          APPW_SET_PROP_FADE,         { ARG_V(0) } },
  { ID_SWITCH_DARKMODE,          APPW_SET_PROP_PERIOD,       { ARG_V(60) } },
  { ID_ROTARY_TEXT_COLOR,        APPW_SET_PROP_POS,          { ARG_V(40) } },
  { ID_ROTARY_TEXT_COLOR,        APPW_SET_PROP_ROTATE,       { ARG_V(0) } },
  { ID_ROTARY_TEXT_COLOR,        APPW_SET_PROP_VALUE,        { ARG_V(0) } },
  { ID_ROTARY_TEXT_COLOR,        APPW_SET_PROP_RANGE,        { ARG_V(1350),
                                                               ARG_V(1350) } },
  { ID_ROTARY_TEXT_COLOR,        APPW_SET_PROP_SPAN,         { ARG_V(43199),
                                                               ARG_V(0) } },
  { ID_ROTARY_TEXT_COLOR,        APPW_SET_PROP_PERIOD,       { ARG_V(60) } },
  { ID_ROTARY_TEXT_COLOR,        APPW_SET_PROP_OFFSET,       { ARG_V(900) } },
  { ID_ROTARY_TEXT_COLOR,        APPW_SET_PROP_SBITMAPS,     { ARG_VP(0, acDARK_Rotary_BG_Gray_80x80),
                                                               ARG_VP(0, acRotary_Marker_GreenBullet_11x11), } },
  { ID_TEXT_DARKMODE,            APPW_SET_PROP_COLOR,        { ARG_V(0xff1a1a1a) } },
  { ID_TEXT_DARKMODE,            APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_HCENTER | GUI_ALIGN_VCENTER),
                                                               ARG_V(0),
                                                               ARG_V(0) } },
  { ID_TEXT_DARKMODE,            APPW_SET_PROP_FONT,         { ARG_VPF(0, acNettoOT_32_Normal_EXT_AA4, &APPW__aFont[2]) } },
  { ID_TEXT_DARKMODE,            APPW_SET_PROP_TEXTID,       { ARG_V(ID_TEXT_DARKMODE_TXT) } },
  { ID_TEXT_TEXT_COLOR,          APPW_SET_PROP_COLOR,        { ARG_V(0xff1a1a1a) } },
  { ID_TEXT_TEXT_COLOR,          APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_HCENTER | GUI_ALIGN_VCENTER),
                                                               ARG_V(0),
                                                               ARG_V(0) } },
  { ID_TEXT_TEXT_COLOR,          APPW_SET_PROP_TEXTID,       { ARG_V(ID_TEXT_COLOR_TXT) } },
  { ID_TEXT_TEXT_COLOR,          APPW_SET_PROP_FONT,         { ARG_VPF(0, acNettoOT_32_Normal_EXT_AA4, &APPW__aFont[2]) } },
  { ID_TEXT_BACKLIGHT,           APPW_SET_PROP_COLOR,        { ARG_V(0xff1a1a1a) } },
  { ID_TEXT_BACKLIGHT,           APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_HCENTER | GUI_ALIGN_VCENTER),
                                                               ARG_V(0),
                                                               ARG_V(0) } },
  { ID_TEXT_BACKLIGHT,           APPW_SET_PROP_TEXTID,       { ARG_V(ID_TEXT_BACKLIGHT_TXT) } },
  { ID_TEXT_BACKLIGHT,           APPW_SET_PROP_FONT,         { ARG_VPF(0, acNettoOT_32_Normal_EXT_AA4, &APPW__aFont[2]) } },
};

/*********************************************************************
*
*       _aAction
*/
static GUI_CONST_STORAGE APPW_ACTION_ITEM _aAction[] = {
  { ID_SCREEN_LOG,               APPW_NOTIFICATION_INITDIALOG,     ID_SLIDER_BACKLIGHT,         APPW_JOB_SETVALUE,       ID_SCREEN_LOG__APPW_NOTIFICATION_INITDIALOG__ID_SLIDER_BACKLIGHT__APPW_JOB_SETVALUE,
    { ARG_V(50),
    }, 0, NULL
  },
  { ID_SWITCH_DARKMODE,          WM_NOTIFICATION_CLEAR,            ID_VAR_DARKMODE,             APPW_JOB_SETVALUE,       ID_SCREEN_LOG__ID_SWITCH_DARKMODE__WM_NOTIFICATION_CLEAR,
    { ARG_V(0),
    }, 0, NULL
  },
  { ID_SWITCH_DARKMODE,          WM_NOTIFICATION_SET,              ID_VAR_DARKMODE,             APPW_JOB_SETVALUE,       ID_SCREEN_LOG__ID_SWITCH_DARKMODE__WM_NOTIFICATION_SET,
    { ARG_V(1),
    }, 0, NULL
  },
  { ID_ROTARY_TEXT_COLOR,        WM_NOTIFICATION_VALUE_CHANGED,    ID_VAR_TEXT_COLOR_CHANGE,    APPW_JOB_SETVALUE,       ID_SCREEN_LOG__ID_ROTARY_TEXT_COLOR__WM_NOTIFICATION_VALUE_CHANGED,
    { ARG_V(0),
    }, 0, NULL
  },
  { ID_VAR_TEXT_COLOR_CHANGE,    WM_NOTIFICATION_VALUE_CHANGED,    0,                           APPW_JOB_NULL,           ID_SCREEN_LOG__WM_NOTIFICATION_VALUE_CHANGED_TEXT_COLOR,
  },
  { ID_SLIDER_BACKLIGHT,         WM_NOTIFICATION_VALUE_CHANGED,    ID_VAR_BACKLIGHT,            APPW_JOB_SETVALUE,       ID_SCREEN_LOG__ID_SLIDER_BACKLIGHT__WM_NOTIFICATION_VALUE_CHANGED,
    { ARG_V(0),
    }, 0, NULL
  },
  { ID_VAR_BACKLIGHT,            WM_NOTIFICATION_VALUE_CHANGED,    0,                           APPW_JOB_NULL,           ID_SCREEN_LOG__WM_NOTIFICATION_VALUE_CHANGED_LCD_BACKLIGHT,
  },
  { ID_BUTTON_LOGO_BACK,         WM_NOTIFICATION_CLICKED,          0,                           APPW_JOB_SHOWSCREEN,     ID_SCREEN_LOG__ID_BUTTON_LOGO_BACK__WM_NOTIFICATION_CLICKED,
    { ARG_V(ID_SCREEN_MAIN),
    }, 0, NULL
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

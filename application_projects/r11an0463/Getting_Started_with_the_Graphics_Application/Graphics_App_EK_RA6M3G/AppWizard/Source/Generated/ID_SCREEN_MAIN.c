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
File        : ID_SCREEN_MAIN.c
Purpose     : Generated file do NOT edit!
---------------------------END-OF-HEADER------------------------------
*/

#include "Resource.h"
#include "ID_SCREEN_MAIN.h"

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
    ID_SCREEN_MAIN, 0,
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
    ID_IMAGE_SUNNY_BGRD, ID_SCREEN_MAIN,
    { { { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      480, 272, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_IMAGE_Create,
    ID_IMAGE_CLOUDY_BGRD, ID_SCREEN_MAIN,
    { { { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      480, 272, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_IMAGE_Create,
    ID_IMAGE_RAINY_BGRG, ID_SCREEN_MAIN,
    { { { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      480, 272, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_BUTTON_Create,
    ID_BUTTON_NEXT_SCREEN, ID_SCREEN_MAIN,
    { { { DISPOSE_MODE_REL_PARENT, 280, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      200, 42, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_TEXT_CURRENT_WEEK_DAY, ID_SCREEN_MAIN,
    { { { DISPOSE_MODE_REL_PARENT, 13, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 11, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      25, 17, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_TEXT_CURRENT_TIME, ID_SCREEN_MAIN,
    { { { DISPOSE_MODE_REL_PARENT, 72, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 11, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      54, 17, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_TEXT_AMPM, ID_SCREEN_MAIN,
    { { { DISPOSE_MODE_REL_PARENT, 122, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 11, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      30, 17, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_TEXT_CURRENT_DATE, ID_SCREEN_MAIN,
    { { { DISPOSE_MODE_REL_PARENT, 182, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 11, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      86, 17, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_TEXT_FORECAST_DAY, ID_SCREEN_MAIN,
    { { { DISPOSE_MODE_REL_PARENT, 13, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 53, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      47, 25, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_TEXT_FORECAST_SUM, ID_SCREEN_MAIN,
    { { { DISPOSE_MODE_REL_PARENT, 13, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 121, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      120, 25, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_TEXT_HUMIDITY, ID_SCREEN_MAIN,
    { { { DISPOSE_MODE_REL_PARENT, 34, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 147, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      80, 25, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_TEXT_HUMIDITY_NO, ID_SCREEN_MAIN,
    { { { DISPOSE_MODE_REL_PARENT, 13, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 147, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      25, 25, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_TEXT_FORECAST_TEMP, ID_SCREEN_MAIN,
    { { { DISPOSE_MODE_REL_PARENT, 48, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 80, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      30, 40, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_TEXT_DEGR_F_CURRENT, ID_SCREEN_MAIN,
    { { { DISPOSE_MODE_REL_PARENT, 77, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 80, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      39, 32, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_WINDOW_Create,
    ID_WINDOW_THERMOSTAT, ID_SCREEN_MAIN,
    { { { DISPOSE_MODE_REL_PARENT, 314, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 7, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 76, 0, 0 },
      },
      0, 154, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_IMAGE_Create,
    ID_IMAGE_THERMOSTAT_BGRD, ID_WINDOW_THERMOSTAT,
    { { { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      154, 154, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_IMAGE_Create,
    ID_IMAGE_THERMOSTAT, ID_WINDOW_THERMOSTAT,
    { { { DISPOSE_MODE_REL_PARENT, 4, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 6, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 74, 0, 0 },
      },
      145, 0, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_BUTTON_Create,
    ID_BUTTON_TEMP_UP, ID_WINDOW_THERMOSTAT,
    { { { DISPOSE_MODE_REL_PARENT, 8, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 86, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      60, 60, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_BUTTON_Create,
    ID_BUTTON_TEMP_DOWN, ID_WINDOW_THERMOSTAT,
    { { { DISPOSE_MODE_REL_PARENT, 86, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 86, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      60, 60, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_TEXT_THERMOSTAT, ID_WINDOW_THERMOSTAT,
    { { { DISPOSE_MODE_REL_PARENT, 34, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 8, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      97, 17, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_TEXT_TARGET_TEMP, ID_WINDOW_THERMOSTAT,
    { { { DISPOSE_MODE_REL_PARENT, 53, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 32, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      32, 43, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_TEXT_DEGR_F_TARGET, ID_WINDOW_THERMOSTAT,
    { { { DISPOSE_MODE_REL_PARENT, 80, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 32, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      36, 35, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_WINDOW_Create,
    ID_WINDOW_ANIMATION, ID_SCREEN_MAIN,
    { { { DISPOSE_MODE_REL_PARENT, 137, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 45, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 166, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 76, 0, 0 },
      },
      0, 0, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_IMAGE_Create,
    ID_IMAGE_ANIM_RAINY_BGRD, ID_WINDOW_ANIMATION,
    { { { DISPOSE_MODE_REL_PARENT, 12, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      167, 143, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_IMAGE_Create,
    ID_IMAGE_ANIM_RAINY_BGRD_2, ID_WINDOW_ANIMATION,
    { { { DISPOSE_MODE_REL_PARENT, 6, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 6, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      174, 143, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_IMAGE_Create,
    ID_IMAGE_ANIM_CLOUDY_BGRD, ID_WINDOW_ANIMATION,
    { { { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      175, 143, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_IMAGE_Create,
    ID_IMAGE_ANIM_CLOUD_MOTION, ID_WINDOW_ANIMATION,
    { { { DISPOSE_MODE_REL_PARENT, -136, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 78, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      176, 73, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_IMAGE_Create,
    ID_IMAGE_ANIM_SUNNY_BRGD, ID_WINDOW_ANIMATION,
    { { { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      163, 149, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_WINDOW_Create,
    ID_WINDOW_SUN, ID_SCREEN_MAIN,
    { { { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 196, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 408, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
      },
      0, 0, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_WINDOW_Create,
    ID_WINDOW_MON, ID_SCREEN_MAIN,
    { { { DISPOSE_MODE_REL_PARENT, 68, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 196, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 339, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
      },
      0, 0, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_WINDOW_Create,
    ID_WINDOW_TUE, ID_SCREEN_MAIN,
    { { { DISPOSE_MODE_REL_PARENT, 136, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 196, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 265, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
      },
      0, 0, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_WINDOW_Create,
    ID_WINDOW_WED, ID_SCREEN_MAIN,
    { { { DISPOSE_MODE_REL_PARENT, 202, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 196, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 204, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
      },
      0, 0, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_WINDOW_Create,
    ID_WINDOW_THU, ID_SCREEN_MAIN,
    { { { DISPOSE_MODE_REL_PARENT, 271, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 196, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 135, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
      },
      0, 0, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_WINDOW_Create,
    ID_WINDOW_FRI, ID_SCREEN_MAIN,
    { { { DISPOSE_MODE_REL_PARENT, 339, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 196, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 67, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
      },
      0, 0, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_WINDOW_Create,
    ID_WINDOW_SAT, ID_SCREEN_MAIN,
    { { { DISPOSE_MODE_REL_PARENT, 407, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 196, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
      },
      0, 0, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_IMAGE_Create,
    ID_IMAGE_SUN_PRESSED, ID_WINDOW_SUN,
    { { { DISPOSE_MODE_REL_PARENT, 6, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 10, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      60, 60, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_IMAGE_Create,
    ID_IMAGE_SUN, ID_WINDOW_SUN,
    { { { DISPOSE_MODE_REL_PARENT, 7, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 10, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      60, 60, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_IMAGE_Create,
    ID_IMAGE_SUNNY_SUN, ID_WINDOW_SUN,
    { { { DISPOSE_MODE_REL_PARENT, 20, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 24, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      36, 32, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_TEXT_SUN, ID_WINDOW_SUN,
    { { { DISPOSE_MODE_REL_PARENT, 24, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 13, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      34, 16, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_TEXT_SUN_RANGE, ID_WINDOW_SUN,
    { { { DISPOSE_MODE_REL_PARENT, 19, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 53, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      45, 17, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_BUTTON_Create,
    ID_BUTTON_SUN, ID_WINDOW_SUN,
    { { { DISPOSE_MODE_REL_PARENT, 6, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 10, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      60, 60, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_IMAGE_Create,
    ID_IMAGE_MON_PRESSED, ID_WINDOW_MON,
    { { { DISPOSE_MODE_REL_PARENT, 6, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 10, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      60, 60, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_IMAGE_Create,
    ID_IMAGE_MON, ID_WINDOW_MON,
    { { { DISPOSE_MODE_REL_PARENT, 7, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 10, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      60, 60, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_IMAGE_Create,
    ID_IMAGE_CLOUDY_MON, ID_WINDOW_MON,
    { { { DISPOSE_MODE_REL_PARENT, 20, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 24, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      36, 32, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_TEXT_MON, ID_WINDOW_MON,
    { { { DISPOSE_MODE_REL_PARENT, 24, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 13, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      34, 16, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_TEXT_MON_RANGE, ID_WINDOW_MON,
    { { { DISPOSE_MODE_REL_PARENT, 19, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 53, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      45, 17, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_BUTTON_Create,
    ID_BUTTON_MON, ID_WINDOW_MON,
    { { { DISPOSE_MODE_REL_PARENT, 6, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 10, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      60, 60, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_IMAGE_Create,
    ID_IMAGE_TUE_PRESSED, ID_WINDOW_TUE,
    { { { DISPOSE_MODE_REL_PARENT, 6, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 10, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      60, 60, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_IMAGE_Create,
    ID_IMAGE_TUE, ID_WINDOW_TUE,
    { { { DISPOSE_MODE_REL_PARENT, 6, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 10, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      60, 60, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_IMAGE_Create,
    ID_IMAGE_CLOUDY_TUE, ID_WINDOW_TUE,
    { { { DISPOSE_MODE_REL_PARENT, 20, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 24, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      36, 32, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_TEXT_TUE, ID_WINDOW_TUE,
    { { { DISPOSE_MODE_REL_PARENT, 24, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 13, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      34, 16, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_TEXT_TUE_RANGE, ID_WINDOW_TUE,
    { { { DISPOSE_MODE_REL_PARENT, 19, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 53, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      45, 17, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_BUTTON_Create,
    ID_BUTTON_TUE, ID_WINDOW_TUE,
    { { { DISPOSE_MODE_REL_PARENT, 6, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 10, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      60, 60, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_IMAGE_Create,
    ID_IMAGE_WED_PRESSED, ID_WINDOW_WED,
    { { { DISPOSE_MODE_REL_PARENT, 6, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 10, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      60, 60, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_IMAGE_Create,
    ID_IMAGE_WED, ID_WINDOW_WED,
    { { { DISPOSE_MODE_REL_PARENT, 6, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 10, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      60, 60, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_IMAGE_Create,
    ID_IMAGE_RAINY_WED, ID_WINDOW_WED,
    { { { DISPOSE_MODE_REL_PARENT, 20, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 24, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      36, 32, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_TEXT_WED, ID_WINDOW_WED,
    { { { DISPOSE_MODE_REL_PARENT, 24, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 13, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      34, 16, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_TEXT_WED_RANGE, ID_WINDOW_WED,
    { { { DISPOSE_MODE_REL_PARENT, 19, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 53, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      45, 17, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_BUTTON_Create,
    ID_BUTTON_WED, ID_WINDOW_WED,
    { { { DISPOSE_MODE_REL_PARENT, 6, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 10, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      60, 60, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_IMAGE_Create,
    ID_IMAGE_THU_PRESSED, ID_WINDOW_THU,
    { { { DISPOSE_MODE_REL_PARENT, 8, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 10, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      60, 60, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_IMAGE_Create,
    ID_IMAGE_THU, ID_WINDOW_THU,
    { { { DISPOSE_MODE_REL_PARENT, 6, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 10, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      60, 60, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_IMAGE_Create,
    ID_IMAGE_RAINY_THU, ID_WINDOW_THU,
    { { { DISPOSE_MODE_REL_PARENT, 20, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 24, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      36, 32, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_TEXT_THU, ID_WINDOW_THU,
    { { { DISPOSE_MODE_REL_PARENT, 24, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 13, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      33, 16, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_TEXT_THU_RANGE, ID_WINDOW_THU,
    { { { DISPOSE_MODE_REL_PARENT, 19, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 53, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      45, 17, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_BUTTON_Create,
    ID_BUTTON_THU, ID_WINDOW_THU,
    { { { DISPOSE_MODE_REL_PARENT, 6, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 10, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      60, 60, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_IMAGE_Create,
    ID_IMAGE_FRI_PRESSED, ID_WINDOW_FRI,
    { { { DISPOSE_MODE_REL_PARENT, 6, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 10, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      60, 60, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_IMAGE_Create,
    ID_IMAGE_FRI, ID_WINDOW_FRI,
    { { { DISPOSE_MODE_REL_PARENT, 7, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 10, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      60, 60, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_IMAGE_Create,
    ID_IMAGE_CLOUDY_FRI, ID_WINDOW_FRI,
    { { { DISPOSE_MODE_REL_PARENT, 20, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 24, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      36, 32, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_TEXT_FRI, ID_WINDOW_FRI,
    { { { DISPOSE_MODE_REL_PARENT, 27, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 13, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      35, 16, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_TEXT_FRI_RANGE, ID_WINDOW_FRI,
    { { { DISPOSE_MODE_REL_PARENT, 19, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 53, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      45, 17, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_BUTTON_Create,
    ID_BUTTON_FRI, ID_WINDOW_FRI,
    { { { DISPOSE_MODE_REL_PARENT, 6, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 10, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      60, 60, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_IMAGE_Create,
    ID_IMAGE_SAT_PRESSED, ID_WINDOW_SAT,
    { { { DISPOSE_MODE_REL_PARENT, 6, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 10, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      60, 60, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_IMAGE_Create,
    ID_IMAGE_SAT, ID_WINDOW_SAT,
    { { { DISPOSE_MODE_REL_PARENT, 7, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 10, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      60, 60, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_IMAGE_Create,
    ID_IMAGE_SUNNY_SAT, ID_WINDOW_SAT,
    { { { DISPOSE_MODE_REL_PARENT, 20, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 24, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      36, 32, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_TEXT_SAT, ID_WINDOW_SAT,
    { { { DISPOSE_MODE_REL_PARENT, 25, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 13, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      35, 16, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_TEXT_SAT_RANGE, ID_WINDOW_SAT,
    { { { DISPOSE_MODE_REL_PARENT, 19, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 53, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      45, 17, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_BUTTON_Create,
    ID_BUTTON_SAT, ID_WINDOW_SAT,
    { { { DISPOSE_MODE_REL_PARENT, 6, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 10, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      60, 60, 0, 0, 0, 0
    },
    { 0, 0 }
  },
};

/*********************************************************************
*
*       _aSetup
*/
static GUI_CONST_STORAGE APPW_SETUP_ITEM _aSetup[] = {
  { ID_IMAGE_SUNNY_BGRD,        APPW_SET_PROP_JPEG,         { ARG_VP(0, acbg_sunny),
                                                              ARG_V(94596) } },
  { ID_IMAGE_SUNNY_BGRD,        APPW_SET_PROP_UNTOUCHABLE,  { ARG_V(0) } },
  { ID_IMAGE_CLOUDY_BGRD,       APPW_SET_PROP_JPEG,         { ARG_VP(0, acbg_partlycloudy),
                                                              ARG_V(104890) } },
  { ID_IMAGE_RAINY_BGRG,        APPW_SET_PROP_JPEG,         { ARG_VP(0, acbg_rainy),
                                                              ARG_V(86886) } },
  { ID_TEXT_CURRENT_WEEK_DAY,   APPW_SET_PROP_COLOR,        { ARG_V(0xffc0c0c0) } },
  { ID_TEXT_CURRENT_WEEK_DAY,   APPW_SET_PROP_FONT,         { ARG_VPF(0, acNettoOT_16_Normal_EXT_AA4, &APPW__aFont[0]) } },
  { ID_TEXT_CURRENT_TIME,       APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_TEXT_CURRENT_TIME,       APPW_SET_PROP_FONT,         { ARG_VPF(0, acNettoOT_16_Normal_EXT_AA4, &APPW__aFont[0]) } },
  { ID_TEXT_AMPM,               APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_TEXT_AMPM,               APPW_SET_PROP_FONT,         { ARG_VPF(0, acNettoOT_16_Normal_EXT_AA4, &APPW__aFont[0]) } },
  { ID_TEXT_CURRENT_DATE,       APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_TEXT_CURRENT_DATE,       APPW_SET_PROP_FONT,         { ARG_VPF(0, acNettoOT_16_Normal_EXT_AA4, &APPW__aFont[0]) } },
  { ID_TEXT_FORECAST_DAY,       APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_TEXT_FORECAST_DAY,       APPW_SET_PROP_FONT,         { ARG_VPF(0, acNettoOT_16_Normal_EXT_AA4, &APPW__aFont[0]) } },
  { ID_TEXT_FORECAST_SUM,       APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_TEXT_FORECAST_SUM,       APPW_SET_PROP_FONT,         { ARG_VPF(0, acNettoOT_16_Normal_EXT_AA4, &APPW__aFont[0]) } },
  { ID_TEXT_HUMIDITY,           APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_TEXT_HUMIDITY,           APPW_SET_PROP_FONT,         { ARG_VPF(0, acNettoOT_16_Normal_EXT_AA4, &APPW__aFont[0]) } },
  { ID_TEXT_HUMIDITY,           APPW_SET_PROP_TEXTID,       { ARG_V(ID_TEXT_HUMIDITY_TXT) } },
  { ID_TEXT_HUMIDITY_NO,        APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_TEXT_HUMIDITY_NO,        APPW_SET_PROP_FONT,         { ARG_VPF(0, acNettoOT_16_Normal_EXT_AA4, &APPW__aFont[0]) } },
  { ID_TEXT_FORECAST_TEMP,      APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_TEXT_FORECAST_TEMP,      APPW_SET_PROP_FONT,         { ARG_VPF(0, acNettoOT_32_Normal_EXT_AA4, &APPW__aFont[1]) } },
  { ID_TEXT_DEGR_F_CURRENT,     APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_TEXT_DEGR_F_CURRENT,     APPW_SET_PROP_TEXTID,       { ARG_V(ID_TEXT_DEGR_F_TXT) } },
  { ID_TEXT_DEGR_F_CURRENT,     APPW_SET_PROP_FONT,         { ARG_VPF(0, acNettoOT_32_Normal_EXT_AA4, &APPW__aFont[1]) } },
  { ID_IMAGE_THERMOSTAT_BGRD,   APPW_SET_PROP_TILE,         { ARG_V(0) } },
  { ID_IMAGE_THERMOSTAT_BGRD,   APPW_SET_PROP_SBITMAP,      { ARG_VP(0, acbg_thermostat),
                                                              ARG_V(586), } },
  { ID_IMAGE_THERMOSTAT,        APPW_SET_PROP_TILE,         { ARG_V(0) } },
  { ID_IMAGE_THERMOSTAT,        APPW_SET_PROP_SBITMAP,      { ARG_VP(0, acbox_thermostat),
                                                              ARG_V(517), } },
  { ID_BUTTON_TEMP_UP,          APPW_SET_PROP_SBITMAPS,     { ARG_VP(0, acbutton_plus),
                                                              ARG_VP(0, acbutton_plus_pressed), } },
  { ID_BUTTON_TEMP_UP,          APPW_SET_PROP_COLORS,       { ARG_V(0xffc0c0c0),
                                                              ARG_V(0xffc0c0c0),
                                                              ARG_V(GUI_INVALID_COLOR) } },
  { ID_BUTTON_TEMP_DOWN,        APPW_SET_PROP_SBITMAPS,     { ARG_VP(0, acbutton_minus),
                                                              ARG_VP(0, acbutton_minus_pressed), } },
  { ID_BUTTON_TEMP_DOWN,        APPW_SET_PROP_COLORS,       { ARG_V(0xffc0c0c0),
                                                              ARG_V(0xffc0c0c0),
                                                              ARG_V(GUI_INVALID_COLOR) } },
  { ID_TEXT_THERMOSTAT,         APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_TEXT_THERMOSTAT,         APPW_SET_PROP_TEXTID,       { ARG_V(ID_TEXT_THERMOSTAT_TXT) } },
  { ID_TEXT_THERMOSTAT,         APPW_SET_PROP_FONT,         { ARG_VPF(0, acNettoOT_16_Normal_EXT_AA4, &APPW__aFont[0]) } },
  { ID_TEXT_TARGET_TEMP,        APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_TEXT_TARGET_TEMP,        APPW_SET_PROP_DECMODE,      { ARG_V(0),
                                                              ARG_V(0),
                                                              ARG_V(0),
                                                              ARG_V(0) } },
  { ID_TEXT_TARGET_TEMP,        APPW_SET_PROP_RANGE,        { ARG_V(65),
                                                              ARG_V(85) } },
  { ID_TEXT_TARGET_TEMP,        APPW_SET_PROP_FONT,         { ARG_VPF(0, acNettoOT_32_Normal_EXT_AA4, &APPW__aFont[1]) } },
  { ID_TEXT_DEGR_F_TARGET,      APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_TEXT_DEGR_F_TARGET,      APPW_SET_PROP_FONT,         { ARG_VPF(0, acNettoOT_32_Normal_EXT_AA4, &APPW__aFont[1]) } },
  { ID_TEXT_DEGR_F_TARGET,      APPW_SET_PROP_TEXTID,       { ARG_V(ID_TEXT_DEGR_F_TXT) } },
  { ID_IMAGE_ANIM_RAINY_BGRD,   APPW_SET_PROP_SBITMAP,      { ARG_VP(0, acanime_raining_1),
                                                              ARG_V(24170), } },
  { ID_IMAGE_ANIM_RAINY_BGRD_2, APPW_SET_PROP_SBITMAP,      { ARG_VP(0, acanime_raining_2),
                                                              ARG_V(27955), } },
  { ID_IMAGE_ANIM_CLOUDY_BGRD,  APPW_SET_PROP_SBITMAP,      { ARG_VP(0, acanime_sun_oncloud),
                                                              ARG_V(28848), } },
  { ID_IMAGE_ANIM_CLOUD_MOTION, APPW_SET_PROP_SBITMAP,      { ARG_VP(0, acanime_cloud),
                                                              ARG_V(13100), } },
  { ID_IMAGE_ANIM_SUNNY_BRGD,   APPW_SET_PROP_SBITMAP,      { ARG_VP(0, acanime_sun_shining),
                                                              ARG_V(23155), } },
  { ID_IMAGE_SUN_PRESSED,       APPW_SET_PROP_SBITMAP,      { ARG_VP(0, acbottom_button_trans_pressed),
                                                              ARG_V(659), } },
  { ID_IMAGE_SUN,               APPW_SET_PROP_SBITMAP,      { ARG_VP(0, acbottom_button_trans),
                                                              ARG_V(660), } },
  { ID_IMAGE_SUNNY_SUN,         APPW_SET_PROP_SBITMAP,      { ARG_VP(0, acicon_sunny),
                                                              ARG_V(2344), } },
  { ID_TEXT_SUN,                APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_TEXT_SUN,                APPW_SET_PROP_FONT,         { ARG_VPF(0, acNettoOT_16_Normal_EXT_AA4, &APPW__aFont[0]) } },
  { ID_TEXT_SUN,                APPW_SET_PROP_TEXTID,       { ARG_V(ID_TEXT_SUN_TXT) } },
  { ID_TEXT_SUN_RANGE,          APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_TEXT_SUN_RANGE,          APPW_SET_PROP_FONT,         { ARG_VPF(0, acNettoOT_16_Normal_EXT_AA4, &APPW__aFont[0]) } },
  { ID_IMAGE_MON_PRESSED,       APPW_SET_PROP_SBITMAP,      { ARG_VP(0, acbottom_button_trans_pressed),
                                                              ARG_V(659), } },
  { ID_IMAGE_MON,               APPW_SET_PROP_SBITMAP,      { ARG_VP(0, acbottom_button_trans),
                                                              ARG_V(660), } },
  { ID_IMAGE_CLOUDY_MON,        APPW_SET_PROP_SBITMAP,      { ARG_VP(0, acicon_cloudy),
                                                              ARG_V(2960), } },
  { ID_TEXT_MON,                APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_TEXT_MON,                APPW_SET_PROP_TEXTID,       { ARG_V(ID_TEXT_MON_TXT) } },
  { ID_TEXT_MON,                APPW_SET_PROP_FONT,         { ARG_VPF(0, acNettoOT_16_Normal_EXT_AA4, &APPW__aFont[0]) } },
  { ID_TEXT_MON_RANGE,          APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_TEXT_MON_RANGE,          APPW_SET_PROP_FONT,         { ARG_VPF(0, acNettoOT_16_Normal_EXT_AA4, &APPW__aFont[0]) } },
  { ID_IMAGE_TUE_PRESSED,       APPW_SET_PROP_SBITMAP,      { ARG_VP(0, acbottom_button_trans_pressed),
                                                              ARG_V(659), } },
  { ID_IMAGE_TUE,               APPW_SET_PROP_SBITMAP,      { ARG_VP(0, acbottom_button_trans),
                                                              ARG_V(660), } },
  { ID_IMAGE_CLOUDY_TUE,        APPW_SET_PROP_SBITMAP,      { ARG_VP(0, acicon_cloudy),
                                                              ARG_V(2960), } },
  { ID_TEXT_TUE,                APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_TEXT_TUE,                APPW_SET_PROP_TEXTID,       { ARG_V(ID_TEXT_TUE_TXT) } },
  { ID_TEXT_TUE,                APPW_SET_PROP_FONT,         { ARG_VPF(0, acNettoOT_16_Normal_EXT_AA4, &APPW__aFont[0]) } },
  { ID_TEXT_TUE_RANGE,          APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_TEXT_TUE_RANGE,          APPW_SET_PROP_FONT,         { ARG_VPF(0, acNettoOT_16_Normal_EXT_AA4, &APPW__aFont[0]) } },
  { ID_IMAGE_WED_PRESSED,       APPW_SET_PROP_SBITMAP,      { ARG_VP(0, acbottom_button_trans_pressed),
                                                              ARG_V(659), } },
  { ID_IMAGE_WED,               APPW_SET_PROP_SBITMAP,      { ARG_VP(0, acbottom_button_trans),
                                                              ARG_V(660), } },
  { ID_IMAGE_RAINY_WED,         APPW_SET_PROP_SBITMAP,      { ARG_VP(0, acicon_rainy),
                                                              ARG_V(3084), } },
  { ID_TEXT_WED,                APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_TEXT_WED,                APPW_SET_PROP_FONT,         { ARG_VPF(0, acNettoOT_16_Normal_EXT_AA4, &APPW__aFont[0]) } },
  { ID_TEXT_WED,                APPW_SET_PROP_TEXTID,       { ARG_V(ID_TEXT_WED_TXT) } },
  { ID_TEXT_WED_RANGE,          APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_TEXT_WED_RANGE,          APPW_SET_PROP_FONT,         { ARG_VPF(0, acNettoOT_16_Normal_EXT_AA4, &APPW__aFont[0]) } },
  { ID_IMAGE_THU_PRESSED,       APPW_SET_PROP_SBITMAP,      { ARG_VP(0, acbottom_button_trans_pressed),
                                                              ARG_V(659), } },
  { ID_IMAGE_THU,               APPW_SET_PROP_SBITMAP,      { ARG_VP(0, acbottom_button_trans),
                                                              ARG_V(660), } },
  { ID_IMAGE_RAINY_THU,         APPW_SET_PROP_SBITMAP,      { ARG_VP(0, acicon_rainy),
                                                              ARG_V(3084), } },
  { ID_TEXT_THU,                APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_TEXT_THU,                APPW_SET_PROP_FONT,         { ARG_VPF(0, acNettoOT_16_Normal_EXT_AA4, &APPW__aFont[0]) } },
  { ID_TEXT_THU,                APPW_SET_PROP_TEXTID,       { ARG_V(ID_TEXT_THU_TXT) } },
  { ID_TEXT_THU_RANGE,          APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_TEXT_THU_RANGE,          APPW_SET_PROP_FONT,         { ARG_VPF(0, acNettoOT_16_Normal_EXT_AA4, &APPW__aFont[0]) } },
  { ID_IMAGE_FRI_PRESSED,       APPW_SET_PROP_SBITMAP,      { ARG_VP(0, acbottom_button_trans_pressed),
                                                              ARG_V(659), } },
  { ID_IMAGE_FRI,               APPW_SET_PROP_SBITMAP,      { ARG_VP(0, acbottom_button_trans),
                                                              ARG_V(660), } },
  { ID_IMAGE_CLOUDY_FRI,        APPW_SET_PROP_SBITMAP,      { ARG_VP(0, acicon_cloudy),
                                                              ARG_V(2960), } },
  { ID_TEXT_FRI,                APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_TEXT_FRI,                APPW_SET_PROP_FONT,         { ARG_VPF(0, acNettoOT_16_Normal_EXT_AA4, &APPW__aFont[0]) } },
  { ID_TEXT_FRI,                APPW_SET_PROP_TEXTID,       { ARG_V(ID_TEXT_FRI_TXT) } },
  { ID_TEXT_FRI_RANGE,          APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_TEXT_FRI_RANGE,          APPW_SET_PROP_FONT,         { ARG_VPF(0, acNettoOT_16_Normal_EXT_AA4, &APPW__aFont[0]) } },
  { ID_IMAGE_SAT_PRESSED,       APPW_SET_PROP_SBITMAP,      { ARG_VP(0, acbottom_button_trans_pressed),
                                                              ARG_V(659), } },
  { ID_IMAGE_SAT,               APPW_SET_PROP_SBITMAP,      { ARG_VP(0, acbottom_button_trans),
                                                              ARG_V(660), } },
  { ID_IMAGE_SUNNY_SAT,         APPW_SET_PROP_SBITMAP,      { ARG_VP(0, acicon_sunny),
                                                              ARG_V(2344), } },
  { ID_TEXT_SAT,                APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_TEXT_SAT,                APPW_SET_PROP_FONT,         { ARG_VPF(0, acNettoOT_16_Normal_EXT_AA4, &APPW__aFont[0]) } },
  { ID_TEXT_SAT,                APPW_SET_PROP_TEXTID,       { ARG_V(ID_TEXT_SAT_TXT) } },
  { ID_TEXT_SAT_RANGE,          APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_TEXT_SAT_RANGE,          APPW_SET_PROP_FONT,         { ARG_VPF(0, acNettoOT_16_Normal_EXT_AA4, &APPW__aFont[0]) } },
};

/*********************************************************************
*
*       _aAction
*/
static GUI_CONST_STORAGE APPW_ACTION_ITEM _aAction[] = {
  { ID_SCREEN_MAIN,             APPW_NOTIFICATION_INITDIALOG,     ID_IMAGE_SUN_PRESSED,       APPW_JOB_SETVIS,         ID_SCREEN_MAIN__APPW_NOTIFICATION_INITDIALOG__ID_IMAGE_SUN_PRESSED__APPW_JOB_SETVIS,
    { ARG_V(APPW_SET_OFF),
    }, 0, NULL
  },
  { ID_SCREEN_MAIN,             APPW_NOTIFICATION_INITDIALOG,     ID_IMAGE_SUN,               APPW_JOB_SETVIS,         ID_SCREEN_MAIN__APPW_NOTIFICATION_INITDIALOG__ID_IMAGE_SUN__APPW_JOB_SETVIS,
    { ARG_V(APPW_SET_ON),
    }, 0, NULL
  },
  { ID_SCREEN_MAIN,             APPW_NOTIFICATION_INITDIALOG,     ID_IMAGE_MON_PRESSED,       APPW_JOB_SETVIS,         ID_SCREEN_MAIN__APPW_NOTIFICATION_INITDIALOG__ID_IMAGE_MON_PRESSED__APPW_JOB_SETVIS,
    { ARG_V(APPW_SET_OFF),
    }, 0, NULL
  },
  { ID_SCREEN_MAIN,             APPW_NOTIFICATION_INITDIALOG,     ID_IMAGE_MON,               APPW_JOB_SETVIS,         ID_SCREEN_MAIN__APPW_NOTIFICATION_INITDIALOG__ID_IMAGE_MON__APPW_JOB_SETVIS,
    { ARG_V(APPW_SET_ON),
    }, 0, NULL
  },
  { ID_SCREEN_MAIN,             APPW_NOTIFICATION_INITDIALOG,     ID_IMAGE_TUE_PRESSED,       APPW_JOB_SETVIS,         ID_SCREEN_MAIN__APPW_NOTIFICATION_INITDIALOG__ID_IMAGE_TUE_PRESSED__APPW_JOB_SETVIS,
    { ARG_V(APPW_SET_OFF),
    }, 0, NULL
  },
  { ID_SCREEN_MAIN,             APPW_NOTIFICATION_INITDIALOG,     ID_IMAGE_TUE,               APPW_JOB_SETVIS,         ID_SCREEN_MAIN__APPW_NOTIFICATION_INITDIALOG__ID_IMAGE_TUE__APPW_JOB_SETVIS,
    { ARG_V(APPW_SET_ON),
    }, 0, NULL
  },
  { ID_SCREEN_MAIN,             APPW_NOTIFICATION_INITDIALOG,     ID_IMAGE_WED_PRESSED,       APPW_JOB_SETVIS,         ID_SCREEN_MAIN__APPW_NOTIFICATION_INITDIALOG__ID_IMAGE_WED_PRESSED__APPW_JOB_SETVIS,
    { ARG_V(APPW_SET_OFF),
    }, 0, NULL
  },
  { ID_SCREEN_MAIN,             APPW_NOTIFICATION_INITDIALOG,     ID_IMAGE_WED,               APPW_JOB_SETVIS,         ID_SCREEN_MAIN__APPW_NOTIFICATION_INITDIALOG__ID_IMAGE_WED__APPW_JOB_SETVIS,
    { ARG_V(APPW_SET_ON),
    }, 0, NULL
  },
  { ID_SCREEN_MAIN,             APPW_NOTIFICATION_INITDIALOG,     ID_IMAGE_THU_PRESSED,       APPW_JOB_SETVIS,         ID_SCREEN_MAIN__APPW_NOTIFICATION_INITDIALOG__ID_IMAGE_THU_PRESSED__APPW_JOB_SETVIS,
    { ARG_V(APPW_SET_OFF),
    }, 0, NULL
  },
  { ID_SCREEN_MAIN,             APPW_NOTIFICATION_INITDIALOG,     ID_IMAGE_THU,               APPW_JOB_SETVIS,         ID_SCREEN_MAIN__APPW_NOTIFICATION_INITDIALOG__ID_IMAGE_THU__APPW_JOB_SETVIS,
    { ARG_V(APPW_SET_ON),
    }, 0, NULL
  },
  { ID_SCREEN_MAIN,             APPW_NOTIFICATION_INITDIALOG,     ID_IMAGE_FRI_PRESSED,       APPW_JOB_SETVIS,         ID_SCREEN_MAIN__APPW_NOTIFICATION_INITDIALOG__ID_IMAGE_FRI_PRESSED__APPW_JOB_SETVIS,
    { ARG_V(APPW_SET_OFF),
    }, 0, NULL
  },
  { ID_SCREEN_MAIN,             APPW_NOTIFICATION_INITDIALOG,     ID_IMAGE_FRI,               APPW_JOB_SETVIS,         ID_SCREEN_MAIN__APPW_NOTIFICATION_INITDIALOG__ID_IMAGE_FRI__APPW_JOB_SETVIS,
    { ARG_V(APPW_SET_ON),
    }, 0, NULL
  },
  { ID_SCREEN_MAIN,             APPW_NOTIFICATION_INITDIALOG,     ID_IMAGE_SAT_PRESSED,       APPW_JOB_SETVIS,         ID_SCREEN_MAIN__APPW_NOTIFICATION_INITDIALOG__ID_IMAGE_SAT_PRESSED__APPW_JOB_SETVIS,
    { ARG_V(APPW_SET_OFF),
    }, 0, NULL
  },
  { ID_SCREEN_MAIN,             APPW_NOTIFICATION_INITDIALOG,     ID_IMAGE_SAT,               APPW_JOB_SETVIS,         ID_SCREEN_MAIN__APPW_NOTIFICATION_INITDIALOG__ID_IMAGE_SAT__APPW_JOB_SETVIS,
    { ARG_V(APPW_SET_ON),
    }, 0, NULL
  },
  { ID_SCREEN_MAIN,             APPW_NOTIFICATION_INITDIALOG,     ID_VAR_TARGET_TEMP,         APPW_JOB_SETVALUE,       ID_SCREEN_MAIN__APPW_NOTIFICATION_INITDIALOG,
    { ARG_V(70),
    }, 0, NULL
  },
  { ID_VAR_TIME_UPDATE,         WM_NOTIFICATION_VALUE_CHANGED,    0,                          APPW_JOB_NULL,           ID_SCREEN_MAIN__WM_NOTIFICATION_VALUE_CHANGED_ID_VAR_TIME_UPDATE,
  },
  { ID_VAR_TARGET_TEMP,         WM_NOTIFICATION_VALUE_CHANGED,    ID_TEXT_TARGET_TEMP,        APPW_JOB_SETVALUE,       ID_SCREEN_MAIN__WM_NOTIFICATION_VALUE_CHANGED__ID_TEXT_TARGET_TEMP__APPW_JOB_SETVALUE,
    { ARG_V(4096),
    }, 65537, NULL
  },
  { ID_BUTTON_TEMP_UP,          WM_NOTIFICATION_CLICKED,          ID_VAR_TARGET_TEMP,         APPW_JOB_ADDVALUE,       ID_SCREEN_MAIN__ID_BUTTON_TEMP_UP__WM_NOTIFICATION_CLICKED,
    { ARG_V(-1),
    }, 1, NULL
  },
  { ID_BUTTON_TEMP_DOWN,        WM_NOTIFICATION_CLICKED,          ID_VAR_TARGET_TEMP,         APPW_JOB_ADDVALUE,       ID_SCREEN_MAIN__ID_BUTTON_TEMP_DOWN__WM_NOTIFICATION_CLICKED,
    { ARG_V(-1),
    }, 1, NULL
  },
  { ID_BUTTON_SUN,              WM_NOTIFICATION_CLICKED,          ID_IMAGE_SUN,               APPW_JOB_SETVIS,         ID_SCREEN_MAIN__ID_BUTTON_SUN__WM_NOTIFICATION_CLICKED__ID_IMAGE_SUN__APPW_JOB_SETVIS,
    { ARG_V(APPW_SET_TOGGLE),
    }, 0, NULL
  },
  { ID_BUTTON_SUN,              WM_NOTIFICATION_CLICKED,          ID_IMAGE_SUN_PRESSED,       APPW_JOB_SETVIS,         ID_SCREEN_MAIN__ID_BUTTON_SUN__WM_NOTIFICATION_CLICKED__ID_IMAGE_SUN_PRESSED__APPW_JOB_SETVIS,
    { ARG_V(APPW_SET_TOGGLE),
    }, 0, NULL
  },
  { ID_BUTTON_SUN,              WM_NOTIFICATION_CLICKED,          ID_IMAGE_ANIM_CLOUD_MOTION, APPW_JOB_SETCOORD,       ID_SCREEN_MAIN__ID_BUTTON_SUN__WM_NOTIFICATION_CLICKED__ID_IMAGE_ANIM_CLOUD_MOTION__APPW_JOB_SETCOORD,
    { ARG_V(-178),
      ARG_V(DISPOSE_INDEX_X0),
    }, 0, NULL
  },
  { ID_BUTTON_SUN,              WM_NOTIFICATION_RELEASED,         ID_IMAGE_SUN,               APPW_JOB_SETVIS,         ID_SCREEN_MAIN__ID_BUTTON_SUN__WM_NOTIFICATION_RELEASED__ID_IMAGE_SUN__APPW_JOB_SETVIS,
    { ARG_V(APPW_SET_TOGGLE),
    }, 0, NULL
  },
  { ID_BUTTON_SUN,              WM_NOTIFICATION_RELEASED,         ID_IMAGE_SUN_PRESSED,       APPW_JOB_SETVIS,         ID_SCREEN_MAIN__ID_BUTTON_SUN__WM_NOTIFICATION_RELEASED__ID_IMAGE_SUN_PRESSED__APPW_JOB_SETVIS,
    { ARG_V(APPW_SET_TOGGLE),
    }, 0, NULL
  },
  { ID_BUTTON_MON,              WM_NOTIFICATION_CLICKED,          ID_IMAGE_MON,               APPW_JOB_SETVIS,         ID_SCREEN_MAIN__ID_BUTTON_MON__WM_NOTIFICATION_CLICKED__ID_IMAGE_MON__APPW_JOB_SETVIS,
    { ARG_V(APPW_SET_TOGGLE),
    }, 0, NULL
  },
  { ID_BUTTON_MON,              WM_NOTIFICATION_CLICKED,          ID_IMAGE_MON_PRESSED,       APPW_JOB_SETVIS,         ID_SCREEN_MAIN__ID_BUTTON_MON__WM_NOTIFICATION_CLICKED__ID_IMAGE_MON_PRESSED__APPW_JOB_SETVIS,
    { ARG_V(APPW_SET_TOGGLE),
    }, 0, NULL
  },
  { ID_BUTTON_MON,              WM_NOTIFICATION_CLICKED,          0,                          APPW_JOB_NULL,           ID_SCREEN_MAIN__ID_BUTTON_MON__WM_NOTIFICATION_CLICKED,
  },
  { ID_BUTTON_MON,              WM_NOTIFICATION_CLICKED,          0,                          APPW_JOB_ANIMCREATE,     ID_SCREEN_MAIN__ID_BUTTON_MON__WM_NOTIFICATION_CLICKED_0,
    { ARG_P(&ID_ANIM_CLOUDY_MOTION_Data),
    }, 0, NULL
  },
  { ID_BUTTON_MON,              WM_NOTIFICATION_RELEASED,         ID_IMAGE_MON,               APPW_JOB_SETVIS,         ID_SCREEN_MAIN__ID_BUTTON_MON__WM_NOTIFICATION_RELEASED__ID_IMAGE_MON__APPW_JOB_SETVIS,
    { ARG_V(APPW_SET_TOGGLE),
    }, 0, NULL
  },
  { ID_BUTTON_MON,              WM_NOTIFICATION_RELEASED,         ID_IMAGE_MON_PRESSED,       APPW_JOB_SETVIS,         ID_SCREEN_MAIN__ID_BUTTON_MON__WM_NOTIFICATION_RELEASED__ID_IMAGE_MON_PRESSED__APPW_JOB_SETVIS,
    { ARG_V(APPW_SET_TOGGLE),
    }, 0, NULL
  },
  { ID_BUTTON_TUE,              WM_NOTIFICATION_CLICKED,          ID_IMAGE_TUE,               APPW_JOB_SETVIS,         ID_SCREEN_MAIN__ID_BUTTON_TUE__WM_NOTIFICATION_CLICKED__ID_IMAGE_TUE__APPW_JOB_SETVIS,
    { ARG_V(APPW_SET_TOGGLE),
    }, 0, NULL
  },
  { ID_BUTTON_TUE,              WM_NOTIFICATION_CLICKED,          ID_IMAGE_TUE_PRESSED,       APPW_JOB_SETVIS,         ID_SCREEN_MAIN__ID_BUTTON_TUE__WM_NOTIFICATION_CLICKED__ID_IMAGE_TUE_PRESSED__APPW_JOB_SETVIS,
    { ARG_V(APPW_SET_TOGGLE),
    }, 0, NULL
  },
  { ID_BUTTON_TUE,              WM_NOTIFICATION_CLICKED,          0,                          APPW_JOB_NULL,           ID_SCREEN_MAIN__ID_BUTTON_TUE__WM_NOTIFICATION_CLICKED_STOP_REMOVE_ANIM,
  },
  { ID_BUTTON_TUE,              WM_NOTIFICATION_CLICKED,          0,                          APPW_JOB_ANIMCREATE,     ID_SCREEN_MAIN__ID_BUTTON_TUE__WM_NOTIFICATION_CLICKED,
    { ARG_P(&ID_ANIM_CLOUDY_MOTION_Data),
    }, 0, NULL
  },
  { ID_BUTTON_TUE,              WM_NOTIFICATION_RELEASED,         ID_IMAGE_TUE,               APPW_JOB_SETVIS,         ID_SCREEN_MAIN__ID_BUTTON_TUE__WM_NOTIFICATION_RELEASED__ID_IMAGE_TUE__APPW_JOB_SETVIS,
    { ARG_V(APPW_SET_TOGGLE),
    }, 0, NULL
  },
  { ID_BUTTON_TUE,              WM_NOTIFICATION_RELEASED,         ID_IMAGE_TUE_PRESSED,       APPW_JOB_SETVIS,         ID_SCREEN_MAIN__ID_BUTTON_TUE__WM_NOTIFICATION_RELEASED__ID_IMAGE_TUE_PRESSED__APPW_JOB_SETVIS,
    { ARG_V(APPW_SET_TOGGLE),
    }, 0, NULL
  },
  { ID_BUTTON_WED,              WM_NOTIFICATION_CLICKED,          ID_IMAGE_WED,               APPW_JOB_SETVIS,         ID_SCREEN_MAIN__ID_BUTTON_WED__WM_NOTIFICATION_CLICKED__ID_IMAGE_WED__APPW_JOB_SETVIS,
    { ARG_V(APPW_SET_TOGGLE),
    }, 0, NULL
  },
  { ID_BUTTON_WED,              WM_NOTIFICATION_CLICKED,          ID_IMAGE_WED_PRESSED,       APPW_JOB_SETVIS,         ID_SCREEN_MAIN__ID_BUTTON_WED__WM_NOTIFICATION_CLICKED__ID_IMAGE_WED_PRESSED__APPW_JOB_SETVIS,
    { ARG_V(APPW_SET_TOGGLE),
    }, 0, NULL
  },
  { ID_BUTTON_WED,              WM_NOTIFICATION_CLICKED,          ID_IMAGE_ANIM_CLOUD_MOTION, APPW_JOB_SETCOORD,       ID_SCREEN_MAIN__ID_BUTTON_WED__WM_NOTIFICATION_CLICKED__ID_IMAGE_ANIM_CLOUD_MOTION__APPW_JOB_SETCOORD,
    { ARG_V(-178),
      ARG_V(DISPOSE_INDEX_X0),
    }, 0, NULL
  },
  { ID_BUTTON_WED,              WM_NOTIFICATION_RELEASED,         ID_IMAGE_WED,               APPW_JOB_SETVIS,         ID_SCREEN_MAIN__ID_BUTTON_WED__WM_NOTIFICATION_RELEASED__ID_IMAGE_WED__APPW_JOB_SETVIS,
    { ARG_V(APPW_SET_TOGGLE),
    }, 0, NULL
  },
  { ID_BUTTON_WED,              WM_NOTIFICATION_RELEASED,         ID_IMAGE_WED_PRESSED,       APPW_JOB_SETVIS,         ID_SCREEN_MAIN__ID_BUTTON_WED__WM_NOTIFICATION_RELEASED__ID_IMAGE_WED_PRESSED__APPW_JOB_SETVIS,
    { ARG_V(APPW_SET_TOGGLE),
    }, 0, NULL
  },
  { ID_BUTTON_THU,              WM_NOTIFICATION_CLICKED,          ID_IMAGE_THU,               APPW_JOB_SETVIS,         ID_SCREEN_MAIN__ID_BUTTON_THU__WM_NOTIFICATION_CLICKED__ID_IMAGE_THU__APPW_JOB_SETVIS,
    { ARG_V(APPW_SET_TOGGLE),
    }, 0, NULL
  },
  { ID_BUTTON_THU,              WM_NOTIFICATION_CLICKED,          ID_IMAGE_THU_PRESSED,       APPW_JOB_SETVIS,         ID_SCREEN_MAIN__ID_BUTTON_THU__WM_NOTIFICATION_CLICKED__ID_IMAGE_THU_PRESSED__APPW_JOB_SETVIS,
    { ARG_V(APPW_SET_TOGGLE),
    }, 0, NULL
  },
  { ID_BUTTON_THU,              WM_NOTIFICATION_CLICKED,          ID_IMAGE_ANIM_CLOUD_MOTION, APPW_JOB_SETCOORD,       ID_SCREEN_MAIN__ID_BUTTON_THU__WM_NOTIFICATION_CLICKED__ID_IMAGE_ANIM_CLOUD_MOTION__APPW_JOB_SETCOORD,
    { ARG_V(-178),
      ARG_V(DISPOSE_INDEX_X0),
    }, 0, NULL
  },
  { ID_BUTTON_THU,              WM_NOTIFICATION_RELEASED,         ID_IMAGE_THU,               APPW_JOB_SETVIS,         ID_SCREEN_MAIN__ID_BUTTON_THU__WM_NOTIFICATION_RELEASED__ID_IMAGE_THU__APPW_JOB_SETVIS,
    { ARG_V(APPW_SET_TOGGLE),
    }, 0, NULL
  },
  { ID_BUTTON_THU,              WM_NOTIFICATION_RELEASED,         ID_IMAGE_THU_PRESSED,       APPW_JOB_SETVIS,         ID_SCREEN_MAIN__ID_BUTTON_THU__WM_NOTIFICATION_RELEASED__ID_IMAGE_THU_PRESSED__APPW_JOB_SETVIS,
    { ARG_V(APPW_SET_TOGGLE),
    }, 0, NULL
  },
  { ID_BUTTON_FRI,              WM_NOTIFICATION_CLICKED,          ID_IMAGE_FRI,               APPW_JOB_SETVIS,         ID_SCREEN_MAIN__ID_BUTTON_FRI__WM_NOTIFICATION_CLICKED__ID_IMAGE_FRI__APPW_JOB_SETVIS,
    { ARG_V(APPW_SET_TOGGLE),
    }, 0, NULL
  },
  { ID_BUTTON_FRI,              WM_NOTIFICATION_CLICKED,          ID_IMAGE_FRI_PRESSED,       APPW_JOB_SETVIS,         ID_SCREEN_MAIN__ID_BUTTON_FRI__WM_NOTIFICATION_CLICKED__ID_IMAGE_FRI_PRESSED__APPW_JOB_SETVIS,
    { ARG_V(APPW_SET_TOGGLE),
    }, 0, NULL
  },
  { ID_BUTTON_FRI,              WM_NOTIFICATION_CLICKED,          0,                          APPW_JOB_NULL,           ID_SCREEN_MAIN__ID_BUTTON_FRI__WM_NOTIFICATION_CLICKED_STOP_REMOVE_ANIM,
  },
  { ID_BUTTON_FRI,              WM_NOTIFICATION_CLICKED,          0,                          APPW_JOB_ANIMCREATE,     ID_SCREEN_MAIN__ID_BUTTON_FRI__WM_NOTIFICATION_CLICKED_ANIM_CREATE,
    { ARG_P(&ID_ANIM_CLOUDY_MOTION_Data),
    }, 0, NULL
  },
  { ID_BUTTON_FRI,              WM_NOTIFICATION_RELEASED,         ID_IMAGE_FRI,               APPW_JOB_SETVIS,         ID_SCREEN_MAIN__ID_BUTTON_FRI__WM_NOTIFICATION_RELEASED__ID_IMAGE_FRI__APPW_JOB_SETVIS,
    { ARG_V(APPW_SET_TOGGLE),
    }, 0, NULL
  },
  { ID_BUTTON_FRI,              WM_NOTIFICATION_RELEASED,         ID_IMAGE_FRI_PRESSED,       APPW_JOB_SETVIS,         ID_SCREEN_MAIN__ID_BUTTON_FRI__WM_NOTIFICATION_RELEASED__ID_IMAGE_FRI_PRESSED__APPW_JOB_SETVIS,
    { ARG_V(APPW_SET_TOGGLE),
    }, 0, NULL
  },
  { ID_BUTTON_SAT,              WM_NOTIFICATION_CLICKED,          ID_IMAGE_SAT,               APPW_JOB_SETVIS,         ID_SCREEN_MAIN__ID_BUTTON_SAT__WM_NOTIFICATION_CLICKED__ID_IMAGE_SAT__APPW_JOB_SETVIS,
    { ARG_V(APPW_SET_TOGGLE),
    }, 0, NULL
  },
  { ID_BUTTON_SAT,              WM_NOTIFICATION_CLICKED,          ID_IMAGE_SAT_PRESSED,       APPW_JOB_SETVIS,         ID_SCREEN_MAIN__ID_BUTTON_SAT__WM_NOTIFICATION_CLICKED__ID_IMAGE_SAT_PRESSED__APPW_JOB_SETVIS,
    { ARG_V(APPW_SET_TOGGLE),
    }, 0, NULL
  },
  { ID_BUTTON_SAT,              WM_NOTIFICATION_CLICKED,          ID_IMAGE_ANIM_CLOUD_MOTION, APPW_JOB_SETCOORD,       ID_SCREEN_MAIN__ID_BUTTON_SAT__WM_NOTIFICATION_CLICKED__ID_IMAGE_ANIM_CLOUD_MOTION__APPW_JOB_SETCOORD,
    { ARG_V(-178),
      ARG_V(DISPOSE_INDEX_X0),
    }, 0, NULL
  },
  { ID_BUTTON_SAT,              WM_NOTIFICATION_RELEASED,         ID_IMAGE_SAT,               APPW_JOB_SETVIS,         ID_SCREEN_MAIN__ID_BUTTON_SAT__WM_NOTIFICATION_RELEASED__ID_IMAGE_SAT__APPW_JOB_SETVIS,
    { ARG_V(APPW_SET_TOGGLE),
    }, 0, NULL
  },
  { ID_BUTTON_SAT,              WM_NOTIFICATION_RELEASED,         ID_IMAGE_SAT_PRESSED,       APPW_JOB_SETVIS,         ID_SCREEN_MAIN__ID_BUTTON_SAT__WM_NOTIFICATION_RELEASED__ID_IMAGE_SAT_PRESSED__APPW_JOB_SETVIS,
    { ARG_V(APPW_SET_TOGGLE),
    }, 0, NULL
  },
  { ID_BUTTON_NEXT_SCREEN,      WM_NOTIFICATION_CLICKED,          0,                          APPW_JOB_SHOWSCREEN,     ID_SCREEN_MAIN__ID_BUTTON_NEXT_SCREEN__WM_NOTIFICATION_CLICKED,
    { ARG_V(ID_SCREEN_LOG),
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
*       ID_SCREEN_MAIN_RootInfo
*/
APPW_ROOT_INFO ID_SCREEN_MAIN_RootInfo = {
  ID_SCREEN_MAIN,
  _aCreate, GUI_COUNTOF(_aCreate),
  _aSetup,  GUI_COUNTOF(_aSetup),
  _aAction, GUI_COUNTOF(_aAction),
  cbID_SCREEN_MAIN,
  0
};

/*************************** End of file ****************************/

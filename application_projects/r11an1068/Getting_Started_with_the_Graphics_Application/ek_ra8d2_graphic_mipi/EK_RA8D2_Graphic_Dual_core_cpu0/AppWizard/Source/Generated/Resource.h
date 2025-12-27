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
File        : Resource.h
Purpose     : Generated file do NOT edit!
---------------------------END-OF-HEADER------------------------------
*/

#ifndef RESOURCE_H
#define RESOURCE_H

#include "AppWizard.h"

/*********************************************************************
*
*       Text
*/
#define ID_TEXT_THERMOSTAT_TXT 0
#define ID_TEXT_DEGR_F_TXT 1
#define ID_TEXT_MON_TXT 2
#define ID_TEXT_TUE_TXT 3
#define ID_TEXT_WED_TXT 4
#define ID_TEXT_THU_TXT 5
#define ID_TEXT_FRI_TXT 6
#define ID_TEXT_SAT_TXT 7
#define ID_TEXT_SUN_TXT 8
#define ID_TEXT_DEFAULT_RANGE 9
#define ID_TEXT_HUMIDITY_TXT 10
#define ID_TEXT_GO_BACK_TXT 11
#define ID_TEXT_LOG_LABEL_TXT 12
#define ID_TEXT_BACKLIGHT_TXT 13
#define ID_TEXT_COLOR_TXT 14
#define ID_TEXT_DARKMODE_TXT 15
#define ID_TEXT_NEXT_SCREEN 16

extern GUI_CONST_STORAGE unsigned char acAPPW_Language_0[];

/*********************************************************************
*
*       Font data
*/
extern APPW_FONT APPW__aFont[3];

/*********************************************************************
*
*       Fonts
*/
extern GUI_CONST_STORAGE unsigned char acNettoOT_24_Normal_EXT_AA4[];
extern GUI_CONST_STORAGE unsigned char acNettoOT_40_Normal_EXT_AA4[];
extern GUI_CONST_STORAGE unsigned char acNettoOT_32_Normal_EXT_AA4[];

/*********************************************************************
*
*       Images
*/
extern GUI_CONST_STORAGE unsigned char acsunny_portrait[];
extern GUI_CONST_STORAGE unsigned char acpartly_cloudy_portrait[];
extern GUI_CONST_STORAGE unsigned char acrainy_portrait[];
extern GUI_CONST_STORAGE unsigned char acbutton_plus[];
extern GUI_CONST_STORAGE unsigned char acbutton_plus_pressed[];
extern GUI_CONST_STORAGE unsigned char acbutton_minus[];
extern GUI_CONST_STORAGE unsigned char acbutton_minus_pressed[];
extern GUI_CONST_STORAGE unsigned char acbottom_button_trans[];
extern GUI_CONST_STORAGE unsigned char acbottom_button_trans_pressed[];
extern GUI_CONST_STORAGE unsigned char acicon_sunny[];
extern GUI_CONST_STORAGE unsigned char acicon_cloudy[];
extern GUI_CONST_STORAGE unsigned char acicon_rainy[];
extern GUI_CONST_STORAGE unsigned char acanime_sun_oncloud[];
extern GUI_CONST_STORAGE unsigned char acanime_cloud[];
extern GUI_CONST_STORAGE unsigned char acanime_sun_shining[];
extern GUI_CONST_STORAGE unsigned char acanime_raining_1[];
extern GUI_CONST_STORAGE unsigned char ac_20px_Renesas_Electronics_logo_svg[];
extern GUI_CONST_STORAGE unsigned char acDARK_Slider_Shaft_V_Bottom_Gray_16x30[];
extern GUI_CONST_STORAGE unsigned char acDARK_Slider_Shaft_V_Top_Blue_16x30[];
extern GUI_CONST_STORAGE unsigned char acDARK_Slider_Thumb_Up_22x22[];
extern GUI_CONST_STORAGE unsigned char acSwitch_Body_Blue_80x30[];
extern GUI_CONST_STORAGE unsigned char acSwitch_Body_WhiteBlackBorder_80x30[];
extern GUI_CONST_STORAGE unsigned char acSwitch_Thumb_WhiteBlackBorder_30x30[];
extern GUI_CONST_STORAGE unsigned char acDARK_Rotary_BG_Gray_80x80[];
extern GUI_CONST_STORAGE unsigned char acRotary_Marker_GreenBullet_11x11[];

/*********************************************************************
*
*       Variables
*/
#define ID_VAR_TARGET_TEMP (GUI_ID_USER + 2048)
#define ID_VAR_TIME_UPDATE (GUI_ID_USER + 2049)
#define ID_VAR_DARKMODE (GUI_ID_USER + 2050)
#define ID_VAR_BACKLIGHT (GUI_ID_USER + 2051)
#define ID_VAR_TEXT_COLOR_CHANGE (GUI_ID_USER + 2052)

/*********************************************************************
*
*       Animations
*/
#define ID_ANIM_CLOUDY_MOTION (GUI_ID_USER + 0)

extern GUI_CONST_STORAGE APPW_ANIM_DATA ID_ANIM_CLOUDY_MOTION_Data;

/*********************************************************************
*
*       Screens
*/
#define ID_SCREEN_MAIN (GUI_ID_USER + 4096)
#define ID_SCREEN_LOG (GUI_ID_USER + 4097)

extern APPW_ROOT_INFO ID_SCREEN_MAIN_RootInfo;
extern APPW_ROOT_INFO ID_SCREEN_LOG_RootInfo;

#define APPW_INITIAL_SCREEN &ID_SCREEN_MAIN_RootInfo

/*********************************************************************
*
*       Project path
*/
#define APPW_PROJECT_PATH "C:/ra-solutions-rvc/application_projects/ek_ra8d2_graphic_mipi/EK_RA8D2_Graphic_Dual_core_cpu0/AppWizard"

#endif  // RESOURCE_H

/*************************** End of file ****************************/

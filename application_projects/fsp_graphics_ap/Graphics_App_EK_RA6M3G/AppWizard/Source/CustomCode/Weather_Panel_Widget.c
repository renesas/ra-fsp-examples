/***********************************************************************************************************************
 * File Name    : Weather_Panel_Widget.c
 * Description  : Contains custom code for the Weather Panel.
 **********************************************************************************************************************/
/**********************************************************************************************************************
* Copyright (c) 2019 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*********************************************************************************************************************/
#include "Application.h"
#include <stdarg.h>
#include <stdio.h>
#include "timer_thread.h"

/* IMAGE IDs that named in AppWizard GUI*/
const int idImageBGRD[] = {
		ID_IMAGE_SUNNY_BGRD,
		ID_IMAGE_RAINY_BGRG,
		ID_IMAGE_CLOUDY_BGRD
};

/* ANIMATION IMAGE IDs that named in AppWizard GUI*/
const int idImageAnimBGRD[] = {
		ID_IMAGE_ANIM_SUNNY_BRGD,
		ID_IMAGE_ANIM_RAINY_BGRD,
		ID_IMAGE_ANIM_CLOUDY_BGRD,
		ID_IMAGE_ANIM_CLOUD_MOTION,
		ID_IMAGE_ANIM_RAINY_BGRD_2
};

/* Button's temp range texts */
const int idTextTempRange[] = {
		ID_TEXT_SUN_RANGE,
		ID_TEXT_MON_RANGE,
		ID_TEXT_TUE_RANGE,
		ID_TEXT_WED_RANGE,
		ID_TEXT_THU_RANGE,
		ID_TEXT_FRI_RANGE,
		ID_TEXT_SAT_RANGE
};

/* Object handles */
static WM_HWIN hImageAnimBGRD[5] 	 = {0};
static WM_HWIN hImageBGRD[3]     	 = {0};
static WM_HWIN hTextForeCastDay  	 = 0;
static WM_HWIN hTextForeCastSum  	 = 0;
static WM_HWIN hTextHumidityNo   	 = 0;
static WM_HWIN hTextForeCastTemp 	 = 0;
static WM_HWIN hTextCurrentDate 	 = 0;
static WM_HWIN hTextCurrentWeekDay 	 = 0;
static WM_HWIN hTextCurrentTime 	 = 0;
static WM_HWIN hTextTargetTemp 	 	 = 0;
static WM_HWIN hAmPm                 = 0;
static WM_HTIMER ghTimer             = 0;

static uint8_t   AnimRainyState      = 0;
rtc_time_t RtcTimeCurrent 	 		 = {0};

volatile app_data_info_t gDataApp    = {0};
char gDataLog[LOG_BUF_SIZE]          = {0};

/* Month/Week day/AMPM*/
const char* const wday_name[] = {
    "SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"
};
const char* const mon_name[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

/*******************************************************************************************************************//**
 * @brief       LogDataAppend
 * @brief       This function save the Weather Panel log
 * @param[IN]   Logging buffer
 * @param[IN]   Buffer size
 * @param[IN]   Input data
 * @retval      Length of string appending to logging buffer
 **********************************************************************************************************************/
static int LogDataAppend(char *buf, size_t buf_size,const char * restrict format, ...)
{
  va_list args                            = {0};
  static char lineBuf[LOG_CHAR_PER_LINE]  = {0};
  int  len                                = 0;

  va_start(args, format);
  len = vsnprintf(lineBuf, LOG_CHAR_PER_LINE, format, args);
  if (len > 0)
  {
    if (strlen(lineBuf) + strlen(buf) > buf_size)
    {
      /* Reset Logging buffer */
        memset(buf, 0, buf_size);
    }
    /* Add new log data*/
    strcat(buf, lineBuf);
  }
  va_end(args);

  return len;
}

/*******************************************************************************************************************//**
 * @brief       AppTimeUpdate
 * @brief       This function updates date and time
 * @param[IN]   WM_MESSAGE
 * @retval      None
 **********************************************************************************************************************/
static void AppTimeUpdate(void)
{
    static char temp[20] = {0};
    /* Update weekday text */
    sprintf(temp, "%.3s", wday_name[RtcTimeCurrent.tm_wday]);
    TEXT_SetText(hTextCurrentWeekDay, &temp[0]);

    /* Update current time text */
    if(12 > RtcTimeCurrent.tm_hour)
    {
        sprintf(temp, "%.2d:%.2d:%.2d", RtcTimeCurrent.tm_hour, RtcTimeCurrent.tm_min, RtcTimeCurrent.tm_sec);
        TEXT_SetText(hAmPm, "AM");
    }
    else
    {
        RtcTimeCurrent.tm_hour -= 12;
        sprintf(temp, "%.2d:%.2d:%.2d", RtcTimeCurrent.tm_hour, RtcTimeCurrent.tm_min, RtcTimeCurrent.tm_sec);
        TEXT_SetText(hAmPm, "PM");
    }
    TEXT_SetText(hTextCurrentTime, &temp[0]);

    /* Update current date text */
    sprintf(temp, "%.2d %.3s %d", RtcTimeCurrent.tm_mday, mon_name[RtcTimeCurrent.tm_mon], 1900 + RtcTimeCurrent.tm_year);
    TEXT_SetText(hTextCurrentDate, &temp[0]);
}

/*******************************************************************************************************************//**
 * @brief       SysForecastUpdate
 * @brief       This function updates weather forecast
 * @param[IN]   WM_MESSAGE
 * @retval      None
 **********************************************************************************************************************/
static void SysForecastUpdate(uint8_t sys_day_forecast, volatile weather_type_t *sys_weather, weather_type_t const sys_weather_type)
{
	/* Hide current Animation and Background images */
	if(sys_weather_type != *sys_weather)
	{
		/* Hide animation and background image */
		WM_HideWindow(hImageAnimBGRD[*sys_weather]);
		WM_HideWindow(hImageBGRD[*sys_weather]);
		/* Hide 2nd animation images */
		if(SYS_WEATHER_CLOUDY == *sys_weather)
		{
			WM_HideWindow(hImageAnimBGRD[IMAGE_ANIM_CLOUD_MOTION]);
		}
		else if(SYS_WEATHER_RAINY == *sys_weather)
		{
			WM_HideWindow(hImageAnimBGRD[IMAGE_ANIM_RAINY_BGRD_2]);
		}
	}
	/* Show the new animation and background images */
	WM_ShowWindow(hImageBGRD[sys_weather_type]);
	if(SYS_WEATHER_CLOUDY == sys_weather_type)
	{
		/* Show 1st and 2nd animation images */
		WM_ShowWindow(hImageAnimBGRD[sys_weather_type]);
		WM_ShowWindow(hImageAnimBGRD[IMAGE_ANIM_CLOUD_MOTION]);
	}
	else if(SYS_WEATHER_RAINY == sys_weather_type)
	{
	    /* Hide 2nd animation image */
	    WM_HideWindow(hImageAnimBGRD[IMAGE_ANIM_RAINY_BGRD_2]);
        /* Show 1st animation image */
        WM_ShowWindow(hImageAnimBGRD[sys_weather_type]);
	}
	else if(SYS_WEATHER_SUNNY == sys_weather_type)
	{
		/* Show 1st animation image */
		WM_ShowWindow(hImageAnimBGRD[sys_weather_type]);
	}
	/* Update weather forecast */
    gDataApp.current_forecast = sys_day_forecast;
    TEXT_SetText(hTextForeCastDay, gDataApp.forecast_info[gDataApp.current_forecast].day_name);
    TEXT_SetText(hTextForeCastSum, gDataApp.forecast_info[gDataApp.current_forecast].description);
    TEXT_SetText(hTextHumidityNo, gDataApp.forecast_info[gDataApp.current_forecast].humidity);
    TEXT_SetText(hTextForeCastTemp, gDataApp.forecast_info[gDataApp.current_forecast].current_temp);
	/* Set new weather type */
	*sys_weather = sys_weather_type;
}

/*******************************************************************************************************************//**
 * @brief       WeatherForecastInit
 * @brief       This function get GUI objects, initializes weather forecast data, date and time
 * @param[IN]   WM_MESSAGE
 * @retval      FSP_SUCCESS     Upon successful get GUI objects
 * @retval      FSP_ERR_INVALID_POINTER if failed
 **********************************************************************************************************************/
static fsp_err_t WeatherForecastInit(WM_MESSAGE * pMsg){

	  WM_HWIN   hWin_temp 		 = 0;
	  /* Update button's temp range texts */
	  for(int i= 0; i<gDataApp.total_forecast; i++)
	  {
		  hWin_temp = WM_GetDialogItem(pMsg->hWin, idTextTempRange[i]);
		  if(0 == hWin_temp)
		  {
			 return FSP_ERR_INVALID_POINTER;
		  }
		  else
		  {
			  TEXT_SetText(hWin_temp, gDataApp.forecast_info[i].temp_range);
			  hWin_temp = 0;
		  }
	  }
	  /* Set default weather forecast (Sunday)*/
	  hTextForeCastDay  = WM_GetDialogItem(pMsg->hWin, ID_TEXT_FORECAST_DAY);
	  if(0 == hTextForeCastDay)
	  {
		  return FSP_ERR_INVALID_POINTER;
	  }
	  else
	  {
		  TEXT_SetText(hTextForeCastDay, gDataApp.forecast_info[gDataApp.current_forecast].day_name);
	  }

	  hTextForeCastSum = WM_GetDialogItem(pMsg->hWin, ID_TEXT_FORECAST_SUM);
	  if(0 == hTextForeCastSum)
	  {
		  return FSP_ERR_INVALID_POINTER;
	  }
	  else
	  {
		  TEXT_SetText(hTextForeCastSum, gDataApp.forecast_info[gDataApp.current_forecast].description);
	  }

	  hTextHumidityNo = WM_GetDialogItem(pMsg->hWin, ID_TEXT_HUMIDITY_NO);
	  if(0 == hTextHumidityNo)
	  {
		  return FSP_ERR_INVALID_POINTER;
	  }
	  else
	  {
		  TEXT_SetText(hTextHumidityNo, gDataApp.forecast_info[gDataApp.current_forecast].humidity);
	  }

	  hTextForeCastTemp = WM_GetDialogItem(pMsg->hWin, ID_TEXT_FORECAST_TEMP);
	  if(0 == hTextForeCastTemp)
	  {
		  return FSP_ERR_INVALID_POINTER;
	  }
	  else
	  {
		  TEXT_SetText(hTextForeCastTemp, gDataApp.forecast_info[gDataApp.current_forecast].current_temp);
	  }

	  hTextTargetTemp = WM_GetDialogItem(pMsg->hWin, ID_TEXT_TARGET_TEMP);
      if(0 == hTextTargetTemp)
      {
          return FSP_ERR_INVALID_POINTER;
      }

	  /* Date and time */
	  hTextCurrentDate = WM_GetDialogItem(pMsg->hWin, ID_TEXT_CURRENT_DATE);
	  if(0 == hTextCurrentDate)
	  {
		  return FSP_ERR_INVALID_POINTER;
	  }

	  hTextCurrentTime = WM_GetDialogItem(pMsg->hWin, ID_TEXT_CURRENT_TIME);
	  if(0 == hTextCurrentTime)
	  {
		  return FSP_ERR_INVALID_POINTER;
	  }

	  hTextCurrentWeekDay = WM_GetDialogItem(pMsg->hWin, ID_TEXT_CURRENT_WEEK_DAY);
	  if(0 == hTextCurrentWeekDay)
	  {
		  return FSP_ERR_INVALID_POINTER;
	  }

	  hAmPm = WM_GetDialogItem(pMsg->hWin, ID_TEXT_AMPM);
	  if(0 == hAmPm)
	  {
		  return FSP_ERR_INVALID_POINTER;
	  }

	  /* Update date, time */
	  AppTimeUpdate();

	  return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief       ImageHandleGet
 * @brief       This function gets and stores image handles
 * @param[IN]   WM_MESSAGE
 * @retval      FSP_SUCCESS     Upon successful get image objects
 * @retval      FSP_ERR_INVALID_POINTER if failed
 **********************************************************************************************************************/
static fsp_err_t ImageHandleGet(WM_MESSAGE * pMsg){
      WM_HWIN hWin_temp = 0;
	  /* Get and store image's handles */
	  for (int i = 0; i<3; i++)
	  {
		  hImageBGRD[i] = WM_GetDialogItem(pMsg->hWin, idImageBGRD[i]);
		  if(0 == hImageBGRD[i])
		  {
			  return FSP_ERR_INVALID_POINTER;
		  }
		  else
		  {
			  if(gDataApp.sys_weather_type == i)
			  {
				  WM_ShowWindow(hImageBGRD[i]);
			  }
			  else
			  {
				  WM_HideWindow(hImageBGRD[i]);
			  }
		  }
	  }
	  /* Animation Window handle */
	  hWin_temp = WM_GetDialogItem(pMsg->hWin, ID_WINDOW_ANIMATION);
	  if(0 == hWin_temp)
	  {
	    return FSP_ERR_INVALID_POINTER;
	  }
	  /* Animation images's handles */
	  for (int i = 0; i<5; i++)
	  {
		  hImageAnimBGRD[i] = WM_GetDialogItem(hWin_temp, idImageAnimBGRD[i]);
		  if(0 == hImageAnimBGRD[i])
		  {
			  return FSP_ERR_INVALID_POINTER;
		  }
		  else
		  {
		      if(gDataApp.sys_weather_type == i)
			  {
				  WM_ShowWindow(hImageAnimBGRD[i]);
			  }
			  else
			  {
				  WM_HideWindow(hImageAnimBGRD[i]);
			  }
		  }
	  }
	  return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief       Custom code for cbID_SCREEN_MAIN in ID_SCREEN_MAIN_Slots.c
 * @brief       This function initializes GUI objects, data and handle GUI events
 * @param[IN]   WM_MESSAGE
 * @retval      None
 **********************************************************************************************************************/
void cuscbID_SCREEN_MAIN(WM_MESSAGE * pMsg) {
  int wId             		 = 0;
  int wMsg            		 = 0;

  switch(pMsg->MsgId) {
	  case WM_INIT_DIALOG:
		  /* Get and store Images's handles */
		  if(ImageHandleGet(pMsg))
		  {
			  APP_ERR_TRAP(FSP_ERR_INVALID_POINTER);
		  }
          /* Set default weather forecast */
          if(WeatherForecastInit(pMsg))
           {
                APP_ERR_TRAP(FSP_ERR_INVALID_POINTER);
           }
          /* Set Thermostat target temperature */
          APPW_SetVarData(ID_VAR_TARGET_TEMP, gDataApp.thermo_target_temp);
          /* Save logging */
          LogDataAppend(gDataLog, sizeof(gDataLog),"\n%s", "Init Dialog");
		  /* Create timer to control effects/animation*/
		  ghTimer = WM_CreateTimer(pMsg->hWin, 0, ANIM_TIMER_PERIOD, 0);
		  break;
	  case WM_TIMER:
		  /* Rainy effect*/
		  if(SYS_WEATHER_RAINY == gDataApp.sys_weather_type)
		  {
			  if(0 == AnimRainyState)
			  {
				  /* Hide 1st animation image, show 2nd animation image */
				  WM_HideWindow(hImageAnimBGRD[gDataApp.sys_weather_type]);
				  WM_ShowWindow(hImageAnimBGRD[IMAGE_ANIM_RAINY_BGRD_2]);
				  AnimRainyState = 1;
			  }
			  else
			  {
				  /* Hide 2nd animation image, show 1st animation image */
				  WM_HideWindow(hImageAnimBGRD[IMAGE_ANIM_RAINY_BGRD_2]);
				  WM_ShowWindow(hImageAnimBGRD[gDataApp.sys_weather_type]);
				  AnimRainyState = 0;
			  }
			  /* Restart timer */
              if(ghTimer)
              {
                  WM_RestartTimer(ghTimer, ANIM_TIMER_PERIOD);
              }
		  }
		  break;
	  case WM_NOTIFY_PARENT:
	    wId    = WM_GetId(pMsg->hWinSrc);
	    wMsg = pMsg->Data.v;
	    switch (wId) {
			case ID_BUTTON_WED:
				if(WM_NOTIFICATION_CLICKED == wMsg)
				{
					SysForecastUpdate(WEEK_DAY_WED , &gDataApp.sys_weather_type, SYS_WEATHER_RAINY);
					/* Save logging */
					LogDataAppend(gDataLog, sizeof(gDataLog),"\n%s:%s", LOG_TAG1, wday_name[WEEK_DAY_WED]);
	                /* Set initial state to toggle between 2 rainy images, triggering by timer */
	                AnimRainyState  = 0;
	                /* Restart timer */
	                if(ghTimer)
	                {
	                    WM_RestartTimer(ghTimer,  ANIM_TIMER_PERIOD);
	                }
				}
				break;
			case ID_BUTTON_THU:
				if(WM_NOTIFICATION_CLICKED == wMsg)
				{
					SysForecastUpdate(WEEK_DAY_THU, &gDataApp.sys_weather_type, SYS_WEATHER_RAINY);
					/* Save logging */
					LogDataAppend(gDataLog, sizeof(gDataLog),"\n%s:%s", LOG_TAG1, wday_name[WEEK_DAY_THU]);
	                /* Set initial state to toggle between 2 rainy images,  triggering by timer */
	                AnimRainyState  = 0;
	                /* Restart timer */
	                if(ghTimer)
	                {
	                    WM_RestartTimer(ghTimer,  ANIM_TIMER_PERIOD);
	                }
				}

				break;
			case ID_BUTTON_SUN:
				if(WM_NOTIFICATION_CLICKED == wMsg)
				{
					SysForecastUpdate(WEEK_DAY_SUN, &gDataApp.sys_weather_type, SYS_WEATHER_SUNNY);
                    /* Save logging */
					LogDataAppend(gDataLog, sizeof(gDataLog),"\n%s:%s", LOG_TAG1, wday_name[WEEK_DAY_SUN]);
				}
				break;
			case ID_BUTTON_SAT:
				if(WM_NOTIFICATION_CLICKED == wMsg)
				{
					SysForecastUpdate(WEEK_DAY_SAT, &gDataApp.sys_weather_type, SYS_WEATHER_SUNNY);
                    /* Save logging */
					LogDataAppend(gDataLog, sizeof(gDataLog),"\n%s:%s", LOG_TAG1, wday_name[WEEK_DAY_SAT]);
				}
				break;
			case ID_BUTTON_MON:
				if(WM_NOTIFICATION_CLICKED == wMsg)
				{
					SysForecastUpdate(WEEK_DAY_MON, &gDataApp.sys_weather_type, SYS_WEATHER_CLOUDY);
                    /* Save logging */
					LogDataAppend(gDataLog, sizeof(gDataLog),"\n%s:%s", LOG_TAG1, wday_name[WEEK_DAY_MON]);
				}
				break;
			case ID_BUTTON_TUE:
				if(WM_NOTIFICATION_CLICKED == wMsg)
				{
					SysForecastUpdate(WEEK_DAY_TUE, &gDataApp.sys_weather_type, SYS_WEATHER_CLOUDY);
                    /* Save logging */
					LogDataAppend(gDataLog, sizeof(gDataLog),"\n%s:%s", LOG_TAG1, wday_name[WEEK_DAY_TUE]);
				}
				break;
			case ID_BUTTON_FRI:
				if(WM_NOTIFICATION_CLICKED == wMsg)
				{
					SysForecastUpdate(WEEK_DAY_FRI, &gDataApp.sys_weather_type, SYS_WEATHER_CLOUDY);
                    /* Save logging */
					LogDataAppend(gDataLog, sizeof(gDataLog),"\n%s:%s", LOG_TAG1, wday_name[WEEK_DAY_FRI]);
				}
			    break;
	    }
	    break;

  }
}

/*******************************************************************************************************************//**
 * @brief       Custom code for ID_SCREEN_MAIN__ID_BUTTON_MON__WM_NOTIFICATION_CLICKED__ID_IMAGE_ANIM_CLOUD_MOTION__APPW_JOB_SETCOORD
 * @brief                       ID_SCREEN_MAIN__ID_BUTTON_TUE__WM_NOTIFICATION_CLICKED__ID_IMAGE_ANIM_CLOUD_MOTION__APPW_JOB_SETCOORD
 * @brief                       ID_SCREEN_MAIN__ID_BUTTON_FRI__WM_NOTIFICATION_CLICKED__ID_IMAGE_ANIM_CLOUD_MOTION__APPW_JOB_SETCOORD
 * @brief       in ID_SCREEN_MAIN_Slots.c
 * @brief       This function stops the existing Animation
 * @param[IN]   WM_MESSAGE
 * @retval      None
 **********************************************************************************************************************/
void cusID_SCREEN_MAIN__ID_BUTTON_MON_TUE_FRI___WM_NOTIFICATION_CLICKED__ID_IMAGE_ANIM_CLOUD_MOTION__APPW_JOB_SETCOORD(WM_MESSAGE * pMsg) {

  /* Stop current/first animation */
  if(WM_NOTIFY_PARENT == pMsg->MsgId)
  {
      GUI_ANIM_HANDLE hAnim = 0;
      hAnim = GUI_ANIM_GetFirst();
      if (0 != hAnim)
      {
          GUI_ANIM_Stop(hAnim);
      }
  }
}

/*******************************************************************************************************************//**
 * @brief       Custom code for ID_SCREEN_MAIN__ID_BUTTON_TEMP_DOWN__WM_NOTIFICATION_CLICKED in ID_SCREEN_MAIN_Slots.c
 * @brief       This function reduces Thermostat's target temperature, save the event to log
 * @param[IN]   WM_MESSAGE
 * @retval      None
 **********************************************************************************************************************/
void cusID_SCREEN_MAIN__ID_BUTTON_TEMP_DOWN__WM_NOTIFICATION_CLICKED(WM_MESSAGE * pMsg) {
  if(WM_NOTIFY_PARENT == pMsg->MsgId)
  {
      int gui_err = 0;
      gDataApp.thermo_target_temp = (uint8_t)APPW_GetVarData(ID_VAR_TARGET_TEMP, &gui_err);
	  if(THERMO_TARGET_TEMP_LOW <= gDataApp.thermo_target_temp)
	   {
	      gDataApp.thermo_target_temp--;
		  APPW_SetVarData (ID_VAR_TARGET_TEMP, gDataApp.thermo_target_temp);
		  LogDataAppend(gDataLog, sizeof(gDataLog),"\n%s:%d", LOG_TAG2, gDataApp.thermo_target_temp);
	   }
  }
}

/*******************************************************************************************************************//**
 * @brief       Custom code for ID_SCREEN_MAIN__ID_BUTTON_TEMP_UP__WM_NOTIFICATION_CLICKED in ID_SCREEN_MAIN_Slots.c
 * @brief       This function increases Thermostat's target temperature, save the event to log
 * @param[IN]   WM_MESSAGE
 * @retval      None
 **********************************************************************************************************************/
void cusID_SCREEN_MAIN__ID_BUTTON_TEMP_UP__WM_NOTIFICATION_CLICKED(WM_MESSAGE * pMsg) {
  if(WM_NOTIFY_PARENT == pMsg->MsgId)
  {
      int gui_err = 0;
      gDataApp.thermo_target_temp = (uint8_t)APPW_GetVarData (ID_VAR_TARGET_TEMP, &gui_err);
	  if(THERMO_TARGET_TEMP_HIGH >= gDataApp.thermo_target_temp)
	   {
	      gDataApp.thermo_target_temp++;
		  APPW_SetVarData (ID_VAR_TARGET_TEMP, gDataApp.thermo_target_temp);
		  LogDataAppend(gDataLog, sizeof(gDataLog),"\n%s:%d", LOG_TAG2, gDataApp.thermo_target_temp);
	   }
   }
}

/*******************************************************************************************************************//**
 * @brief       Custom code for ID_SCREEN_MAIN__WM_NOTIFICATION_VALUE_CHANGED in ID_SCREEN_MAIN_Slots.c
 * @brief       This function updates date time every second
 * @param[IN]   WM_MESSAGE
 * @retval      None
 **********************************************************************************************************************/
void cusID_SCREEN_MAIN__WM_NOTIFICATION_VALUE_CHANGED(WM_MESSAGE * pMsg) {
  if(WM_NOTIFY_PARENT == pMsg->MsgId)
  {
    /* Update time display */
    AppTimeUpdate();
  }
}

/*******************************************************************************************************************//**
 * @brief       Custom code for ID_SCREEN_MAIN__ID_BUTTON_NEXT_SCREEN__WM_NOTIFICATION_CLICKED in ID_SCREEN_MAIN_Slots.c
 * @brief       This function stops the existing animation and delete GUI timer if any
 * @param[IN]   WM_MESSAGE
 * @retval      None
 **********************************************************************************************************************/
void cusID_SCREEN_MAIN__ID_BUTTON_NEXT_SCREEN__WM_NOTIFICATION_CLICKED(WM_MESSAGE * pMsg) {
  if(WM_NOTIFY_PARENT == pMsg->MsgId)
  {
      GUI_ANIM_HANDLE hAnim = 0;
      /* Stop current animations if any */
      hAnim = GUI_ANIM_GetFirst();
      if(hAnim)
      {
          GUI_ANIM_Stop(hAnim);
      }
      /* Delete timer for RAINY animation */
      if(ghTimer)
      {
          WM_DeleteTimer(ghTimer);
      }

    }
}

/*************************** End of file ****************************/

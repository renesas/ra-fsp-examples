/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/***********************************************************************************************************************
 * File Name    : Weather_Panel_Widget.c
 * Description  : Contains custom code for the Weather Panel.
 **********************************************************************************************************************/

#include "Application.h"
#include <stdarg.h>
#include <stdio.h>

/* IMAGE IDs that named in AppWizard GUI*/
const int idImageBGD[] = {
		ID_IMAGE_SUNNY_BGD,
		ID_IMAGE_RAINY_BGD,
		ID_IMAGE_CLOUDY_BGD
};

/* ANIMATION IMAGE IDs that named in AppWizard GUI*/
const int idImageAnimBGD[] = {
        ID_IMAGE_ANIM_SUNNY_BGD,
        ID_IMAGE_ANIM_RAINY_BGD,
        ID_IMAGE_ANIM_CLOUDY_BGD,
        ID_IMAGE_ANIM_CLOUD_MOTION,
 };

/* Button's temp range texts */
const int idTextTempRange[] = {
		ID_TEXT_TEXT_SUN_RANGE,
		ID_TEXT_TEXT_MON_RANGE,
		ID_TEXT_TEXT_TUE_RANGE,
		ID_TEXT_TEXT_WED_RANGE,
		ID_TEXT_TEXT_THU_RANGE,
		ID_TEXT_TEXT_FRI_RANGE,
		ID_TEXT_TEXT_SAT_RANGE
};

/* Object handles */
static WM_HWIN hImageAnimBGD[5] 	        = {0};
static WM_HWIN hImageBGD[3]     	        = {0};
static WM_HWIN hTextForeCastDay  	        = 0;
static WM_HWIN hTextForeCastSum  	        = 0;
static WM_HWIN hTextHumidityNo   	        = 0;
static WM_HWIN hTextForeCastTemp 	        = 0;
static WM_HWIN hTextCurrentrtc 	            = 0;

static volatile WM_HWIN hTextTargetTemp 	= 0;
static WM_HTIMER ghTimer                    = 0;

static uint8_t   AnimRainyState             = 0;

/* Get the buffer address store current time info */
static char *rtc_buf = (char *)BSP_PARTITION_SHARED_MEM_START;

volatile app_data_info_t gDataApp    = {0};
char gDataLog[LOG_BUF_SIZE]          = {0};

/* Month/Week day*/
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

    TEXT_SetText(hTextCurrentrtc, rtc_buf);
    R_IPC_EventGenerate(&g_ipc1_ctrl, IPC_GENERATE_EVENT_IRQ3);
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
		WM_HideWindow(hImageAnimBGD[*sys_weather]);
		WM_HideWindow(hImageBGD[*sys_weather]);
		/* Hide 2nd animation images */
		if(SYS_WEATHER_CLOUDY == *sys_weather)
		{
			WM_HideWindow(hImageAnimBGD[IMAGE_ANIM_CLOUD_MOTION]);
		}
	}
	/* Show the new animation and background images */
	WM_ShowWindow(hImageBGD[sys_weather_type]);
	if(SYS_WEATHER_CLOUDY == sys_weather_type)
	{
		/* Show 1st and 2nd animation images */
		WM_ShowWindow(hImageAnimBGD[sys_weather_type]);
		WM_ShowWindow(hImageAnimBGD[IMAGE_ANIM_CLOUD_MOTION]);
	}
	else if(SYS_WEATHER_RAINY == sys_weather_type)
	{
        /* Show rainy animation image */
        WM_ShowWindow(hImageAnimBGD[sys_weather_type]);
	}
	else if(SYS_WEATHER_SUNNY == sys_weather_type)
	{
		/* Show 1st animation image */
		WM_ShowWindow(hImageAnimBGD[sys_weather_type]);
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

	  hTextCurrentrtc = WM_GetDialogItem(pMsg->hWin, ID_TEXT_CURRENT_RTC);
	  if(0 == hTextCurrentrtc)
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
	  /* Get and store image's handles */
	  for (int i = 0; i<3; i++)
	  {
		  hImageBGD[i] = WM_GetDialogItem(pMsg->hWin, idImageBGD[i]);

		  if(0 == hImageBGD[i])
		  {
			  return FSP_ERR_INVALID_POINTER;
		  }
		  else
		  {
			  if(gDataApp.sys_weather_type == i)
			  {
				  WM_ShowWindow(hImageBGD[i]);
			  }
			  else
			  {
				  WM_HideWindow(hImageBGD[i]);
			  }
		  }
	  }

	  for (int j = 0; j<4; j++)
	  {
		  hImageAnimBGD[j] = WM_GetDialogItem(pMsg->hWin, idImageAnimBGD[j]);
		  if(0 == hImageAnimBGD[j])
		  {
			  return FSP_ERR_INVALID_POINTER;
		  }
		  else
		  {
		      if(gDataApp.sys_weather_type == j)
			  {
				  WM_ShowWindow(hImageAnimBGD[j]);
			  }
			  else
			  {
				  WM_HideWindow(hImageAnimBGD[j]);
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
				  /* show rainy animation image */
				  WM_ShowWindow(hImageAnimBGD[gDataApp.sys_weather_type]);
				  AnimRainyState = 1;
			  }
			  else
			  {
				  /* Hide rainy animation image */
				  WM_HideWindow(hImageAnimBGD[gDataApp.sys_weather_type]);
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
 * @brief       Custom code for ID_SCREEN_MAIN__ID_BUTTON_MON__WM_NOTIFICATION_CLICKED_STOP_REMOVE_ANIM
 * @brief                       ID_SCREEN_MAIN__ID_BUTTON_TUE__WM_NOTIFICATION_CLICKED_STOP_REMOVE_ANIM
 * @brief                       ID_SCREEN_MAIN__ID_BUTTON_FRI__WM_NOTIFICATION_CLICKED_STOP_REMOVE_ANIM
 * @brief       in ID_SCREEN_MAIN_Slots.c
 * @brief       This function stops and removes the existing Animation
 * @param[IN]   WM_MESSAGE
 * @retval      None
 **********************************************************************************************************************/
void cusID_SCREEN_MAIN__WM_NOTIFICATION_CLICKED_STOP_REMOVE_ANIM(WM_MESSAGE * pMsg) {
  /* Stop ID_ANIM_CLOUDY_MOTIO animation */
  if(WM_NOTIFY_PARENT == pMsg->MsgId)
  {
      GUI_ANIM_HANDLE hAnim = 0;
      APPW_PARA_ITEM *pPara    = NULL;
      APPW_PARA_ITEM  dat_Anim[2] = {ARG_V(ID_ANIM_CLOUDY_MOTION), ARG_V(1)};
      pPara = &dat_Anim[0];
      hAnim = GUI_ANIM_Get(ID_ANIM_CLOUDY_MOTION);
      if (0 != hAnim)
      {
          if(GUI_ANIM_IsRunning(hAnim))
            {
              /* Stop and remove ID_ANIM_CLOUDY_MOTION animation */
              APPW_DoJob(ID_SCREEN_MAIN, ID_ANIM_CLOUDY_MOTION, APPW_JOB_ANIMSTOP, pPara);
            }
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
void cusID_SCREEN_MAIN__WM_NOTIFICATION_VALUE_CHANGED_ID_VAR_TIME_UPDATE(WM_MESSAGE * pMsg) {
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
      /* Delete timer for RAINY animation */
      if(ghTimer)
      {
          WM_DeleteTimer(ghTimer);
      }

    }
}

/*************************** End of file ****************************/

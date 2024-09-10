/***********************************************************************************************************************
 * File Name    : Log_Panel_Widget.c
 * Description  : Contains custom code for the Logging Panel.
 **********************************************************************************************************************/
/**********************************************************************************************************************
* Copyright (c) 2019 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*********************************************************************************************************************/
#include <timer_thread.h>
#include "Application.h"

/* Global variables */
static WM_HWIN ghMultiEdit = 0;
extern char gDataLog[LOG_BUF_SIZE];

/*******************************************************************************************************************//**
 * @brief       RotaryInit
 * @brief       This function initializes the Rotary object, set MultiEdit text color
 * @param[IN]   WM_MESSAGE
 * @retval      FSP_SUCCESS     Upon successful initialize the Rotary object
 * @retval      FSP_ERR_INVALID_POINTER if failed
 **********************************************************************************************************************/
static fsp_err_t RotaryInit(WM_MESSAGE * pMsg)
{
    WM_HWIN hWin_temp = 0;
    /* Initialize Rotary */
    hWin_temp = WM_GetDialogItem(pMsg->hWin, ID_ROTARY_TEXT_COLOR);
    if(hWin_temp)
    {
      APPW_SetVarData(ID_VAR_TEXT_COLOR_CHANGE, gDataApp.text_color);
      ROTARY_SetValue(hWin_temp,(long int)gDataApp.text_color);
      MULTIEDIT_SetTextColor(ghMultiEdit, MULTIEDIT_CI_READONLY, GUI_MAKE_COLOR(0x00FFFFFF - gDataApp.text_color));
    }
    else
    {
      return FSP_ERR_INVALID_POINTER;;
    }

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief       SliderInit
 * @brief       This function initializes the Slider object, set LCD backlight intensity
 * @param[IN]   WM_MESSAGE
 * @retval      FSP_SUCCESS     Upon successful initialize the Slider object
 * @retval      FSP_ERR_INVALID_POINTER if failed
 **********************************************************************************************************************/
static fsp_err_t SliderInit(WM_MESSAGE * pMsg)
{
    WM_HWIN hWin_temp = 0;
    /* Initialize the Slider, set LCD backlight intensity (Control PWM duty cycle) */
    APPW_SetVarData(ID_VAR_BACKLIGHT, gDataApp.pwm_duty_cycle);
    hWin_temp = WM_GetDialogItem(pMsg->hWin, ID_SLIDER_BACKLIGHT);
    if(hWin_temp)
    {
      SLIDER_SetValue(hWin_temp, gDataApp.pwm_duty_cycle);
    }
    else
    {
        return FSP_ERR_INVALID_POINTER;
    }
    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief       SwitchInit
 * @brief       This function initializes the Switch object, set MultiEdit background
 * @param[IN]   WM_MESSAGE
 * @retval      FSP_SUCCESS     Upon successful initialize the Switch object
 * @retval      FSP_ERR_INVALID_POINTER if failed
 **********************************************************************************************************************/
static fsp_err_t SwitchInit(WM_MESSAGE * pMsg)
{
    WM_HWIN hWin_temp = 0;
    /* Set switch and MultiEdit background */
    APPW_SetVarData(ID_VAR_DARKMODE, gDataApp.bgrd_darmode);
    hWin_temp = WM_GetDialogItem(pMsg->hWin, ID_SWITCH_DARKMODE);
    if(hWin_temp)
    {
      SWITCH_SetState(hWin_temp, gDataApp.bgrd_darmode);
      if(gDataApp.bgrd_darmode)
      {
          MULTIEDIT_SetBkColor(ghMultiEdit,  MULTIEDIT_CI_READONLY, GUI_BLACK);
      }
      else
      {
          MULTIEDIT_SetBkColor(ghMultiEdit,  MULTIEDIT_CI_READONLY, GUI_CUSTOM_COLOR);
      }
    }
    else
    {
        return FSP_ERR_INVALID_POINTER;
    }
    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief       LogDialogCreate
 * @brief       This function create the Logging Dialog and add logging buffer
 * @param[IN]   WM_MESSAGE
 * @retval      FSP_SUCCESS     Upon successful initialize the MultiEdit
 * @retval      FSP_ERR_INVALID_POINTER if failed
 **********************************************************************************************************************/
static fsp_err_t LogDialogCreate(WM_MESSAGE * pMsg)
{
     WM_HWIN hWin_temp = 0;
     /* Create MultiEdit widget */
     ghMultiEdit = MULTIEDIT_CreateEx(11, 93, 212, 170, pMsg->hWin, WM_CF_SHOW,
                                      MULTIEDIT_CF_AUTOSCROLLBAR_V | MULTIEDIT_CF_READONLY, GUI_ID_MULTIEDIT0, 16, NULL);
     if(ghMultiEdit)
     {
         MULTIEDIT_SetBkColor(ghMultiEdit, MULTIEDIT_CI_READONLY, GUI_CUSTOM_COLOR);
         MULTIEDIT_SetWrapWord(ghMultiEdit);
         MULTIEDIT_SetMaxNumChars(ghMultiEdit, LOG_CHAR_MAX);
         MULTIEDIT_SetTextColor(ghMultiEdit, MULTIEDIT_CI_READONLY, GUI_WHITE);

         /* Set the same font as ID_TEXT_APP_EVENT */
         hWin_temp = WM_GetDialogItem(pMsg->hWin, ID_TEXT_APP_EVENT);
         if(hWin_temp)
         {
             MULTIEDIT_SetFont(ghMultiEdit,TEXT_GetFont(hWin_temp));
         }
         else
         {
            return FSP_ERR_INVALID_POINTER;
         }
         /* Show Log*/
         MULTIEDIT_AddText(ghMultiEdit, gDataLog);
     }
     else
     {
        return FSP_ERR_INVALID_POINTER;
     }
    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief       Custom code for cbID_SCREEN_LOG in ID_SCREEN_LOG_Slots.c
 * @brief       This function creates a MultiEdit widget and initializes GUI objects
 * @param[IN]   WM_MESSAGE
 * @retval      None
 **********************************************************************************************************************/
void cuscbID_SCREEN_LOG(WM_MESSAGE * pMsg) {
    switch(pMsg->MsgId)
    {
        case WM_INIT_DIALOG:
          /* Create MultiEdit widget as Logging dialog */
          if(LogDialogCreate(pMsg))
          {
            /* Handle error */
            APP_ERR_TRAP(FSP_ERR_INVALID_POINTER);
          }
          else
          {
              /* Initialize the Rotary */
              if(RotaryInit(pMsg))
              {
                /* Handle error */
                APP_ERR_TRAP(FSP_ERR_INVALID_POINTER);
              }

              /* Initialize the Switch */
              if(SwitchInit(pMsg))
              {
                /* Handle error */
                APP_ERR_TRAP(FSP_ERR_INVALID_POINTER);
              }

          }
          /* Initialize the Slider*/
          if(SliderInit(pMsg))
          {
            /* Handle error */
            APP_ERR_TRAP(FSP_ERR_INVALID_POINTER);
          }
          break;
    }
}

/*******************************************************************************************************************//**
 * @brief       Custom code for ID_SCREEN_LOG__ID_SWITCH_DARKMODE__WM_NOTIFICATION_SET in ID_SCREEN_LOG_Slots.c
 * @brief       This function sets background color of the MultiEdit widget to black
 * @param[IN]   WM_MESSAGE, integer
 * @retval      None
 **********************************************************************************************************************/
void cusID_SCREEN_LOG__ID_SWITCH_DARKMODE__WM_NOTIFICATION_SET(WM_MESSAGE * pMsg, int * pResult) {
  if(WM_NOTIFY_PARENT == pMsg->MsgId)
  {
    gDataApp.bgrd_darmode = 1;
    MULTIEDIT_SetBkColor(ghMultiEdit,  MULTIEDIT_CI_READONLY, GUI_BLACK);
  }
  *pResult =1;
}

/*******************************************************************************************************************//**
 * @brief       Custom code for ID_SCREEN_LOG__ID_SWITCH_DARKMODE__WM_NOTIFICATION_CLEAR in ID_SCREEN_LOG_Slots.c
 * @brief       This function sets background color of the MultiEdit widget to a pre-defined custom color
 * @param[IN]   WM_MESSAGE, integer
 * @retval      None
 **********************************************************************************************************************/
void cusID_SCREEN_LOG__ID_SWITCH_DARKMODE__WM_NOTIFICATION_CLEAR(WM_MESSAGE * pMsg, int * pResult) {
 if(WM_NOTIFY_PARENT == pMsg->MsgId)
  {
    gDataApp.bgrd_darmode = 0;
    MULTIEDIT_SetBkColor(ghMultiEdit,  MULTIEDIT_CI_READONLY, GUI_CUSTOM_COLOR);
  }
  *pResult =1;
}

/*******************************************************************************************************************//**
 * @brief       Custom code for ID_SCREEN_LOG__WM_NOTIFICATION_VALUE_CHANGED_0 in ID_SCREEN_LOG_Slots.c
 * @brief       This function updates LCD backlight intensity based on the slider value
 * @param[IN]   WM_MESSAGE
 * @retval      None
 **********************************************************************************************************************/
void cusID_SCREEN_LOG__WM_NOTIFICATION_VALUE_CHANGED_0(WM_MESSAGE * pMsg){
  if(WM_NOTIFY_PARENT == pMsg->MsgId)
  {
      fsp_err_t err                 = FSP_SUCCESS;
      timer_info_t info             = {0};
      uint32_t current_period_count = 0;
      uint32_t duty_cycle_count     = 0;
      int gui_err                   = 0;

      /* Get the current period setting. */
      if(R_GPT_InfoGet(&g_timer_PWM_ctrl, &info))
      {
          APP_ERR_TRAP(err);
      }

      /* Get the Slider/ID_VAR_BACKLIGHT value*/
      gDataApp.pwm_duty_cycle = (uint8_t)APPW_GetVarData(ID_VAR_BACKLIGHT, &gui_err);
      if(gui_err)
      {
          APP_ERR_TRAP(gui_err);
      }

      current_period_count = info.period_counts;
      /* Calculate the desired duty cycle based on the current period. Note that if the period could be larger than
       * UINT32_MAX / 100, this calculation could overflow. A cast to uint64_t is used to prevent this. The cast is
       * not required for 16-bit timers. */

      duty_cycle_count = (uint32_t) (((uint64_t) current_period_count * gDataApp.pwm_duty_cycle)/GPT_PWM_MAX_PERCENT);
      R_GPT_DutyCycleSet(&g_timer_PWM_ctrl, duty_cycle_count, GPT_IO_PIN_GTIOCA);
  }
}

/*******************************************************************************************************************//**
 * @brief       Custom code for ID_SCREEN_LOG__WM_NOTIFICATION_VALUE_CHANGED in ID_SCREEN_LOG_Slots.c
 * @brief       This function sets text color of the MultiEdit widget
 * @param[IN]   WM_MESSAGE
 * @retval      None
 **********************************************************************************************************************/
void cusID_SCREEN_LOG__WM_NOTIFICATION_VALUE_CHANGED(WM_MESSAGE * pMsg) {
  if(WM_NOTIFY_PARENT == pMsg->MsgId)
  {
    int gui_err = 0;
    gDataApp.text_color = APPW_GetVarData(ID_VAR_TEXT_COLOR_CHANGE, &gui_err);
    if(gui_err)
    {
      APP_ERR_TRAP(gui_err);
    }
    else
    {
      MULTIEDIT_SetTextColor(ghMultiEdit, MULTIEDIT_CI_READONLY, GUI_MAKE_COLOR(0x00FFFFFF - gDataApp.text_color));
    }
  }
}

/*******************************************************************************************************************//**
 * @brief       Custom code for ID_SCREEN_LOG__ID_BUTTON_BACK__WM_NOTIFICATION_CLICKED in ID_SCREEN_LOG_Slots.c
 * @brief       This function deletes the MultiEdit widget created previously
 * @param[IN]   WM_MESSAGE
 * @retval      None
 **********************************************************************************************************************/
void cusID_SCREEN_LOG__ID_BUTTON_BACK__WM_NOTIFICATION_CLICKED(WM_MESSAGE * pMsg) {
  if(WM_NOTIFY_PARENT == pMsg->MsgId)
  {
      /* Clean up the MultiEdit widget */
      if(ghMultiEdit)
      {
          WM_DeleteWindow(ghMultiEdit);
          ghMultiEdit = 0;
      }
  }

}
/*************************** End of file ****************************/

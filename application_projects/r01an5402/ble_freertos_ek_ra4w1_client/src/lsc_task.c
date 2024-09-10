/**********************************************************************************************************************
* Copyright (c) 2019 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*********************************************************************************************************************/
#include "hal_data.h"
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
#include "r_ble_api.h"
#include "task_function.h"

#include "r_ble_lsc.h"

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/
static uint16_t             gs_conn_hdl;
static EventGroupHandle_t   xLscEvent;
static uint16_t             wait_event;

/***********************************************************************************************************************
Export global variables
***********************************************************************************************************************/
extern TaskHandle_t         g_lsc_task;
extern EventGroupHandle_t   g_ble_event_group_handle;
extern long                 g_lsc_ntf_value;
extern uint8_t              g_blink_rate;

/***********************************************************************************************************************
ProtoType
***********************************************************************************************************************/

void lsc_task_entry(void * pvParameters);
void set_lsc_event(EventBits_t uxBitsToSet);
void delete_lsc_task_rsrc(void);
extern void r_ble_wake_up_task(void *EventGroupHandle);

/* LSC_TASK entry function */
/* pvParameters contains TaskHandle_t */
void lsc_task_entry(void * pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);

    ble_status_t    retval;
    EventBits_t     event;

    gs_conn_hdl = *(uint16_t *)pvParameters;
    xLscEvent = xEventGroupCreate();
    wait_event = LSC_WAIT_EN_CCCD | LSC_WAIT_DIS_CCCD | LSC_WAIT_RECV_NTF | LSC_WAIT_WR_BLINK | LSC_WAIT_RD_BLINK;
    while (1)
    {
        event = xEventGroupWaitBits(
                                        xLscEvent,
                                        wait_event,
                                        pdTRUE,
                                        pdFALSE,
                                        portMAX_DELAY);

        if (LSC_WAIT_EN_CCCD & event)
        {
            retval = R_BLE_LSC_WriteSwitchStateCliCnfg(gs_conn_hdl, (uint16_t *)&g_lsc_ntf_value);
            if (BLE_SUCCESS == retval)
            {
                wait_event = LSC_WAIT_DIS_CCCD | LSC_WAIT_RECV_NTF | LSC_WAIT_WR_BLINK | LSC_WAIT_RD_BLINK;
                r_ble_wake_up_task((void *)g_ble_event_group_handle);
            }
        }
        else if (LSC_WAIT_DIS_CCCD & event)
        {
            retval = R_BLE_LSC_WriteSwitchStateCliCnfg(gs_conn_hdl, (uint16_t *)&g_lsc_ntf_value);
            if (BLE_SUCCESS == retval)
            {
                wait_event = LSC_WAIT_EN_CCCD | LSC_WAIT_RECV_NTF | LSC_WAIT_WR_BLINK | LSC_WAIT_RD_BLINK;
                r_ble_wake_up_task((void *)g_ble_event_group_handle);
            }
        }
        else if (LSC_WAIT_WR_BLINK & event)
        {
            R_BLE_LSC_WriteBlinkRate(gs_conn_hdl, &g_blink_rate);
        }
        else if (LSC_WAIT_RD_BLINK & event)
        {
            retval = R_BLE_LSC_ReadBlinkRate(gs_conn_hdl);
            if (BLE_SUCCESS == retval){
                r_ble_wake_up_task((void *)g_ble_event_group_handle);
            }
        }
    }

    vTaskDelete(NULL);
}

/***********************************************************************************************************************
* Function Name: set_lsc_event
* Description  : Unblock the lsc task waiting for the bit.
* Arguments    : uxBitsToSet  ; The bit to set the lsc event.
* Return Value : none
***********************************************************************************************************************/
void set_lsc_event(EventBits_t uxBitsToSet)
{
    switch(uxBitsToSet)
    {
        case LSC_WAIT_EN_CCCD:
        {
            xEventGroupClearBits(xLscEvent, LSC_WAIT_DIS_CCCD);
        } break;
        case LSC_WAIT_DIS_CCCD:
        {
            xEventGroupClearBits(xLscEvent, LSC_WAIT_EN_CCCD);
        } break;

        case LSC_WAIT_WR_BLINK:
        {
            xEventGroupSetBits(xLscEvent, LSC_WAIT_WR_BLINK);
        } break;
        case LSC_WAIT_RD_BLINK:
        {
            xEventGroupSetBits(xLscEvent, LSC_WAIT_RD_BLINK);
        } break;

        default:
            /* Do Nothing */
            break;
    }

    xEventGroupSetBits(xLscEvent, uxBitsToSet);
}

/***********************************************************************************************************************
* Function Name: delete_lsc_task_rsrc
* Description  : Release the lsc task resources.
* Arguments    : none
* Return Value : none
***********************************************************************************************************************/
void delete_lsc_task_rsrc(void)
{
    if(NULL != g_lsc_task)
    {
        vEventGroupDelete(xLscEvent);
        vTaskDelete(g_lsc_task);
        g_lsc_task = NULL;
    }
}

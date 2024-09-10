/**********************************************************************************************************************
* Copyright (c) 2019 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*********************************************************************************************************************/
#include "hal_data.h"
#include "tx_api.h"
#include "r_ble_api.h"
#include "task_function.h"
#include "r_ble_lss.h"

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/
static uint16_t             gs_conn_hdl;
static uint16_t             cccd;
static uint16_t             wait_event;
static TX_EVENT_FLAGS_GROUP xLssEvent;
static TX_TIMER             xBlinkTimerHdl;

/***********************************************************************************************************************
Export global variables
***********************************************************************************************************************/
extern TX_THREAD ble_lss_task;
/***********************************************************************************************************************
ProtoType
***********************************************************************************************************************/
void blink_timer_cb(unsigned long Parameters);
void lss_task_entry(unsigned long Parameters);
void set_lss_event(unsigned long uxBitsToSet);
void delete_lss_task_rsrc(void);

/* LSS_TASK entry function */
/* pvParameters contains TaskHandle_t */
void lss_task_entry(unsigned long Parameters)
{
    uint8_t push_state;
    unsigned long event;

    gs_conn_hdl = (uint16_t)Parameters;
    tx_event_flags_create(&xLssEvent, "LSS_EVENT_FLAG");

    /* Create Timer for LED blink */
    tx_timer_create(&xBlinkTimerHdl, "Blink", blink_timer_cb, NULL, 0x00000001, 0x00000001, TX_NO_ACTIVATE);

    R_BLE_LSS_GetSwitchStateCliCnfg(gs_conn_hdl, &cccd);
    if(BLE_GATTS_CLI_CNFG_INDICATION != cccd)
    {
        cccd = BLE_GATTS_CLI_CNFG_DEFAULT;
    }

    wait_event = LSS_WAIT_EN_CCCD | LSS_WAIT_DIS_CCCD | LSS_WAIT_WR_BLINK | LSS_WAIT_PUSH_SW;

    while (1)
    {
        tx_event_flags_get(&xLssEvent, wait_event, TX_OR_CLEAR, &event, TX_WAIT_FOREVER);

        if(LSS_WAIT_EN_CCCD & event)
        {
            wait_event = LSS_WAIT_PUSH_SW | LSS_WAIT_DIS_CCCD | LSS_WAIT_WR_BLINK;
        }
        else if(LSS_WAIT_DIS_CCCD & event)
        {
            wait_event = LSS_WAIT_EN_CCCD | LSS_WAIT_WR_BLINK;
        }
        else if((LSS_WAIT_PUSH_SW & event) && (BLE_GATTS_CLI_CNFG_NOTIFICATION == cccd))
        {
            push_state = 1;
            R_BLE_LSS_NotifySwitchState(gs_conn_hdl, &push_state);
        }
        else if(LSS_WAIT_WR_BLINK & event)
        {
            uint8_t blink_rate;
            R_BLE_LSS_GetBlinkRate(&blink_rate);

            if (0 == blink_rate)
            {
                tx_timer_deactivate(&xBlinkTimerHdl);
                /* LED OFF */
                g_ioport.p_api->pinWrite(g_ioport.p_ctrl, BSP_IO_PORT_01_PIN_06, BSP_IO_LEVEL_HIGH);
            }
            else if (0xff == blink_rate)
            {
                tx_timer_deactivate(&xBlinkTimerHdl);
                /* LED ON */
                g_ioport.p_api->pinWrite(g_ioport.p_ctrl, BSP_IO_PORT_01_PIN_06, BSP_IO_LEVEL_LOW);
            }
            else
            {
                tx_timer_deactivate(&xBlinkTimerHdl);
                tx_timer_change(&xBlinkTimerHdl, 0x00000001, blink_rate * 100);
                tx_timer_activate(&xBlinkTimerHdl);
            }
        }
    }
//    vTaskDelete(NULL);
}

/***********************************************************************************************************************
* Function Name: set_lss_event
* Description  : Unblock the lss task waiting for the bit.
* Arguments    : uxBitsToSet  ; The bit to set the bas event.
* Return Value : none
***********************************************************************************************************************/
void set_lss_event(unsigned long uxBitsToSet)
{
    R_BLE_LSS_GetSwitchStateCliCnfg(gs_conn_hdl, &cccd);

    switch(uxBitsToSet)
    {
        case LSS_WAIT_EN_CCCD:
            tx_event_flags_set(&xLssEvent, uxBitsToSet, TX_OR);
            break;
        case LSS_WAIT_DIS_CCCD:
            uxBitsToSet = LSS_WAIT_DIS_CCCD;
            tx_event_flags_set(&xLssEvent, uxBitsToSet, TX_OR);
            break;
        case LSS_WAIT_PUSH_SW:
            if(BLE_GATTS_CLI_CNFG_NOTIFICATION == cccd)
            {
                tx_event_flags_set(&xLssEvent, uxBitsToSet, TX_OR);
            }
            break;
        case LSS_WAIT_WR_BLINK:
            tx_event_flags_set(&xLssEvent, uxBitsToSet, TX_OR);
            break;

        default:
            break;
    }
}

/***********************************************************************************************************************
* Function Name: delete_lss_task_rsrc
* Description  : Release the lss task resources.
* Arguments    : none
* Return Value : none
***********************************************************************************************************************/
void delete_lss_task_rsrc(void)
{
    tx_timer_deactivate(&xBlinkTimerHdl);
    tx_timer_delete(&xBlinkTimerHdl);
    tx_event_flags_delete(&xLssEvent);
    tx_thread_terminate(&ble_lss_task);
    tx_thread_delete(&ble_lss_task);
}

/******************************************************************************
 User function definitions
*******************************************************************************/
void blink_timer_cb(unsigned long Parameters)
{
    FSP_PARAMETER_NOT_USED(Parameters);
    bsp_io_level_t    level;
    g_ioport.p_api->pinRead(g_ioport.p_ctrl, BSP_IO_PORT_01_PIN_06, &level);
    g_ioport.p_api->pinWrite(g_ioport.p_ctrl, BSP_IO_PORT_01_PIN_06, (level == BSP_IO_LEVEL_HIGH)   ?   BSP_IO_LEVEL_LOW : BSP_IO_LEVEL_HIGH);
}

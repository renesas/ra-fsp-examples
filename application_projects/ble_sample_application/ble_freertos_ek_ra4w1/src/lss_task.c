#include "hal_data.h"
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
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
static EventGroupHandle_t   xLssEvent;
static uint16_t             cccd;
static uint16_t             wait_event;
static TimerHandle_t        xBlinkTimerHdl;

/***********************************************************************************************************************
Export global variables
***********************************************************************************************************************/
extern TaskHandle_t         g_lss_task;

/***********************************************************************************************************************
ProtoType
***********************************************************************************************************************/
void blink_timer_cb(TimerHandle_t timer_hdl);
void lss_task_entry(void *pvParameters);
void set_lss_event(EventBits_t uxBitsToSet);
void delete_lss_task_rsrc(void);
void blink_timer_cb(TimerHandle_t timer_hdl);

/* LSS_TASK entry function */
/* pvParameters contains TaskHandle_t */
void lss_task_entry(void *pvParameters)
{
    uint8_t push_state;
    EventBits_t event;

    gs_conn_hdl = *(uint16_t *)pvParameters;
    xLssEvent = xEventGroupCreate();

    /* Create Timer for LED blink */
    xBlinkTimerHdl = xTimerCreate("Blink" , 1000, pdTRUE , 0, blink_timer_cb);

    R_BLE_LSS_GetSwitchStateCliCnfg(gs_conn_hdl, &cccd);
    if(BLE_GATTS_CLI_CNFG_INDICATION != cccd)
    {
        cccd = BLE_GATTS_CLI_CNFG_DEFAULT;
    }

    wait_event = LSS_WAIT_EN_CCCD | LSS_WAIT_DIS_CCCD | LSS_WAIT_WR_BLINK | LSS_WAIT_PUSH_SW;

    while (1)
    {
        event = xEventGroupWaitBits(
                                    xLssEvent,
                                    wait_event,
                                    pdTRUE,
                                    pdFALSE,
                                    portMAX_DELAY);

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
              xTimerStop(xBlinkTimerHdl, 100);
            }
            else if (0xff == blink_rate)
            {
              xTimerStop(xBlinkTimerHdl, 100);
            }
            else
            {
              xTimerChangePeriod(xBlinkTimerHdl , (TickType_t)blink_rate * 100, (TickType_t)100);
            }
        }
    }
    vTaskDelete(NULL);
}

/***********************************************************************************************************************
* Function Name: set_lss_event
* Description  : Unblock the lss task waiting for the bit.
* Arguments    : uxBitsToSet  ; The bit to set the bas event.
* Return Value : none
***********************************************************************************************************************/
void set_lss_event(EventBits_t uxBitsToSet)
{
    R_BLE_LSS_GetSwitchStateCliCnfg(gs_conn_hdl, &cccd);

    switch(uxBitsToSet)
    {
        case LSS_WAIT_EN_CCCD:
            xEventGroupClearBits(xLssEvent, (LSS_WAIT_DIS_CCCD | LSS_WAIT_PUSH_SW));
            xEventGroupSetBits(xLssEvent, uxBitsToSet);
            break;
        case LSS_WAIT_DIS_CCCD:
            uxBitsToSet = LSS_WAIT_DIS_CCCD;
            xEventGroupClearBits(xLssEvent, (LSS_WAIT_EN_CCCD | LSS_WAIT_PUSH_SW));
            xEventGroupSetBits(xLssEvent, uxBitsToSet);
            break;
        case LSS_WAIT_PUSH_SW:
            if(BLE_GATTS_CLI_CNFG_NOTIFICATION == cccd)
            {
                xEventGroupSetBits(xLssEvent, uxBitsToSet);
            }
            break;
        case LSS_WAIT_WR_BLINK:
            xEventGroupSetBits(xLssEvent, uxBitsToSet);
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
    if(NULL != g_lss_task)
    {
        xTimerStop(xBlinkTimerHdl, 100);
        xTimerDelete(xBlinkTimerHdl, 0);

        vEventGroupDelete(xLssEvent);
        vTaskDelete(g_lss_task);
        g_lss_task = NULL;
    }
}

/******************************************************************************
 User function definitions
*******************************************************************************/
void blink_timer_cb(TimerHandle_t timer_hdl)
{
    FSP_PARAMETER_NOT_USED(timer_hdl);
    bsp_io_level_t    level;
    g_ioport.p_api->pinRead(g_ioport.p_ctrl, BSP_IO_PORT_01_PIN_06, &level);
    g_ioport.p_api->pinWrite(g_ioport.p_ctrl, BSP_IO_PORT_01_PIN_06, (level == BSP_IO_LEVEL_HIGH)   ?   BSP_IO_LEVEL_LOW : BSP_IO_LEVEL_HIGH);
}

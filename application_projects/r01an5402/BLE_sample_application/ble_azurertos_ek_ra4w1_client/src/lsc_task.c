#include "hal_data.h"
#include "tx_api.h"
#include "r_ble_api.h"
#include "task_function.h"
#include "r_ble_lsc.h"

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/
static uint16_t                 gs_conn_hdl;
static TX_EVENT_FLAGS_GROUP     xLscEvent;
static uint16_t                 wait_event;

/***********************************************************************************************************************
Export global variables
***********************************************************************************************************************/
extern TX_THREAD            ble_lsc_task;
extern long                 g_lsc_ntf_value;
extern uint8_t              g_blink_rate;
extern void                 *g_ble_event_group_handle;

/***********************************************************************************************************************
ProtoType
***********************************************************************************************************************/
void lsc_task_entry(unsigned long Parameters);
void set_lsc_event(unsigned long uxBitsToSet);
void delete_lsc_task_rsrc(void);
extern void r_ble_wake_up_task (void * EventGroupHandle);

/* LSC_TASK entry function */
/* pvParameters contains TaskHandle_t */
void lsc_task_entry(unsigned long Parameters)
{

    ble_status_t    retval;
    unsigned long event;

    gs_conn_hdl = (uint16_t)Parameters;
    tx_event_flags_create(&xLscEvent, "LSC_EVENT_FLAG");
    wait_event = LSC_WAIT_EN_CCCD | LSC_WAIT_DIS_CCCD | LSC_WAIT_RECV_NTF | LSC_WAIT_WR_BLINK | LSC_WAIT_RD_BLINK;

    while (1)
    {
        tx_event_flags_get(&xLscEvent, wait_event, TX_OR_CLEAR, &event, TX_WAIT_FOREVER);

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
            retval = R_BLE_LSC_WriteBlinkRate(gs_conn_hdl, &g_blink_rate);
            if (BLE_SUCCESS == retval)
            {
                wait_event = LSC_WAIT_EN_CCCD | LSC_WAIT_RECV_NTF | LSC_WAIT_WR_BLINK | LSC_WAIT_RD_BLINK;
                r_ble_wake_up_task((void *)g_ble_event_group_handle);
            }
        }
        else if (LSC_WAIT_RD_BLINK & event)
        {
            retval = R_BLE_LSC_ReadBlinkRate(gs_conn_hdl);
            if (BLE_SUCCESS == retval){
                r_ble_wake_up_task((void *)g_ble_event_group_handle);
            }
        }
    }
}

/***********************************************************************************************************************
* Function Name: set_lsc_event
* Description  : Unblock the lsc task waiting for the bit.
* Arguments    : uxBitsToSet  ; The bit to set the lsc event.
* Return Value : none
***********************************************************************************************************************/
void set_lsc_event(unsigned long uxBitsToSet)
{
    switch(uxBitsToSet)
    {
        case LSC_WAIT_EN_CCCD:
        {
            tx_event_flags_set(&xLscEvent, LSC_WAIT_EN_CCCD, TX_OR);
        } break;
        case LSC_WAIT_DIS_CCCD:
        {
            tx_event_flags_set(&xLscEvent, LSC_WAIT_DIS_CCCD, TX_OR);
        } break;
        case LSC_WAIT_WR_BLINK:
        {
            tx_event_flags_set(&xLscEvent, LSC_WAIT_WR_BLINK, TX_OR);
        } break;
        case LSC_WAIT_RD_BLINK:
        {
            tx_event_flags_set(&xLscEvent, LSC_WAIT_RD_BLINK, TX_OR);
        } break;
        default:
            /* Do Nothing */
            break;
    }
}

/***********************************************************************************************************************
* Function Name: delete_lsc_task_rsrc
* Description  : Release the lsc task resources.
* Arguments    : none
* Return Value : none
***********************************************************************************************************************/
void delete_lsc_task_rsrc(void)
{
    tx_event_flags_delete(&xLscEvent);
    tx_thread_terminate(&ble_lsc_task);
    tx_thread_delete(&ble_lsc_task);
}

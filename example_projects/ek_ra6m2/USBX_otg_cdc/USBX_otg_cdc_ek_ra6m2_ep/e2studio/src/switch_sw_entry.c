/***********************************************************************************************************************
 * File Name    : switch_sw_entry.c
 * Description  : Contains macros and functions used in switch_sw_entry.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2022 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "Switch_SW.h"
#include "hal_data.h"
#include "common_data.h"
#include "stdbool.h"
#include "ux_api.h"
#include "common_utils.h"
#include "usbx_otg_cdc.h"

/*******************************************************************************************************************//**
 * @addtogroup usbx_otg_cdc_ep
 * @{
 **********************************************************************************************************************/
/******************************************************************************
 * Exported global functions (to be accessed by other files)
 ******************************************************************************/
extern volatile ULONG   g_change_device_mode;

/******************************************************************************
 * Private global variables

 ******************************************************************************/
volatile uint8_t switch_pressed_flag = VALUE_0;


/******************************************************************************
 * Function Name   : g_external_irq_callback
 * Description     : Callback function for switch press
 * Arguments       : none
 * Return value    : none
 ******************************************************************************/
void g_external_irq_callback(external_irq_callback_args_t *p_args)
{
    if(VALUE_0 == p_args->channel)
    {
        switch_pressed_flag = UX_TRUE;
    }
}

/******************************************************************************
 * Function Name   : Switch_SW_entry
 * Description     : Function to check switch press
 * Arguments       : none
 * Return value    : none
 ******************************************************************************/
void Switch_SW_entry(void)
{

    fsp_err_t   err = UX_SUCCESS;
    uint8_t loop = VALUE_0;

    /* Configure the external interrupt. */
    err = R_ICU_ExternalIrqOpen(&g_external_irq1_ctrl, &g_external_irq1_cfg);
    if (UX_SUCCESS != err)
    {
        PRINT_ERR_STR("R_ICU_ExternalIrqOpen API failed..");
        ERROR_TRAP(err);
    }

    err = R_ICU_ExternalIrqEnable(&g_external_irq1_ctrl);
    if (UX_SUCCESS != err)
    {
        PRINT_ERR_STR("R_ICU_ExternalIrqEnable API failed..");
        ERROR_TRAP(err);
    }

    while(true)
    {
        loop = VALUE_0;

        /* this loop is for key bouncing */
        while(loop < VALUE_20)
        {
            if(UX_TRUE == switch_pressed_flag )
            {
                tx_thread_sleep(VALUE_10);
                loop++;
            }
            else
            {
                break;
            }
        }
        if(UX_TRUE == switch_pressed_flag)
        {
            switch_pressed_flag = false;

            if( (UX_OTG_DEVICE_A == _ux_system_otg->ux_system_otg_device_type) && (UX_OTG_MODE_HOST == g_change_device_mode) )
            {
                g_change_device_mode =  UX_OTG_VBUS_OFF_APL; //UX_OTG_VBUS_OFF_AP
            }

            if((UX_OTG_MODE_SLAVE == g_change_device_mode))
            {
                g_change_device_mode =  UX_OTG_HNP;  //UX_OTG_HNP
            }
        }
        else
        {
            tx_thread_sleep (VALUE_30);
        }
    }
}
/*******************************************************************************************************************//**
 * @} (end defgroup usbx_otg_cdc_ep)
 **********************************************************************************************************************/

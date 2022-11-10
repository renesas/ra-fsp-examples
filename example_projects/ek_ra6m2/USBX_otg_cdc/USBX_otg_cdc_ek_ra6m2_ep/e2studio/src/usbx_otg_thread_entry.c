/***********************************************************************************************************************
 * File Name    : usbx_otg_thread_entry.c
 * Description  : Contains macros and functions used in usbx_otg_thread_entry.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2022 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#include "usbx_otg_thread.h"
#include "usbx_hcdc_thread.h"
#include "r_usb_basic.h"
#include "r_usb_basic_cfg.h"
#include "ux_api.h"
#include "ux_system.h"
#include "ux_device_class_cdc_acm.h"
#include "common_utils.h"
#include "usbx_otg_cdc.h"

/*******************************************************************************************************************//**
 * @addtogroup usbx_otg_cdc_ep
 * @{
 **********************************************************************************************************************/

/******************************************************************************
 * Private global variables and functions

 ******************************************************************************/

static UX_SLAVE_CLASS_CDC_ACM_PARAMETER g_ux_device_class_cdc_acm0_parameter;
static uint32_t g_ux_pool_memory[MEMPOOL_SIZE / VALUE_4];
volatile uint8_t g_attach = USB_NO;
volatile uint8_t g_apl_state = UX_DEVICE_REMOVED;
volatile ULONG   g_change_device_mode = UX_OTG_MODE_IDLE;
static void usb_pin_setting (void);
void apl_device_swich_complete_cb(ULONG mode);

/******************************************************************************
 * Exported global functions (to be accessed by other files)
 ******************************************************************************/

extern UINT ux_system_host_cdc_acm_change_function(ULONG event, UX_HOST_CLASS * host_class, VOID * instance);
extern void ux_peri_cdc_instance_activate (VOID * cdc_instance);
extern void ux_peri_cdc_instance_deactivate (VOID * cdc_instance);


/******************************************************************************
 * Exported global variable (to be accessed by other files)
 ******************************************************************************/
extern uint8_t g_device_framework_full_speed[];
extern uint8_t g_device_framework_hi_speed[];
extern uint8_t g_string_framework[];
extern uint8_t g_language_id_framework[];
extern TX_THREAD usbx_hcdc_thread;
extern TX_THREAD usbx_pcdc_thread;
extern volatile UCHAR led_status;

/******************************************************************************
 * Function Name   : apl_device_swich_complete_cb
 * Description     : USB OTG current mode callback
 * Arguments       : ULONG mode
 * Return value    : none
 ******************************************************************************/
void apl_device_swich_complete_cb(ULONG mode)
{
    if (UX_OTG_MODE_SLAVE == mode)
    {
        _ux_system_otg ->ux_system_otg_slave_role_swap_flag = VALUE_0;
    }
    g_change_device_mode = mode;

}

/******************************************************************************
 * Function Name   : usbx_otg_thread_entry
 * Description     : USB OTG main function
 * Arguments       : ULONG mode
 * Return value    : none
 ******************************************************************************/
void usbx_otg_thread_entry(void)
{

    ULONG usb_mode = UX_OTG_MODE_IDLE;
    UINT state  = VALUE_0;
    UINT status = UX_SUCCESS;
    fsp_pack_version_t version  = { RESET_VALUE };

    rtt_thread_init_check();

    /* Version get API for FLEX Pack version */
    R_FSP_VersionGet (&version);


    /* print the banner and EP info. */
    app_rtt_print_data(RTT_OUTPUT_MESSAGE_BANNER, RESET_VALUE, NULL);

    /* USB pin configuration */
    usb_pin_setting();

    /* ux_system_initialization */
    status =_ux_system_initialize ((CHAR *)g_ux_pool_memory, MEMPOOL_SIZE, UX_NULL, VALUE_0);
    if (UX_SUCCESS != status)
    {
        PRINT_ERR_STR("ux_system_initialize API failed..");
        ERROR_TRAP(status);
    }

    PRINT_INFO_STR("ux_system_initialize Successful!");

    /* ux_device stack initialization */
    status = _ux_device_stack_initialize(g_device_framework_hi_speed,
                                         VALUE_108,
                                         g_device_framework_full_speed,
                                         VALUE_98,
                                         g_string_framework,
                                         VALUE_105,
                                         g_language_id_framework,
                                         VALUE_2,
                                         NULL);

    /* Error Handle */
    if (UX_SUCCESS != status)
    {
        PRINT_ERR_STR("ux_device_stack_initialize API failed..");
        ERROR_TRAP(status);
    }
    PRINT_INFO_STR("ux_device_stack_initialize Successful!");

    /* CallBack Initialization */
    g_ux_device_class_cdc_acm0_parameter.ux_slave_class_cdc_acm_instance_activate =   ux_peri_cdc_instance_activate;
    g_ux_device_class_cdc_acm0_parameter.ux_slave_class_cdc_acm_instance_deactivate = ux_peri_cdc_instance_deactivate;

    /* ux_device stack class registration */
    status = _ux_device_stack_class_register (_ux_system_slave_class_cdc_acm_name, _ux_device_class_cdc_acm_entry, VALUE_1, VALUE_0,
                                              (void *) &g_ux_device_class_cdc_acm0_parameter);

    /* Error Handle */
    if (UX_SUCCESS != status)
    {
        PRINT_ERR_STR("ux_device_stack_class_register API failed..");
        ERROR_TRAP(status);
    }

    PRINT_INFO_STR("ux_device_stack_class_register Successful!");

    /* ux_host stack initialization */
    status = ux_host_stack_initialize (ux_system_host_cdc_acm_change_function);

    /* Error Handle */
    if (UX_SUCCESS != status)
    {
        PRINT_ERR_STR("ux_host_stack_initialize API failed..");
        ERROR_TRAP(status);
    }

    PRINT_INFO_STR("ux_host_stack_initialize Successful!");

    /* Set callback function to be called when the OTG role swap */
    status = R_USB_OtgCallbackSet(&g_basic0_ctrl, apl_device_swich_complete_cb);

    /* Error Handle */
    if (UX_SUCCESS != status)
    {
        PRINT_ERR_STR("R_USB_OtgCallbackSet API failed..");
        ERROR_TRAP(status);
    }

#if defined(APL_USB_OTG_A_DEVICE)

    /* Open and enable irq */
    status = R_ICU_ExternalIrqOpen(&g_external_irq0_ctrl, &g_external_irq0_cfg);
    if (UX_SUCCESS != status)
    {
        PRINT_ERR_STR("R_ICU_ExternalIrqOpen API failed..");
        ERROR_TRAP(status);
    }

    status = R_ICU_ExternalIrqEnable(&g_external_irq0_ctrl);
    if (UX_SUCCESS != status)
    {
        PRINT_ERR_STR("R_ICU_ExternalIrqEnable API failed..");
        ERROR_TRAP(status);
    }

#endif

    /* Open usb driver */
    status = R_USB_Open(&g_basic0_ctrl, &g_basic0_cfg);

    /* Error Handle */
    if (FSP_SUCCESS != status)
    {
        PRINT_ERR_STR("R_USB_OPEN API failed..");
        ERROR_TRAP(status);
    }

    PRINT_INFO_STR("USB driver open Successful!");


#if defined(APL_USB_OTG_A_DEVICE)
    /* Wait until device mode changes to Host Mode */
    while(true)
    {
        if (g_change_device_mode == UX_OTG_MODE_HOST)
        {
            break;
        }
        tx_thread_sleep(VALUE_1);
    }

#endif  /* defined(APL_USB_OTG_A_DEVICE) */

    while(true)
    {
        if (g_change_device_mode != usb_mode)
        {
            usb_mode = g_change_device_mode;

            switch(usb_mode)
            {
                /* OTG host mode */
                case    UX_OTG_MODE_HOST:
                {
                    led_status = VALUE_0;

                    /* check hcdc thread state */
                    status =  tx_thread_info_get(&usbx_hcdc_thread, TX_NULL, &state, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL);
                    if (UX_SUCCESS != status)
                    {
                        PRINT_ERR_STR("tx_thread_info_get API failed..");
                        break;
                    }
                    /* resume hcdc thread if thread is suspended */
                    if (TX_SUSPENDED == state)
                    {
                        tx_thread_resume(&usbx_hcdc_thread);
                    }

                    break;
                }
                /* OTG slave mode */
                case    UX_OTG_MODE_SLAVE:
                {
                    led_status = VALUE_1;

                    /* check pcdc thread state */
                    status = tx_thread_info_get(&usbx_pcdc_thread, TX_NULL, &state, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL);
                    if (UX_SUCCESS != status)
                    {
                        PRINT_ERR_STR("tx_thread_info_get API failed..");
                        break;
                    }
                    /* resume pcdc thread if thread is suspended */
                    if(TX_SUSPENDED == state)
                    {
                        tx_thread_resume(&usbx_pcdc_thread);
                    }

                    break;
                }
                /* Turn Off VBUS supply */
                case UX_OTG_VBUS_OFF_APL:
                {

                    g_usb_on_usb.vbusSet(&g_basic0_ctrl, USB_OFF);
                    break;
                }
                /* Swap Roles  */
                case UX_OTG_HNP:
                {
                    if (UX_OTG_FEATURE_A_HNP_SUPPORT
                            == (_ux_system_otg -> ux_system_otg_slave_set_feature_flag & UX_OTG_FEATURE_A_HNP_SUPPORT))
                    {
                        _ux_system_otg ->ux_system_otg_slave_role_swap_flag = UX_OTG_HOST_REQUEST_FLAG;
                    }
                    break;
                }
                default:
                {
                    /* UX_MODE_IDLE */
                    break;
                }
            }
        }
        tx_thread_sleep(VALUE_1);
    }
}

/******************************************************************************
 * Function Name   : usb_pin_setting
 * Description     : USB port mode and Switch mode Initialize
 * Arguments       : none
 * Return value    : none
 ******************************************************************************/
static void usb_pin_setting (void)
{
    R_PMISC->PWPR         = VALUE_0;       ///< Clear B0WI bit - writing to PFSWE bit enabled
    R_PMISC->PWPR_b.PFSWE = VALUE_1;       ///< Set PFSWE bit - writing to PFS register enabled

    R_PFS->PORT[VALUE_4].PIN[VALUE_7].PmnPFS_b.PMR  = VALUE_0x1;
    R_PFS->PORT[VALUE_4].PIN[VALUE_7].PmnPFS_b.PSEL = VALUE_0x13;  //VBUS

    R_PFS->PORT[VALUE_4].PIN[VALUE_8].PmnPFS_b.PMR  = VALUE_0x1;
    R_PFS->PORT[VALUE_4].PIN[VALUE_8].PmnPFS_b.PSEL = VALUE_0x13;  //ID
    R_PFS->PORT[VALUE_4].PIN[VALUE_8].PmnPFS_b.ISEL = VALUE_0x1;

#if defined(APL_USB_OTG_A_DEVICE)
    R_PFS->PORT[VALUE_4].PIN[VALUE_9].PmnPFS_b.PMR  = VALUE_0x1;
    R_PFS->PORT[VALUE_4].PIN[VALUE_9].PmnPFS_b.PSEL = VALUE_0x13;  //EXICEN

    R_PFS->PORT[VALUE_4].PIN[VALUE_15].PmnPFS_b.PMR  = VALUE_0x1;
    R_PFS->PORT[VALUE_4].PIN[VALUE_15].PmnPFS_b.PSEL = VALUE_0x13;  //VBUSEN
#else
    R_PFS->PORT[VALUE_4].PIN[VALUE_15].PmnPFS_b.PMR  = VALUE_0x1;
    R_PFS->PORT[VALUE_4].PIN[VALUE_15].PmnPFS_b.PSEL = VALUE_0x00;  //VBUSEN
#endif

    R_PMISC->PWPR        = VALUE_0;  ///< Clear PFSWE bit - writing to PFS register disabled
    R_PMISC->PWPR_b.B0WI = VALUE_1;  ///< Set B0WI bit - writing to PFSWE bit disabled
}

/*******************************************************************************************************************//**
 * @} (end addtogroup usbx_otg_cdc_ep)
 **********************************************************************************************************************/


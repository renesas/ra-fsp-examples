/***********************************************************************************************************************
 * File Name    : usbx_pcdc_thread_entry.c
 * Description  : Contains macros and functions used in usbx_pcdc_thread_entry.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2022 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "usbx_pcdc_thread.h"
#include <usbx_otg_thread.h>
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
 * Exported global functions (to be accessed by other files)
 ******************************************************************************/
extern TX_THREAD usbx_pcdc_thread;

/******************************************************************************
 * Private global variables and functions

 ******************************************************************************/
/* A pointer to store CDC-ACM device instance. */
static UX_SLAVE_CLASS_CDC_ACM   * g_cdc = UX_NULL;
UINT apl_status_change_cb (ULONG status);
void ux_peri_cdc_instance_activate (VOID * cdc_instance);
void ux_peri_cdc_instance_deactivate (VOID * cdc_instance);
static UCHAR     receiveBuffer[READ_LENGTH];
CHAR      g_info_msg[SIZE_192] = {'\0'};

/******************************************************************************
 * Function Name   : ux_peri_cdc_instance_activate
 * Description     : USB callback function for USB instance activate
 * Arguments       : VOID * cdc_instance  : CDC instance
 * Return value    : none
 ******************************************************************************/
void ux_peri_cdc_instance_activate (VOID * cdc_instance)
{
    /* Save the CDC instance.  */
    g_cdc = (UX_SLAVE_CLASS_CDC_ACM *) cdc_instance;
    tx_semaphore_put(&g_peri_instance_activate_semaphore);
}

/******************************************************************************
 * Function Name   : ux_peri_cdc_instance_deactivate
 * Description     : USB callback function for USB instance deactivate
 * Arguments       : VOID * cdc_instance  : CDC instance
 * Return value    : none
 ******************************************************************************/
void ux_peri_cdc_instance_deactivate (VOID * cdc_instance)
{
    FSP_PARAMETER_NOT_USED(cdc_instance);
    g_cdc = UX_NULL;

}

/******************************************************************************
 * Function Name   : usbx_pcdc_thread_entry
 * Description     : USB CDC Peripheral function
 * Arguments       : none
 * Return value    : none
 ******************************************************************************/
void usbx_pcdc_thread_entry(void)
{
    UINT status = UX_SUCCESS;
    ULONG actual_length = VALUE_0;
    CHAR  info[SIZE_192] = {'\0'};


    while (true)
    {
        /* Wait until CDC-ACM device instance activated. */
        tx_semaphore_get(&g_peri_instance_activate_semaphore, TX_WAIT_FOREVER);

        if (g_cdc != NULL)
        {
            /* Read from the CDC class.  */
            status =  _ux_device_class_cdc_acm_read(g_cdc, receiveBuffer, READ_LENGTH, &actual_length);
            if (UX_SUCCESS == status)
            {
                PRINT_INFO_STR("CDC ACM data read Successful!");
                memcpy(info, receiveBuffer, SIZE_192 - 1);
                snprintf(&g_info_msg[0], sizeof(g_info_msg),"Data Read : %s\r\n", info);
                PRINT_INFO_STR(g_info_msg);

                /* Write to the CDC class.  */
                status =  _ux_device_class_cdc_acm_write(g_cdc, receiveBuffer, actual_length, &actual_length);
                if (UX_SUCCESS == status)
                {
                    PRINT_INFO_STR("CDC ACM data write Successful!");
                    memcpy(info, receiveBuffer, SIZE_192 - 1);
                    snprintf(&g_info_msg[0], sizeof(g_info_msg),"Data Written : %s\r\n", info);
                    PRINT_INFO_STR(g_info_msg);

                }
                else
                {
                    PRINT_ERR_STR("_ux_host_class_cdc_acm_write API failed..");
                }
            }
            else
            {
                PRINT_ERR_STR("_ux_host_class_cdc_acm_read API failed..");
            }

            tx_thread_suspend(&usbx_pcdc_thread);
        }
        else
        {
            tx_thread_sleep(VALUE_10);
        }
    }
}
/*******************************************************************************************************************//**
 * @} (end defgroup usbx_otg_cdc_ep)
 **********************************************************************************************************************/

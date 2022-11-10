/***********************************************************************************************************************
 * File Name    : usbx_hcdc_thread_entry.c
 * Description  : Contains macros and functions used in usbx_hcdc_thread_entry.c
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

#include "usbx_hcdc_thread.h"
#include "r_usb_basic.h"
#include "r_usb_basic_cfg.h"
#include "ux_api.h"
#include "ux_system.h"
#include "ux_host_class_cdc_acm.h"
#include "common_utils.h"
#include "usbx_otg_cdc.h"

/*******************************************************************************************************************//**
 * @addtogroup usbx_otg_cdc_ep
 * @{
 **********************************************************************************************************************/

/******************************************************************************
 * Exported global variable (to be accessed by other files)
 ******************************************************************************/
extern TX_THREAD usbx_hcdc_thread;

/******************************************************************************
 Private global variables and functions
 ******************************************************************************/
static UCHAR    g_cdc_recv_buffer[UX_RECEPTION_BUFFER_SIZE];
static UX_HOST_CLASS_CDC_ACM * cdc_acm_class_instance = NULL;
static ULONG    g_cdc_host_class_instance_count = VALUE_0;
static UCHAR    g_cdc_xmit_buffer[UX_XMIT_BUFFER_SIZE];
static CHAR     g_cdc_str[] = "Hello from RA Host! This is sent from FSP\n Host device board to FSP Peripheral device board for CDC-ACM communication test";
static CHAR     g_msg_info[SIZE_192] = {'\0'};

/* private functions */
UINT ux_system_host_cdc_acm_change_function(ULONG event, UX_HOST_CLASS * host_class, VOID * instance);


/******************************************************************************
 * Function Name   : ux_system_host_cdc_acm_change_function
 * Description     : This is the function to notify changes happened to the USBX Host Class CDC.
 * Arguments       : ULONG event : Event code from happened to the USBX Host system.
 *                 : VOID * instance : Pointer to a USBX Host class instance, which occurs a event.
 * Return value    : USBX error code
 ******************************************************************************/
UINT ux_system_host_cdc_acm_change_function(ULONG event, UX_HOST_CLASS * host_class, VOID * instance)
{
    UINT status = UX_HOST_CLASS_INSTANCE_UNKNOWN;
    FSP_PARAMETER_NOT_USED(host_class);

    /* Callback user function for the USBX Host Class CDC-ACM. */
    /* Check if there is a device insertion. */
    if (UX_DEVICE_INSERTION == event)
    {

        if (UX_SUCCESS == _ux_utility_memory_compare(_ux_system_host_class_cdc_acm_name, host_class,
                                                     _ux_utility_string_length_get(_ux_system_host_class_cdc_acm_name)))
        {
            cdc_acm_class_instance = (UX_HOST_CLASS_CDC_ACM *)instance;

            if (UX_HOST_CLASS_CDC_DATA_CLASS ==
                    cdc_acm_class_instance->ux_host_class_cdc_acm_interface->ux_interface_descriptor.bInterfaceClass)
            {
                PRINT_INFO_STR("DEVICE INSERTED");
                /* Return successful completion.  */
                status = UX_SUCCESS;
                tx_semaphore_put(&g_host_cdc_instance_semaphore);
            }
            else
            {
                /* It seems the DATA class is on the second interface. Or we hope !  */
                cdc_acm_class_instance = cdc_acm_class_instance->ux_host_class_cdc_acm_next_instance;
                if (cdc_acm_class_instance == NULL)
                {
                    /* We did not find a proper data interface.  */
                    cdc_acm_class_instance = NULL;
                    status = UX_HOST_CLASS_INSTANCE_UNKNOWN;
                }
                else
                {
                    /* Check again this interface, if this is not the data interface, we give up.  */
                    if (UX_HOST_CLASS_CDC_DATA_CLASS ==
                            cdc_acm_class_instance->ux_host_class_cdc_acm_interface->ux_interface_descriptor.bInterfaceClass)
                    {
                        PRINT_INFO_STR("DEVICE INSERTED");
                        /* Return successful completion.  */
                        status = UX_SUCCESS;
                        tx_semaphore_put(&g_host_cdc_instance_semaphore);
                    }
                    else
                    {
                        /* We did not find a proper data interface.  */
                        cdc_acm_class_instance = NULL;
                        status = UX_HOST_CLASS_INSTANCE_UNKNOWN;
                    }
                }
            }
        }
    }
    /* Check if there is a device removal. */
    else if (UX_DEVICE_REMOVAL == event)
    {
        PRINT_INFO_STR("DEVICE REMOVED");
        cdc_acm_class_instance = NULL;
        g_cdc_host_class_instance_count--;
    }

    return status;
}

/******************************************************************************
 * Function Name   : usbx_hcdc_thread_entry
 * Description     : USB CDC Host function
 * Arguments       : none
 * Return value    : none
 ******************************************************************************/
void usbx_hcdc_thread_entry(void)
{

    UINT    status = UX_SUCCESS;
    ULONG   actual_length = VALUE_0;
    CHAR    info[SIZE_192] = {'\0'};

    rtt_thread_init_check();

    /* Copy data to transmit buffer */
    _ux_utility_memory_copy (g_cdc_xmit_buffer, g_cdc_str, sizeof(g_cdc_str));

    while (true)
    {
        /* Wait for semaphore */
        tx_semaphore_get(&g_host_cdc_instance_semaphore, UX_WAIT_FOREVER);
        if (NULL != cdc_acm_class_instance)
        {

            /* Perform a cdc-acm write */
            status = _ux_host_class_cdc_acm_write(cdc_acm_class_instance, &g_cdc_xmit_buffer[VALUE_0], (strlen(g_cdc_str)), &actual_length);
            if (UX_SUCCESS == status)
            {
                PRINT_INFO_STR("CDC ACM data write Successful!");
                memcpy(info, g_cdc_xmit_buffer, SIZE_192 - 1);
                snprintf(&g_msg_info[0], sizeof(g_msg_info),"Data Written : %s\r\n", info);
                PRINT_INFO_STR(g_msg_info);

                memset (g_cdc_recv_buffer, VALUE_0, UX_RECEPTION_BUFFER_SIZE);
                /* Perform a cdc-acm read */
                status = _ux_host_class_cdc_acm_read (cdc_acm_class_instance, g_cdc_recv_buffer, actual_length, &actual_length);
                if (UX_SUCCESS == status)
                {
                    PRINT_INFO_STR("CDC ACM data read Successful!");
                    memcpy(info, g_cdc_recv_buffer, SIZE_192 - 1);
                    snprintf(&g_msg_info[0], sizeof(g_msg_info),"Data Read : %s\r\n", info);
                    PRINT_INFO_STR(g_msg_info);

                    /* Compare write and read data  */
                    if((memcmp((UCHAR *)&g_cdc_xmit_buffer[VALUE_0], (UCHAR *)&g_cdc_recv_buffer[VALUE_0], strlen(g_cdc_str))) != VALUE_0)
                    {
                        PRINT_ERR_STR("CDC ACM Write Data and Read Data comparison Failed..");
                    }
                    else
                    {
                        PRINT_INFO_STR("CDC ACM Write Data and Read Data comparison Successful!");
                    }
                }
                else
                {
                    PRINT_ERR_STR("_ux_host_class_cdc_acm_read API failed..");
                }
            }
            else
            {
                PRINT_ERR_STR("_ux_host_class_cdc_acm_write API failed..");
            }
            /* suspend hcdc thread  */
            tx_thread_suspend(&usbx_hcdc_thread);
        }
        else
        {
            tx_thread_sleep(VALUE_10);
        }
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup usbx_otg_cdc_ep)
 **********************************************************************************************************************/


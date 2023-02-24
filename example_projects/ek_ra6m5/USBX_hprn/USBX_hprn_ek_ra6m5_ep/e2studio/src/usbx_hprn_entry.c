/***********************************************************************************************************************
 * File Name    : usbx_hprn__entry.c
 * Description  : Contains definitions and functions used for usbx_hprn functionality.
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

#include "usbx_hprn.h"
#include "r_usb_basic.h"
#include "r_usb_basic_cfg.h"
#include "ux_api.h"
#include "ux_system.h"
#include "ux_host_class_printer.h"
#include "common_utils.h"
#include "data_printer.h"
#include "usbx_hprn_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup usbx_hprn_ep
 * @{
 ***/

/* Private function */
static UINT ux_host_usr_event_notification(ULONG event, UX_HOST_CLASS * host_class, VOID * instance);

/* A pointer to store Printer instance. */
static UX_HOST_CLASS_PRINTER * p_printer = UX_NULL;

/* Private Variables*/
static ULONG    g_write_actual_length = RESET_VALUE;
static uint8_t  g_printer_id_buf[DATA_MAX] = {RESET_VALUE};
static uint32_t g_ux_pool_memory[MEMPOOL_SIZE / VALUE_4];

/* USBX HPRN Thread entry function */
void usbx_hprn_entry(void)
{
        UINT     status = UX_SUCCESS;
        ULONG    actual_flags = RESET_VALUE;
        ULONG    port_status  = RESET_VALUE;
        fsp_err_t   err = FSP_SUCCESS;
        unsigned char rByte[BUFFER_SIZE_DOWN] = {NULL_CHAR};
        UINT    count = RESET_VALUE;

        /* Initialize the RTT Thread.*/
        rtt_thread_init_check();

        /* Print the EP Banner and EP info. */
        app_rtt_print_data(RTT_OUTPUT_MESSAGE_BANNER, RESET_VALUE, NULL);

        /* ux_system_initialization */
        status = _ux_system_initialize(g_ux_pool_memory, MEMPOOL_SIZE, UX_NULL, RESET_VALUE);
        if (UX_SUCCESS != status)
        {
            /* _ux_system_initialize api failed.. */
            PRINT_ERR_STR(" _ux_system_initialize api failed.");
            ERROR_TRAP(err);
        }
        else
        {
            /* ux_system initialized successfully! */
            PRINT_INFO_STR("ux_system initialized successfully");
        }
        /* ux host stack initialization */
        status = ux_host_stack_initialize(ux_host_usr_event_notification);
        if (UX_SUCCESS != status)
        {
            /* _ux_system_initialize api failed.. */
            PRINT_ERR_STR(" ux_host_stack_initialize API failed.");
            ERROR_TRAP(err);
        }
        else
        {
            /* ux_system initialized successfully! */
            PRINT_INFO_STR("ux_host_stack initialized successfully");
        }
        /* Open USB driver */
        err = R_USB_Open(&g_basic_ctrl, &g_basic_cfg);
        /* Error Handle */
        if (FSP_SUCCESS != err)
        {
            /* Error */
            PRINT_ERR_STR(" USB driver opened Error");
            ERROR_TRAP(err);
        }
        else
        {
            /* USB driver opened successfully! */
            PRINT_INFO_STR(" USB driver opened successfully");
        }

        PRINT_INFO_STR(" USBX initialization complete");

        while (true)
        {
            /* retrieves event flags from the specified event flags group to detect the Printer device.*/
            tx_event_flags_get(&g_printer_activate_event_flags, HPRN_FLAG, TX_OR, &actual_flags, TX_WAIT_FOREVER);

            if (UX_NULL != p_printer)
            {
                PRINT_INFO_STR("Printer detected.");
                PRINT_INFO_STR("Enter any Key to Print Test page");

                while(true)
                {
                    if (APP_CHECK_DATA)
                    {
                      APP_READ(rByte);
                      break;
                    }
                    tx_thread_sleep(THREAD_WAIT);
                }

                /* GET_PORT_STATUS */
                status = ux_host_class_printer_status_get(p_printer, &port_status);
                if (UX_SUCCESS != status)
                {
                   /* _ux_system_initialize api failed.. */
                   PRINT_INFO_STR(" ux_host_class_printer_status_get api failed.");
                }
                else
                {
                   /* ux_system initialized successfully! */
                   PRINT_INFO_STR("ux_host_class_printer_status_get successfully ");
                   status = ux_host_class_printer_device_id_get(p_printer, g_printer_id_buf, DATA_MAX);
                   if (UX_SUCCESS != status)
                   {
                      /* _ux_system_initialize api failed.. */
                       PRINT_INFO_STR("ux_host_class_printer_device_id_get api failed.");
                   }
                   else
                   {
                       PRINT_INFO_STR(" ux_host_class_printer_device_id_get successfully.");
                       PRINT_INFO_STR("Device ID : ");
                       for(count = DATA_MIN; count<DATA_MAX; count++)
                       {
                           PRINT_DATA(g_printer_id_buf[count]);
                       }
                       PRINT_NEXT_LINE("");

                       /*Send the data to device*/
                       status = ux_host_class_printer_write(p_printer, (UCHAR *)print_tes_page, sizeof(print_tes_page), &g_write_actual_length);

                       if (UX_SUCCESS != status)
                       {
                           /* _ux_system_initialize api failed.. */
                           PRINT_INFO_STR(" ux_host_class_printer_write api failed.");
                       }
                       else
                       {
                           /* ux_system initialized successfully! */
                           if(g_write_actual_length == sizeof(print_tes_page))
                           {
                               PRINT_INFO_STR("ux_host_class_printer_write successfully ");
                           }
                           else
                           {
                               PRINT_INFO_STR("Data length mismatch");
                           }
                       }
                   }
                }
            }
            else
            {
                PRINT_INFO_STR("Printer not detected, please check the hardware connections.");
            }
        }
}

/*******************************************************************************************************************//**
 * @brief     ux_host_usr_event_notification - This function used to check whether the printer device connected or not.
 * @param[IN] event : Device enumeraton events
 *            host class: type of host class like printer,etc.
 *            instance : printer instance
 * @retval    UX_SUCCESS
 **********************************************************************************************************************/
static UINT ux_host_usr_event_notification (ULONG event, UX_HOST_CLASS * host_class, VOID * instance)
{
    uint32_t status = RESET_VALUE;
    if (_ux_utility_memory_compare(_ux_system_host_class_printer_name, host_class,
                                   _ux_utility_string_length_get(_ux_system_host_class_printer_name)) ==
        UX_SUCCESS)
    {
        /* Check if there is a device insertion. */
        if (UX_FSP_DEVICE_INSERTION == event)
        {
            p_printer = (UX_HOST_CLASS_PRINTER *) instance;
            PRINT_INFO_STR("USB Printer device inserted");
            if (UX_NULL != p_printer)
            {
                /* This sets or clears event flags in an event flags group */
                tx_event_flags_set(&g_printer_activate_event_flags, HPRN_FLAG, TX_OR);
            }

            status = _ux_host_class_printer_name_get(p_printer);
            if(status != RESET_VALUE)
            {
                PRINT_INFO_STR("_ux_host_class_printer_name_get failed");
            }
        }
        /* Check if there is a device removal */
        else if (UX_FSP_DEVICE_REMOVAL == event)
        {
            /* This sets or clears event flags in an event flags group */
            tx_event_flags_set(&g_printer_activate_event_flags, ~HPRN_FLAG, TX_AND);
            PRINT_INFO_STR("USB Printer device removed");
            p_printer = UX_NULL;
        }
    }
    return UX_SUCCESS;
}

/*******************************************************************************************************************//**
 * @} (end addtogroup usbx_hprn_ep)
 **********************************************************************************************************************/

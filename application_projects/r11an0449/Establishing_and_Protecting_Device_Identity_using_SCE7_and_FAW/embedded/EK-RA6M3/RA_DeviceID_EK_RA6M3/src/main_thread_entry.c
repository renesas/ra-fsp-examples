/***********************************************************************************************************************
 * File Name    : main_thread_entry.c
 * Description  : handles program flow control.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
 ***********************************************************************************************************************/
#include <main_thread.h>
#include "usb_pcdc_desc.h"
#include "framedProtocolTarget.h"
#include "common_util.h"
#include "stdio.h"
#include "common_utils.h" //RTT Viewer support

#define READ_BUFFER_SIZE            1030


usb_callback_t g_usb_cb;
usb_descriptor_t g_usb_descriptor =
{
    g_apl_device,                     // Device Descriptor
    g_apl_configuration,              // Configuration descriptor for FS
    g_apl_hs_configuration,           // Configuration descriptor for HS
    g_apl_qualifier_descriptor,       // Qualifier descriptor
    g_apl_string_table,               // String descriptor table
    sizeof(g_apl_string_table) /
        sizeof(g_apl_string_table[0]) // Size of string descriptor table
};
extern usb_instance_ctrl_t g_usb_ctrl;

static usb_setup_t                  g_usb_setup;
static usb_pcdc_linecoding_t        g_usb_lc;
static usb_pcdc_ctrllinestate_t     g_usb_cls;
static usb_status_t                 g_usb_status;
static uint8_t                      g_usb_buf[READ_BUFFER_SIZE];
static void g_usb_class_req_handler(usb_setup_t * p_req);

static uint8_t g_usb_class_type = 0x00;
static bool data_ready = false;
static uint32_t data_size = 0;

/*****************************************************************************************************************
 *  @brief      usbTransmit
 *  @param[in]  pBuffer: the buffer which holds the data being sent to the PC via frameProtocol
 *  @param[in]  numBytes: number of bytes to send
 *  @retval     status:   true if usb write is successful
 *  ****************************************************************************************************************/

static bool usbTransmit(const uint8_t *pBuffer, const uint16_t numBytes)
{
    fsp_err_t status = FSP_SUCCESS;

    status = R_USB_Write (&g_usb_ctrl, (uint8_t*)pBuffer, numBytes, g_usb_class_type);

    /* Handle error */
   if (FSP_SUCCESS != status)
   {
       return false;
   }
    return true;

}

/*****************************************************************************************************************
 *  @brief      main_thread_entry: handle USB initialization, framedProtocol data transmit function assignment
 *  @retval     none
 *  ****************************************************************************************************************/
void main_thread_entry(void *pvParameters)
{
    fsp_err_t status = FSP_SUCCESS;;


    APP_PRINT("Enabling framed data protocol over USB\r\n");


    fpInitialise(usbTransmit);


    APP_PRINT("Connecting the kit to Host PC...\r\n");


    status = R_USB_Open(&g_usb_ctrl, &g_usb_cfg);
    if (FSP_SUCCESS != status)
    {
        /* Fatal error */
        APP_ERR_TRAP(status);
    }

    APP_PRINT("USB CDC instance created. Run the Device Id Host application!!!\r\n");


    while(true)
    {
        /* Delay task for 250 ms */
        vTaskDelay(250);
        if(data_ready == true)
        {
            for(uint32_t index = 0; index < data_size; index++)
            {
               fpReceiveByte(g_usb_buf[index]);
            }
            data_ready = false;
            status = R_USB_Read(&g_usb_ctrl, g_usb_buf, READ_BUFFER_SIZE, USB_CLASS_PCDC);
            if (FSP_SUCCESS != status)
            {
              /* Fatal error */
              APP_ERR_TRAP(status);
            }
        }
    }
    vTaskSuspend(NULL);

    FSP_PARAMETER_NOT_USED(pvParameters);
}

/*****************************************************************************************************************
 *  @brief      g_usb_cb: defined in the configurator
 *  @param[in]  p_ev: USB event buffer
 *  @param[in]  handle: USB handle
 *  @param[in]  handle: USB state
 *  @retval     status:   true if USB write is successful
 *  ****************************************************************************************************************/

void g_usb_cb(usb_event_info_t * p_ev, usb_hdl_t handle, usb_onoff_t state)
{
    fsp_err_t status = FSP_SUCCESS;

    switch (p_ev->event)
    {
        case USB_STATUS_REQUEST:
        {
            g_usb_class_req_handler(&p_ev->setup);
        }
        break;

        case USB_STATUS_READ_COMPLETE:
        {
            data_size = p_ev->data_size;
            data_ready = true;

        }
        break;

        case USB_STATUS_WRITE_COMPLETE:
        {
        }
        break;

        case USB_STATUS_CONFIGURED:
        {
            g_usb_status = USB_STATUS_CONFIGURED;

        }
        break;

        case USB_STATUS_REQUEST_COMPLETE:
        {

            if (USB_STATUS_CONFIGURED == g_usb_status)
            {
                R_USB_Read(&g_usb_ctrl, g_usb_buf, READ_BUFFER_SIZE, USB_CLASS_PCDC);
                if (FSP_SUCCESS != status)
                {
                   /* Fatal error */
                   APP_ERR_TRAP(status);
                }
                g_usb_status = USB_STATUS_REQUEST_COMPLETE;
            }
        }
        break;

        case USB_STATUS_DETACH:
        {
            g_usb_status = USB_STATUS_DETACH;
        }
        break;

        default:
        {
            ;
        }
    }

    FSP_PARAMETER_NOT_USED(state);
    FSP_PARAMETER_NOT_USED(handle);
}

/*****************************************************************************************************************
 *  @brief      g_usb_class_req_handler: handle USB_STATUS_REQUEST event
 *  @param[in]  p_req: USB status request buffer
 *  @retval     none
 *  ****************************************************************************************************************/

static void g_usb_class_req_handler(usb_setup_t * p_req)
{
    switch (p_req->request_type & USB_BREQUEST)
    {
        case USB_PCDC_SET_LINE_CODING:
        {
            R_USB_PeriControlDataGet(&g_usb_ctrl, (void *) &g_usb_lc,
                    sizeof(g_usb_lc) - sizeof(g_usb_lc.rsv));
        }
        break;

        case USB_PCDC_GET_LINE_CODING:
        {
            R_USB_PeriControlDataSet(&g_usb_ctrl, (void *) &g_usb_lc,
                    sizeof(g_usb_lc) - sizeof(g_usb_lc.rsv));
        }
        break;

        case USB_PCDC_SET_CONTROL_LINE_STATE:
        {
            R_USB_PeriControlDataGet(&g_usb_ctrl, (void *) &g_usb_cls,
                    sizeof(g_usb_cls));

            g_usb_cls.bdtr = (unsigned) (g_usb_setup.request_value >> 0) & 0x01;
            g_usb_cls.brts = (unsigned) (g_usb_setup.request_value >> 1) & 0x01;
        }
        break;

        default:
        {
            printf("Unsupported USB class request: %x\n", p_req->request_type);

            R_USB_PeriControlStatusSet(&g_usb_ctrl, USB_SETUP_STATUS_STALL);
        }
        break;
    }
}

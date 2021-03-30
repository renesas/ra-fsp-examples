/***********************************************************************************************************************
 * File Name    : usb_host_vendor_thread_entry.c
 * Description  : Contains entry function of USB Vendor host.
 ***********************************************************************************************************************/
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
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/
#include <usb_host_vendor_thread.h>
#include "common_utils.h"
#include "usb_host_vendor_thread_entry.h"

/*******************************************************************************************************************//**
 * @addtogroup usb_host_vendor_ep
 * @{
 **********************************************************************************************************************/

/* Global variables */
static uint8_t g_buf[BUF_SIZE] = {RESET_VALUE};
static uint8_t g_request_buf[REQ_SIZE] = {RESET_VALUE};
/* Variable to capture USB event. */
static volatile usb_event_info_t * p_usb_event = NULL ;
static volatile bool g_err_flag = false;
volatile uint8_t pipe = RESET_VALUE;
uint8_t bulk_in_pipe = RESET_VALUE;         /* Bulk In  Pipe */
uint8_t bulk_out_pipe = RESET_VALUE;        /* Bulk Out Pipe */
uint16_t max_packet_size = USB_APL_MXPS;

/* Function definitions */
static fsp_err_t process_usb_events(void);
static fsp_err_t usb_configured_event_process(void);
static fsp_err_t usb_status_complete_request(void);
static void handle_error(fsp_err_t err, char * err_str);
static void buffer_init (uint8_t * buf, uint16_t size);
static fsp_err_t buffer_check (uint32_t length);

/*******************************************************************************************************************//**
 * @brief     Entry function of USB host vendor thread.
 * @param[IN] pointer to  pvParameters
 * @retval    None
 **********************************************************************************************************************/
void usb_host_vendor_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);

    fsp_pack_version_t version = {RESET_VALUE};
    fsp_err_t err              = FSP_SUCCESS;
    BaseType_t err_queue       = pdFALSE;

    /* Version get API for FLEX pack information */
    R_FSP_VersionGet(&version);

    /* Example Project information printed on the Console */
    APP_PRINT(BANNER_INFO, EP_VERSION, version.major, version.minor, version.patch);
    APP_PRINT(EP_INFO);

    /* max packet size check */
    if (RESET_VALUE == (BUF_SIZE % USB_APL_MXPS))
    {
        max_packet_size = USB_APL_MXPS;
    }
    else
    {
        max_packet_size = RESET_VALUE;
    }

    /* Open USB instance */
    err = R_USB_Open(&g_basic_ctrl, &g_basic_cfg);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nR_USB_Open failed\r\n");
        APP_ERR_TRAP(err);
    }

    APP_PRINT("\r\nUSB Opened successfully.\n\r");

    while (true)
    {
        /* Handle error if queue send fails*/
        if (true == g_err_flag)
        {
            handle_error(FSP_ERR_ABORTED, "\r\nError in sending usb event through queue\r\n");
        }

        /* Receive message from queue */
        err_queue = xQueueReceive(g_queue, &p_usb_event,(portMAX_DELAY));
        /* Handle error */
        if (pdTRUE != err_queue)
        {
            handle_error(FSP_ERR_ABORTED, "\r\nError in receiving event through queue\r\n");
        }

        /* process USB events */
        err = process_usb_events();
        handle_error(err, "\r\nprocess_usb_events failed.\r\n");
    }
}

/*******************************************************************************************************************//**
 * @brief     Function processes USB events.
 * @param[IN] None
 * @retval    Any Other Error code apart from FSP_SUCCESS on Unsuccessful operation.
 **********************************************************************************************************************/
fsp_err_t process_usb_events(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* USB event received */
    switch (p_usb_event->event)
    {
        case USB_STATUS_CONFIGURED:
        {
            /* Process USB configured event */
            err = usb_configured_event_process();
            if (FSP_SUCCESS != err)
            {
                APP_ERR_PRINT("\r\nusb_configured_event_process failed.\r\n");
            }
            break;
        }

        case USB_STATUS_READ_COMPLETE:
        {
            /* check for in pipe */
            if ((bulk_in_pipe == p_usb_event->pipe) && (FSP_ERR_USB_FAILED != p_usb_event->status))
            {
                pipe = bulk_out_pipe;

                /* Data comparison read from peripheral */
                err = buffer_check(p_usb_event->data_size);
                if (FSP_SUCCESS != err )
                {
                    APP_ERR_PRINT("\r\nbuffer_check failed\r\n");
                    return FSP_ERR_USB_FAILED;
                }
                else
                {
                    APP_DBG_PRINT("\r\nData is successfully received\r\n");
                    APP_DBG_PRINT("Received data is:\n");
                    /* print the received data */
                    for (uint16_t cnt = RESET_VALUE; cnt < BUF_SIZE; cnt++)
                    {
                        if(LOG_LEVEL >= LVL_DEBUG)
                        {
                            APP_PRINT("%d ", g_buf[cnt]);
                        }
                    }

                    /* Write data back to peripheral */
                    err = R_USB_PipeWrite(&g_basic_ctrl, &g_buf[RESET_VALUE], p_usb_event->data_size, (uint8_t)pipe);
                    if (FSP_SUCCESS != err)
                    {
                        APP_ERR_PRINT("\r\nR_USB_PipeWrite failed.\r\n");
                    }
                    else
                    {
                        APP_PRINT("\r\nUSB Write is initiated from Host Vendor class\r\n");
                    }
                }
            }
            else
            {
                /* Do nothing */
            }
            break;
        }

        case USB_STATUS_WRITE_COMPLETE:
        {
            /* check for out pipe */
            if ((bulk_out_pipe == p_usb_event->pipe) && (FSP_ERR_USB_FAILED != p_usb_event->status))
            {
                if (USB_APL_MXPS == max_packet_size)
                {
                    pipe = bulk_in_pipe;
                    /* Send ZLP */
                    err = R_USB_PipeWrite(&g_basic_ctrl, RESET_VALUE, RESET_VALUE, p_usb_event->pipe);
                    if (FSP_SUCCESS != err)
                    {
                        APP_ERR_PRINT("\r\nR_USB_PipeWrite failed.\r\n");
                    }
                    else
                    {
                        APP_PRINT("\nZLP is successfully written from Host vendor");
                    }
                }
                else
                {
                    APP_DBG_PRINT("\r\nData is successfully sent\r\n");
                    pipe = bulk_in_pipe;
                    memset(g_buf, RESET_VALUE, BUF_SIZE);
                    /* Read data back */
                    err = R_USB_PipeRead(&g_basic_ctrl, &g_buf[RESET_VALUE], BUF_SIZE, (uint8_t)pipe);
                    if (FSP_SUCCESS != err)
                    {
                        APP_ERR_PRINT("\r\nR_USB_PipeRead failed.\r\n");
                    }
                    else
                    {
                        APP_PRINT("\nUSB Read is initiated from Host Vendor class");
                    }
                }
            }
            else
            {
                /* Do Nothing */
            }
            break;
        }

        case USB_STATUS_REQUEST_COMPLETE:
        {
            /* process USB status complete request event */
            err = usb_status_complete_request();
            if (FSP_SUCCESS != err)
            {
                APP_ERR_PRINT("\r\nusb_status_complete_request failed.\r\n");
            }

            break;
        }

        case USB_STATUS_DETACH:
        {
            APP_PRINT("\nUSB STATUS : USB_STATUS_DETACH\r\n");
            break;
        }
        case USB_STATUS_SUSPEND:
        {
            APP_PRINT("\nUSB STATUS : USB_STATUS_SUSPEND\r\n");
            break;
        }
        case USB_STATUS_RESUME:
        {
            APP_PRINT("\nUSB STATUS : USB_STATUS_RESUME\r\n");
            break;
        }
        default:
        {
            break;
        }
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief     Function processes USB configured event.
 * @param[IN] None
 * @retval    Any Other Error code apart from FSP_SUCCESS on Unsuccessful operation.
 **********************************************************************************************************************/
static fsp_err_t usb_configured_event_process(void)
{
    fsp_err_t err        = FSP_SUCCESS;
    uint16_t used_pipe   = RESET_VALUE;
    usb_pipe_t pipe_info = {RESET_VALUE};
    usb_setup_t setup    = {RESET_VALUE};

    APP_PRINT("USB Configured Successfully\r\n");

    /* Get USB Pipe Information */
    err = R_USB_UsedPipesGet(&g_basic_ctrl, &used_pipe, ADDRESS);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nR_USB_UsedPipesGet failed\r\n");
    }
    else
    {
        for (pipe = START_PIPE; pipe < END_PIPE; pipe++)
        {
            /* check for the used pipe */
            if ((used_pipe & (START_PIPE << pipe)) != RESET_VALUE)
            {
                /* Get the pipe Info */
                err = R_USB_PipeInfoGet(&g_basic_ctrl, &pipe_info, (uint8_t)pipe);
                if (FSP_SUCCESS != err)
                {
                    APP_ERR_PRINT("\r\nR_USB_PipeInfoGet failed\r\n");
                }

                APP_PRINT("\r\nBulkPiPe: %d Pipe Number: %d ", pipe_info.transfer_type, pipe);

                if (USB_EP_DIR_IN != (pipe_info.endpoint & USB_EP_DIR_IN))
                {
                    /* Out Transfer */
                    if (USB_TRANSFER_TYPE_BULK == pipe_info.transfer_type)
                    {
                        bulk_out_pipe = pipe;
                    }
                    else
                    {
                        /* Do nothing */
                    }
                }
                else/* if (USB_EP_DIR_OUT != (pipe_info.endpoint & USB_EP_DIR_OUT))*/
                {
                    /* In Transfer */
                    if (USB_TRANSFER_TYPE_BULK == pipe_info.transfer_type)
                    {
                        bulk_in_pipe = pipe;
                    }
                    else
                    {
                        /* Do nothing */
                    }
                }
            }
            else
            {
                /* do nothing */
            }
        }
        APP_DBG_PRINT("\r\nAll pipe info is fetched from USB Vendor Host driver\r\n");

        setup.request_type   = SET_VENDOR_NO_DATA; /* bRequestCode:SET_VENDOR, bmRequestType */
        setup.request_value  = RESET_VALUE;        /* wValue:Zero */
        setup.request_index  = RESET_VALUE;        /* wIndex:Interface */
        setup.request_length = RESET_VALUE;        /* wLength: Data Length */

        buffer_init(g_request_buf, REQ_SIZE);
        /* Request Control transfer */
        err = R_USB_HostControlTransfer(&g_basic_ctrl, &setup, &g_request_buf[RESET_VALUE], p_usb_event->device_address);
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\nR_USB_HostControlTransfer failed\r\n");
        }
        else
        {
            APP_PRINT("\r\nUSB host control transfer for USB_SET_VENDOR_NO_DATA initiated from Host Vendor class\r\n");
        }
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief     Function processes USB status complete request event.
 * @param[IN] None
 * @retval    Any Other Error code apart from FSP_SUCCESS on Unsuccessful operation.
 **********************************************************************************************************************/
static fsp_err_t usb_status_complete_request(void)
{
    fsp_err_t err     = FSP_SUCCESS;
    usb_setup_t setup = {RESET_VALUE};

    if (USB_SET_VENDOR_NO_DATA == (p_usb_event->setup.request_type & USB_BREQUEST))
    {
        setup.request_type   = SET_VENDOR;         /* bRequestCode:SET_VENDOR, bmRequestType */
        setup.request_value  = RESET_VALUE;        /* wValue:Zero */
        setup.request_index  = RESET_VALUE;        /* wIndex:Interface */
        setup.request_length = REQ_SIZE;           /* wLength: Data Length */

        buffer_init(g_request_buf, REQ_SIZE);
        /* Request Control transfer */
        err = R_USB_HostControlTransfer(&g_basic_ctrl, &setup, &g_request_buf[RESET_VALUE], p_usb_event->device_address);
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\nR_USB_HostControlTransfer failed\r\n");
        }
        else
        {
            APP_PRINT("\r\nUSB host control transfer for USB_SET_VENDOR initiated from Host Vendor class\r\n");
        }
    }
    else if (USB_SET_VENDOR == (p_usb_event->setup.request_type & USB_BREQUEST))
    {
        setup.request_type   = GET_VENDOR;         /* bRequestCode:GET_VENDOR, bmRequestType */
        setup.request_value  = RESET_VALUE;        /* wValue:Zero */
        setup.request_index  = RESET_VALUE;        /* wIndex:Interface */
        setup.request_length = REQ_SIZE;           /* wLength: Data Length */

        memset(g_request_buf, RESET_VALUE, REQ_SIZE);
        /* Request Control transfer */
        err = R_USB_HostControlTransfer(&g_basic_ctrl, &setup, &g_request_buf[RESET_VALUE], p_usb_event->device_address);
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\nR_USB_HostControlTransfer failed\r\n");
        }
        else
        {
            APP_PRINT("\r\nUSB host control transfer for USB_GET_VENDOR initiated from Host Vendor class\r\n");
        }
    }
    else if (USB_GET_VENDOR == (p_usb_event->setup.request_type & USB_BREQUEST))
    {
        buffer_init(g_buf, BUF_SIZE);
        /* Bulk Out Transfer */
        err = R_USB_PipeWrite(&g_basic_ctrl, &g_buf[RESET_VALUE], (uint32_t)(BUF_SIZE - max_packet_size), (uint8_t)bulk_out_pipe);
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\nR_USB_PipeWrite failed\r\n");
        }
        else
        {
            APP_PRINT("\r\nUSB write operation initiated from Host Vendor class\r\n");
        }
    }
    else
    {
        /* Unsupported request */
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function is callback for USB.
 * @param[IN]   usb_event_info_t  *p_event_info
 * @param[IN]   usb_hdl_t         handler
 * @param[IN]   usb_onoff_t       on_off
 * @retval      None.
 ***********************************************************************************************************************/
void usb_host_vendor_callback(usb_event_info_t* p_event_info, usb_hdl_t handler, usb_onoff_t state)
{
    FSP_PARAMETER_NOT_USED (handler);
    FSP_PARAMETER_NOT_USED (state);

    /* Send event received to queue */
    if (pdTRUE != (xQueueSend(g_queue, (const void *)&p_event_info, (TickType_t)(RESET_VALUE))))
    {
        g_err_flag = true;
    }
}

/******************************************************************************
 * @brief        buffer initialization
 * @param[IN]    pointer to buf
 * @param[IN]    size
 * @retval       none
 ******************************************************************************/
static void buffer_init (uint8_t * buf, uint16_t size)
{
    for (uint16_t cnt = RESET_VALUE; cnt < size; cnt++)
    {
        buf[cnt] = (uint8_t)cnt;
    }
}

/*******************************************************************************************************************//**
 *  @brief       Closes the USB module , Print and traps error.
 *  @param[IN]   status    error status
 *  @param[IN]   err_str   error string
 *  @retval      None
 **********************************************************************************************************************/
static void handle_error(fsp_err_t err, char * err_str)
{
    if (FSP_SUCCESS != err)
    {
        if (FSP_SUCCESS != R_USB_Close(&g_basic_ctrl))
        {
            APP_ERR_PRINT ("\r\n** R_USB_Close API Failed ** \r\n ");
        }
        APP_PRINT(err_str);
        APP_ERR_TRAP(err);
    }
}

/*******************************************************************************************************************//**
 *  @brief       Buffer check.
 *  @param[IN]   None
 *  @retval      FSP_ERR_ABORTED on unsuccessful operation
 **********************************************************************************************************************/
static fsp_err_t buffer_check (uint32_t length)
{
    for (uint16_t cnt = RESET_VALUE; cnt < (uint16_t) length; cnt++)
    {
        if ((uint8_t)(cnt & USB_VALUE_FF) != g_buf[cnt])
        {
            return FSP_ERR_ABORTED;
        }
    }
    R_BSP_SoftwareDelay(DELAY, BSP_DELAY_UNITS_MILLISECONDS);
    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @} (end addtogroup usb_host_vendor_ep)
 **********************************************************************************************************************/

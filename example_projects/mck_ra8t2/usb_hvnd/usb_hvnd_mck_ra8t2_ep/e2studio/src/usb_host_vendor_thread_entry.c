/***********************************************************************************************************************
 * File Name    : usb_host_vendor_thread_entry.c
 * Description  : Contains entry function of USB Vendor host.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "usb_host_vendor_thread.h"
#include "common_utils.h"
#include "usb_host_vendor_thread_entry.h"

/*******************************************************************************************************************//**
 * @addtogroup usb_host_vendor_ep
 * @{
 **********************************************************************************************************************/

/* Global variables */
static uint8_t g_buf[BUF_SIZE]         = {RESET_VALUE};
static uint8_t g_request_buf[REQ_SIZE] = {RESET_VALUE};
/* Variables to capture USB event */
static volatile usb_event_info_t * p_usb_event = NULL ;
static volatile bool g_err_flag = false;
volatile uint8_t pipe    = RESET_VALUE;
uint8_t bulk_in_pipe     = RESET_VALUE;         /* Bulk In pipe */
uint8_t bulk_out_pipe    = RESET_VALUE;         /* Bulk Out pipe */
uint16_t max_packet_size = USB_APL_MXPS;

/* Function definitions */
static fsp_err_t process_usb_events(void);
static fsp_err_t usb_configured_event_process(void);
static fsp_err_t usb_status_complete_request(void);
static void handle_error(fsp_err_t err, char * err_str);
static void buffer_init (uint8_t * buf, uint16_t size);
static fsp_err_t buffer_check (uint32_t length);

/*******************************************************************************************************************//**
 * @brief     This is entry function for the USB Host Vendor thread.
 * @param[in] pvParameters   Pointer to thread input parameters (if any).
 * @retval    None.
 **********************************************************************************************************************/
void usb_host_vendor_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED(pvParameters);

    fsp_pack_version_t version = {RESET_VALUE};
    fsp_err_t err              = FSP_SUCCESS;
    BaseType_t err_queue       = pdFALSE;

    /* Version get API for FLEX pack information */
    R_FSP_VersionGet(&version);

    /* Example project information printed on the Console */
    APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor,\
	          version.version_id_b.patch);
    APP_PRINT(EP_INFO);

    /* Max packet size */
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
        APP_ERR_PRINT("\r\nR_USB_Open API failed\r\n");
        APP_ERR_TRAP(err);
    }

    APP_PRINT("\r\nUSB opened successfully\r\n");

    while (true)
    {
        /* Handle error if sending the USB event to the queue fails */
        if (true == g_err_flag)
        {
            handle_error(FSP_ERR_ABORTED, "\r\nFailed to send USB event to the queue\r\n");
        }

        /* Receive message from queue */
        err_queue = xQueueReceive(g_queue, &p_usb_event, (portMAX_DELAY));
        /* Handle error */
        if (pdTRUE != err_queue)
        {
            handle_error(FSP_ERR_ABORTED, "\r\nFailed to receive USB event from the queue\r\n");
        }

        /* Process USB events */
        err = process_usb_events();
        handle_error(err, "\r\nprocess_usb_events failed\r\n");
    }
}

/*******************************************************************************************************************//**
 * @brief     This function processes USB events.
 * @param[in] None.
 * @retval    Any other error code apart from FSP_SUCCESS on unsuccessful operation.
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
                APP_ERR_PRINT("\r\nusb_configured_event_process failed\r\n");
            }
            break;
        }

        case USB_STATUS_READ_COMPLETE:
        {
            /* Check for Bulk In pipe */
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
                    /* Print the received data */
                    for (uint16_t cnt = RESET_VALUE; cnt < BUF_SIZE; cnt++)
                    {
                        if (LOG_LEVEL >= LVL_DEBUG)
                        {
                            APP_PRINT("%d", g_buf[cnt]);
                        }
                    }

                    /* Write data back to peripheral */
                    err = R_USB_PipeWrite(&g_basic_ctrl, &g_buf[RESET_VALUE], p_usb_event->data_size, (uint8_t)pipe);
                    if (FSP_SUCCESS != err)
                    {
                        APP_ERR_PRINT("\r\nR_USB_PipeWrite API failed\r\n");
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
            /* Check for out pipe */
            if ((bulk_out_pipe == p_usb_event->pipe) && (FSP_ERR_USB_FAILED != p_usb_event->status))
            {
                if (USB_APL_MXPS == max_packet_size)
                {
                    pipe = bulk_in_pipe;
                    /* Send Zero-Length Packet (ZLP) */
                    err = R_USB_PipeWrite(&g_basic_ctrl, RESET_VALUE, RESET_VALUE, p_usb_event->pipe);
                    if (FSP_SUCCESS != err)
                    {
                        APP_ERR_PRINT("\r\nR_USB_PipeWrite API failed\r\n");
                    }
                    else
                    {
                        APP_PRINT("\r\nZLP is successfully written by Host Vendor");
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
                        APP_ERR_PRINT("\r\nR_USB_PipeRead API failed\r\n");
                    }
                    else
                    {
                        APP_PRINT("\nUSB Read is initiated by Host Vendor class");
                    }
                }
            }
            else
            {
                /* Do nothing */
            }
            break;
        }
        case USB_STATUS_REQUEST_COMPLETE:
        {
            /* Process USB status complete request event */
            err = usb_status_complete_request();
            if (FSP_SUCCESS != err)
            {
                APP_ERR_PRINT("\r\nusb_status_complete_request failed\r\n");
            }

            break;
        }
        case USB_STATUS_DETACH:
        {
            APP_PRINT("\nUSB STATUS: USB_STATUS_DETACH\r\n");
            break;
        }
        case USB_STATUS_SUSPEND:
        {
            APP_PRINT("\nUSB STATUS: USB_STATUS_SUSPEND\r\n");
            break;
        }
        case USB_STATUS_RESUME:
        {
            APP_PRINT("\nUSB STATUS: USB_STATUS_RESUME\r\n");
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
 * @brief     This function processes USB configured event.
 * @param[in] None.
 * @retval    Any other error code apart from FSP_SUCCESS on unsuccessful operation.
 **********************************************************************************************************************/
static fsp_err_t usb_configured_event_process(void)
{
    fsp_err_t err        = FSP_SUCCESS;
    uint16_t used_pipe   = RESET_VALUE;
    usb_pipe_t pipe_info = {.endpoint = RESET_VALUE, .maxpacketsize = RESET_VALUE, .transfer_type = RESET_VALUE};
    usb_setup_t setup    = {.request_type = RESET_VALUE, .request_value = RESET_VALUE,\
                            .request_index = RESET_VALUE, .request_length = RESET_VALUE};

    APP_PRINT("USB configured successfully\r\n");

    /* Get USB pipe information */
    err = R_USB_UsedPipesGet(&g_basic_ctrl, &used_pipe, ADDRESS);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nR_USB_UsedPipesGet API failed\r\n");
    }
    else
    {
        for (pipe = START_PIPE; pipe < END_PIPE; pipe++)
        {
            /* Check for the used pipe */
            if ((used_pipe & (START_PIPE << pipe)) != RESET_VALUE)
            {
                /* Get the pipe information */
                err = R_USB_PipeInfoGet(&g_basic_ctrl, &pipe_info, (uint8_t)pipe);
                if (FSP_SUCCESS != err)
                {
                    APP_ERR_PRINT("\r\nR_USB_PipeInfoGet API failed\r\n");
                }

                APP_PRINT("\r\nBulkPipe: %d Pipe Number: %d", pipe_info.transfer_type, pipe);

                if (USB_EP_DIR_IN != (pipe_info.endpoint & USB_EP_DIR_IN))
                {
                    /* Out transfer */
                    if (USB_TRANSFER_TYPE_BULK == pipe_info.transfer_type)
                    {
                        bulk_out_pipe = pipe;
                    }
                    else
                    {
                        /* Do nothing */
                    }
                }
                else /* if (USB_EP_DIR_OUT != (pipe_info.endpoint & USB_EP_DIR_OUT)) */
                {
                    /* In transfer */
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
                /* Do nothing */
            }
        }
        APP_DBG_PRINT("\r\nAll pipe info is fetched from USB Vendor Host driver\r\n");

        setup.request_type   = SET_VENDOR_NO_DATA; /* bRequestCode:SET_VENDOR, bmRequestType */
        setup.request_value  = RESET_VALUE;        /* wValue:Zero */
        setup.request_index  = RESET_VALUE;        /* wIndex:Interface */
        setup.request_length = RESET_VALUE;        /* wLength: Data Length */

        buffer_init(g_request_buf, REQ_SIZE);
        /* Request control transfer */
        err = R_USB_HostControlTransfer(&g_basic_ctrl, &setup, &g_request_buf[RESET_VALUE],\
		                                p_usb_event->device_address);
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\nR_USB_HostControlTransfer API failed\r\n");
        }
        else
        {
            APP_PRINT("\r\nUSB host control transfer for USB_SET_VENDOR_NO_DATA initiated by Host Vendor class\r\n");
        }
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function processes USB status complete request event.
 * @param[in] None.
 * @retval    Any other error code apart from FSP_SUCCESS on Unsuccessful operation.
 **********************************************************************************************************************/
static fsp_err_t usb_status_complete_request(void)
{
    fsp_err_t err     = FSP_SUCCESS;
    usb_setup_t setup = {.request_type = RESET_VALUE, .request_value = RESET_VALUE,\
                         .request_index = RESET_VALUE, .request_length = RESET_VALUE};

    if (USB_SET_VENDOR_NO_DATA == (p_usb_event->setup.request_type & USB_BREQUEST))
    {
        setup.request_type   = SET_VENDOR;         /* bRequestCode:SET_VENDOR, bmRequestType */
        setup.request_value  = RESET_VALUE;        /* wValue:Zero */
        setup.request_index  = RESET_VALUE;        /* wIndex:Interface */
        setup.request_length = REQ_SIZE;           /* wLength: Data Length */

        buffer_init(g_request_buf, REQ_SIZE);
        /* Request control transfer */
        err = R_USB_HostControlTransfer(&g_basic_ctrl, &setup, &g_request_buf[RESET_VALUE],\
		                                p_usb_event->device_address);
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\nR_USB_HostControlTransfer API failed\r\n");
        }
        else
        {
            APP_PRINT("\r\nUSB host control transfer for USB_SET_VENDOR initiated by Host Vendor class\r\n");
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
        err = R_USB_HostControlTransfer(&g_basic_ctrl, &setup, &g_request_buf[RESET_VALUE],\
		                                p_usb_event->device_address);
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\nR_USB_HostControlTransfer API failed\r\n");
        }
        else
        {
            APP_PRINT("\r\nUSB host control transfer for USB_GET_VENDOR initiated by Host Vendor class\r\n");
        }
    }
    else if (USB_GET_VENDOR == (p_usb_event->setup.request_type & USB_BREQUEST))
    {
        buffer_init(g_buf, BUF_SIZE);
        /* Bulk Out pipe transfer */
        err = R_USB_PipeWrite(&g_basic_ctrl, &g_buf[RESET_VALUE], (uint32_t)(BUF_SIZE - max_packet_size),\
		                      (uint8_t)bulk_out_pipe);
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\nR_USB_PipeWrite API failed\r\n");
        }
        else
        {
            APP_PRINT("\r\nUSB write operation initiated by Host Vendor class\r\n");
        }
    }
    else
    {
        /* Unsupported request */
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief     This is callback function for USB Host Vendor class events.
 * @param[in] p_event_info Pointer to the USB event information structure.
 * @param[in] handler      USB handler (unused).
 * @param[in] state        USB state (unused).
 * @retval    None.
 **********************************************************************************************************************/
void usb_host_vendor_callback(usb_event_info_t* p_event_info, usb_hdl_t handler, usb_onoff_t state)
{
    FSP_PARAMETER_NOT_USED(handler);
    FSP_PARAMETER_NOT_USED(state);

    /* Send the received event to the queue */
    if (pdTRUE != (xQueueSend(g_queue, (const void *)&p_event_info, (TickType_t)(RESET_VALUE))))
    {
        g_err_flag = true;
    }
}

/*******************************************************************************************************************//**
 * @brief        This function used to initialize buffer.
 * @param[in]    buf     Pointer to the buffer to initialize.
 * @param[in]    size    Size of the buffer in bytes.
 * @retval       None.
 **********************************************************************************************************************/
static void buffer_init(uint8_t * buf, uint16_t size)
{
    for (uint16_t cnt = RESET_VALUE; cnt < size; cnt++)
    {
        buf[cnt] = (uint8_t)cnt;
    }
}

/*******************************************************************************************************************//**
 *  @brief       This function used to closes the USB module upon error, prints error message, and traps the error.
 *  @param[in]   err       Error status code.
 *  @param[in]   err_str   Pointer to the error message string.
 *  @retval      None.
 **********************************************************************************************************************/
static void handle_error(fsp_err_t err, char * err_str)
{
    if (FSP_SUCCESS != err)
    {
        if (FSP_SUCCESS != R_USB_Close(&g_basic_ctrl))
        {
            APP_ERR_PRINT("\r\nR_USB_Close API failed\r\n");
        }
        APP_PRINT(err_str);
        APP_ERR_TRAP(err);
    }
}

/*******************************************************************************************************************//**
 *  @brief       This function used to check buffer.
 *  @param[in]   length  Number of bytes to check in the buffer.
 *  @retval      FSP_ERR_ABORTED on unsuccessful operation.
 **********************************************************************************************************************/
static fsp_err_t buffer_check(uint32_t length)
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

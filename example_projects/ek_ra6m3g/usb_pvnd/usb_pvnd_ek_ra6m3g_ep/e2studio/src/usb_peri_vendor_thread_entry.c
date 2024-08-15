/***********************************************************************************************************************
 * File Name    : usb_peri_vendor_thread_entry.c
 * Description  : Contains entry function of USB Vendor Peripheral.
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#include "usb_peri_vendor_thread.h"
#include "common_utils.h"
#include "usb_peri_vendor_thread_entry.h"
/*******************************************************************************************************************//**
 * @addtogroup usb_peripheral_vendor_ep
 * @{
 **********************************************************************************************************************/
/* Global variables */
static uint8_t g_buf[BUF_SIZE] = {RESET_VALUE};
static uint8_t g_request_buf[REQ_SIZE] = {RESET_VALUE};
/* Variable to capture USB event. */
static volatile usb_event_info_t * p_usb_event = NULL ;
static volatile bool g_err_flag = false;
volatile uint8_t pipe = RESET_VALUE;
uint8_t bulk_in_pipe = RESET_VALUE;       /* Bulk In  Pipe */
uint8_t bulk_out_pipe = RESET_VALUE;      /* Bulk Out Pipe */
uint16_t max_packet_size = USB_APL_MXPS;

/* Function definitions */
static fsp_err_t process_usb_events(void);
static fsp_err_t usb_configured_event_process(void);
static fsp_err_t usb_status_request(void);
static void handle_error(fsp_err_t err, char * err_str);
static fsp_err_t buffer_check (uint32_t length);

/*******************************************************************************************************************//**
 * @brief     Entry function of USB peripheral vendor thread.
 * @param[IN] pointer to  pvParameters
 * @retval    None
 **********************************************************************************************************************/
void usb_peri_vendor_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);

    fsp_pack_version_t version = {RESET_VALUE};
    fsp_err_t err              = FSP_SUCCESS;
    BaseType_t err_queue       = pdFALSE;

    /* Version get API for FLEX pack information */
    R_FSP_VersionGet(&version);

    /* Example Project information printed on the Console */
    APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
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
            /* check for out pipe */
            if ((bulk_out_pipe == p_usb_event->pipe) && (FSP_ERR_USB_FAILED != p_usb_event->status))
            {
                pipe = bulk_in_pipe;

                /* Data comparison read from host */
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

                    /* Write data back to host */
                    err = R_USB_PipeWrite(&g_basic_ctrl, &g_buf[RESET_VALUE], p_usb_event->data_size, pipe);
                    if (FSP_SUCCESS != err)
                    {
                        APP_ERR_PRINT("\r\nR_USB_PipeWrite failed.\r\n");
                    }
                    else
                    {
                        APP_PRINT("\r\nUSB Write is initiated from Peripheral Vendor class");
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
            /* check for in pipe */
            if ((bulk_in_pipe == p_usb_event->pipe) && (FSP_ERR_USB_FAILED != p_usb_event->status))
            {
                if (USB_APL_MXPS == max_packet_size)
                {
                    pipe = bulk_out_pipe;
                    /* Send ZLP */
                    err = R_USB_PipeWrite(&g_basic_ctrl, RESET_VALUE, RESET_VALUE, p_usb_event->pipe);
                    if (FSP_SUCCESS != err)
                    {
                        APP_ERR_PRINT("\r\nR_USB_PipeWrite failed.\r\n");
                    }
                    else
                    {
                        APP_PRINT("\nZLP is successfully written from Peripheral vendor");
                    }
                }
                else
                {
                    APP_DBG_PRINT("\r\nData is successfully sent\r\n");
                    pipe = bulk_out_pipe;
                    memset(g_buf, RESET_VALUE, BUF_SIZE);
                    /* Read data back */
                    err = R_USB_PipeRead(&g_basic_ctrl, &g_buf[RESET_VALUE], BUF_SIZE, (uint8_t)pipe);
                    if (FSP_SUCCESS != err)
                    {
                        APP_ERR_PRINT("\r\nR_USB_PipeRead failed.\r\n");
                    }
                    else
                    {
                        APP_PRINT("\r\nUSB Read is initiated from Peripheral Vendor class\r\n");
                    }
                }
            }
            break;
        }

        case USB_STATUS_REQUEST:
        {
            /* process USB status request event */
            err = usb_status_request();
            if (FSP_SUCCESS != err)
            {
                APP_ERR_PRINT("\r\nusb_status_request failed.\r\n");
            }
            break;
        }

        case USB_STATUS_REQUEST_COMPLETE:
        {
            if (USB_GET_VENDOR == (p_usb_event->setup.request_type & USB_BREQUEST))
            {
                /* Start reading data */
                err = R_USB_PipeRead(&g_basic_ctrl, &g_buf[RESET_VALUE], (BUF_SIZE), bulk_out_pipe);
                if (FSP_SUCCESS != err)
                {
                    APP_ERR_PRINT("\r\nR_USB_PipeRead failed.\r\n");
                }
                else
                {
                    APP_PRINT("\r\nUSB Read operation  initiated from Peripheral Vendor class\r\n");
                }
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

    APP_PRINT("USB Configured Successfully\r\n");

    /* Get USB Pipe Information */
    err = R_USB_UsedPipesGet(&g_basic_ctrl, &used_pipe, USB_CLASS_PVND);
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

                APP_PRINT("\r\nBulkPiPe: %d Pipe Number: %d", pipe_info.transfer_type, pipe);

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
                else
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
                /* Do nothing */
            }
        }
        APP_DBG_PRINT("\r\nAll pipe info is fetched from USB Vendor Peripheral driver\r\n");
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief     Function processes USB status complete request event.
 * @param[IN] None
 * @retval    Any Other Error code apart from FSP_SUCCESS on Unsuccessful operation.
 **********************************************************************************************************************/
static fsp_err_t usb_status_request(void)
{
    fsp_err_t err     = FSP_SUCCESS;
    uint16_t request_length = RESET_VALUE;
    if (USB_SET_VENDOR_NO_DATA == (p_usb_event->setup.request_type & USB_BREQUEST))
    {
        /* Set ACk to host */
        err = R_USB_PeriControlStatusSet(&g_basic_ctrl, USB_SETUP_STATUS_ACK);
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\nR_USB_PeriControlStatusSet failed\r\n");
        }
        else
        {
            APP_PRINT("\r\nUSB ACK successfully sent to host\r\n");
        }
    }
    else if (USB_SET_VENDOR == (p_usb_event->setup.request_type & USB_BREQUEST))
    {
        request_length = p_usb_event->setup.request_length;
        /* Get data length from host */
        err = R_USB_PeriControlDataGet(&g_basic_ctrl, &g_request_buf[RESET_VALUE], request_length);
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\nR_USB_PeriControlDataGet failed\r\n");
        }
        else
        {
            APP_PRINT("\r\nUSB data length is received from host\r\n");
        }
    }
    else if (USB_GET_VENDOR == (p_usb_event->setup.request_type & USB_BREQUEST))
    {
        /* Set data length in peripheral */
        err = R_USB_PeriControlDataSet(&g_basic_ctrl, &g_request_buf[RESET_VALUE], request_length);
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\nR_UAB_PeriControlDataSet failed\r\n");
        }
        else
        {
            APP_PRINT("\r\nUSB data length is set successfully in Peripheral\r\n");
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
void usb_peri_vendor_callback(usb_event_info_t* p_event_info, usb_hdl_t handler, usb_onoff_t state)
{
    FSP_PARAMETER_NOT_USED (handler);
    FSP_PARAMETER_NOT_USED (state);

    /* Send event received to queue */
    if (pdTRUE != (xQueueSend(g_queue, (const void *)&p_event_info, (TickType_t)(RESET_VALUE))))
    {
        g_err_flag = true;
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
 * @} (end addtogroup usb_peripheral_vendor_ep)
 **********************************************************************************************************************/

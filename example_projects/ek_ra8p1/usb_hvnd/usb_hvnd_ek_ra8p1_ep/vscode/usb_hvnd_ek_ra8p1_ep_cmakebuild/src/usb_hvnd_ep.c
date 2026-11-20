/***********************************************************************************************************************
 * File Name    : usb_hvnd_ep.c
 * Description  : Contains data structures and functions used in usb_hvnd_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stddef.h>
#include <string.h>
#include "usb_hvnd_ep.h"

/* USB event information */
volatile usb_event_info_t * gp_usb_hvnd_event = NULL;

/* Flag set when the USB event cannot be sent to the queue */
volatile bool g_b_queue_err = false;

/* Data buffer and pipe information */
uint8_t g_buf[BUF_SIZE] = {RESET_VALUE};
uint8_t g_bulk_in_pipe = RESET_VALUE;
uint8_t g_bulk_out_pipe = RESET_VALUE;
uint16_t g_max_packet_size = USB_APL_MXPS;

/* Private variables */
static uint8_t s_request_buf[REQ_SIZE] = {RESET_VALUE};
static volatile bool s_usb_opened = false;

/*******************************************************************************************************************//**
 * @brief     This function initializes the USB module.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t usb_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Max packet size check */
    if (RESET_VALUE == (BUF_SIZE % USB_APL_MXPS))
    {
        g_max_packet_size = USB_APL_MXPS;
    }
    else
    {
        g_max_packet_size = RESET_VALUE;
    }

    err = R_USB_Open(&g_basic_ctrl, &g_basic_cfg);
    if (FSP_SUCCESS == err)
    {
        s_usb_opened = true;
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function gets the bulk in and bulk out pipe numbers used by the USB class.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Got successfully.
 * @retval    Any other error codes     Failed to get.
 **********************************************************************************************************************/
fsp_err_t usb_pipes_get(void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint16_t used_pipe = RESET_VALUE;
    usb_pipe_t pipe_info = {.endpoint = RESET_VALUE, .maxpacketsize = RESET_VALUE, .transfer_type = RESET_VALUE};

    err = R_USB_UsedPipesGet(&g_basic_ctrl, &used_pipe, ADDRESS);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    for (uint8_t pipe = START_PIPE; pipe < END_PIPE; pipe++)
    {
        /* Check for the used pipe */
        if (RESET_VALUE != (used_pipe & (1U << pipe)))
        {
            err = R_USB_PipeInfoGet(&g_basic_ctrl, &pipe_info, pipe);
            if (FSP_SUCCESS != err)
            {
                return err;
            }

            /* Only the bulk pipes are used for the data transfer */
            if (USB_TRANSFER_TYPE_BULK == pipe_info.transfer_type)
            {
                if (USB_EP_DIR_IN != (pipe_info.endpoint & USB_EP_DIR_IN))
                {
                    g_bulk_out_pipe = pipe;
                }
                else
                {
                    g_bulk_in_pipe = pipe;
                }
            }
        }
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function requests a vendor class control transfer to the device.
 * @param[in] request_type              Request type (SET_VENDOR_NO_DATA, SET_VENDOR, or GET_VENDOR).
 * @param[in] request_length            Length of the data stage.
 * @retval    FSP_SUCCESS               Requested successfully.
 * @retval    Any other error codes     Failed to request.
 **********************************************************************************************************************/
fsp_err_t usb_control_transfer(uint16_t request_type, uint16_t request_length)
{
    usb_setup_t setup = {.request_type = request_type, .request_value = RESET_VALUE,
                         .request_index = RESET_VALUE, .request_length = request_length};

    /* The request buffer holds the data to send, or is cleared to receive the data */
    if (GET_VENDOR == request_type)
    {
        memset(s_request_buf, RESET_VALUE, REQ_SIZE);
    }
    else
    {
        buffer_init(s_request_buf, REQ_SIZE);
    }

    return R_USB_HostControlTransfer(&g_basic_ctrl, &setup, &s_request_buf[RESET_VALUE],
                                     gp_usb_hvnd_event->device_address);
}

/*******************************************************************************************************************//**
 * @brief     This function checks the data read from the device.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Checked successfully.
 * @retval    FSP_ERR_ABORTED           The received data is incorrect.
 **********************************************************************************************************************/
fsp_err_t usb_data_check(void)
{
    for (uint16_t cnt = RESET_VALUE; cnt < (uint16_t)gp_usb_hvnd_event->data_size; cnt++)
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
 * @brief     This function writes the data buffer to the device. A zero-length packet is sent when the size is 0.
 * @param[in] size                      Number of bytes to write.
 * @retval    FSP_SUCCESS               Written successfully.
 * @retval    Any other error codes     Failed to write.
 **********************************************************************************************************************/
fsp_err_t usb_bulk_write(uint32_t size)
{
    return R_USB_PipeWrite(&g_basic_ctrl, &g_buf[RESET_VALUE], size, g_bulk_out_pipe);
}

/*******************************************************************************************************************//**
 * @brief     This function requests a read of the data sent from the device.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Requested successfully.
 * @retval    Any other error codes     Failed to request.
 **********************************************************************************************************************/
fsp_err_t usb_bulk_read(void)
{
    memset(g_buf, RESET_VALUE, BUF_SIZE);

    return R_USB_PipeRead(&g_basic_ctrl, &g_buf[RESET_VALUE], BUF_SIZE, g_bulk_in_pipe);
}

/*******************************************************************************************************************//**
 * @brief     This function de-initializes the opened USB module.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void usb_deinit(void)
{
    /* De-initialize the opened USB module */
    if (s_usb_opened)
    {
        R_USB_Close(&g_basic_ctrl);
        s_usb_opened = false;
    }
}

/*******************************************************************************************************************//**
 * @brief     This function waits for a USB event sent by the callback to the queue.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Received successfully, the event is stored in gp_usb_hvnd_event.
 * @retval    FSP_ERR_ABORTED           Failed to receive the USB event from the queue.
 **********************************************************************************************************************/
fsp_err_t queue_receive(void)
{
    if (pdTRUE != xQueueReceive(g_queue, &gp_usb_hvnd_event, portMAX_DELAY))
    {
        return FSP_ERR_ABORTED;
    }

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief     This function is a callback for USB HVND module.
 * @param[in] p_event_info              Pointer to the USB event information.
 * @param[in] handler                   USB handler (unused).
 * @param[in] state                     USB state (unused).
 * @retval    None.
 **********************************************************************************************************************/
void usb_host_vendor_callback(usb_event_info_t * p_event_info, usb_hdl_t handler, usb_onoff_t state)
{
    FSP_PARAMETER_NOT_USED(handler);
    FSP_PARAMETER_NOT_USED(state);

    /* Send the received event to the queue */
    if (pdTRUE != xQueueSend(g_queue, (const void *)&p_event_info, (TickType_t)RESET_VALUE))
    {
        g_b_queue_err = true;
    }
}

/*******************************************************************************************************************//**
 * @brief      This function fills the buffer with an incrementing data pattern.
 * @param[out] p_buf                    Pointer to the buffer to fill.
 * @param[in]  size                     Size of the buffer in bytes.
 * @retval     None.
 **********************************************************************************************************************/
void buffer_init(uint8_t * p_buf, uint16_t size)
{
    for (uint16_t cnt = RESET_VALUE; cnt < size; cnt++)
    {
        p_buf[cnt] = (uint8_t)cnt;
    }
}

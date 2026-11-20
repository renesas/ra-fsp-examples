/***********************************************************************************************************************
 * File Name    : usb_hhid_ep.c
 * Description  : Contains data structures and functions used in usb_hhid_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "usb_hhid_ep.h"

volatile bool g_err_flag = false;

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

    err = R_USB_Open(&g_usb_basic_ctrl, &g_usb_basic_cfg);
    if (FSP_SUCCESS == err)
    {
        s_usb_opened = true;
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief      This function obtains max packet size for the connected HID device.
 * @param[in]  p_api_ctrl               Pointer to the USB control block.
 * @param[out] p_size                   Pointer to store the maximum packet size.
 * @param[in]  direction                Transfer direction.
 * @param[in]  device_address           USB device address.
 * @retval     FSP_SUCCESS              Obtained successfully.
 * @retval     Any other error codes    Failed to obtain.
 **********************************************************************************************************************/
fsp_err_t usb_hhid_max_packet_size_get(usb_ctrl_t * const p_api_ctrl, uint16_t * p_size, uint8_t direction,
                                       uint8_t device_address)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_USB_HHID_MaxPacketSizeGet(p_api_ctrl, p_size, direction, device_address);

    return err;
}

/*******************************************************************************************************************//**
 * @brief      This function reads data from the USB device.
 * @param[in]  p_api_ctrl               Pointer to the USB control block.
 * @param[out] p_buf                    Pointer to the receive buffer.
 * @param[in]  size                     Number of bytes to read.
 * @param[in]  destination              USB device address.
 * @retval     FSP_SUCCESS              Read successfully.
 * @retval     Any other error codes    Failed to read.
 **********************************************************************************************************************/
fsp_err_t usb_read(usb_ctrl_t * const p_api_ctrl, uint8_t * p_buf, uint32_t size, uint8_t destination)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_USB_Read(p_api_ctrl, p_buf, size, destination);

    return err;
}

/*******************************************************************************************************************//**
 * @brief         This function performs a USB host control transfer.
 * @param[in]     p_api_ctrl            Pointer to the USB control block.
 * @param[in]     p_setup               Pointer to the USB setup packet.
 * @param[in,out] p_buf                 Pointer to the data buffer used for the control transfer.
 * @param[in]     device_address        USB device address.
 * @retval        FSP_SUCCESS           Operated successfully.
 * @retval        Any other error codes Failed to operate.
 **********************************************************************************************************************/
fsp_err_t usb_host_control_transfer(usb_ctrl_t * const p_api_ctrl, usb_setup_t * p_setup, uint8_t * p_buf,
                                    uint8_t device_address)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_USB_HostControlTransfer(p_api_ctrl, p_setup, p_buf, device_address);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function de-initializes the USB module.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void usb_deinit(void)
{
    /* De-initialize the opened USB module */
    if (s_usb_opened)
    {
        R_USB_Close(&g_usb_basic_ctrl);
        s_usb_opened = false;
    }
}

/*******************************************************************************************************************//**
 * @brief     This function is a callback function for USB HHID module.
 * @param[in] p_event_info              USB event information.
 * @param[in] handler                   USB transfer handle.
 * @param[in] on_off                    USB on/off state.
 * @retval    None.
 **********************************************************************************************************************/
void usb_hhid_callback(usb_event_info_t * p_event_info, usb_hdl_t handler, usb_onoff_t on_off)
{
    FSP_PARAMETER_NOT_USED(handler);
    FSP_PARAMETER_NOT_USED(on_off);

    /* Send event received to queue */
    if (pdTRUE != (xQueueSend(g_event_queue, (const void *)&p_event_info, (TickType_t)(RESET_VALUE))))
    {
        g_err_flag = true;
    }
}

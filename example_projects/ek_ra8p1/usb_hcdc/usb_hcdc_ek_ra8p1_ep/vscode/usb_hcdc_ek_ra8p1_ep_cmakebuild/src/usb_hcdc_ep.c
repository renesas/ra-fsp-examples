/***********************************************************************************************************************
 * File Name    : usb_hcdc_ep.c
 * Description  : Contains data structures and functions used in usb_hcdc_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "usb_hcdc_ep.h"

/* Global variable */
volatile bool g_err_flag = false;

/* Private variable */
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

    err = R_USB_Open(&g_basic_ctrl, &g_basic_cfg);
    if (FSP_SUCCESS == err)
    {
        s_usb_opened = true;
    }

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
 * @brief     This function writes data to the USB device.
 * @param[in] p_api_ctrl                Pointer to the USB control block.
 * @param[in] p_buf                     Pointer to the transmit buffer.
 * @param[in] size                      Number of bytes to write.
 * @param[in] destination               USB device address.
 * @retval    FSP_SUCCESS               Wrote successfully.
 * @retval    Any other error codes     Failed to write.
 **********************************************************************************************************************/
fsp_err_t usb_write(usb_ctrl_t * const p_api_ctrl, uint8_t const * const p_buf, uint32_t size, uint8_t destination)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_USB_Write(p_api_ctrl, p_buf, size, destination);

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
        R_USB_Close(&g_basic_ctrl);
        s_usb_opened = false;
    }
}

/*******************************************************************************************************************//**
 * @brief     This function is a callback for USB HCDC module.
 * @param[in] p_event_info              Triggered event.
 * @param[in] cur_task                  Current task handle.
 * @param[in] usb_state                 State of USB.
 * @retval    None.
 **********************************************************************************************************************/
void usb_rtos_callback(usb_event_info_t * p_event_info, usb_hdl_t cur_task, usb_onoff_t usb_state)
{
    FSP_PARAMETER_NOT_USED(cur_task);
    FSP_PARAMETER_NOT_USED(usb_state);

    /* Send event received to queue */
    if (pdTRUE != (xQueueSend(g_usb_queue, (const void *)&p_event_info, (TickType_t)(NO_WAIT_TIME))))
    {
        g_err_flag = true;
    }
}


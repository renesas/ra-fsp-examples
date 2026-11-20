/***********************************************************************************************************************
 * File Name    : usbx_dfu_ep.c
 * Description  : Contains data structures and functions used in usbx_dfu_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "usbx_dfu_ep.h"

/*******************************************************************************************************************//**
 * @brief     This function initializes the USB module.
 * @param[in] p_api_ctrl                Pointer to the USB control block.
 * @param[in] p_cfg                     Pointer to the USB configuration.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t usb_init(usb_ctrl_t * const p_api_ctrl, usb_cfg_t const * const p_cfg)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_USB_Open(p_api_ctrl, p_cfg);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function un-initializes the USB module.
 * @param[in] p_api_ctrl                Pointer to the USB control block.
 * @retval    FSP_SUCCESS               Un-initialized successfully.
 * @retval    Any other error codes     Failed to un-initialize.
 **********************************************************************************************************************/
fsp_err_t usb_deinit(usb_ctrl_t * const p_api_ctrl)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_USB_Close(p_api_ctrl);

    return err;
}

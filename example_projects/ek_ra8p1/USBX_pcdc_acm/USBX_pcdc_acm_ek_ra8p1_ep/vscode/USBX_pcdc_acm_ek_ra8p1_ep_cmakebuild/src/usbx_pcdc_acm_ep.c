/***********************************************************************************************************************
 * File Name    : usbx_pcdc_acm_ep.c
 * Description  : Contains data structures and functions used in usbx_pcdc_acm_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "usbx_pcdc_acm_ep.h"

/*******************************************************************************************************************//**
 * @brief     This function initializes the USB module.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t usb_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_USB_Open(&g_basic0_ctrl, &g_basic0_cfg);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function un-initializes the USB module.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Un-initialized successfully.
 * @retval    Any other error codes     Failed to un-initialize.
 **********************************************************************************************************************/
fsp_err_t usb_uninit(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_USB_Close(&g_basic0_ctrl);

    return err;
}

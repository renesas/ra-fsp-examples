/***********************************************************************************************************************
 * File Name    : usb_pmsc_ep.c
 * Description  : Contains data structures and functions used in usb_pmsc_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "usb_pmsc_ep.h"

/* Event flag to identify the USB event */
volatile usb_event_info_t * gp_usb_event = NULL;

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

/***********************************************************************************************************************
 * @brief     This function receives a USB event from the USB event queue.
 * @param[in] None.
 * @retval    pdTRUE                    Received successfully.
 * @retval    Any other error codes     Failed to receive.
 **********************************************************************************************************************/
BaseType_t usb_pmsc_event_get(void)
{
    return xQueueReceive(g_event_queue, &gp_usb_event, (portMAX_DELAY));
}

/*******************************************************************************************************************//**
 * @brief     This function de-initializes the USB module.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void usb_deinit(void)
{
    if (s_usb_opened)
    {
        R_USB_Close(&g_basic_ctrl);
        s_usb_opened = false;
    }
}

/*******************************************************************************************************************//**
 * @brief     This function is a callback function for USB PMSC module.
 * @param[in] p_event_info              USB event information.
 * @param[in] handler                   USB transfer handle.
 * @param[in] on_off                    USB on/off state.
 * @retval    None.
 **********************************************************************************************************************/
void pmsc_freertos_callback(usb_event_info_t * p_event_info, usb_hdl_t handler, usb_onoff_t on_off)
{
    FSP_PARAMETER_NOT_USED(handler);
    FSP_PARAMETER_NOT_USED(on_off);
    xQueueSendFromISR(g_event_queue, (const void *)&p_event_info, NULL);
}

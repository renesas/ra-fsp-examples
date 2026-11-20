/***********************************************************************************************************************
 * File Name    : usb_phid_ep.c
 * Description  : Contains data structures and functions used in usb_phid_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stddef.h>
#include "usb_phid_ep.h"

/* USB event information */
volatile usb_event_info_t * gp_usb_phid_event = NULL;

/* Numlock data received from the host */
uint16_t g_numlock = RESET_VALUE;

/* Private variables */
static uint8_t s_send_data[BUFF_SIZE] BSP_ALIGN_VARIABLE(ALIGN);
static uint8_t * sp_idle_value = NULL;
static uint8_t s_idle = RESET_VALUE;
static uint8_t s_data[DATA_LEN] = {RESET_VALUE};
static uint8_t s_buf[DATA_LEN] = {RESET_VALUE};
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
 * @brief     This function processes USB enumeration requests.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Processed successfully.
 * @retval    Any other error codes     Failed to process.
 **********************************************************************************************************************/
fsp_err_t usb_enumeration(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Check for request type */
    if (USB_SET_REPORT == (gp_usb_phid_event->setup.request_type & USB_BREQUEST))
    {
        /* Get the numlock data */
        err = R_USB_PeriControlDataGet(&g_basic_ctrl, (uint8_t *)&g_numlock, SIZE_NUM);
        if (FSP_SUCCESS != err)
        {
            return err;
        }
    }
    else if (USB_GET_DESCRIPTOR == (gp_usb_phid_event->setup.request_type & USB_BREQUEST))
    {
        /* Check for request value */
        if (USB_GET_REPORT_DESCRIPTOR == gp_usb_phid_event->setup.request_value)
        {
            err = R_USB_PeriControlDataSet(&g_basic_ctrl, (uint8_t *)g_apl_report, USB_RECEIVE_REPORT_DESCRIPTOR);
            if (FSP_SUCCESS != err)
            {
                return err;
            }
        }
        else if (USB_GET_HID_DESCRIPTOR == gp_usb_phid_event->setup.request_value)
        {
            for (uint8_t hid_des = RESET_VALUE; hid_des < USB_RECEIVE_HID_DESCRIPTOR; hid_des++)
            {
                s_send_data[hid_des] = g_apl_configuration[CD_LENGTH + hid_des];
            }

            /* Configuration descriptor address set */
            err = R_USB_PeriControlDataSet(&g_basic_ctrl, s_send_data, USB_RECEIVE_HID_DESCRIPTOR);
            if (FSP_SUCCESS != err)
            {
                return err;
            }
        }
        else
        {
            /* Do nothing */
        }
    }
    else if (USB_SET_IDLE == (gp_usb_phid_event->setup.request_type & USB_BREQUEST))
    {
        /* Get the SetIdle value */
        sp_idle_value = (uint8_t *)&gp_usb_phid_event->setup.request_value;

        s_idle = sp_idle_value[IDLE_VAL_INDEX];

        err = R_USB_PeriControlStatusSet(&g_basic_ctrl, USB_SETUP_STATUS_ACK);
        if (FSP_SUCCESS != err)
        {
            return err;
        }
    }
    else
    {
        /* Do nothing */
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function updates the USB status.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Updated successfully.
 * @retval    Any other error codes     Failed to update.
 **********************************************************************************************************************/
fsp_err_t usb_status_update(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Check for request type */
    if (USB_SET_IDLE == (gp_usb_phid_event->setup.request_type & USB_BREQUEST))
    {
        sp_idle_value = (uint8_t *)&gp_usb_phid_event->setup.request_value;
        s_idle = sp_idle_value[IDLE_VAL_INDEX];
    }
    else if (USB_SET_PROTOCOL == (gp_usb_phid_event->setup.request_type & USB_BREQUEST))
    {
        /* Do nothing */
    }
    else if (USB_SET_REPORT == (gp_usb_phid_event->setup.request_type & USB_BREQUEST))
    {
        /* Do nothing - numlock status is printed by the application layer */
    }
    else
    {
        /* Send the zero data (8 bytes) */
        err = R_USB_Write(&g_basic_ctrl, s_buf, DATA_LEN, USB_CLASS_PHID);
        if (FSP_SUCCESS != err)
        {
            return err;
        }
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function performs the USB write operation.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Wrote successfully.
 * @retval    Any other error codes     Failed to write.
 **********************************************************************************************************************/
fsp_err_t usb_write_operation(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Check flag from user input */
    if (true == g_write_flag)
    {
        /* Usage ID of A */
        static uint8_t data = USAGE_ID_A;

        if (USAGE_ID_0 >= data)
        {
            /* Copy data to the key code byte of the HID report buffer */
            s_data[KEYCODE_BYTE_INDEX] = data;

            /* Increase the data */
            data++;

            /* Send the 8 bytes data */
            err = R_USB_Write(&g_basic_ctrl, s_data, DATA_LEN, USB_CLASS_PHID);
            if (FSP_SUCCESS != err)
            {
                return err;
            }
        }
        else
        {
            data = USAGE_ID_A;
            g_write_flag = false;

            /* Send the zero data (8 bytes) */
            err = R_USB_Write(&g_basic_ctrl, s_buf, DATA_LEN, USB_CLASS_PHID);
            if (FSP_SUCCESS != err)
            {
                return err;
            }
        }
    }
    else
    {
        /* Send the zero data (8 bytes) */
        err = R_USB_Write(&g_basic_ctrl, s_buf, DATA_LEN, USB_CLASS_PHID);
        if (FSP_SUCCESS != err)
        {
            return err;
        }
    }

    return err;
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
 * @brief     This function is a callback for USB PHID module.
 * @param[in] p_usb_event               Pointer to the USB event information.
 * @param[in] task                      USB task (unused).
 * @param[in] state                     USB state (unused).
 * @retval    None.
 **********************************************************************************************************************/
void usb_phid_callback(usb_event_info_t * p_usb_event, usb_hdl_t task, usb_onoff_t state)
{
    FSP_PARAMETER_NOT_USED(task);
    FSP_PARAMETER_NOT_USED(state);

    /* Queue the USB event information */
    (void)xQueueSendFromISR(g_event_queue, (const void *)&p_usb_event, NULL);
}

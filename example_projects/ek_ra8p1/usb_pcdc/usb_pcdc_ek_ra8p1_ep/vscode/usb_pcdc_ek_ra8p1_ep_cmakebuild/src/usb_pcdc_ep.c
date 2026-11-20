/***********************************************************************************************************************
 * File Name    : usb_pcdc_ep.c
 * Description  : Contains data structures and functions used in usb_pcdc_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "usb_pcdc_ep.h"

/* USB descriptor table used by the USB driver */
const usb_descriptor_t usb_descriptor =
{
    g_apl_device,                   /* Pointer to the device descriptor */
    g_apl_configuration,            /* Pointer to the configuration descriptor for Full-Speed */
    g_apl_hs_configuration,         /* Pointer to the configuration descriptor for Hi-Speed */
    g_apl_qualifier_descriptor,     /* Pointer to the qualifier descriptor */
    g_apl_string_table,             /* Pointer to the string descriptor table */
    NUM_STRING_DESCRIPTOR           /* Number of the string descriptors */
};

/* Global variables */
uint8_t g_read_buf[READ_BUF_SIZE] = {RESET_VALUE};
bool g_b_usb_attach = false;
bool g_b_usb_write_complete = false;

/* Private variable */
static usb_pcdc_linecoding_t s_line_coding;

/*******************************************************************************************************************//**
 * @brief     This function opens the USB instance.
 * @param[in] None.
 * @retval    FSP_SUCCESS                   Opened successfully.
 * @retval    Any other error codes         Failed to open.
 **********************************************************************************************************************/
fsp_err_t usb_init(void)
{
    return R_USB_Open(&g_basic0_ctrl, &g_basic0_cfg);
}

/*******************************************************************************************************************//**
 * @brief      This function gets the USB event.
 * @param[out] p_event_info                 Pointer to store the USB event information.
 * @param[out] p_usb_event                  Pointer to store the USB event status.
 * @retval     FSP_SUCCESS                  Got successfully.
 * @retval     Any other error codes        Failed to get.
 **********************************************************************************************************************/
fsp_err_t usb_event_get(usb_event_info_t * p_event_info, usb_status_t * p_usb_event)
{
    return R_USB_EventGet(p_event_info, p_usb_event);
}

/*******************************************************************************************************************//**
 * @brief     This function requests a read of the data sent from the terminal.
 * @param[in] None.
 * @retval    FSP_SUCCESS                   Requested successfully.
 * @retval    Any other error codes         Failed to request.
 **********************************************************************************************************************/
fsp_err_t usb_read(void)
{
    return R_USB_Read(&g_basic0_ctrl, g_read_buf, READ_BUF_SIZE, USB_CLASS_PCDC);
}

/*******************************************************************************************************************//**
 * @brief     This function writes a string to the terminal.
 * @param[in] p_data                        Pointer to the string to be written.
 * @retval    FSP_SUCCESS                   Written successfully.
 * @retval    Any other error codes         Failed to write.
 **********************************************************************************************************************/
fsp_err_t usb_write(const char * p_data)
{
    uint32_t len = (uint32_t)strlen(p_data);

    /* Clear the ready flag for the USB write in progress */
    g_b_usb_write_complete = false;

    return R_USB_Write(&g_basic0_ctrl, (uint8_t const *)p_data, len, USB_CLASS_PCDC);
}

/*******************************************************************************************************************//**
 * @brief     This function handles the CDC class request received from the host.
 * @param[in] p_event_info                  Pointer to the USB event information.
 * @retval    FSP_SUCCESS                   Handled successfully.
 * @retval    Any other error codes         Failed to handle.
 **********************************************************************************************************************/
fsp_err_t usb_class_request(usb_event_info_t * p_event_info)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Check for the specific CDC class request IDs */
    if (USB_PCDC_SET_LINE_CODING == (p_event_info->setup.request_type & USB_BREQUEST))
    {
        err = R_USB_PeriControlDataGet(&g_basic0_ctrl, (uint8_t *)&s_line_coding, LINE_CODING_LENGTH);
    }
    else if (USB_PCDC_GET_LINE_CODING == (p_event_info->setup.request_type & USB_BREQUEST))
    {
        err = R_USB_PeriControlDataSet(&g_basic0_ctrl, (uint8_t *)&s_line_coding, LINE_CODING_LENGTH);
    }
    else if (USB_PCDC_SET_CONTROL_LINE_STATE == (p_event_info->setup.request_type & USB_BREQUEST))
    {
        err = R_USB_PeriControlStatusSet(&g_basic0_ctrl, USB_SETUP_STATUS_ACK);
    }
    else
    {
        /* Do nothing */
    }

    return err;
}

/***********************************************************************************************************************
* File Name    : phid_main_thread_entry.c
* Description  : Contains functions from the PHID main thread.
***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "phid_main_thread.h"
#include "common_utils.h"
#include "usb_phid_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup usb_phid_ep
 * @{
 **********************************************************************************************************************/

/* External variables */
extern uint8_t g_apl_configuration[];
extern uint8_t g_apl_report[];
extern bool g_write_flag;

/* Private function declarations */
static void usb_enumeration(void);
static void usb_status_update(void);
static void usb_write_operation(void);
static void deinit_usb(void);

/* Private variables */
static uint8_t send_data[BUFF_SIZE] BSP_ALIGN_VARIABLE(ALIGN);
static uint8_t * p_idle_value = NULL;
static usb_event_info_t * p_usb_phid_event = NULL;
static uint8_t g_idle = RESET_VALUE;
static uint8_t g_buf[DATA_LEN]  = {RESET_VALUE,}; /* HID NULL data */
static uint8_t g_data[DATA_LEN] = {RESET_VALUE,};
static uint16_t g_numlock = RESET_VALUE;

/* Error status flag */
static volatile bool g_err_flag = false;

/*******************************************************************************************************************//**
 * @brief       PHID main thread entry
 * @param[IN]   pvParameters
 * @retval      None
 **********************************************************************************************************************/
void phid_main_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED(pvParameters);
    BaseType_t err_queue       = pdFALSE;

    fsp_err_t err = FSP_SUCCESS;
    /* Open USB instance */
    err = R_USB_Open(&g_basic_ctrl, &g_basic_cfg);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nR_USB_Open failed\r\n");
        APP_ERR_TRAP(err);
    }

    while (true)
    {
        /* Check if USB event is received */
        err_queue = xQueueReceive(g_event_queue, &p_usb_phid_event, (portMAX_DELAY));
        if(pdTRUE != err_queue)
        {
            APP_ERR_PRINT("\r\nNo USB event received. Please check the USB connection.\r\n");
        }

        /* Check for USB event */
        switch (p_usb_phid_event->event)
        {
            /* Configured event */
            case USB_STATUS_CONFIGURED:
            {
                APP_PRINT("\r\nUSB status configured successfully\r\n");
                break;
            }
            /* Receive class request */
            case USB_STATUS_REQUEST:
            {
                usb_enumeration();
                break;
            }
            /* Complete class request */
            case USB_STATUS_REQUEST_COMPLETE:
            {
                usb_status_update();
                break;
            }
            /* Write complete event */
            case USB_STATUS_WRITE_COMPLETE:
            {
                vTaskDelay(DELAY/portTICK_PERIOD_MS);

                usb_write_operation();
                break;
            }
            /* Suspend event */
            case USB_STATUS_SUSPEND:
            {
                APP_PRINT("\r\nUSB STATUS: SUSPEND\r\n");
                break;
            }
            /* Detach event */
            case USB_STATUS_DETACH:
            {
                APP_PRINT("\r\nUSB STATUS: DETACH\r\n");
                break;
            }
            default:
            {
                break;
            }
        }
        vTaskDelay(TASK_DELAY);
    }
}

/*******************************************************************************************************************//**
 * @brief       USB PHID callback.
 * @param[IN]   p_usb_event     Pointer to the USB event information.
 * @param[IN]   task            USB task (unused).
 * @param[IN]   state           USB state (unused).
 * @retval      None
 **********************************************************************************************************************/
void usb_phid_callback(usb_event_info_t * p_usb_event , usb_hdl_t task, usb_onoff_t state)
{
    FSP_PARAMETER_NOT_USED(task);
    FSP_PARAMETER_NOT_USED(state);
    /* Capture the USB event info */
    if (pdTRUE != (xQueueSendFromISR(g_event_queue, (const void *) &p_usb_event, NULL)))
    {
        g_err_flag = true;
    }
}

/*******************************************************************************************************************//**
 * @brief       Handles USB PHID enumeration requests from the host.
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
static void usb_enumeration(void)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Check for request type */
    if (USB_SET_REPORT == (p_usb_phid_event->setup.request_type & USB_BREQUEST))
    {
        /* Get the NumLock data */
        err = R_USB_PeriControlDataGet(&g_basic_ctrl, (uint8_t *) &g_numlock, SIZE_NUM);
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\nR_USB_PeriControlDataGet API failed\r\n");
            deinit_usb();
            APP_ERR_TRAP(err);
        }
    }
    else if (USB_GET_DESCRIPTOR == (p_usb_phid_event->setup.request_type & USB_BREQUEST))
    {
        /* Check for request value */
        if (USB_GET_REPORT_DESCRIPTOR == p_usb_phid_event->setup.request_value)
        {
            err = R_USB_PeriControlDataSet(&g_basic_ctrl, (uint8_t *) g_apl_report, USB_RECEIVE_REPORT_DESCRIPTOR);
            if (FSP_SUCCESS != err)
            {
                APP_ERR_PRINT("\r\nR_USB_PeriControlDataSet API failed\r\n");
                deinit_usb();
                APP_ERR_TRAP(err);
            }
        }
        else if (USB_GET_HID_DESCRIPTOR == p_usb_phid_event->setup.request_value)
        {
            for (uint8_t hid_des = RESET_VALUE; hid_des < USB_RECEIVE_HID_DESCRIPTOR; hid_des++)
            {
                send_data[hid_des] = g_apl_configuration[CD_LENGTH + hid_des];
            }

            /* Configuration descriptor address set */
            err = R_USB_PeriControlDataSet(&g_basic_ctrl, send_data, USB_RECEIVE_HID_DESCRIPTOR);
            if (FSP_SUCCESS != err)
            {
                APP_ERR_PRINT("\r\nR_USB_PeriControlDataSet API failed\r\n");
                deinit_usb();
                APP_ERR_TRAP(err);
            }
        }
        else
        {
            /* Do nothing */
        }
    }
    else if (USB_SET_IDLE == (p_usb_phid_event->setup.request_type & USB_BREQUEST))
    {
        /* Get SetIdle value */
        p_idle_value = (uint8_t *) &p_usb_phid_event->setup.request_value;

        g_idle = p_idle_value[IDLE_VAL_INDEX];

        err = R_USB_PeriControlStatusSet(p_usb_phid_event, USB_SETUP_STATUS_ACK);
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\nR_USB_PeriControlStatusSet API failed\r\n");
            deinit_usb();
            APP_ERR_TRAP(err);
        }
    }
    else
    {
        /* Do nothing */
    }
}

/*******************************************************************************************************************//**
 * @brief       USB status update.
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
static void usb_status_update(void)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Check for request type */
    if (USB_SET_IDLE == (p_usb_phid_event->setup.request_type & USB_BREQUEST))
    {
        p_idle_value = (uint8_t *) &p_usb_phid_event->setup.request_value;

        g_idle = p_idle_value[IDLE_VAL_INDEX];
    }
    else if (USB_SET_PROTOCOL == (p_usb_phid_event->setup.request_type & USB_BREQUEST))
    {
        /* Do nothing */
    }
    else if (USB_SET_REPORT == (p_usb_phid_event->setup.request_type & USB_BREQUEST))
    {
        if ((0x2 == g_numlock) || (0x3 == g_numlock) || (0x6 == g_numlock) || (0x7 == g_numlock))
        {
            /* Print status of g_numlock */
            APP_PRINT("\r\nCaps Lock is ON\r\n");
        }
        else
        {
            APP_PRINT("\r\nCaps Lock is OFF\r\n");
        }

    }
    else
    {
        /* Sending the zero data (8 bytes) */
        err = R_USB_Write(&g_basic_ctrl, g_buf, DATA_LEN, USB_CLASS_PHID);
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\nR_USB_Write API failed\r\n");
            deinit_usb();
            APP_ERR_TRAP(err);
        }
    }
}

/*******************************************************************************************************************//**
 * @brief       USB write operation.
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
static void usb_write_operation(void)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Check flag from user input */
    if (true == g_write_flag)
    {
        /* Usage ID of A */
        static uint8_t data = USAGE_ID_A;

        if (USAGE_ID_0 >= data)
        {
            /* Copy data to 3rd Index of buffer */
            g_data[INDEX_THIRD] = data;
            /* Increment the data */
            data++;
            /* Sending the 8 bytes data */
            err = R_USB_Write(&g_basic_ctrl, (uint8_t *) g_data, DATA_LEN, USB_CLASS_PHID);
            if (FSP_SUCCESS != err)
            {
                APP_ERR_PRINT("\r\nR_USB_Write API failed\r\n");
                deinit_usb();
                APP_ERR_TRAP(err);
            }
        }
        else
        {
            data = USAGE_ID_A;
            g_write_flag = false;
            /* Sending the zero data (8 bytes) */
            err = R_USB_Write(&g_basic_ctrl, (uint8_t *) g_buf, DATA_LEN, USB_CLASS_PHID);
            if (FSP_SUCCESS != err)
            {
                APP_ERR_PRINT("\r\nR_USB_Write API failed\r\n");
                deinit_usb();
                APP_ERR_TRAP(err);
            }
        }
    }
    else
    {
        /* Sending the zero data (8 bytes) */
        err = R_USB_Write(&g_basic_ctrl, (uint8_t *) g_buf, DATA_LEN, USB_CLASS_PHID);
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\nR_USB_Write API failed\r\n");
            deinit_usb();
            APP_ERR_TRAP(err);
        }
    }
}

/*******************************************************************************************************************//**
 * @brief       De-initialize USB.
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
static void deinit_usb(void)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Close USB instance */
    err = R_USB_Close(&g_basic_ctrl);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nR_USB_Close API failed\r\n ");
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup usb_phid_ep)
 **********************************************************************************************************************/

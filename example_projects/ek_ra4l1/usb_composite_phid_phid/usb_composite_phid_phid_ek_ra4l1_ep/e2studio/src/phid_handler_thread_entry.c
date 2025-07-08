/***********************************************************************************************************************
 * File Name    : phid_main_thread_entry.c
 * Description  : Contains functions from the phid main thread
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2022 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "phid_handler_thread.h"
#include "common_utils.h"
#include "usb_phid_phid.h"

/*******************************************************************************************************************//**
 * @addtogroup usb_phid_phid
 * @{
 **********************************************************************************************************************/
/* External variables*/
extern uint8_t g_apl_configuration[];
extern uint8_t g_apl_report_keyboard[];
extern uint8_t g_apl_report_mouse[];
extern bool g_b_key_flag;
extern bool g_b_mouse_flag;

/* Private function declarations */
static void usb_enumeration(void);
static void usb_status_update(void);
static void usb_write_operation(void);
static void deinit_usb(void);
static void user_keyboard_emulator(void);
static void user_mouse_emulator(void);

/* Private variables */
static usb_event_info_t * p_usb_basic_event = NULL;
static uint8_t * p_idle_value = NULL;
static uint8_t g_idle = RESET_VALUE;
static uint16_t g_key_numlock = RESET_VALUE;
static uint8_t g_key_zero[KEY_DATA_LEN]  = {RESET_VALUE,};      /* HID Keyboard NULL data */
static uint8_t g_key_data[KEY_DATA_LEN]  = {RESET_VALUE,};
static uint8_t g_mouse_zero[MOUSE_DATA_LEN]  = {RESET_VALUE,};  /* HID MOUSE NULL data */
static uint8_t g_mouse_data[MOUSE_DATA_LEN]  = {RESET_VALUE,};

/*******************************************************************************************************************//**
 * @brief       phid handler thread entry
 * @param[IN]   pvParameters
 * @retval      None
 **********************************************************************************************************************/
void phid_handler_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);
    BaseType_t err_queue       = pdFALSE;

    fsp_err_t err = FSP_SUCCESS;

    /* Wait for information display on RTT Viewer */
    R_BSP_SoftwareDelay(500, BSP_DELAY_UNITS_MILLISECONDS);

    /* Open USB instance */
    err = R_USB_Open(&g_basic0_ctrl, &g_basic0_cfg);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nR_USB_Open failed\r\n");
        APP_ERR_TRAP(err);
    }

    while (true)
    {
        /* Check if USB event is received */
        err_queue = xQueueReceive(g_event_queue, &p_usb_basic_event, (portMAX_DELAY));
        if(pdTRUE != err_queue)
        {
            APP_ERR_PRINT("\r\nNo USB Event received. Please check USB connection \r\n");
            deinit_usb();
            APP_ERR_TRAP(FSP_ERR_INTERNAL);
        }
        /* Check for usb event */
        switch (p_usb_basic_event->event)
        {
            /* Configured event */
            case USB_STATUS_CONFIGURED:
            {
                APP_PRINT("\r\nUSB Status Configured Successful\r\n");
                break;
            }
            /* Receive Class Request */
            case USB_STATUS_REQUEST:
            {
                usb_enumeration();
                break;
            }
            /* Complete Class Request */
            case USB_STATUS_REQUEST_COMPLETE:
            {
                usb_status_update();
                break;
            }
            /* Write complete event */
            case USB_STATUS_WRITE_COMPLETE:
            {
                usb_write_operation();
                break;
            }
            /* Suspend event */
            case USB_STATUS_SUSPEND:
            {
                APP_PRINT("\r\nUSB STATUS : SUSPEND\r\n");
                break;
            }
            /* Detach event */
            case USB_STATUS_DETACH:
            {
                APP_PRINT("\r\nUSB STATUS : DETACH\r\n");
                break;
            }
            default:
            {
                break;
            }
        }
        vTaskDelay (TASK_DELAY);
    }
}

/*******************************************************************************************************************//**
 * @brief       usb basic0 callback
 * @param[IN]   usb_event_info_t    usb event
 * @param[IN]   usb_hdl_t           task
 * @param[IN]   usb_onoff_t         state
 * @retval      None
 **********************************************************************************************************************/
void usb_basic0_callback(usb_event_info_t * p_usb_event , usb_hdl_t task, usb_onoff_t state)
{
    FSP_PARAMETER_NOT_USED (task);
    FSP_PARAMETER_NOT_USED (state);
    /* Capture new usb event info */
    if (pdTRUE != (xQueueSend(g_event_queue, (const void *)&p_usb_event, (TickType_t)(RESET_VALUE))))
    {
        APP_ERR_PRINT("\r\nxQueueSend failed\r\n");
        deinit_usb();
        APP_ERR_TRAP(FSP_ERR_INTERNAL);
    }
}

/*******************************************************************************************************************//**
 * @brief       usb enumeration
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
static void usb_enumeration(void)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Check for request type */
    if (USB_SET_REPORT == (p_usb_basic_event->setup.request_type & USB_BREQUEST))
    {
        /* Check for keyboard report descriptor request */
        if (0U == p_usb_basic_event->setup.request_index)
        {
            /* Get the NumLock data */
            err = R_USB_PeriControlDataGet(&g_basic0_ctrl, (uint8_t *) &g_key_numlock, KEY_SIZE_NUM);
            if (FSP_SUCCESS != err)
            {
                APP_ERR_PRINT("\r\nR_USB_PeriControlDataGet API failed\r\n");
                deinit_usb();
                APP_ERR_TRAP(err);
            }
        }
    }
    else if (USB_GET_DESCRIPTOR == (p_usb_basic_event->setup.request_type & USB_BREQUEST))
    {
        /* Check for request value */
        if (USB_GET_REPORT_DESCRIPTOR == p_usb_basic_event->setup.request_value)
        {
            /* Check for keyboard report descriptor request  */
            if (0U == p_usb_basic_event->setup.request_index)
            {
                err = R_USB_PeriControlDataSet (&g_basic0_ctrl, (uint8_t *)g_apl_report_keyboard, PHID_RD_LENGTH);
                if (FSP_SUCCESS != err)
                {
                    APP_ERR_PRINT("\r\nR_USB_PeriControlDataSet failed\r\n");
                    deinit_usb();
                    APP_ERR_TRAP(err);
                }
            }
            /* Check for mouse report descriptor request  */
            else if(1U == p_usb_basic_event->setup.request_index)
            {
                err = R_USB_PeriControlDataSet (&g_basic0_ctrl, (uint8_t *)g_apl_report_mouse, PHID2_RD_LENGTH);
                if (FSP_SUCCESS != err)
                {
                    APP_ERR_PRINT("\r\nR_USB_PeriControlDataSet failed\r\n");
                    deinit_usb();
                    APP_ERR_TRAP(err);
                }
            }
            /* Don't match request index */
            else
            {
                err = R_USB_PeriControlStatusSet(&g_basic0_ctrl, USB_SETUP_STATUS_STALL);
                if (FSP_SUCCESS != err)
                {
                    APP_ERR_PRINT("\r\nR_USB_PeriControlStatusSet failed\r\n");
                    deinit_usb();
                    APP_ERR_TRAP(err);
                }
            }
        }
        /* Check for get hid descriptor request  */
        else if (USB_GET_HID_DESCRIPTOR == p_usb_basic_event->setup.request_value)
        {
            /* Check for get keyboard hid descriptor request  */
            if(0U == p_usb_basic_event->setup.request_index)
            {
                err = R_USB_PeriControlDataSet(&g_basic0_ctrl, &g_apl_configuration[PHID_D_INDEX], HID_D_LENGTH);
                if (FSP_SUCCESS != err)
                {
                    APP_ERR_PRINT("\r\nR_USB_PeriControlDataSet failed\r\n");
                    deinit_usb();
                    APP_ERR_TRAP(err);
                }
            }
            /* Check for get mouse hid descriptor request  */
            else if(1U == p_usb_basic_event->setup.request_index)
            {
                err = R_USB_PeriControlDataSet (&g_basic0_ctrl, &g_apl_configuration[PHID2_D_INDEX], HID_D_LENGTH);
                if (FSP_SUCCESS != err)
                {
                    APP_ERR_PRINT("\r\nR_USB_PeriControlDataSet failed\r\n");
                    deinit_usb();
                    APP_ERR_TRAP(err);
                }
            }
            /* Don't match request index */
            else
            {
                err = R_USB_PeriControlStatusSet(&g_basic0_ctrl, USB_SETUP_STATUS_STALL);
                if (FSP_SUCCESS != err)
                {
                    APP_ERR_PRINT("\r\nR_USB_PeriControlStatusSet failed\r\n");
                    deinit_usb();
                    APP_ERR_TRAP(err);
                }
            }
        }
        /* Don't support request */
        else
        {
            err = R_USB_PeriControlStatusSet(&g_basic0_ctrl, USB_SETUP_STATUS_STALL);
            if (FSP_SUCCESS != err)
            {
                APP_ERR_PRINT("\r\nR_USB_PeriControlStatusSet failed\r\n");
                deinit_usb();
                APP_ERR_TRAP(err);
            }
        }
    }
    else if (USB_SET_IDLE == (p_usb_basic_event->setup.request_type & USB_BREQUEST))
    {
        /* Get SetIdle value */
        p_idle_value = (uint8_t *) &p_usb_basic_event->setup.request_value;

        g_idle = p_idle_value[IDLE_VAL_INDEX];

        err = R_USB_PeriControlStatusSet (&g_basic0_ctrl, USB_SETUP_STATUS_ACK);
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\nR_USB_PeriControlStatusSet failed\r\n");
            deinit_usb();
            APP_ERR_TRAP(err);
        }
    }
    else
    {
        err = R_USB_PeriControlStatusSet (&g_basic0_ctrl, USB_SETUP_STATUS_STALL);
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\nR_USB_PeriControlStatusSet failed\r\n");
            deinit_usb();
            APP_ERR_TRAP(err);
        }
    }
}

/*******************************************************************************************************************//**
 * @brief       usb status update
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
static void usb_status_update(void)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Check for request type */
    if (USB_SET_IDLE == (p_usb_basic_event->setup.request_type & USB_BREQUEST))
    {
        p_idle_value = (uint8_t *) &p_usb_basic_event->setup.request_value;

        g_idle = p_idle_value[IDLE_VAL_INDEX];
    }
    else if (USB_SET_PROTOCOL == (p_usb_basic_event->setup.request_type & USB_BREQUEST))
    {
        /* None */
    }
    else
    {
        /* Check to write zero data keyboard report */
        if(0U == p_usb_basic_event->setup.request_index)
        {
            err = R_USB_Write (&g_basic0_ctrl, (uint8_t *) g_key_zero, KEY_DATA_LEN, USB_CLASS_PHID);
            if (FSP_SUCCESS != err)
            {
                APP_ERR_PRINT("\r\nR_USB_Write failed\r\n");
                deinit_usb();
                APP_ERR_TRAP(err);
            }
        }
        /* Write zero data mouse report */
        else
        {
            err = R_USB_Write (&g_basic0_ctrl, (uint8_t *) g_mouse_zero, MOUSE_DATA_LEN, USB_CLASS_PHID2);
            if (FSP_SUCCESS != err)
            {
                APP_ERR_PRINT("\r\nR_USB_Write failed\r\n");
                deinit_usb();
                APP_ERR_TRAP(err);
            }
        }
    }
}

/*******************************************************************************************************************//**
 * @brief       usb write operation
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
static void usb_write_operation(void)
{
    /* Check writing complete keyboard report */
    if(USB_CLASS_PHID == p_usb_basic_event->type)
    {
        /* Write mouse report */
        user_mouse_emulator();
    }
    /* Check writing complete mouse report */
    else
    {
        /* Write keyboard report */
        user_keyboard_emulator();
    }
}

/*******************************************************************************************************************//**
 * @brief       de-initialize USB
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
static void deinit_usb(void)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Close usb instance */
    err = R_USB_Close(&g_basic0_ctrl);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nR_USB_Close\r\n");
        deinit_usb();
        APP_ERR_TRAP(err);
    }
}

/*******************************************************************************************************************//**
 * @brief       keyboard emulator handler
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
static void user_keyboard_emulator(void)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Check flag from user input */
    if (true == g_b_key_flag)
    {
        /* Usage ID of A */
        static uint8_t key_data = USAGE_ID_A;

        if (USAGE_ID_0 >= key_data)
        {
            /* Copy data to 3rd Index of buffer*/
            g_key_data[KEY_DATA_INDEX] = key_data;
            /* increment the data */
            key_data++;
            /* Sending the 8 bytes data */
            err = R_USB_Write (&g_basic0_ctrl, (uint8_t *) g_key_data, KEY_DATA_LEN, USB_CLASS_PHID);
            if (FSP_SUCCESS != err)
            {
                APP_ERR_PRINT("\r\nR_USB_Write failed\r\n");
                deinit_usb();
                APP_ERR_TRAP(err);
            }
        }
        else
        {
            key_data = USAGE_ID_A;
            g_b_key_flag = false;
            /* Sending the zero data (8 bytes) */
            err = R_USB_Write (&g_basic0_ctrl, (uint8_t *) g_key_zero, KEY_DATA_LEN, USB_CLASS_PHID);
            if (FSP_SUCCESS != err)
            {
                APP_ERR_PRINT("\r\nR_USB_Write failed\r\n");
                deinit_usb();
                APP_ERR_TRAP(err);
            }
        }
    }
    /* Stop keyboard emulator, no key is pressed */
    else
    {
        /* Sending the zero data (8 bytes) */
        err = R_USB_Write(&g_basic0_ctrl, (uint8_t *) g_key_zero, KEY_DATA_LEN, USB_CLASS_PHID);
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\nR_USB_Write failed\r\n");
            deinit_usb();
            APP_ERR_TRAP(err);
        }
    }
}

/*******************************************************************************************************************//**
 * @brief       mouse emulator handler
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
static void user_mouse_emulator(void)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Check user input from RTT viewer to run mouse emulator */
    if (true == g_b_mouse_flag)
    {
        static uint16_t mouse_times = 0U;

        if(MOVE_RIGHT_TIMES >= mouse_times)
        {
            mouse_times ++;
            g_mouse_data[MOUSE_X_INDEX] = (uint8_t)MOUSE_X_RIGHT(MOUSE_SPEED);
            g_mouse_data[MOUSE_Y_INDEX] = (uint8_t)MOUSE_STAND;
        }
        else if (MOVE_DOWN_TIMES >= mouse_times)
        {
            mouse_times ++;
            g_mouse_data[MOUSE_X_INDEX] = (uint8_t)MOUSE_STAND;
            g_mouse_data[MOUSE_Y_INDEX] = (uint8_t)MOUSE_Y_DOWN(MOUSE_SPEED);
        }
        else if (MOVE_LEFT_TIMES >= mouse_times)
        {
            mouse_times ++;
            g_mouse_data[MOUSE_X_INDEX] = (uint8_t)MOUSE_X_LEFT(MOUSE_SPEED);
            g_mouse_data[MOUSE_Y_INDEX] = (uint8_t)MOUSE_STAND;
        }
        else if (MOVE_UP_TIMES >= mouse_times)
        {
            mouse_times ++;
            g_mouse_data[MOUSE_X_INDEX] = (uint8_t)MOUSE_STAND;
            g_mouse_data[MOUSE_Y_INDEX] = (uint8_t)MOUSE_Y_UP(MOUSE_SPEED);
        }
        else
        {
            g_b_mouse_flag = false;
            mouse_times = 0U;
            g_mouse_data[MOUSE_X_INDEX] = (uint8_t)MOUSE_STAND;
            g_mouse_data[MOUSE_Y_INDEX] = (uint8_t)MOUSE_STAND;
        }

        /* Sending the mouse data (3 bytes) */
        err = R_USB_Write (&g_basic0_ctrl, (uint8_t *) g_mouse_data, MOUSE_DATA_LEN, USB_CLASS_PHID2);
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\nR_USB_Write failed\r\n");
            deinit_usb();
            APP_ERR_TRAP(err);
        }
    }
    /* Stop mouse emulator, no mouse activity */
    else
    {
        /* Sending the zero data (3 bytes) */
        err = R_USB_Write(&g_basic0_ctrl, (uint8_t *) g_mouse_zero, MOUSE_DATA_LEN, USB_CLASS_PHID2);
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\nR_USB_Write failed\r\n");
            deinit_usb();
            APP_ERR_TRAP(err);
        }
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup usb_phid_phid)
 **********************************************************************************************************************/

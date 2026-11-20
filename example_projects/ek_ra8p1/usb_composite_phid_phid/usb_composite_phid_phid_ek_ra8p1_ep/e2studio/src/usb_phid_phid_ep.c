/***********************************************************************************************************************
 * File Name    : usb_phid_phid_ep.c
 * Description  : Contains data structures and functions used in the usb_phid_phid_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "usb_phid_phid_ep.h"

/* Global variable */
bool g_b_key_flag = false;
bool g_b_mouse_flag = false;

/* Private function declarations */
static fsp_err_t user_keyboard_emulator(void);
static fsp_err_t user_mouse_emulator(void);

/* Private variables */
static bool s_b_usb_is_open = false;
static uint8_t * s_p_idle_value = NULL;
static uint8_t s_idle = RESET_VALUE;
static uint16_t s_key_numlock = RESET_VALUE;
static uint8_t s_key_zero[KEY_DATA_LEN] = {RESET_VALUE,};       /* HID Keyboard NULL data */
static uint8_t s_key_data[KEY_DATA_LEN] = {RESET_VALUE,};
static uint8_t s_mouse_zero[MOUSE_DATA_LEN] = {RESET_VALUE,};   /* HID Mouse NULL data */
static uint8_t s_mouse_data[MOUSE_DATA_LEN] = {RESET_VALUE,};

/*******************************************************************************************************************//**
 * @brief     This function initializes the USB module.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t usb_init(void)
{
    fsp_err_t err = R_USB_Open(&g_basic0_ctrl, &g_basic0_cfg);

    if (FSP_SUCCESS == err)
    {
        s_b_usb_is_open = true;
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function closes the opened USB module.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void usb_deinit(void)
{
    /* De-initialize the opened USB module */
    if (s_b_usb_is_open)
    {
        R_USB_Close(&g_basic0_ctrl);
        s_b_usb_is_open = false;
    }
}

/*******************************************************************************************************************//**
 * @brief     This function is a callback for USB module stack.
 * @param[in] p_event                   Pointer to the USB event.
 * @param[in] task                      USB task handle (unused).
 * @param[in] state                     USB on/off state (unused).
 * @retval    None.
 **********************************************************************************************************************/
void usb_basic0_callback(usb_event_info_t * p_event, usb_hdl_t task, usb_onoff_t state)
{
    FSP_PARAMETER_NOT_USED(task);
    FSP_PARAMETER_NOT_USED(state);

    /* Capture new USB event info */
    if (pdTRUE != xQueueSend(g_event_queue, (const void *)&p_event, (TickType_t)(RESET_VALUE)))
    {
        /* xQueueSend fail handle */
        usb_deinit();
        __BKPT(0);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function handles a USB class request event (USB_STATUS_REQUEST).
 * @param[in] p_event                   Pointer to the USB event.
 * @retval    FSP_SUCCESS               Handled successfully.
 * @retval    Any other error codes     Failed to handle.
 **********************************************************************************************************************/
fsp_err_t usb_enumeration(usb_event_info_t * p_event)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Check for request type */
    if (USB_SET_REPORT == (p_event->setup.request_type & USB_BREQUEST))
    {
        /* Check for Keyboard report descriptor request */
        if (0U == p_event->setup.request_index)
        {
            /* Get the NumLock data */
            err = R_USB_PeriControlDataGet(&g_basic0_ctrl, (uint8_t *)&s_key_numlock, KEY_SIZE_NUM);
        }
    }
    else if (USB_GET_DESCRIPTOR == (p_event->setup.request_type & USB_BREQUEST))
    {
        /* Check for request value */
        if (USB_GET_REPORT_DESCRIPTOR == p_event->setup.request_value)
        {
            /* Check for Keyboard report descriptor request */
            if (0U == p_event->setup.request_index)
            {
                err = R_USB_PeriControlDataSet(&g_basic0_ctrl, (uint8_t *)g_apl_report_keyboard, PHID_RD_LENGTH);
            }

            /* Check for Mouse report descriptor request */
            else if (1U == p_event->setup.request_index)
            {
                err = R_USB_PeriControlDataSet(&g_basic0_ctrl, (uint8_t *)g_apl_report_mouse, PHID2_RD_LENGTH);
            }

            /* Don't match request index */
            else
            {
                err = R_USB_PeriControlStatusSet(&g_basic0_ctrl, USB_SETUP_STATUS_STALL);
            }
        }

        /* Check for get HID descriptor request */
        else if (USB_GET_HID_DESCRIPTOR == p_event->setup.request_value)
        {
            /* Check for get Keyboard HID descriptor request  */
            if (0U == p_event->setup.request_index)
            {
                err = R_USB_PeriControlDataSet(&g_basic0_ctrl, &g_apl_configuration[PHID_D_INDEX], HID_D_LENGTH);
            }

            /* Check for get Mouse HID descriptor request */
            else if (1U == p_event->setup.request_index)
            {
                err = R_USB_PeriControlDataSet(&g_basic0_ctrl, &g_apl_configuration[PHID2_D_INDEX], HID_D_LENGTH);
            }

            /* Don't match request index */
            else
            {
                err = R_USB_PeriControlStatusSet(&g_basic0_ctrl, USB_SETUP_STATUS_STALL);
            }
        }

        /* Don't support request */
        else
        {
            err = R_USB_PeriControlStatusSet(&g_basic0_ctrl, USB_SETUP_STATUS_STALL);
        }
    }
    else if (USB_SET_IDLE == (p_event->setup.request_type & USB_BREQUEST))
    {
        /* Get set IDLE value */
        s_p_idle_value = (uint8_t *)&p_event->setup.request_value;

        s_idle = s_p_idle_value[IDLE_VAL_INDEX];

        err = R_USB_PeriControlStatusSet(&g_basic0_ctrl, USB_SETUP_STATUS_ACK);
    }
    else
    {
        err = R_USB_PeriControlStatusSet(&g_basic0_ctrl, USB_SETUP_STATUS_STALL);
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function handles a USB class request complete event (USB_STATUS_REQUEST_COMPLETE).
 * @param[in] p_event                   Pointer to the USB event.
 * @retval    FSP_SUCCESS               Handled successfully.
 * @retval    Any other error codes     Failed to handle.
 **********************************************************************************************************************/
fsp_err_t usb_status_update(usb_event_info_t * p_event)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Check for request type */
    if (USB_SET_IDLE == (p_event->setup.request_type & USB_BREQUEST))
    {
        s_p_idle_value = (uint8_t *)&p_event->setup.request_value;

        s_idle = s_p_idle_value[IDLE_VAL_INDEX];
    }
    else if (USB_SET_PROTOCOL == (p_event->setup.request_type & USB_BREQUEST))
    {
        /* Do nothing */
    }
    else if (USB_SET_REPORT == (p_event->setup.request_type & USB_BREQUEST))
    {
        /* Do nothing */
    }
    else
    {
        /* Check to write zero data Keyboard report */
        if (0U == p_event->setup.request_index)
        {
            err = R_USB_Write(&g_basic0_ctrl, (uint8_t *)s_key_zero, KEY_DATA_LEN, USB_CLASS_PHID);
        }

        /* Write zero data Mouse report */
        else
        {
            err = R_USB_Write(&g_basic0_ctrl, (uint8_t *)s_mouse_zero, MOUSE_DATA_LEN, USB_CLASS_PHID2);
        }
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function handles a USB write complete event (USB_STATUS_WRITE_COMPLETE).
 * @param[in] p_event                   Pointer to the USB event.
 * @retval    FSP_SUCCESS               Handled successfully.
 * @retval    Any other error codes     Failed to handle.
 **********************************************************************************************************************/
fsp_err_t usb_write_operation(usb_event_info_t * p_event)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Check writing complete Keyboard report */
    if (USB_CLASS_PHID == p_event->type)
    {
        /* Write Mouse report */
        err = user_mouse_emulator();
    }

    /* Check writing complete Mouse report */
    else
    {
        /* Write Keyboard report */
        err = user_keyboard_emulator();
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function handle Keyboard emulator.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Handled successfully.
 * @retval    Any other error codes     Failed to handle.
 **********************************************************************************************************************/
static fsp_err_t user_keyboard_emulator(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Check flag from user input */
    if (true == g_b_key_flag)
    {
        /* Usage ID of A */
        static uint8_t s_key_usage_id = USAGE_ID_A;

        if (USAGE_ID_0 >= s_key_usage_id)
        {
            /* Copy data to 3rd index of buffer */
            s_key_data[KEY_DATA_INDEX] = s_key_usage_id;

            /* Increment the data */
            s_key_usage_id++;

            /* Sending the 8 bytes data */
            err = R_USB_Write(&g_basic0_ctrl, (uint8_t *)s_key_data, KEY_DATA_LEN, USB_CLASS_PHID);
        }
        else
        {
            s_key_usage_id = USAGE_ID_A;
            g_b_key_flag = false;

            /* Sending the zero data (8 bytes) */
            err = R_USB_Write(&g_basic0_ctrl, (uint8_t *)s_key_zero, KEY_DATA_LEN, USB_CLASS_PHID);
        }
    }

    /* Stop Keyboard emulator, no key is pressed */
    else
    {
        /* Sending the zero data (8 bytes) */
        err = R_USB_Write(&g_basic0_ctrl, (uint8_t *)s_key_zero, KEY_DATA_LEN, USB_CLASS_PHID);
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function handle Mouse emulator.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Handled successfully.
 * @retval    Any other error codes     Failed to handle.
 **********************************************************************************************************************/
static fsp_err_t user_mouse_emulator(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Check user input from the terminal to run Mouse emulator */
    if (true == g_b_mouse_flag)
    {
        static uint16_t s_mouse_times = 0U;

        if (MOVE_RIGHT_TIMES > s_mouse_times)
        {
            s_mouse_times++;
            s_mouse_data[MOUSE_X_INDEX] = (uint8_t)MOUSE_X_RIGHT(MOUSE_SPEED);
            s_mouse_data[MOUSE_Y_INDEX] = (uint8_t)MOUSE_STAND;
        }
        else if (MOVE_DOWN_TIMES > s_mouse_times)
        {
            s_mouse_times++;
            s_mouse_data[MOUSE_X_INDEX] = (uint8_t)MOUSE_STAND;
            s_mouse_data[MOUSE_Y_INDEX] = (uint8_t)MOUSE_Y_DOWN(MOUSE_SPEED);
        }
        else if (MOVE_LEFT_TIMES > s_mouse_times)
        {
            s_mouse_times++;
            s_mouse_data[MOUSE_X_INDEX] = (uint8_t)MOUSE_X_LEFT(MOUSE_SPEED);
            s_mouse_data[MOUSE_Y_INDEX] = (uint8_t)MOUSE_STAND;
        }
        else if (MOVE_UP_TIMES > s_mouse_times)
        {
            s_mouse_times++;
            s_mouse_data[MOUSE_X_INDEX] = (uint8_t)MOUSE_STAND;
            s_mouse_data[MOUSE_Y_INDEX] = (uint8_t)MOUSE_Y_UP(MOUSE_SPEED);
        }
        else
        {
            g_b_mouse_flag = false;
            s_mouse_times = 0U;
            s_mouse_data[MOUSE_X_INDEX] = (uint8_t)MOUSE_STAND;
            s_mouse_data[MOUSE_Y_INDEX] = (uint8_t)MOUSE_STAND;
        }

        /* Sending the Mouse data (3 bytes) */
        err = R_USB_Write(&g_basic0_ctrl, (uint8_t *)s_mouse_data, MOUSE_DATA_LEN, USB_CLASS_PHID2);
    }

    /* Stop Mouse emulator, no Mouse activity */
    else
    {
        /* Sending the zero data (3 bytes) */
        err = R_USB_Write(&g_basic0_ctrl, (uint8_t *)s_mouse_zero, MOUSE_DATA_LEN, USB_CLASS_PHID2);
    }

    return err;
}

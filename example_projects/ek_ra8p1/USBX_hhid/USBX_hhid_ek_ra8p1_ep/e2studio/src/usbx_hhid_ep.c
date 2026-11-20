/***********************************************************************************************************************
 * File Name    : usbx_hhid_ep.c
 * Description  : Contains data structures and functions used in usbx_hhid_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "usbx_hhid_ep.h"

/* Global variable */
UX_HOST_CLASS_HID * gp_hid_class_instance;

/* Private variables */
static uint8_t s_ux_pool_memory[MEMPOOL_SIZE];
static bool s_usbx_system_initialized = false;
static bool s_usbx_host_initialized = false;
static bool s_usb_opened = false;

/* Private function */
static UINT apl_host_hid_change_function(ULONG event, UX_HOST_CLASS * host_class, VOID * instance);

/*******************************************************************************************************************//**
 * @brief     This function initializes the USBX system, USBX host stack, and USB peripheral.
 * @param[in] None.
 * @retval    UX_SUCCESS                Initialized successfully.
 * @retval    Any other error code      Failed to initialize.
 **********************************************************************************************************************/
UINT usbx_hhid_init(void)
{
    UINT status = UX_SUCCESS;
    fsp_err_t err = FSP_SUCCESS;

    status = ux_system_initialize((CHAR *)s_ux_pool_memory, MEMPOOL_SIZE, UX_NULL, 0U);
    if (UX_SUCCESS != status)
    {
        return status;
    }
    s_usbx_system_initialized = true;

    status = ux_host_stack_initialize(apl_host_hid_change_function);
    if (UX_SUCCESS != status)
    {
        return status;
    }
    s_usbx_host_initialized = true;

    err = R_USB_Open(&g_basic0_ctrl, &g_basic0_cfg);
    if (FSP_SUCCESS != err)
    {
        return (UINT) err;
    }
    s_usb_opened = true;

    return UX_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief     This function de-initializes the USB peripheral, USBX host stack, and USBX system.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void usbx_hhid_deinit(void)
{
    if (s_usb_opened)
    {
        R_USB_Close(&g_basic0_ctrl);
        s_usb_opened = false;
    }
    if (s_usbx_host_initialized)
    {
        ux_host_stack_uninitialize();
        s_usbx_host_initialized = false;
    }
    if (s_usbx_system_initialized)
    {
        ux_system_uninitialize();
        s_usbx_system_initialized = false;
    }
    gp_hid_class_instance = UX_NULL;
}

/*******************************************************************************************************************//**
 * @brief     This function waits for the requested USB connection event.
 * @param[in] requested_event            USB connection event to wait for.
 * @param[out] actual_flags              Pointer to the actual event flags.
 * @param[in] wait_option                ThreadX wait option.
 * @retval    TX_SUCCESS                 Retrieved successfully.
 * @retval    Any other error code       Failed to retrieve.
 **********************************************************************************************************************/
UINT usbx_hhid_wait_for_connection(ULONG requested_event, ULONG * actual_flags, ULONG wait_option)
{
    return tx_event_flags_get(&g_usb_plug_events, requested_event, TX_AND_CLEAR, actual_flags, wait_option);
}

/*******************************************************************************************************************//**
 * @brief     This function gets the type of the connected USB HID device.
 * @param[in] None.
 * @retval    KEYBOARD                  USB keyboard is connected.
 * @retval    MOUSE                     USB mouse is connected.
 * @retval    UNSUPPORTED               Unsupported USB HID device is connected.
 * @retval    NONE                      USB HID instance is not available.
 **********************************************************************************************************************/
usbx_hhid_device_t usbx_hhid_device_get(void)
{
    UX_HOST_CLASS_HID_CLIENT * p_hid_client = NULL;

    if (NULL == gp_hid_class_instance)
    {
        return NONE;
    }

    p_hid_client = gp_hid_class_instance->ux_host_class_hid_client;

    if (UX_SUCCESS == ux_utility_memory_compare(p_hid_client->ux_host_class_hid_client_name,
                         _ux_system_host_class_hid_client_keyboard_name,
                         _ux_utility_string_length_get(_ux_system_host_class_hid_client_keyboard_name)))
    {
        return KEYBOARD;
    }

    if (UX_SUCCESS == ux_utility_memory_compare(p_hid_client->ux_host_class_hid_client_name,
                         _ux_system_host_class_hid_client_mouse_name,
                         _ux_utility_string_length_get(_ux_system_host_class_hid_client_mouse_name)))
    {
        return MOUSE;
    }

    return UNSUPPORTED;
}

/*******************************************************************************************************************//**
 * @brief      This function gets the latest key value and keyboard state from the connected HID keyboard.
 * @param[out] p_key                    Pointer to the key value.
 * @param[out] p_state                  Pointer to the keyboard state.
 * @retval     UX_SUCCESS               Got successfully.
 * @retval     Any other error code     Failed to get.
 **********************************************************************************************************************/
UINT usbx_hhid_keyboard_key_get(uint8_t * const p_key, ULONG * const p_state)
{
    UX_HOST_CLASS_HID_CLIENT * p_hid_client = gp_hid_class_instance->ux_host_class_hid_client;

    return ux_host_class_hid_keyboard_key_get(
               (UX_HOST_CLASS_HID_KEYBOARD *) p_hid_client->ux_host_class_hid_client_local_instance,
               (ULONG *) p_key, p_state);
}

/*******************************************************************************************************************//**
 * @brief      This function gets the current button state from the connected HID mouse.
 * @param[out] p_buttons                Pointer to the mouse button state.
 * @retval     UX_SUCCESS               Got successfully.
 * @retval     Any other error code     Failed to get.
 **********************************************************************************************************************/
UINT usbx_hhid_mouse_buttons_get(ULONG * const p_buttons)
{
    UX_HOST_CLASS_HID_CLIENT * p_hid_client = gp_hid_class_instance->ux_host_class_hid_client;

    return ux_host_class_hid_mouse_buttons_get(
               (UX_HOST_CLASS_HID_MOUSE *) p_hid_client->ux_host_class_hid_client_local_instance,
               p_buttons);
}

/*******************************************************************************************************************//**
 * @brief      This function gets the current X-axis and Y-axis position from the connected HID mouse.
 * @param[out] p_x                      Pointer to the X-axis position.
 * @param[out] p_y                      Pointer to the Y-axis position.
 * @retval     UX_SUCCESS               Got successfully.
 * @retval     Any other error code     Failed to get.
 **********************************************************************************************************************/
UINT usbx_hhid_mouse_position_get(SLONG * const p_x, SLONG * const p_y)
{
    UX_HOST_CLASS_HID_CLIENT * p_hid_client = gp_hid_class_instance->ux_host_class_hid_client;

    return ux_host_class_hid_mouse_position_get(
               (UX_HOST_CLASS_HID_MOUSE *) p_hid_client->ux_host_class_hid_client_local_instance,
               p_x, p_y);
}

/*******************************************************************************************************************//**
 * @brief     This function checks the USB device status and notifies to perform operation.
 * @param[in] event                     USBX host change event.
 * @param[in] host_class                Pointer to the USBX host class.
 * @param[in] instance                  Pointer to the USBX HID class instance.
 * @retval    UX_SUCCESS.
 **********************************************************************************************************************/
static UINT apl_host_hid_change_function(ULONG event, UX_HOST_CLASS * host_class, VOID * instance)
{
    FSP_PARAMETER_NOT_USED(host_class);

    if (UX_DEVICE_INSERTION == event)
    {
        /* Get a pointer to a USB Host HID Class instance */
        gp_hid_class_instance = (UX_HOST_CLASS_HID *)instance;

        /* Set the event flag as USB Host device is plugged in */
        tx_event_flags_set(&g_usb_plug_events, EVENT_USB_PLUG_IN, TX_OR);
    }
    else if (UX_DEVICE_REMOVAL == event)
    {
        /* Update USB Host HID Class instance to NULL */
        gp_hid_class_instance = UX_NULL;

        /* Set the event flag as USB Host device is plugged out */
        tx_event_flags_set(&g_usb_plug_events, EVENT_USB_PLUG_OUT, TX_OR);
    }
    else
    {
        /* Do nothing */
    }
    return UX_SUCCESS;
}

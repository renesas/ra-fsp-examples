/***********************************************************************************************************************
 * File Name    : usbx_host_hid_key_board_hub_entry.c
 * Description  : Contains data structures and functions used in usbx_host_hid_key_board_hub_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2022 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "usbx_host_hid_key_board_hub.h"
#include "tx_api.h"
#include "ux_system.h"
#include "ux_host_class_hub.h"
#include "usbx_host_hub.h"
#include "common_utils.h"

/*******************************************************************************************************************//**
 * @addtogroup usbx_host_hub_ep
 * @{
 **********************************************************************************************************************/

/* Private function prototypes */
UINT ux_host_hid_hub_class_system_change_function(ULONG event, UX_HOST_CLASS * host_class, VOID * instance);

/******************************************************************************
 Private global variables and functions
 ******************************************************************************/
UX_HOST_CLASS_HID * gp_hid_class_instance[UX_HOST_CLASS_HID_MAX_CLIENTS];

/* Mem pool size of 1024*x bytes is required for USBX class pre built libraries
 * and it is valid only if it with default USBX configurations. */
static uint32_t g_ux_pool_memory[MEMPOOL_SIZE / BYTE_SIZE];

static UX_HOST_CLASS_HID_CLIENT * gp_hid_client;

/*******************************************************************************************************************//**

 * @brief      This is the function to notify changes happened to the USBX Host Class HID.
 * @param[in]  event      Event code from happened to the USBX Host system.
 * @param[in]  instance   Pointer to a USBX Host class instance, which occurs a event.
 * @retval     USBX UX_SUCCESS code
 **********************************************************************************************************************/
UINT ux_host_hid_hub_class_system_change_function(ULONG event, UX_HOST_CLASS * host_class, VOID * instance)
{
    UINT  icount = RESET_VALUE;
    UX_HOST_CLASS_HID_CLIENT * lp_temp_hid_client;
    UX_HOST_CLASS_HID * lp_hid_class_instance;

    FSP_PARAMETER_NOT_USED(host_class);

    /*Callback user function for the USBX Host Class HID.*/
    /*Check if there is a device insertion.*/
    if (UX_DEVICE_INSERTION == event)
    {
        while (NULL != gp_hid_class_instance[icount])
        {
            icount++;
        }

        /*Get a pointer to a USB Host HID Class instance.*/
        gp_hid_class_instance[icount] = (UX_HOST_CLASS_HID *)instance;
        lp_temp_hid_client = gp_hid_class_instance[icount]->ux_host_class_hid_client;

        /* Check for the HUB Class */
        if ((UX_HOST_CLASS_HUB_CLASS == gp_hid_class_instance[icount]->ux_host_class_hid_device->ux_device_descriptor.bDeviceClass))
        {
            PRINT_INFO_STR("Hub Device is attached");
            /* HUB device instance activated. */
            tx_semaphore_put(&g_hub_device_connect);
        }
        /* Check for the HID Class keyboard */
        else if (UX_SUCCESS == _ux_utility_memory_compare(lp_temp_hid_client->ux_host_class_hid_client_name,
                                                          _ux_system_host_class_hid_client_keyboard_name,
                                                          _ux_utility_string_length_get(_ux_system_host_class_hid_client_keyboard_name)))
        {
            PRINT_INFO_STR("Key-Board Device is attached");
            /* Set keyboard connection event*/
            tx_event_flags_set(&g_key_board_connect_event_flags, KEYBOARD_DEVICE_CONNECT_FLAG, TX_OR);
        }
        /* Check for the HID Class keyboard */
        else if (UX_SUCCESS == _ux_utility_memory_compare(lp_temp_hid_client->ux_host_class_hid_client_name,
                                                          _ux_system_host_class_hid_client_mouse_name,
                                                          _ux_utility_string_length_get(_ux_system_host_class_hid_client_mouse_name)))
        {
            PRINT_INFO_STR("Mouse Device is attached");
            /* Set mouse device connection event*/
            tx_event_flags_set(&g_hid_mouse_connect_event_flags, MOUSE_DEVICE_CONNECT_FLAG, TX_OR);
        }
        else
        {
            /* Do nothing */
        }

        tx_thread_sleep(SLEEP_TIME_1);
    }
    /*Check if there is a device removal.*/
    else if (UX_DEVICE_REMOVAL == event)
    {
        while (gp_hid_class_instance[icount] != instance)
        {
            icount++;
        }
        /*Get a pointer to a USB Host HID Class instance.*/
        lp_hid_class_instance = (UX_HOST_CLASS_HID *)instance;
        lp_temp_hid_client = lp_hid_class_instance->ux_host_class_hid_client;

        /* Check the HUB device is removed */
        if ((UX_HOST_CLASS_HUB_CLASS == lp_hid_class_instance->ux_host_class_hid_device->ux_device_descriptor.bDeviceClass))
        {
            PRINT_INFO_STR("Hub Device is detached");
        }
        /* Check the HID Class keyboard is removed */
        else if (UX_SUCCESS == _ux_utility_memory_compare(lp_temp_hid_client->ux_host_class_hid_client_name,
                                                          _ux_system_host_class_hid_client_keyboard_name,
                                                          _ux_utility_string_length_get(_ux_system_host_class_hid_client_keyboard_name)))
        {
            PRINT_INFO_STR("Key-Board Device is detached");
            /* Set keyboard Removed event*/
            tx_event_flags_set(&g_key_board_connect_event_flags, KEYBOARD_DEVICE_REMOVED_FLAG, TX_AND);

        }
        /* Check the HID Class Mouse is removed */
        else if (UX_SUCCESS == _ux_utility_memory_compare(lp_temp_hid_client->ux_host_class_hid_client_name,
                                                          _ux_system_host_class_hid_client_mouse_name,
                                                          _ux_utility_string_length_get(_ux_system_host_class_hid_client_mouse_name)))
        {
            PRINT_INFO_STR("Mouse Device is detached");
            /* Set mouse device Removed event*/
            tx_event_flags_set(&g_hid_mouse_connect_event_flags, MOUSE_DEVICE_REMOVED_FLAG, TX_AND);

        }
        tx_thread_sleep(SLEEP_TIME_1);

        gp_hid_class_instance[icount] = NULL;
        lp_hid_class_instance = NULL;
    }
    else
    {
        /* Do nothing */
    }

    lp_temp_hid_client = NULL;
    return UX_SUCCESS;
}


/* RA USBX HID HUB entry function */
void usbx_host_hid_key_board_hub_entry(void)
{
    ULONG   keyboard_key[BUFF_SIZE] = {RESET_VALUE};
    ULONG   keyboard_state          = RESET_VALUE;
    UINT    host_cl_count           = RESET_VALUE;
    ULONG   actual_value            = RESET_VALUE;
    UINT    tx_state                = RESET_VALUE;
    UINT    status                  = RESET_VALUE;

    fsp_pack_version_t version =   { RESET_VALUE };

    /* version get API for FLEX pack information */
    R_FSP_VersionGet (&version);

    /* Initialize the RTT Thread.*/
    rtt_thread_init_check();

    /* Print the EP Banner and EP info. */
    app_rtt_print_data(RTT_OUTPUT_MESSAGE_BANNER, RESET_VALUE, NULL);

    /* Enable access to the PFS registers*/
    R_BSP_PinAccessEnable();

    /* ux_system_initialization */
    status = ux_system_initialize(g_ux_pool_memory, MEMPOOL_SIZE, UX_NULL, UX_ZERO);
    if (UX_SUCCESS != status)
    {
        /* system initialization error */
        PRINT_ERR_STR("ux_system_initialize api failed");
        ERROR_TRAP(status);
    }
    else
    {
        PRINT_INFO_STR("ux_system_initialize successfully");
    }

    /* Initialize ra usbx host hub hid class */
    usbx_host_hud_hid_class_initialize();

    /* Wait until HUB device instance activated. */
    tx_semaphore_get(&g_hub_device_connect, TX_WAIT_FOREVER);

    /* Get the info and check the hid mouse function thread state and resume the Thread */
    status =  tx_thread_info_get(&usbx_host_hid_mouse_hub, TX_NULL, &tx_state, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL);
    if (UX_SUCCESS != status)
    {
        /* error */
        PRINT_ERR_STR("Error in getting thread info.\n");
        ERROR_TRAP(status);
    }
    /* Check the hid mouse function thread state */
    if (TX_SUSPENDED == tx_state)
    {
        /* resume the Thread */
        tx_thread_resume(&usbx_host_hid_mouse_hub);
    }
    else
    {
        /*  Trap error */
        PRINT_ERR_STR("Error in resuming thread.\n");
        ERROR_TRAP(status);
    }

    while (true)
    {
        /* Check HID Key board device connection status. */
        tx_event_flags_get (&g_key_board_connect_event_flags, KEYBOARD_DEVICE_CONNECT_FLAG, TX_OR, &actual_value, TX_WAIT_FOREVER);

        for (host_cl_count = RESET_VALUE ; host_cl_count < UX_HOST_CLASS_HID_MAX_CLIENTS; host_cl_count++)
        {
            /* Check for the valid hid instance */
            if (NULL != gp_hid_class_instance[host_cl_count])
            {
                gp_hid_client = gp_hid_class_instance[host_cl_count]->ux_host_class_hid_client;

                /* Check for HID Class Key-Board */
                if (UX_SUCCESS == _ux_utility_memory_compare(gp_hid_client->ux_host_class_hid_client_name,
                                                             _ux_system_host_class_hid_client_keyboard_name,
                                                             _ux_utility_string_length_get(_ux_system_host_class_hid_client_keyboard_name)))
                {
                    /* Get the key and keyboard state */
                    status = ux_host_class_hid_keyboard_key_get ((UX_HOST_CLASS_HID_KEYBOARD *)gp_hid_client->ux_host_class_hid_client_local_instance, (ULONG *)keyboard_key, &keyboard_state);

                    if (UX_SUCCESS == status || UX_NO_KEY_PRESS == status)
                    {
                        /* Check if CAPS lock key pressed */
                        if (keyboard_state != RESET_VALUE || keyboard_key !=RESET_VALUE)
                        {
                            if (RESET_VALUE != keyboard_key[BUFF_INDEX])
                            {
                                if((KEY_HIGH_VAL >= keyboard_key[BUFF_INDEX]) && (KEY_LOW_VAL <= keyboard_key[BUFF_INDEX]))
                                {
                                    app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_PRINT_KEY_PRESS, sizeof(keyboard_key), keyboard_key);
                                }
                                else
                                {
                                    PRINT_ERR_STR("Pressed key is out of specified key range");
                                }
                            }
                        }
                        tx_thread_sleep(SLEEP_TIME_1);
                        keyboard_key[BUFF_INDEX] = RESET_VALUE;
                        keyboard_state           =RESET_VALUE;
                    }
                }
                else
                {
                    tx_thread_sleep(SLEEP_TIME_5);
                }
            }
            else
            {
                tx_thread_sleep(SLEEP_TIME_10);
            }
        }
    }
}

/* Host stack initialization and HUB, HID class register and hid client register */
void usbx_host_hud_hid_class_initialize(void)
{
    UINT status = RESET_VALUE;

    /* host stack initialization*/
    status = ux_host_stack_initialize (ux_host_hid_hub_class_system_change_function);
    if (UX_SUCCESS != status)
    {
        /* stack initialization error */
        PRINT_ERR_STR("ux_host_stack_initialize api failed");
        ERROR_TRAP(status);
    }
    else
    {
        PRINT_INFO_STR("ux_host_stack_initialize successfully");
    }
    tx_thread_sleep(SLEEP_TIME_100);
    /*Start the USB module by calling open function */
    status = R_USB_Open(&g_basic_ctrl, &g_basic_cfg);
    if (UX_SUCCESS != status)
    {
        /* USB module by open fun error */
        PRINT_ERR_STR("R_USB_Open api failed");
        ERROR_TRAP(status);
    }
    else
    {
        PRINT_INFO_STR("R_USB_Open initialized successfully");
    }
}

/* Host stack un-initialization and HUB, HID class un-register and hid client un-register */
void usbx_host_hud_hid_class_un_init(void)
{
    UINT status = UX_SUCCESS;

    /*Stop the USB module by calling close function */
    status = R_USB_Close(&g_basic_ctrl);
    if (UX_SUCCESS != status)
    {
        /* USB module by close fun error */
        PRINT_ERR_STR("R_USB_Close api failed");
        ERROR_TRAP(status);
    }
    else
    {
        PRINT_INFO_STR("R_USB_Close successfully");
    }
    /*Invoke host un-initializes function*/
    status = ux_host_stack_uninitialize();
    if (UX_SUCCESS != status)
    {
        /* stack un-initialization error */
        PRINT_ERR_STR("ux_host_stack_uninitialize api failed");
        ERROR_TRAP(status);
    }
    else
    {
        PRINT_INFO_STR("ux_host_stack_uninitialize successfully");
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup usbx_host_hub_ep)
 **********************************************************************************************************************/

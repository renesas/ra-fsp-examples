/***********************************************************************************************************************
 * File Name    : pmsc_thread_entry.c
 * Description  : Contains functions from the pmsc main thread
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "pmsc_thread.h"
#include "usbx_pmsc_ep.h"
#include "common_utils.h"

/*******************************************************************************************************************//**
 * @addtogroup USBX_pmsc_ep
 * @{
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * Exported global variable (to be accessed by other files)
 **********************************************************************************************************************/
extern uint8_t g_device_framework_full_speed[];
extern uint8_t g_device_framework_hi_speed[];
extern uint8_t g_string_framework[];
extern uint8_t g_language_id_framework[];

/* Memory pool size of 14k is required for USBX device class pre-built libraries
 * and it is valid only if it with default USBX configurations */
static uint32_t g_ux_pool_memory[MEMPOOL_SIZE / BYTE_SIZE];
static ULONG actual_flags = RESET_VALUE;

/* Private function declaration */
UINT usbx_status_callback(ULONG status);

/* PMSC Thread entry function */
void pmsc_thread_entry(void)
{
    /* To check UX API return status */
    UINT status = UX_SUCCESS;
    /* To check FSP API return status */
    fsp_err_t err = FSP_SUCCESS;

    /* Initialize the RTT Thread */
    rtt_thread_init_check();

    /* Print the banner and EP info */
    app_rtt_print_data(RTT_OUTPUT_MESSAGE_BANNER, RESET_VALUE, NULL);

    /* ux_system initialization */
    status = ux_system_initialize(g_ux_pool_memory, MEMPOOL_SIZE, UX_NULL, RESET_VALUE);
    if (UX_SUCCESS != status)
    {
        PRINT_ERR_STR("ux_system_initialize API failed");
        ERROR_TRAP(status);
    }
    PRINT_INFO_STR("ux_system initialized successfully");

    /* ux_device stack initialization */
    status = ux_device_stack_initialize(g_device_framework_hi_speed,
                                        DEVICE_FRAME_LENGTH_HIGH_SPEED,
                                        g_device_framework_full_speed,
                                        DEVICE_FRAME_LENGTH_FULL_SPEED,
                                        g_string_framework,
                                        STRING_FRAMEWORK_LENGTH,
                                        g_language_id_framework,
                                        LANGUAGE_ID_FRAME_WORK_LENGTH,
                                        &usbx_status_callback);
    if (UX_SUCCESS != status)
    {
        PRINT_ERR_STR("ux_device_stack_initialize API failed");
        ERROR_TRAP(status);
    }
    PRINT_INFO_STR("ux_device stack initialized successfully");

    /* Open USB driver */
    err = R_USB_Open(&g_basic0_ctrl, &g_basic0_cfg);
    if (FSP_SUCCESS != err)
    {
        PRINT_ERR_STR("R_USB_Open API failed");
        ERROR_TRAP(err);
    }
    PRINT_INFO_STR("USB driver opened successfully");

    PRINT_INFO_STR("Insert the USB MSC cable");

    /* Wait until device inserted */
    tx_event_flags_get(&g_msc_event_flags0, USB_MSC_PLUG_IN, TX_AND_CLEAR, &actual_flags, TX_WAIT_FOREVER);
    if(USB_MSC_PLUG_IN == actual_flags)
    {
        PRINT_INFO_STR("USB MSC device is plugged in");
    }

    /* Reset the event flag */
    actual_flags = RESET_VALUE;

    while (true)
    {
        /* Check if the USB device has been unplugged */
        tx_event_flags_get(&g_msc_event_flags0, USB_MSC_PLUG_OUT, TX_AND_CLEAR, &actual_flags, TX_NO_WAIT);
        if (USB_MSC_PLUG_OUT == (actual_flags & USB_MSC_PLUG_OUT))
        {
            /* Reset the event flag */
            actual_flags = RESET_VALUE;
            PRINT_INFO_STR("USB MSC device is unplugged. Please reconnect the USB device cable.");
        }
        /* Check if USB device has been plugged in */
        tx_event_flags_get(&g_msc_event_flags0, USB_MSC_PLUG_IN, TX_AND_CLEAR, &actual_flags, TX_NO_WAIT);
        if (USB_MSC_PLUG_IN == (actual_flags & USB_MSC_PLUG_IN))
        {
            /* Reset the event flag */
            actual_flags = RESET_VALUE;
            PRINT_INFO_STR("USB MSC device is plugged in");
        }
        tx_thread_sleep(1);
    }
}

/*******************************************************************************************************************//**
 * @brief     USB callback function for USB status change.
 * @param[IN] status    USB status code.
 * @retval    Zero
 **********************************************************************************************************************/
UINT usbx_status_callback(ULONG status)
{
    switch (status)
    {
        case UX_DEVICE_ATTACHED:
        {
            /* Set USB PLUG-IN event */
            tx_event_flags_set(&g_msc_event_flags0, USB_MSC_PLUG_IN, TX_OR);
        }
        break;

        case UX_DEVICE_REMOVED:
        {
            /* Set USB PLUG-OUT event */
            tx_event_flags_set(&g_msc_event_flags0, USB_MSC_PLUG_OUT, TX_OR);
        }
        break;

        default:
        {
            /* Do nothing */
        }
        break;
    }
    return 0;
}

/*******************************************************************************************************************//**
 * @} (end addtogroup USBX_pmsc_ep)
 **********************************************************************************************************************/

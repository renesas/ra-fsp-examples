/***********************************************************************************************************************
 * File Name    : pcdc_acm_thread_entry.c
 * Description  : Contains macros and functions used in pcdc_acm_thread_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "pcdc_acm_thread.h"
#include "usbx_pcdc_acm_ep.h"
#include "common_utils.h"

/*******************************************************************************************************************//**
 * @addtogroup ubsx_pcdc_acm_ep
 * @{
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * Macro definitions
 **********************************************************************************************************************/
#define CDCACM_FLAG                         ((ULONG) 0x0001)
#define CDCACM_ACTIVATE_FLAG                ((ULONG) 0x0004)
#define CDCACM_DEACTIVATE_FLAG              ((ULONG) 0x0008)

/*******************************************************************************************************************//**
 * Exported global variables and functions (to be accessed by other files)
 **********************************************************************************************************************/
extern uint8_t g_device_framework_full_speed[];
extern uint8_t g_device_framework_hi_speed[];
extern uint8_t g_language_id_framework[];
extern uint8_t g_string_framework[];
extern uint32_t usb_peri_usbx_initialize(uint32_t dcd_io);

/*******************************************************************************************************************//**
 * Global functions and variables
 **********************************************************************************************************************/
UINT usbx_status_callback(ULONG status);

/*******************************************************************************************************************//**
 * Private global variables and functions
 **********************************************************************************************************************/
static void ux_cdc_device0_instance_activate(void * cdc_instance);
static void ux_cdc_device0_instance_deactivate(void * cdc_instance);
static void usb_connection_status_check(void);
static void usbx_pcdc_operations(void);
/* Memory pool size of 18k is required for USBX device class pre-built libraries
 * and it is valid only if it with default USBX configurations */
static uint32_t g_ux_pool_memory[MEMPOOL_SIZE / BYTE_SIZE];
static UX_SLAVE_CLASS_CDC_ACM_PARAMETER g_ux_device_class_cdc_acm0_parameter;
static UX_SLAVE_CLASS_CDC_ACM * g_cdc;
static ULONG g_actual_length;
static uint8_t g_buf[DATA_LEN];
static bool b_print_status = false;

/* PCDC ACM Thread entry function */
void pcdc_acm_thread_entry(void)
{
    /* To check UX API return status */
    UINT status = UX_SUCCESS;
    /* To check FSP API return status */
    fsp_err_t err = FSP_SUCCESS;
    ULONG actual_flags = 0x0000;
    /* Initialize the RTT Thread */
    rtt_thread_init_check();

    /* Print the EP banner and information */
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
    status = ux_device_stack_initialize(g_device_framework_hi_speed, DEVICE_FRAME_LENGTH_HIGH_SPEED,\
                                        g_device_framework_full_speed, DEVICE_FRAME_LENGTH_FULL_SPEED,\
                                        g_string_framework, STRING_FRAMEWORK_LENGTH,\
                                        g_language_id_framework, LANGUAGE_ID_FRAME_WORK_LENGTH,\
                                        &usbx_status_callback);
    /* Handle error */
    if (UX_SUCCESS != status)
    {
        PRINT_ERR_STR("ux_device_stack_initialize API failed");
        ERROR_TRAP(status);
    }

    PRINT_INFO_STR("ux_device_stack initialized successfully");

    /* The activate command is used when the host has sent a SET_CONFIGURATION command
     * and this interface has to be mounted. Both Bulk endpoints have to be mounted
     * and the cdc_acm thread needs to be activated */
    g_ux_device_class_cdc_acm0_parameter.ux_slave_class_cdc_acm_instance_activate = ux_cdc_device0_instance_activate;

    /* The deactivate command is used when the device has been extracted.
     * The device endpoints have to be dismounted and the cdc_acm thread canceled */
    g_ux_device_class_cdc_acm0_parameter.ux_slave_class_cdc_acm_instance_deactivate =\
                                         ux_cdc_device0_instance_deactivate;

    /* ux_device stack class registration */
    status = ux_device_stack_class_register(_ux_system_slave_class_cdc_acm_name,\
                                            _ux_device_class_cdc_acm_entry, CONFIG_NUMB,\
                                            INTERFACE_NUMB0, (void*) &g_ux_device_class_cdc_acm0_parameter);
    /* Handle error */
    if (UX_SUCCESS != status)
    {
        PRINT_ERR_STR("ux_device_stack_class_register API failed");
        ERROR_TRAP(status);
    }

    PRINT_INFO_STR("ux_device_stack_class registered successfully");
    /* Initialize the peripheral mode according to the USB speed selection */
    if (USB_SPEED_FS == g_basic0_cfg.usb_speed)
    {
        /* Peripheral mode initialization with Full Speed */
        status = usb_peri_usbx_initialize(R_USB_FS0_BASE);
    }
    else if (USB_SPEED_HS == g_basic0_cfg.usb_speed)
    {
        /* Peripheral mode initialization with High Speed */
        status = usb_peri_usbx_initialize (R_USB_HS0_BASE);
    }
    else
    {
        /* Do nothing */
    }
    /* Error Handle */
    if (UX_SUCCESS != status)
    {
        PRINT_ERR_STR("usb_peri_usbx_initialize API failed");
        ERROR_TRAP(status);
    }

    PRINT_INFO_STR("usb_peri_usbx initialized successfully");

    /* Open USB driver */
    err = R_USB_Open(&g_basic0_ctrl, &g_basic0_cfg);
    /* Error Handle */
    if (FSP_SUCCESS != err)
    {
        PRINT_ERR_STR("R_USB_Open API failed");
        ERROR_TRAP(status);
    }

    PRINT_INFO_STR("USB driver opened successfully");

    PRINT_INFO_STR("To verify the USBX_PCDC operations, open the serial terminal and check the status");
    /* Wait for enumeration event */
    status = tx_event_flags_get(&g_cdcacm_event_flags0, CDCACM_ACTIVATE_FLAG, TX_OR, &actual_flags, TX_WAIT_FOREVER);
    if ((actual_flags & CDCACM_ACTIVATE_FLAG) && (TX_SUCCESS == status))
    {
        PRINT_INFO_STR("USB enumeration completed");
    }
    else if (!(actual_flags & CDCACM_ACTIVATE_FLAG))
    {
        PRINT_INFO_STR("USB instance deactivated");
    }

    /* USB PCDC operations will echo the user input on serial terminal */
    while (true)
    {
        usbx_pcdc_operations ();
        tx_thread_sleep (1);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function checks the USB device status and notifies the user by printing the status message.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void usb_connection_status_check(void)
{
    ULONG actual_flags = RESET_VALUE;
    /* Wait for USB connection event */
    tx_event_flags_get(&g_cdcacm_event_flags0, CDCACM_FLAG, TX_OR, &actual_flags, TX_WAIT_FOREVER);
    if (actual_flags & CDCACM_FLAG)
    {

        if (true != b_print_status)
        {
            PRINT_INFO_STR("Status of USB Device is attached");
            b_print_status = true;  /* Flag is updated */
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        b_print_status = false;     /* Clear the flag */

        PRINT_INFO_STR("Status of USB Device is removed");
        while (!(actual_flags & CDCACM_FLAG))
        {
            ;   /* Wait until the event update */
        }
    }

}

/*******************************************************************************************************************//**
 * @brief     This function performs the USB write/read operation and echoes back the user input on serial terminal.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void usbx_pcdc_operations(void)
{
    UINT status = UX_SUCCESS;
    uint32_t data_size = RESET_VALUE;
    volatile UX_SLAVE_DEVICE *device;
    device = &_ux_system_slave->ux_system_slave_device;

    /* Verify the status of USB */
    usb_connection_status_check();

    /* Wait until USB device is configured to slave */
    while (device->ux_slave_device_state != UX_DEVICE_CONFIGURED)
    {
        ;   /* Wait until the slave device state is configured */
    }

    /* USB writes the display message on serial terminal */
    status = ux_device_class_cdc_acm_write(g_cdc,\
             (UCHAR*) "\r\nEnter any key to echo back the entered data\r\nUser Input:", WRITE_DATA_LEN,\
             &g_actual_length);

    /* Handle error */
    if (UX_SUCCESS != status)
    {
        PRINT_ERR_STR("ux_device_class_cdc_acm_write API failed");
        ERROR_TRAP(status);
    }

    /* Clear the buffer */
    memset(g_buf, RESET_VALUE, sizeof(g_buf));

    /* USB reads the input data from the user from serial terminal */
    status = ux_device_class_cdc_acm_read(g_cdc, g_buf, DATA_LEN, &g_actual_length);
    /* Handle error */
    if (UX_SUCCESS != status)
    {
        PRINT_ERR_STR("ux_device_class_cdc_acm_read API failed");
        ERROR_TRAP(status);
    }
    /* Update the data length from the read input */
    data_size = g_actual_length;
    /* Write back the read data on to the serial terminal */
    status = ux_device_class_cdc_acm_write(g_cdc, g_buf, data_size, &g_actual_length);
    /* Handle error */
    if (UX_SUCCESS != status)
    {
        PRINT_ERR_STR("ux_device_class_cdc_acm_write API failed");
        ERROR_TRAP(status);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function activates the CDC instance.
 * @param[in] cdc_instance    Pointer to the area store the instance pointer.
 * @retval    None.
 **********************************************************************************************************************/
static void ux_cdc_device0_instance_activate (void * cdc_instance)
{
    /* Save the CDC instance */
    g_cdc = (UX_SLAVE_CLASS_CDC_ACM *) cdc_instance;
    tx_event_flags_set(&g_cdcacm_event_flags0, CDCACM_ACTIVATE_FLAG, TX_OR);
}

/*******************************************************************************************************************//**
 * @brief     This function deactivates the CDC instance.
 * @param[in] cdc_instance    Pointer to area store the instance pointer.
 * @retval    None.
 **********************************************************************************************************************/
static void ux_cdc_device0_instance_deactivate (void * cdc_instance)
{
    FSP_PARAMETER_NOT_USED(cdc_instance);
    g_cdc = UX_NULL;
    tx_event_flags_set(&g_cdcacm_event_flags0, CDCACM_DEACTIVATE_FLAG, TX_OR);
}

/*******************************************************************************************************************//**
 * @brief     In this function, USB callback events will be captured into one variable.
 * @param[in] status    Whenever any event occurred, status gets update.
 * @retval    Zero.
 **********************************************************************************************************************/
UINT usbx_status_callback (ULONG status)
{
    switch (status)
    {
        case UX_DEVICE_ATTACHED:
        {
            tx_event_flags_set(&g_cdcacm_event_flags0, CDCACM_FLAG, TX_OR);
        }
        break;

        case UX_DEVICE_REMOVED:
        {
            tx_event_flags_set(&g_cdcacm_event_flags0, ~CDCACM_FLAG, TX_AND);
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
